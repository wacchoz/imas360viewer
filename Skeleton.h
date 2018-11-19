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

	D3DXVECTOR3 translate;
	D3DXQUATERNION q;	// 回転はquaternion
	D3DXVECTOR3 scale;

	D3DXMATRIX matLocal;
	D3DXMATRIX matWorld;

	D3DXMATRIX matInitialWorld;		// 初期姿勢でのworld座標系での変換行列
	D3DXMATRIX matInitialInvWorld;	// 初期姿勢でのworld座標系での変換行列の逆行列

	D3DXMATRIX matSkinningTransform;	// initial_invworld_mat にアニメーション用のworld座標系での変換行列を掛けたもの
	
	// 以下は物理演算用（チェーンのみで使用）
	D3DXVECTOR3 pos;
	D3DXVECTOR3 prev_pos;
	D3DXVECTOR3 velocity;
	D3DXQUATERNION initial_q;
	float length_to_parent;		// 初期状態でのボーン長さ
	bool bCollisionSphere;
	bool bCollisionCylinder;
	D3DXVECTOR3 debug;
};


struct Skeleton
{
	std::vector<Joint> m_Joint;

	SKL m_SKL;
	MPK m_MPK;

	FacialPoseArray* m_pFacialPoseArray;
	
	float previous_frame;	// 前回のフレーム（物理演算用）

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
			m_Joint[i].translate.x = num.key[0].ch[i*3  ].data1 / 1000.0f;
			m_Joint[i].translate.y = num.key[0].ch[i*3  ].data2 / 1000.0f;
			m_Joint[i].translate.z = num.key[0].ch[i*3  ].data3 / 1000.0f;
			float rotX = num.key[0].ch[i*3+1].data1 / 32768.0f * D3DX_PI;
			float rotY = num.key[0].ch[i*3+1].data2 / 32768.0f * D3DX_PI;
			float rotZ = num.key[0].ch[i*3+1].data3 / 32768.0f * D3DX_PI;
			m_Joint[i].scale.x = num.key[0].ch[i*3+2].data1 / 32768.0f * 2.0f;
			m_Joint[i].scale.y = num.key[0].ch[i*3+2].data2 / 32768.0f * 2.0f;
			m_Joint[i].scale.z = num.key[0].ch[i*3+2].data3 / 32768.0f * 2.0f;

			// rotのquaternion計算
			D3DXMATRIX rotX_mat, rotY_mat, rotZ_mat, tmp_M;
			D3DXMatrixRotationX(&rotX_mat, rotX);
			D3DXMatrixRotationY(&rotY_mat, rotY);
			D3DXMatrixRotationZ(&rotZ_mat, rotZ);
			tmp_M = rotX_mat * rotY_mat * rotZ_mat;
			D3DXQuaternionRotationMatrix( &m_Joint[i].q, &tmp_M);

			// 初期回転を保存（物理演算用）
			m_Joint[i].initial_q = m_Joint[i].q;
		}

		// 各Jointでの行列を計算
		CalculateWorldMatrix();

		// 初期状態をコピーして保存しておく
		for(int i=0; i<m_Joint.size(); i++){
			m_Joint[i].matInitialWorld = m_Joint[i].matWorld;
		}		

		// initialのworldの逆行列を計算（スキニングでinitialで必要な情報はこの行列だけ）
		for(int i=0; i<m_Joint.size(); i++){
			D3DXMatrixInverse(& m_Joint[i].matInitialInvWorld, NULL, & m_Joint[i].matInitialWorld);
		}

		for(int i=0; i<m_Joint.size(); i++){
			
			int parentID = m_Joint[i].parentJoint;
			if( parentID == -1 ) continue;

			D3DXVECTOR3 parent(m_Joint[parentID].matInitialWorld._41, m_Joint[parentID].matInitialWorld._42, m_Joint[parentID].matInitialWorld._43 ); 
			D3DXVECTOR3 self(m_Joint[i].matInitialWorld._41, m_Joint[i].matInitialWorld._42, m_Joint[i].matInitialWorld._43 ); 
			D3DXVECTOR3 diff = parent-self;

			m_Joint[i].length_to_parent = D3DXVec3Length( &diff );
		}

		return true;	
	}

public:
	void CalculateWorldMatrix()
	{
		// 各Jointでのlocalな行列を計算
		for(int i=0; i<m_Joint.size(); i++)
		{
			D3DXMATRIX trans_mat;
			D3DXMatrixTranslation(&trans_mat, m_Joint[i].translate.x, m_Joint[i].translate.y, m_Joint[i].translate.z );
			D3DXMATRIX rotation;
			D3DXMatrixRotationQuaternion( &rotation, &m_Joint[i].q);
			D3DXMATRIX scale_mat;
			D3DXMatrixScaling(&scale_mat, m_Joint[i].scale.x, m_Joint[i].scale.y, m_Joint[i].scale.z );

			m_Joint[i].matLocal = rotation * scale_mat * trans_mat;
		}

		// 親をたどって行列を合成
		// 本当は木構造にした方が計算量は減る
		for(int i=0; i<m_Joint.size(); i++){
			int current_node;
			current_node = i;
			D3DXMATRIX matTemp = m_Joint[current_node].matLocal;

 			while( m_Joint[current_node].parentJoint != -1)
			{
				current_node = m_Joint[current_node].parentJoint;
				matTemp = matTemp * m_Joint[current_node].matLocal; 
			}

			m_Joint[i].matWorld = matTemp;
		}
	}

private:
	// １個だけ計算（IDだけ更新されたという前提）
	void CalculateWorldMatrixForJoint( int ID )
	{
		D3DXMATRIX trans_mat;
		D3DXMatrixTranslation(&trans_mat, m_Joint[ID].translate.x, m_Joint[ID].translate.y, m_Joint[ID].translate.z );
		D3DXMATRIX rotation;
		D3DXMatrixRotationQuaternion( &rotation, &m_Joint[ID].q);
		D3DXMATRIX scale_mat;
		D3DXMatrixScaling(&scale_mat, m_Joint[ID].scale.x, m_Joint[ID].scale.y, m_Joint[ID].scale.z );

		m_Joint[ID].matLocal = rotation * scale_mat * trans_mat;

		int current_node;
		current_node = ID;
		D3DXMATRIX matTemp = m_Joint[current_node].matLocal;

 		while( m_Joint[current_node].parentJoint != -1)
		{
			current_node = m_Joint[current_node].parentJoint;
			matTemp = matTemp * m_Joint[current_node].matLocal; 
		}

		m_Joint[ID].matWorld = matTemp;
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

	void UpdateBody( BodyPose& body_pose )
	{
		// 姿勢の登録
		m_Joint[2].translate = body_pose.position;

		for(int i=0; i< body_pose.joint_pose.size(); i++)
		{
			m_Joint[i].q = body_pose.joint_pose[i].q;
		}
	}

	void UpdateFaceEmotion( FacialPose& facialpose )
	{
		// 全体
		for(int i=83; i<=95; i++)
		{
			m_Joint[i].translate = facialpose.joint_pose[i].translate;
			m_Joint[i].q = facialpose.joint_pose[i].q;

		}
		{
			int i=96;
			m_Joint[i].translate = facialpose.joint_pose[i].translate;
			m_Joint[i].q = facialpose.joint_pose[i].q;
		}
		{
			int i=97;
			m_Joint[i].translate = facialpose.joint_pose[i].translate;
			m_Joint[i].q = facialpose.joint_pose[i].q;
			m_Joint[i].scale = facialpose.joint_pose[i].scale;
		}
		{
			int i=98;
			m_Joint[i].translate = facialpose.joint_pose[i].translate;
			m_Joint[i].q = facialpose.joint_pose[i].q;
		}
		{
			int i=99;
			m_Joint[i].translate = facialpose.joint_pose[i].translate;
			m_Joint[i].q = facialpose.joint_pose[i].q;
			m_Joint[i].scale = facialpose.joint_pose[i].scale;
		}
		for(int i=100; i<=135; i++)
		{
			m_Joint[i].translate = facialpose.joint_pose[i].translate;
			m_Joint[i].q = facialpose.joint_pose[i].q;
		}
		// eye
		for(int i=100; i< 122; i++)
		{
			m_Joint[i].translate = facialpose.joint_pose[i].translate;
		}
		// mouth
		for(int i=122; i<136; i++)
		{
			m_Joint[i].translate = facialpose.joint_pose[i].translate;
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
		D3DXVECTOR2& voice = lip_pose.v;

		for(int i=0; i<14; i++)
		{
			D3DXVECTOR3 mouthN, mouthA, mouthI, mouthU, mouthE, mouthO;

			mouthN = m_pFacialPoseArray->facial_pose[1].joint_pose[i+122].translate;
			mouthA = m_pFacialPoseArray->facial_pose[2].joint_pose[i+122].translate;
			mouthI = m_pFacialPoseArray->facial_pose[3].joint_pose[i+122].translate;
			mouthU = m_pFacialPoseArray->facial_pose[4].joint_pose[i+122].translate;
			mouthE = m_pFacialPoseArray->facial_pose[5].joint_pose[i+122].translate;
			mouthO = m_pFacialPoseArray->facial_pose[6].joint_pose[i+122].translate;

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

			float tA = max( 0.0f, 1.0f - abs(voA - vo));

			D3DXVECTOR2 vector = voiceI - voice;
			float tI = max( 0.0f, 1.0f - abs(voI-vo)) * ( 1.0f - D3DXVec2Length(&vector) );

			float tU = max( 0.0f, 1.0f - abs(voU-vo) );

			D3DXVECTOR2 vector2 = voiceE - voice;
			float tE = max( 0.0f, 1.0f - abs(voE-vo)) * ( 1.0f - D3DXVec2Length(&vector2) );

			float tO = max( 0.0f, 1.0f - abs(voO-vo) );

			D3DXVECTOR2 vector3 = voiceN - voice;
			float tN = max( 0.0f, 1.0f - D3DXVec2Length(&vector3) );

			// interpolate
			m_Joint[i+122].translate = ((mouthI * tI + mouthU * tU + mouthE * tE + mouthO * tO)/(tI+tU+tE+tO) * (1.0f-tA) + mouthA * tA) * (1.0f-tN) + mouthN * tN;
		}
	}

	void CloseLip()
	{
		for(int i=0; i<14; i++)
		{
			D3DXVECTOR3 mouthN;

			mouthN = m_pFacialPoseArray->facial_pose[1].joint_pose[i+122].translate;

			m_Joint[i+122].translate = mouthN;
		}
	}

	void OpenLip()
	{
		for(int i=0; i<14; i++)
		{
			D3DXVECTOR3 mouthO;

			mouthO = m_pFacialPoseArray->facial_pose[6].joint_pose[i+122].translate;

			m_Joint[i+122].translate = mouthO;
		}
	}


	void InitPhysics()
	{
		for( int i=0; i<m_Joint.size(); i++)
		{
			m_Joint[i].prev_pos = D3DXVECTOR3( m_Joint[i].matWorld._41, m_Joint[i].matWorld._42, m_Joint[i].matWorld._43 );
			m_Joint[i].velocity = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

			m_Joint[i].bCollisionSphere = false;
			m_Joint[i].bCollisionCylinder = false;
		}

		previous_frame = -1.0f;

	}

	void DisablePhysics()
	{
		// ユレモノの回転を初期値に戻す
		for(int i=0; i<m_SKL.yur2.rig_list.size(); i++)
		for(int j=0; j<m_SKL.yur2.rig_list[i].chain_list.size(); j++)
		{
				CHAINS & chain = m_SKL.yur2.rig_list[i].chain_list[j];

				for( int nodeNo=0; nodeNo<chain.chain_joint_data_list.size(); nodeNo++)
				{
					int boneID = chain.chain_joint_data_list[ nodeNo ].bone_ID;
					m_Joint[boneID].q = m_Joint[boneID].initial_q;
				}
		}
	}

	void ApplyPhysics(float frame)
	{
		float delta_t = frame - previous_frame;
		if( delta_t < 2.0f ) return;	// 頻繁に呼び出すと物理演算がおかしくなる（計算精度のせい？）
		
		// 衝突初期化
		for(int i = 0; i < m_Joint.size(); i++)
		{
			m_Joint[ i ].bCollisionCylinder = false;
			m_Joint[ i ].bCollisionSphere = false;
		}


		for(int i = 0; i < m_SKL.yur2.rig_list.size(); i++)
		{
			RIG & rig = m_SKL.yur2.rig_list[i];

			for(int j = 0; j < rig.chain_list.size(); j++)
			{
				CHAINS & chain = rig.chain_list[j];

				int rootParent = m_Joint [ chain.chain_joint_data_list[ 0 ].bone_ID ].parentJoint;
				int root = chain.chain_joint_data_list[ 0 ].bone_ID;

				// rootParentはボーンアニメーションによる更新
				m_Joint[ rootParent ].pos = D3DXVECTOR3( m_Joint[rootParent].matWorld._41, m_Joint[rootParent].matWorld._42, m_Joint[rootParent].matWorld._43 );
				// rootも位置はボーンアニメーションにより更新される（回転は物理演算による）
				m_Joint[ root ].pos = D3DXVECTOR3( m_Joint[root].matWorld._41, m_Joint[root].matWorld._42, m_Joint[root].matWorld._43 );

				// 第１段階（速度減衰＋重力）
				for( int nodeNo=1; nodeNo<chain.chain_joint_data_list.size(); nodeNo++)
				{
					int parentID = m_Joint[ chain.chain_joint_data_list[ nodeNo].bone_ID ].parentJoint;
					int boneID = chain.chain_joint_data_list[ nodeNo ].bone_ID;

					float damping = chain.chain_joint_data_list[nodeNo].damping;
					float spring = chain.chain_joint_data_list[nodeNo].spring;

					// 速度減衰
					m_Joint[boneID].velocity *= exp( - damping * delta_t * 2.0 );
					// 速度分移動
					m_Joint[boneID].pos = m_Joint[boneID].prev_pos + m_Joint[boneID].velocity * delta_t;
					// 重力（物理的には間違いだが、こっちっぽい）
					m_Joint[boneID].pos.y -= 0.03f * delta_t;
				}

				// 第２段階（ボーン長さを戻す）
				for( int nodeNo=1; nodeNo<chain.chain_joint_data_list.size(); nodeNo++)
				{
					int parentID = m_Joint[ chain.chain_joint_data_list[ nodeNo].bone_ID ].parentJoint;
					int boneID = chain.chain_joint_data_list[ nodeNo ].bone_ID;

					D3DXVECTOR3 dir = m_Joint[boneID].pos - m_Joint[parentID].pos;
					D3DXVec3Normalize( &dir, &dir);
					D3DXVECTOR3 parent_pos = m_Joint[parentID].pos;
					m_Joint[boneID].pos = parent_pos + dir * m_Joint[boneID].length_to_parent;
				}

				// 第３段階（行列計算；　posに合うように現在のqを更新、さらに元の位置に戻すバネ計算）
				for( int nodeNo=1; nodeNo<(int)chain.chain_joint_data_list.size(); nodeNo++)
				{
					int parentID = m_Joint[ chain.chain_joint_data_list[ nodeNo].bone_ID ].parentJoint;
					int boneID = chain.chain_joint_data_list[ nodeNo ].bone_ID;

					// 初期座標を親の初期座標系に移す
					D3DXVECTOR3 initial_rel, rel;
					D3DXVECTOR3 initial_position = D3DXVECTOR3( m_Joint[boneID].matInitialWorld._41,  m_Joint[boneID].matInitialWorld._42,  m_Joint[boneID].matInitialWorld._43);
					D3DXVec3TransformCoord( &initial_rel, &initial_position, & m_Joint[ parentID ].matInitialInvWorld );

					// 現在座標を親の現在座標系に移す
					D3DXMATRIX mat_invParent;
					D3DXMatrixInverse( & mat_invParent, NULL, &m_Joint[ parentID ].matWorld);
					D3DXVec3TransformCoord( &rel, &m_Joint[boneID].pos, & mat_invParent );

					// initial_relからrelへの回転を計算
					D3DXVec3Normalize( &rel, &rel);
					D3DXVec3Normalize( &initial_rel, &initial_rel);

					D3DXVECTOR3 axis;
					D3DXVec3Cross( & axis, & initial_rel, & rel);
					D3DXVec3Normalize( &axis, &axis);
					float dot = D3DXVec3Dot(&initial_rel, &rel);
					if( dot > 1.0f ) dot = 1.0f;
					else if( dot < -1.0f ) dot = -1.0f;
					float theta = acos( dot );

					D3DXQUATERNION q;
					D3DXQuaternionRotationAxis( &q, &axis, theta );
					m_Joint[parentID].q = q * m_Joint[parentID].q;			// これはinitial_qに掛けないのか？
					D3DXQuaternionNormalize( &m_Joint[parentID].q, &m_Joint[parentID].q);

					// 角度バネ（おそらく実機の実装とは違う）
					float spring = chain.chain_joint_data_list[nodeNo].spring;
					D3DXQuaternionSlerp(&m_Joint[parentID].q, &m_Joint[parentID].initial_q, & m_Joint[parentID].q, exp( - spring * delta_t * 2.5f/*/ m_Joint[boneID].length_to_parent * D3DX_PI */ )  );

					// parentIDとboneIDのworld更新
					CalculateWorldMatrixForJoint( parentID );
					CalculateWorldMatrixForJoint( boneID );

					m_Joint[ boneID ].pos = D3DXVECTOR3( m_Joint[boneID].matWorld._41, m_Joint[boneID].matWorld._42, m_Joint[boneID].matWorld._43 );
				}



				// 第４段階（衝突計算）
				for( int nodeNo = 1; nodeNo < chain.chain_joint_data_list.size(); nodeNo++)
				{
					int boneID = chain.chain_joint_data_list[nodeNo].bone_ID;
					D3DXVECTOR3& chain_pos = m_Joint[ boneID ].pos;
					float chain_radius = chain.chain_joint_data_list[nodeNo].radius;

					// Sphere Collision Detect
					for(int s = 0; s < rig.sphere_list.size(); s++ )
					{
						BoundingSphere& sphere = rig.sphere_list[s];

						D3DXVECTOR3 sphere_pos;
						D3DXVECTOR3& translate = sphere.pos;
						D3DXMATRIX& matworld = m_Joint[ sphere.bone_ID ].matWorld;

						sphere_pos.x = matworld._11 * translate.x + matworld._21 * translate.y + matworld._31 * translate.z + matworld._41;
						sphere_pos.y = matworld._12 * translate.x + matworld._22 * translate.y + matworld._32 * translate.z + matworld._42;
						sphere_pos.z = matworld._13 * translate.x + matworld._23 * translate.y + matworld._33 * translate.z + matworld._43;

						float distance = sqrt( (sphere_pos.x -  chain_pos.x) * (sphere_pos.x -  chain_pos.x) 
												+ (sphere_pos.y -  chain_pos.y) * (sphere_pos.y -  chain_pos.y)
												+ (sphere_pos.z -  chain_pos.z) * (sphere_pos.z -  chain_pos.z));

						if( distance < sphere.radius + chain_radius )
						{
							D3DXVECTOR3 target = sphere_pos + ( chain_pos - sphere_pos ) / distance * (sphere.radius + chain_radius);
							chain_pos = target;// + ( chain_pos - target ) * exp( - delta_t );


							// めりこみ方向の速度をゼロにする？
							
//							m_Joint[ boneID ].velocity = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 

							m_Joint[ boneID ].bCollisionSphere = true;
						}
					}

					// Cylinder Collision Detect
					for(int c = 0; c < rig.cylinder_list.size(); c++)
					{
						BoundingCylinder& cylinder = rig.cylinder_list[c];

						D3DXVECTOR3 X1;
						D3DXVECTOR3 X2;
						float radius1 = cylinder.radius1;
						float radius2 = cylinder.radius2;

						D3DXVECTOR3& p1 = cylinder.pos1;
						D3DXVECTOR3& p2 = cylinder.pos2;
						D3DXMATRIX& matworld1 = m_Joint[ cylinder.ID1 ].matWorld;
						D3DXMATRIX& matworld2 = m_Joint[ cylinder.ID2 ].matWorld;

						X1.x = matworld1._11 * p1.x + matworld1._21 * p1.y + matworld1._31 * p1.z + matworld1._41;
						X1.y = matworld1._12 * p1.x + matworld1._22 * p1.y + matworld1._32 * p1.z + matworld1._42;
						X1.z = matworld1._13 * p1.x + matworld1._23 * p1.y + matworld1._33 * p1.z + matworld1._43;

						X2.x = matworld2._11 * p2.x + matworld2._21 * p2.y + matworld2._31 * p2.z + matworld2._41;
						X2.y = matworld2._12 * p2.x + matworld2._22 * p2.y + matworld2._32 * p2.z + matworld2._42;
						X2.z = matworld2._13 * p2.x + matworld2._23 * p2.y + matworld2._33 * p2.z + matworld2._43;

						D3DXVECTOR3 v = X2 - X1;	// 主軸
						float length = D3DXVec3Length( &v );
						D3DXVec3Normalize( & v, & v );

						float tan_theta = (radius2 - radius1) / length;
						float R1, R2, y1, y2;
						y1 = radius1 * tan_theta;
						y2 = radius2 * tan_theta;
						R1 = sqrt( y1 * y1 + radius1 * radius1 );
						R2 = sqrt( y2 * y2 + radius2 * radius2 );

						const int MAX = 10;

						for(int sect= 0; sect < MAX; sect++)
						{
							float a = y1 + (y2-y1+length)/MAX * sect;
							D3DXVECTOR3 C = (( length - a ) * X1 + a * X2)/length;
							float r = R1 + (R2 - R1 ) / MAX * sect;

							// 中心C, 半径rの球
							float distance = sqrt( (C.x -  chain_pos.x) * (C.x -  chain_pos.x) 
													+ (C.y -  chain_pos.y) * (C.y -  chain_pos.y)
													+ (C.z -  chain_pos.z) * (C.z -  chain_pos.z));

							if( distance <r + chain_radius )
							{
								D3DXVECTOR3 target = C + ( chain_pos - C ) / distance * (r + chain_radius + 0.05f);
								chain_pos = target;// + ( chain_pos - target ) * exp( - delta_t );

								m_Joint[ boneID ].bCollisionCylinder = true;
							}
						}
					}
				}

				// 第５段階（ボーン長さを戻す）
				for( int nodeNo=1; nodeNo<chain.chain_joint_data_list.size(); nodeNo++)
				{
					int parentID = m_Joint[ chain.chain_joint_data_list[ nodeNo].bone_ID ].parentJoint;
					int boneID = chain.chain_joint_data_list[ nodeNo ].bone_ID;

					D3DXVECTOR3 dir = m_Joint[boneID].pos - m_Joint[parentID].pos;
					D3DXVec3Normalize( &dir, &dir);
					D3DXVECTOR3 parent_pos = m_Joint[parentID].pos;
					m_Joint[boneID].pos = parent_pos + dir * m_Joint[boneID].length_to_parent;
				}

				// 第６段階（行列計算；　posに合うように現在のqを更新）
				for( int nodeNo=1; nodeNo<(int)chain.chain_joint_data_list.size(); nodeNo++)
				{
					int parentID = m_Joint[ chain.chain_joint_data_list[ nodeNo].bone_ID ].parentJoint;
					int boneID = chain.chain_joint_data_list[ nodeNo ].bone_ID;

					// 初期座標を親の初期座標系に移す
					D3DXVECTOR3 initial_rel, rel;
					D3DXVECTOR3 initial_position = D3DXVECTOR3( m_Joint[boneID].matInitialWorld._41,  m_Joint[boneID].matInitialWorld._42,  m_Joint[boneID].matInitialWorld._43);
					D3DXVec3TransformCoord( &initial_rel, &initial_position, & m_Joint[ parentID ].matInitialInvWorld );

					// 現在座標を親の現在座標系に移す
					D3DXMATRIX mat_invParent;
					D3DXMatrixInverse( & mat_invParent, NULL, &m_Joint[ parentID ].matWorld);
					D3DXVec3TransformCoord( &rel, &m_Joint[boneID].pos, & mat_invParent );

					// initial_relからrelへの回転を計算
					D3DXVec3Normalize( &rel, &rel);
					D3DXVec3Normalize( &initial_rel, &initial_rel);

					D3DXVECTOR3 axis;
					D3DXVec3Cross( & axis, & initial_rel, & rel);
					D3DXVec3Normalize( &axis, &axis);
					float dot = D3DXVec3Dot(&initial_rel, &rel);
					if( dot > 1.0f ) dot = 1.0f;
					else if( dot < -1.0f ) dot = -1.0f;
					float theta = acos( dot );

					D3DXQUATERNION q;
					D3DXQuaternionRotationAxis( &q, &axis, theta );
					m_Joint[parentID].q = q * m_Joint[parentID].q;			// これはinitial_qに掛けないのか？
					D3DXQuaternionNormalize( &m_Joint[parentID].q, &m_Joint[parentID].q);

					// parentIDとboneIDのworld更新
					CalculateWorldMatrixForJoint( parentID );
					CalculateWorldMatrixForJoint( boneID );
				}

				// 位置・速度を更新
				for( int nodeNo=1; nodeNo<chain.chain_joint_data_list.size(); nodeNo++)
				{
					int boneID = chain.chain_joint_data_list[ nodeNo ].bone_ID;

					m_Joint[boneID].pos = D3DXVECTOR3( m_Joint[boneID].matWorld._41, m_Joint[boneID].matWorld._42, m_Joint[boneID].matWorld._43 );
					m_Joint[boneID].velocity = (m_Joint[boneID].pos - m_Joint[boneID].prev_pos) / delta_t;

					m_Joint[boneID].prev_pos = m_Joint[boneID].pos;
				}
			}
		}

		previous_frame = frame;
	}

	void Render()
	{
		glPointSize(2);

		glColor3f(0.0f,  0.0f,  1.0f);

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
				glColor3f(0.0f,  0.0f,  1.0f);
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

	void RenderRig()
	{
		// chain data
		glColor3f(1.0f,  1.0f,  0.0f);
		for(int i=0; i<m_SKL.yur2.rig_list.size(); i++)
		{
			for(int j=0; j<m_SKL.yur2.rig_list[i].chain_list.size(); j++)
			{
				for(int k=0; k<m_SKL.yur2.rig_list[i].chain_list[j].chain_joint_data_list.size(); k++)
				{
					int boneID = m_SKL.yur2.rig_list[i].chain_list[j].chain_joint_data_list[k].bone_ID;
					float radius = m_SKL.yur2.rig_list[i].chain_list[j].chain_joint_data_list[k].radius;

					if( m_Joint[ boneID ].bCollisionCylinder )
						glColor3f(1.0f,  0.0f,  0.0f);
					else if(m_Joint[ boneID ].bCollisionSphere)
						glColor3f(0.5f,  0.0f,  1.0f);
					else
						glColor3f(1.0f,  1.0f,  0.0f);

					glPushMatrix();
						glTranslatef(
							m_Joint[ boneID ].matWorld._41,
							m_Joint[ boneID ].matWorld._42,
							m_Joint[ boneID ].matWorld._43);
						glutSolidSphere( radius, 10, 10 );
					glPopMatrix();
				}
			}
		}

		// bounding sphere
		glColor3f(0.0f,  1.0f,  1.0f);
		for(int i=0; i<m_SKL.yur2.rig_list.size(); i++)
		{
			for(int j=0; j<m_SKL.yur2.rig_list[i].sphere_list.size(); j++)
			{
				D3DXVECTOR3& translate = m_SKL.yur2.rig_list[i].sphere_list[j].pos;
				D3DXMATRIX& matworld = m_Joint[ m_SKL.yur2.rig_list[i].sphere_list[j].bone_ID ].matWorld;
				float radius = m_SKL.yur2.rig_list[i].sphere_list[j].radius;

				glPushMatrix();

					glTranslatef(
						matworld._11 * translate.x + matworld._21 * translate.y + matworld._31 * translate.z + matworld._41,
						matworld._12 * translate.x + matworld._22 * translate.y + matworld._32 * translate.z + matworld._42,
						matworld._13 * translate.x + matworld._23 * translate.y + matworld._33 * translate.z + matworld._43);

					// 以下の回転行列は球を回転させる見栄えだけのため
					GLfloat m[16];
					m[ 0] = matworld._11; m[ 1] = matworld._21; m[ 2] = matworld._31; m[ 3] = 0.0f;
					m[ 4] = matworld._12; m[ 5] = matworld._22; m[ 6] = matworld._32; m[ 7] = 0.0f;
					m[ 8] = matworld._13; m[ 9] = matworld._23; m[10] = matworld._33; m[11] = 0.0f;
					m[12] = matworld._14; m[13] = matworld._24; m[14] = matworld._34; m[15] = 1.0f;
					glMultMatrixf( m );

					glutSolidSphere( radius, 16, 16 );
				glPopMatrix();
			}
		}

		// bounding cylinder
		glColor3f(1.0f,  0.0f,  1.0f);
		for(int i=0; i<m_SKL.yur2.rig_list.size(); i++)
		{
			for(int j=0; j<m_SKL.yur2.rig_list[i].cylinder_list.size(); j++)
			{
				D3DXVECTOR3& p1 = m_SKL.yur2.rig_list[i].cylinder_list[j].pos1;
				D3DXVECTOR3& p2 = m_SKL.yur2.rig_list[i].cylinder_list[j].pos2;
				D3DXMATRIX& matworld1 = m_Joint[ m_SKL.yur2.rig_list[i].cylinder_list[j].ID1 ].matWorld;
				D3DXMATRIX& matworld2 = m_Joint[ m_SKL.yur2.rig_list[i].cylinder_list[j].ID2 ].matWorld;

				float radius1 =  m_SKL.yur2.rig_list[i].cylinder_list[j].radius1;
				float radius2 =  m_SKL.yur2.rig_list[i].cylinder_list[j].radius2;

				D3DXVECTOR3 q1, q2;

				q1.x = matworld1._11 * p1.x + matworld1._21 * p1.y + matworld1._31 * p1.z + matworld1._41;
				q1.y = matworld1._12 * p1.x + matworld1._22 * p1.y + matworld1._32 * p1.z + matworld1._42;
				q1.z = matworld1._13 * p1.x + matworld1._23 * p1.y + matworld1._33 * p1.z + matworld1._43;

				q2.x = matworld2._11 * p2.x + matworld2._21 * p2.y + matworld2._31 * p2.z + matworld2._41;
				q2.y = matworld2._12 * p2.x + matworld2._22 * p2.y + matworld2._32 * p2.z + matworld2._42;
				q2.z = matworld2._13 * p2.x + matworld2._23 * p2.y + matworld2._33 * p2.z + matworld2._43;

				D3DXVECTOR3 dir = q2 - q1;	//方向ベクトル

				D3DXVECTOR3 normal1(1, 1, (-dir.x-dir.y)/dir.z);
				D3DXVec3Normalize( &normal1, &normal1);

				D3DXVECTOR3 normal2;
				D3DXVec3Cross( &normal2, &dir, &normal1);
				D3DXVec3Normalize( &normal2, &normal2);

				const int MAX = 10;
				// 上面
				glBegin(GL_TRIANGLE_FAN);
				glVertex3f( q1.x, q1.y, q1.z );
				for(int i=0; i<MAX; i++)
				{
					D3DXVECTOR3 a = q1 + radius1 * ( normal1 * cos( 2 * D3DX_PI * i / MAX ) + normal2 * sin ( 2 * D3DX_PI * i / MAX ) ); 
					glVertex3f(a.x, a.y, a.z);
				}
				glEnd();

				// 下面
				glBegin(GL_TRIANGLE_FAN);
				glVertex3f( q2.x, q2.y, q2.z );
				for(int i=0; i<MAX; i++)
				{
					D3DXVECTOR3 a = q2 + radius2 * ( normal1 * cos( 2 * D3DX_PI * i / MAX ) + normal2 * sin ( 2 * D3DX_PI * i / MAX ) ); 
					glVertex3f(a.x, a.y, a.z);
				}
				glEnd();

				// 側面
				glBegin( GL_TRIANGLE_STRIP );
				for(int i=0; i<=MAX; i++)
				{
					D3DXVECTOR3 a = q1 + radius1 * ( normal1 * cos( 2 * D3DX_PI * i / MAX ) + normal2 * sin ( 2 * D3DX_PI * i / MAX ) ); 
					D3DXVECTOR3 b = q2 + radius2 * ( normal1 * cos( 2 * D3DX_PI * i / MAX ) + normal2 * sin ( 2 * D3DX_PI * i / MAX ) ); 
					glVertex3f(a.x, a.y, a.z);
					glVertex3f(b.x, b.y, b.z);
				}
				glEnd();
			}
		}
	}

};

}	// namespace

#endif