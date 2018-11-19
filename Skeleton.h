#ifndef _SKELETON_HEADER_
#define _SKELETON_HEADER_

#include "File.h"
#include "NUM.h"
#include "SKL.h"
#include "MPK.h"

#include "Motion.h"

#include "d3dx9math.h"
#include <vector>

#include <GL/glut.h>


namespace imas
{

struct Joint{
	int parentJoint;

	float posX;
	float posY;
	float posZ;
	float rotX;
	float rotY;
	float rotZ;
	float scaleX;
	float scaleY;
	float scaleZ;

	D3DXMATRIX matLocal;
	D3DXMATRIX matWorld;

	D3DXMATRIX matInitialWorld;		// 初期姿勢でのworld座標系での変換行列
	D3DXMATRIX matInitialInvWorld;	// 初期姿勢でのworld座標系での変換行列の逆行列

	D3DXMATRIX matSkinningTransform;	// initial_invworld_mat にアニメーション用のworld座標系での変換行列を掛けたもの
};


struct Skeleton
{
	std::vector<Joint> m_Joint;

	SKL m_SKL;
	MPK m_MPK;

	FacialPoseArray* m_pFacialPoseArray;

public:

	bool LoadSKL(File* pFile)
	{
		if( ! m_SKL.Load( pFile ) ) return false;

		m_Joint.resize( m_SKL.joint.size() );

		for(int i=0; i<m_Joint.size(); i++)
		{
			m_Joint[i].parentJoint = m_SKL.joint[i].parentJoint;
		}
		return true;
	}

	bool LoadInitialNUM(File* pFile)
	{
		NUM num;

		if( ! num.Load(pFile) ) return false;

		for(int i=0; i<m_Joint.size(); i++)
		{
			m_Joint[i].posX = num.key[0].ch[i*3  ].data1 / 1000.0f;
			m_Joint[i].posY = num.key[0].ch[i*3  ].data2 / 1000.0f;
			m_Joint[i].posZ = num.key[0].ch[i*3  ].data3 / 1000.0f;
			m_Joint[i].rotX = num.key[0].ch[i*3+1].data1 / 32768.0f * D3DX_PI;
			m_Joint[i].rotY = num.key[0].ch[i*3+1].data2 / 32768.0f * D3DX_PI;
			m_Joint[i].rotZ = num.key[0].ch[i*3+1].data3 / 32768.0f * D3DX_PI;
			m_Joint[i].scaleX = num.key[0].ch[i*3+2].data1 / 32768.0f * 2.0f;
			m_Joint[i].scaleY = num.key[0].ch[i*3+2].data2 / 32768.0f * 2.0f;
			m_Joint[i].scaleZ = num.key[0].ch[i*3+2].data3 / 32768.0f * 2.0f;
		}

		// 各Jointでの行列を計算
		CalculateWorldMatrix();

		// 初期状態をコピーして保存しておく
		for(int i=0; i<m_Joint.size(); i++){
			m_Joint[i].matInitialWorld = m_Joint[i].matWorld;
		}		

		// initialのworldの逆行列を計算（initialで必要な情報はこの行列だけ）
		for(int i=0; i<m_Joint.size(); i++){
			D3DXMatrixInverse(& m_Joint[i].matInitialInvWorld, NULL, & m_Joint[i].matInitialWorld);
		}

		return true;	
	}

private:
	void CalculateWorldMatrix()
	{
		// 各Jointでのlocalな行列を計算
		for(int i=0; i<m_Joint.size(); i++){

			D3DXMATRIX trans_mat;
			D3DXMatrixTranslation(&trans_mat, m_Joint[i].posX, m_Joint[i].posY, m_Joint[i].posZ );
			D3DXMATRIX rotX_mat;
			D3DXMatrixRotationX(&rotX_mat, m_Joint[i].rotX);
			D3DXMATRIX rotY_mat;
			D3DXMatrixRotationY(&rotY_mat, m_Joint[i].rotY);
			D3DXMATRIX rotZ_mat;
			D3DXMatrixRotationZ(&rotZ_mat, m_Joint[i].rotZ);
			D3DXMATRIX scale_mat;
			D3DXMatrixScaling(&scale_mat, m_Joint[i].scaleX, m_Joint[i].scaleY, m_Joint[i].scaleZ );

			m_Joint[i].matLocal = rotX_mat * rotY_mat * rotZ_mat * scale_mat * trans_mat;
		}

		// 親をたどって行列を合成
		// 本当は木構造にした方が計算量は減る
		for(int i=0; i<m_Joint.size(); i++){
			int current_node;
			current_node = i;
			D3DXMATRIX matTemp = m_Joint[current_node].matLocal;

 			while( m_Joint[current_node].parentJoint != -1){
				current_node = m_Joint[current_node].parentJoint;
				matTemp = matTemp * m_Joint[current_node].matLocal; 
			}

			m_Joint[i].matWorld = matTemp;
		}
	}

public:
	void UpdateSkinningMatrix()
	{
		// 再計算
		CalculateWorldMatrix();

		// initialの逆行列と掛けて、スキニングに使える形にしておく
		for(int i=0; i<m_Joint.size(); i++)
		{
			m_Joint[i].matSkinningTransform = m_Joint[i].matInitialInvWorld * m_Joint[i].matWorld;
		}
	}

public:
	void AttachFacialPoseArray( FacialPoseArray* pFacialArray )
	{
		m_pFacialPoseArray = pFacialArray;
	}

	void UpdateBody( Pose& body_pose )
	{
		// 姿勢の登録
		m_Joint[2].posX = body_pose.position.x;
		m_Joint[2].posY = body_pose.position.y;
		m_Joint[2].posZ = body_pose.position.z;

		for(int i=0; i< body_pose.joint_pose.size(); i++)
		{
			m_Joint[i].rotX = body_pose.joint_pose[i].rotX;
			m_Joint[i].rotY = body_pose.joint_pose[i].rotY;
			m_Joint[i].rotZ = body_pose.joint_pose[i].rotZ;
		}
	}

	void UpdateFaceEmotion( FacialPose& facialpose )
	{
		// 全体
		for(int i=83; i<=95; i++)
		{
			m_Joint[i].posX = facialpose.pose[i].posX;
			m_Joint[i].posY = facialpose.pose[i].posY;
			m_Joint[i].posZ = facialpose.pose[i].posZ;

			m_Joint[i].rotX = facialpose.pose[i].rotX;
			m_Joint[i].rotY = facialpose.pose[i].rotY;
			m_Joint[i].rotZ = facialpose.pose[i].rotZ;
		}
		{
			int i=96;
			m_Joint[i].posX = facialpose.pose[i].posX;
			m_Joint[i].posY = facialpose.pose[i].posY;
			m_Joint[i].posZ = facialpose.pose[i].posZ;

			m_Joint[i].rotX = facialpose.pose[i].rotX;
			m_Joint[i].rotY = facialpose.pose[i].rotY;
			m_Joint[i].rotZ = facialpose.pose[i].rotZ;
		}
		{
			int i=97;
			m_Joint[i].posX = facialpose.pose[i].posX;
			m_Joint[i].posY = facialpose.pose[i].posY;
			m_Joint[i].posZ = facialpose.pose[i].posZ;

			m_Joint[i].rotX = facialpose.pose[i].rotX;
			m_Joint[i].rotY = facialpose.pose[i].rotY;
			m_Joint[i].rotZ = facialpose.pose[i].rotZ;

			m_Joint[i].scaleX = facialpose.pose[i].scaleX;
			m_Joint[i].scaleY = facialpose.pose[i].scaleY;
			m_Joint[i].scaleZ = facialpose.pose[i].scaleZ;
		}
		{
			int i=98;
			m_Joint[i].posX = facialpose.pose[i].posX;
			m_Joint[i].posY = facialpose.pose[i].posY;
			m_Joint[i].posZ = facialpose.pose[i].posZ;

			m_Joint[i].rotX = facialpose.pose[i].rotX;
			m_Joint[i].rotY = facialpose.pose[i].rotY;
			m_Joint[i].rotZ = facialpose.pose[i].rotZ;
		}
		{
			int i=99;
			m_Joint[i].posX = facialpose.pose[i].posX;
			m_Joint[i].posY = facialpose.pose[i].posY;
			m_Joint[i].posZ = facialpose.pose[i].posZ;

			m_Joint[i].rotX = facialpose.pose[i].rotX;
			m_Joint[i].rotY = facialpose.pose[i].rotY;
			m_Joint[i].rotZ = facialpose.pose[i].rotZ;

			m_Joint[i].scaleX = facialpose.pose[i].scaleX;
			m_Joint[i].scaleY = facialpose.pose[i].scaleY;
			m_Joint[i].scaleZ = facialpose.pose[i].scaleZ;
		}
		for(int i=100; i<=135; i++)
		{
			m_Joint[i].posX = facialpose.pose[i].posX;
			m_Joint[i].posY = facialpose.pose[i].posY;
			m_Joint[i].posZ = facialpose.pose[i].posZ;

			m_Joint[i].rotX = facialpose.pose[i].rotX;
			m_Joint[i].rotY = facialpose.pose[i].rotY;
			m_Joint[i].rotZ = facialpose.pose[i].rotZ;
		}
		// eye
		for(int i=100; i< 122; i++)
		{
			m_Joint[i].posX = facialpose.pose[i].posX;
			m_Joint[i].posY = facialpose.pose[i].posY;
			m_Joint[i].posZ = facialpose.pose[i].posZ;
		}
		// mouth
		for(int i=122; i<136; i++)
		{
			m_Joint[i].posX = facialpose.pose[i].posX;
			m_Joint[i].posY = facialpose.pose[i].posY;
			m_Joint[i].posZ = facialpose.pose[i].posZ;
		}
	}

	void UpdateFaceEmotion( int ID )
	{
		UpdateFaceEmotion( m_pFacialPoseArray->facial_pose[ID] );
	}

	void UpdateFaceEmotionLERP( int faceID1, int faceID2, float t )	// t: 0.0-1.0
	{
		FacialPose facialpose;
		facialpose = FacialPoseLerp( m_pFacialPoseArray->facial_pose[ faceID1 ],  m_pFacialPoseArray->facial_pose[ faceID2 ], t );
		UpdateFaceEmotion( facialpose );
	}

	void UpdateLip( LipPose& lip_pose )
	{
		D3DXVECTOR2 voice;
		voice.x = lip_pose.x;
		voice.y = lip_pose.y;

		for(int i=0; i<14; i++)
		{
			D3DXVECTOR3 mouthN, mouthA, mouthI, mouthU, mouthE, mouthO;

			mouthN.x = m_pFacialPoseArray->facial_pose[1].pose[i+122].posX;
			mouthN.y = m_pFacialPoseArray->facial_pose[1].pose[i+122].posY;
			mouthN.z = m_pFacialPoseArray->facial_pose[1].pose[i+122].posZ;

			mouthA.x = m_pFacialPoseArray->facial_pose[2].pose[i+122].posX;
			mouthA.y = m_pFacialPoseArray->facial_pose[2].pose[i+122].posY;
			mouthA.z = m_pFacialPoseArray->facial_pose[2].pose[i+122].posZ;

			mouthI.x = m_pFacialPoseArray->facial_pose[3].pose[i+122].posX;
			mouthI.y = m_pFacialPoseArray->facial_pose[3].pose[i+122].posY;
			mouthI.z = m_pFacialPoseArray->facial_pose[3].pose[i+122].posZ;

			mouthU.x = m_pFacialPoseArray->facial_pose[4].pose[i+122].posX;
			mouthU.y = m_pFacialPoseArray->facial_pose[4].pose[i+122].posY;
			mouthU.z = m_pFacialPoseArray->facial_pose[4].pose[i+122].posZ;

			mouthE.x = m_pFacialPoseArray->facial_pose[5].pose[i+122].posX;
			mouthE.y = m_pFacialPoseArray->facial_pose[5].pose[i+122].posY;
			mouthE.z = m_pFacialPoseArray->facial_pose[5].pose[i+122].posZ;

			mouthO.x = m_pFacialPoseArray->facial_pose[6].pose[i+122].posX;
			mouthO.y = m_pFacialPoseArray->facial_pose[6].pose[i+122].posY;
			mouthO.z = m_pFacialPoseArray->facial_pose[6].pose[i+122].posZ;

			float vo;
			if(voice.x == 0.0f && voice.y == 0.0f)
				vo = 0.0f;
			else
				vo = atan2( voice.x, voice.y );

			float const voA = atan2(0.8f, 0.2f);	const D3DXVECTOR2 voiceA = D3DXVECTOR2( 0.8f, 0.2f );
			float const voE = atan2(0.5f, 0.0f);	const D3DXVECTOR2 voiceE = D3DXVECTOR2( 0.5f, 0.0f );
			float const voI = atan2(0.25f, 0.0f);	const D3DXVECTOR2 voiceI = D3DXVECTOR2( 0.25f, 0.0f );
			float const voO = atan2(0.6f, 0.8f);	const D3DXVECTOR2 voiceO = D3DXVECTOR2( 0.6f, 0.8f );
			float const voU = atan2(0.32f, 0.9f);	const D3DXVECTOR2 voiceU = D3DXVECTOR2( 0.32f, 0.9f );
													const D3DXVECTOR2 voiceN = D3DXVECTOR2( 0.0f, 0.0f );

			float num = max( 0.0f, 1.0f - abs(voA - vo));

			D3DXVECTOR2 vector = voiceI - voice;
			float num2 = max( 0.0f, 1.0f - abs(voI-vo)) * ( 1.0f - D3DXVec2Length(&vector) );

			float num3 = max( 0.0f, 1.0f - abs(voU-vo) );

			D3DXVECTOR2 vector2 = voiceE - voice;
			float num4 = max( 0.0f, 1.0f - abs(voE-vo)) * ( 1.0f - D3DXVec2Length(&vector2) );

			float num5 = max( 0.0f, 1.0f - abs(voO-vo) );

			D3DXVECTOR2 vector3 = voiceN - voice;
			float num6 = max( 0.0f, 1.0f - D3DXVec2Length(&vector3) );

			// interpolate
			m_Joint[i+122].posX = ((mouthI.x * num2 + mouthU.x * num3 + mouthE.x * num4 + mouthO.x * num5)/(num2+num3+num4+num5) * (1.0f-num) + mouthA.x * num) * (1.0f-num6) + mouthN.x * num6;
			m_Joint[i+122].posY = ((mouthI.y * num2 + mouthU.y * num3 + mouthE.y * num4 + mouthO.y * num5)/(num2+num3+num4+num5) * (1.0f-num) + mouthA.y * num) * (1.0f-num6) + mouthN.y * num6;
			m_Joint[i+122].posZ = ((mouthI.z * num2 + mouthU.z * num3 + mouthE.z * num4 + mouthO.z * num5)/(num2+num3+num4+num5) * (1.0f-num) + mouthA.z * num) * (1.0f-num6) + mouthN.z * num6;
		}
	}

	void CloseLip()
	{
		for(int i=0; i<14; i++)
		{
			D3DXVECTOR3 mouthN;

			mouthN.x = m_pFacialPoseArray->facial_pose[1].pose[i+122].posX;
			mouthN.y = m_pFacialPoseArray->facial_pose[1].pose[i+122].posY;
			mouthN.z = m_pFacialPoseArray->facial_pose[1].pose[i+122].posZ;

			m_Joint[i+122].posX = m_pFacialPoseArray->facial_pose[1].pose[i+122].posX;
			m_Joint[i+122].posY = m_pFacialPoseArray->facial_pose[1].pose[i+122].posY;
			m_Joint[i+122].posZ = m_pFacialPoseArray->facial_pose[1].pose[i+122].posZ;
		}
	}


	void Render()
	{
		glPointSize(2);

		glBegin(GL_POINTS);
		for(int i=0; i<m_Joint.size(); i++){
			if(i== 59 || i==60 || i==66 || i==67 || i== 74 || i==75 || i==79 || i==80) continue;

			glVertex3f(
				m_Joint[i].matWorld._41,
				m_Joint[i].matWorld._42,
				m_Joint[i].matWorld._43
			);
		}
		glEnd();

		for(int i=0; i<m_Joint.size(); i++){
			if(i== 59 || i==60 || i==66 || i==67 || i== 74 || i==75 || i==79 || i==80) continue;

			int parent = m_Joint[i].parentJoint;

			if( parent != -1){
				glBegin(GL_LINES);
					glVertex3f(
						m_Joint[i].matWorld._41,
						m_Joint[i].matWorld._42,
						m_Joint[i].matWorld._43
					);
					glVertex3f(
						m_Joint[parent].matWorld._41,
						m_Joint[parent].matWorld._42,
						m_Joint[parent].matWorld._43
					);
				glEnd();
			}
		}
	}

};

}	// namespace

#endif