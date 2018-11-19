#ifndef _MOTION_HEADER_
#define _MOTION_HEADER_

#include "BNAFile.h"
#include "File.h"
#include "NUM.h"
#include "MPK.h"
#include "GameConstant.h"

#include <vector>
#include <d3dx9math.h>

namespace imas
{

struct JointPose	// やや冗長だが…
{
	D3DXVECTOR3 translate;
	D3DXVECTOR3 rot;		// こっちは参考用。スキニングに直接は使わない
	D3DXVECTOR3 scale;

	D3DXQUATERNION q;		// 回転はquaternionを正とする

	JointPose() : translate(D3DXVECTOR3(0.0f,0.0f,0.0f)), rot(D3DXVECTOR3(0.0f,0.0f,0.0f)),
					scale(D3DXVECTOR3(1.0f,1.0f,1.0f)), q(D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f)){};

	void CalcQuaternion()
	{
		D3DXMATRIX rotX_mat, rotY_mat, rotZ_mat, tmp_M;
		D3DXMatrixRotationX(&rotX_mat, rot.x);
		D3DXMatrixRotationY(&rotY_mat, rot.y);
		D3DXMatrixRotationZ(&rotZ_mat, rot.z);

		tmp_M = rotX_mat * rotY_mat * rotZ_mat;

		D3DXQuaternionRotationMatrix( &q, &tmp_M);
	}
};


// Body用
struct BodyPose
{
	D3DXVECTOR3 position;	// 腰の位置
	std::vector< JointPose > joint_pose;	// 回転のみ使用
};

// Body Poseのlerp用
BodyPose PoseLerp(BodyPose& pose1, BodyPose& pose2, float t);


class Motion
{
public:
	std::vector< BodyPose > body_pose;

public:
	enum POSITION { CENTER, RIGHT, LEFT};
	enum MOTIONTYPE { MAIN, LOOP};

public:
	BodyPose GetPose( float key )
	{
		if( key < 0 ) return body_pose[ 0 ];
		if( key >= body_pose.size() - 1 ) return body_pose[ body_pose.size() - 1 ];
		
		float fraction = key - (int) key;

		BodyPose result = PoseLerp( body_pose[ (int) key ], body_pose[ (int) key+1 ], fraction );
		
		return result;
	}

	int GetSize()
	{
		return body_pose.size();
	}

	bool Load(imas::BNAFile* pBNAFile, POSITION pos, MOTIONTYPE motiontype)
	{
		imas::NUM numBody, numHandL, numHandR;

		std::string body, handr, handl;

		if( motiontype == MAIN )
		{
			switch( pos){
			case CENTER:
				body = "_c.num";
				handr = "_c_HANDR.num";
				handl = "_c_HANDL.num";
				break;
			case RIGHT:
				body = "_r.num";
				handr = "_r_HANDR.num";
				handl = "_r_HANDL.num";
				break;
			case LEFT:
				body = "_l.num";
				handr = "_l_HANDR.num";
				handl = "_l_HANDL.num";
				break;
			default:
				// error
				;
			}
		}else
		{
			switch( pos){
			case CENTER:
				body = "_c_loop.num";
				handr = "_c_loop_HANDR.num";
				handl = "_c_loop_HANDL.num";
				break;
			case RIGHT:
				body = "_r_loop.num";
				handr = "_r_loop_HANDR.num";
				handl = "_r_loop_HANDL.num";
				break;
			case LEFT:
				body = "_l_loop.num";
				handr = "_l_loop_HANDR.num";
				handl = "_l_loop_HANDL.num";
				break;
			default:
				// error
				;
				break;
			}
		}
		
		// "omo.bna"だけファイル名が違うので特別扱い
		if( pBNAFile->m_filename.find( "omo.bna" ) != std::string::npos )
		{
			if( motiontype == MAIN )
			{
				switch( pos){
				case CENTER:
					body = "_c_sayc.num";
					handr = "_c_sayc_HANDR.num";
					handl = "_c_sayc_HANDL.num";
					break;
				case RIGHT:
					body = "_r_sayr.num";
					handr = "_r_sayr_HANDR.num";
					handl = "_r_sayr_HANDL.num";
					break;
				case LEFT:
					body = "_l_sayl.num";
					handr = "_l_sayl_HANDR.num";
					handl = "_l_sayl_HANDL.num";
					break;
				default:
					// error
					;
				}
			}else
			{
				switch( pos){
				case CENTER:
					body = "_c_sayc_loop.num";
					handr = "_c_sayc_loop_HANDR.num";
					handl = "_c_sayc_loop_HANDL.num";
					break;
				case RIGHT:
					body = "_r_sayr_loop.num";
					handr = "_r_sayr_loop_HANDR.num";
					handl = "_r_sayr_loop_HANDL.num";
					break;
				case LEFT:
					body = "_l_sayl_loop.num";
					handr = "_l_sayl_loop_HANDR.num";
					handl = "_l_sayl_loop_HANDL.num";
					break;
				default:
					// error
					;
					break;
				}
			}
		}

		// 読み込み
		{
			File* pFile = pBNAFile->GetFileByFilter( body );
			if( pFile == NULL ) return false;
			if( ! numBody.Load( pFile ) ) return false;
		}
		{
			File* pFile = pBNAFile->GetFileByFilter( handr );
			if( pFile == NULL ) return false;
			if( ! numHandR.Load( pFile ) ) return false;
		}
		{
			File* pFile = pBNAFile->GetFileByFilter( handl );
			if( pFile == NULL ) return false;
			if( ! numHandL.Load( pFile ) ) return false;
		}
		// body animation
		int size = min( numBody.key.size(), min( numHandL.key.size(), numHandR.key.size() ) );

		body_pose.resize( size );

		for( int key = 0; key < size; key++ )
		{
			body_pose[key].joint_pose.resize( 54 );

			// アニメーションの登録
			body_pose[key].position.x = numBody.key[key].ch[2].data1 / 1000.0f * 2.0f;
			body_pose[key].position.y = numBody.key[key].ch[2].data2 / 1000.0f * 2.0f;
			body_pose[key].position.z = numBody.key[key].ch[2].data3 / 1000.0f * 2.0f;

			// body
			for(int i=3; i<numBody.key[key].ch.size(); i++)
			{
				body_pose[key].joint_pose[i-1].rot.x = numBody.key[key].ch[i].data1 / 32768.0f * D3DX_PI;
				body_pose[key].joint_pose[i-1].rot.y = numBody.key[key].ch[i].data2 / 32768.0f * D3DX_PI;
				body_pose[key].joint_pose[i-1].rot.z = numBody.key[key].ch[i].data3 / 32768.0f * D3DX_PI;

				body_pose[key].joint_pose[i-1].CalcQuaternion();
			}

			// R hand
			for(int i=0; i<numHandL.key[key].ch.size(); i++)
			{
				body_pose[key].joint_pose[i+24].rot.x = numHandR.key[key].ch[i].data1 / 32768.0f * D3DX_PI;
				body_pose[key].joint_pose[i+24].rot.y = numHandR.key[key].ch[i].data2 / 32768.0f * D3DX_PI;
				body_pose[key].joint_pose[i+24].rot.z = numHandR.key[key].ch[i].data3 / 32768.0f * D3DX_PI;

				body_pose[key].joint_pose[i+24].CalcQuaternion();
			}

			// L hand
			for(int i=0; i<numHandR.key[key].ch.size(); i++)
			{
				body_pose[key].joint_pose[i+39].rot.x = numHandL.key[key].ch[i].data1 / 32768.0f * D3DX_PI;
				body_pose[key].joint_pose[i+39].rot.y = numHandL.key[key].ch[i].data2 / 32768.0f * D3DX_PI;
				body_pose[key].joint_pose[i+39].rot.z = numHandL.key[key].ch[i].data3 / 32768.0f * D3DX_PI;

				body_pose[key].joint_pose[i+39].CalcQuaternion();
			}
		}
		return true;
	}

	bool Load(std::string filename, POSITION pos, MOTIONTYPE motiontype)
	{
		imas::BNAFile bna;
		if( ! bna.Load( filename ) ) return false;

		return  Load( & bna, pos, motiontype );
	}
};




// リップシンク
struct LipPose
{
	D3DXVECTOR2 v;	// 口の動きを表す内部表現
};

// リップシンク
class LipMotion
{
private:
	std::vector<LipPose> lip_pose;

public:
	LipPose GetPose( float key )
	{
		if( key < 0 ) return lip_pose[ 0 ];
		if( key  >= lip_pose.size() - 1 ) return lip_pose[ lip_pose.size() - 1 ];

		float fraction = key - (int) key;
		LipPose result;
		result.v = (1-fraction) * lip_pose[ (int)key ].v + fraction * lip_pose[ (int)key+1 ].v;
		return result;
	}

	bool Load( BNAFile* pBNAFile )
	{
		NUM numLip;

		{
			File* pFile = pBNAFile->GetFileByFilter("lipsync_");
			if( pFile == NULL ) return false;
			if( ! numLip.Load( pFile ) ) return false;
		}

		lip_pose.resize( numLip.key.size() );
		for( int key = 0; key < numLip.key.size(); key++)
		{
			lip_pose[key].v.x = numLip.key[key].ch[0].data1 / 16384.0f;
			lip_pose[key].v.y = numLip.key[key].ch[0].data3 / 16384.0f;
		}

		return true;
	}

	bool Load(std::string filename)
	{
		BNAFile bna;
		if( ! bna.Load( filename ) ) return false;

		return  Load( & bna );
	}
};



// facial.mpkから変換
struct FacialPose
{
	std::vector< JointPose > joint_pose;
};

struct FacialPoseArray
{
	std::vector< FacialPose > facial_pose;

	bool Load( std::string staticmot_file, imas::CHARACTER chara)
	{
		BNAFile staticmot_bnafile;
		if( ! staticmot_bnafile.Load( staticmot_file )) return false;

		std::string filename;
		switch( chara )
		{
		case imas::HARUKA:		filename = "chr01_har_facial.mpk"; break;
		case imas::CHIHAYA:		filename = "chr02_chi_facial.mpk"; break;
		case imas::YUKIHO:		filename = "chr03_yuk_facial.mpk"; break;
		case imas::YAYOI:		filename = "chr04_yay_facial.mpk"; break;
		case imas::RITSUKO:		filename = "chr05_rit_facial.mpk"; break;
		case imas::AZUSA:		filename = "chr06_azu_facial.mpk"; break;
		case imas::IORI:		filename = "chr07_ior_facial.mpk"; break;
		case imas::MAKOTO:		filename = "chr08_mak_facial.mpk"; break;
		case imas::AMI:			filename = "chr09_ami_facial.mpk"; break;
		case imas::MAMI:		filename = "chr10_mam_facial.mpk"; break;
		case imas::MIKI:		filename = "chr11_mik_facial.mpk"; break;
		case imas::MIKI_SHORT:	filename = "chr12_mis_facial.mpk"; break;
		}

		File* pFile;
		MPK mpk;
		if( ! (pFile = staticmot_bnafile.GetFileByFilter( filename )) ) return false;
		if( ! mpk.Load( pFile ) ) return false;

		facial_pose.resize( mpk.num[0].key.size() );

		for(int faceID=0; faceID < mpk.num[0].key.size(); faceID++)
		{
			facial_pose[faceID].joint_pose.resize( 136 );	// メモリを大量に無駄にしているので後で変更する

			for(int i=83; i<=95; i++)
			{
				facial_pose[faceID].joint_pose[i].translate.x = mpk.num[0].key[faceID].ch[(i-83)*2].data1 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.y = mpk.num[0].key[faceID].ch[(i-83)*2].data2 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.z = mpk.num[0].key[faceID].ch[(i-83)*2].data3 / 1000.0f;

				facial_pose[faceID].joint_pose[i].rot.x = mpk.num[0].key[faceID].ch[(i-83)*2+1].data1 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.y = mpk.num[0].key[faceID].ch[(i-83)*2+1].data2 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.z = mpk.num[0].key[faceID].ch[(i-83)*2+1].data3 / 32768.0f * D3DX_PI;

				facial_pose[faceID].joint_pose[i].CalcQuaternion();
			}
			{
				int i=96;
				facial_pose[faceID].joint_pose[i].translate.x = mpk.num[0].key[faceID].ch[26].data1 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.y = mpk.num[0].key[faceID].ch[26].data2 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.z = mpk.num[0].key[faceID].ch[26].data3 / 1000.0f;

				facial_pose[faceID].joint_pose[i].rot.x = mpk.num[0].key[faceID].ch[27].data1 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.y = mpk.num[0].key[faceID].ch[27].data2 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.z = mpk.num[0].key[faceID].ch[27].data3 / 32768.0f * D3DX_PI;

				facial_pose[faceID].joint_pose[i].CalcQuaternion();
			}
			{
				int i=97;
				facial_pose[faceID].joint_pose[i].translate.x = mpk.num[0].key[faceID].ch[28].data1 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.y = mpk.num[0].key[faceID].ch[28].data2 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.z = mpk.num[0].key[faceID].ch[28].data3 / 1000.0f;

				facial_pose[faceID].joint_pose[i].rot.x = mpk.num[0].key[faceID].ch[29].data1 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.y = mpk.num[0].key[faceID].ch[29].data2 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.z = mpk.num[0].key[faceID].ch[29].data3 / 32768.0f * D3DX_PI;

				facial_pose[faceID].joint_pose[i].scale.x = mpk.num[0].key[faceID].ch[30].data1 / 32768.0f * 2.0f;
				facial_pose[faceID].joint_pose[i].scale.y = mpk.num[0].key[faceID].ch[30].data2 / 32768.0f * 2.0f;
				facial_pose[faceID].joint_pose[i].scale.z = mpk.num[0].key[faceID].ch[30].data3 / 32768.0f * 2.0f;

				facial_pose[faceID].joint_pose[i].CalcQuaternion();
			}
			{
				int i=98;
				facial_pose[faceID].joint_pose[i].translate.x = mpk.num[0].key[faceID].ch[31].data1 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.y = mpk.num[0].key[faceID].ch[31].data2 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.z = mpk.num[0].key[faceID].ch[31].data3 / 1000.0f;

				facial_pose[faceID].joint_pose[i].rot.x = mpk.num[0].key[faceID].ch[32].data1 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.y = mpk.num[0].key[faceID].ch[32].data2 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.z = mpk.num[0].key[faceID].ch[32].data3 / 32768.0f * D3DX_PI;

				facial_pose[faceID].joint_pose[i].CalcQuaternion();
			}
			{
				int i=99;
				facial_pose[faceID].joint_pose[i].translate.x = mpk.num[0].key[faceID].ch[33].data1 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.y = mpk.num[0].key[faceID].ch[33].data2 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.z = mpk.num[0].key[faceID].ch[33].data3 / 1000.0f;

				facial_pose[faceID].joint_pose[i].rot.x = mpk.num[0].key[faceID].ch[34].data1 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.y = mpk.num[0].key[faceID].ch[34].data2 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.z = mpk.num[0].key[faceID].ch[34].data3 / 32768.0f * D3DX_PI;

				facial_pose[faceID].joint_pose[i].scale.x = mpk.num[0].key[faceID].ch[35].data1 / 32768.0f * 2.0f;
				facial_pose[faceID].joint_pose[i].scale.y = mpk.num[0].key[faceID].ch[35].data2 / 32768.0f * 2.0f;
				facial_pose[faceID].joint_pose[i].scale.z = mpk.num[0].key[faceID].ch[35].data3 / 32768.0f * 2.0f;

				facial_pose[faceID].joint_pose[i].CalcQuaternion();
			}
			for(int i=100; i<=135; i++)
			{
				facial_pose[faceID].joint_pose[i].translate.x = mpk.num[0].key[faceID].ch[(i-82)*2].data1 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.y = mpk.num[0].key[faceID].ch[(i-82)*2].data2 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.z = mpk.num[0].key[faceID].ch[(i-82)*2].data3 / 1000.0f;

				facial_pose[faceID].joint_pose[i].rot.x = mpk.num[0].key[faceID].ch[(i-82)*2+1].data1 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.y = mpk.num[0].key[faceID].ch[(i-82)*2+1].data2 / 32768.0f * D3DX_PI;
				facial_pose[faceID].joint_pose[i].rot.z = mpk.num[0].key[faceID].ch[(i-82)*2+1].data3 / 32768.0f * D3DX_PI;

				facial_pose[faceID].joint_pose[i].CalcQuaternion();
			}
			// eye
			for(int i=100; i< 122; i++)
			{
				facial_pose[faceID].joint_pose[i].translate.x = mpk.num[3].key[faceID].ch[(i-100)*2].data1 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.y = mpk.num[3].key[faceID].ch[(i-100)*2].data2 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.z = mpk.num[3].key[faceID].ch[(i-100)*2].data3 / 1000.0f;

				facial_pose[faceID].joint_pose[i].CalcQuaternion();
			}
			// mouth
			for(int i=122; i<136; i++)
			{
				facial_pose[faceID].joint_pose[i].translate.x = mpk.num[6].key[faceID].ch[(i-122)*2].data1 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.y = mpk.num[6].key[faceID].ch[(i-122)*2].data2 / 1000.0f;
				facial_pose[faceID].joint_pose[i].translate.z = mpk.num[6].key[faceID].ch[(i-122)*2].data3 / 1000.0f;

				facial_pose[faceID].joint_pose[i].CalcQuaternion();
			}
		}
		return true;
	}
};


FacialPose FacialPoseLerp(FacialPose& pose1, FacialPose& pose2, float t);


}	// namespace

#endif