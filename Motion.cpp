#include "Motion.h"
#include "d3dx9math.h"

using namespace imas;

// ê^ñ ñ⁄Ç…quaternionÇ≈ï‚ä‘
Pose imas::PoseLerp( Pose& pose1, Pose& pose2, float t )
{
	Pose result;

	result.joint_pose.resize( pose1.joint_pose.size() );

	result.position = (1-t) * pose1.position + t * pose2.position;

	for(int i=0; i<pose1.joint_pose.size(); i++)
	{
		result.joint_pose[i].posX = (1-t) * pose1.joint_pose[i].posX + t * pose2.joint_pose[i].posX;
		result.joint_pose[i].posY = (1-t) * pose1.joint_pose[i].posY + t * pose2.joint_pose[i].posY;
		result.joint_pose[i].posZ = (1-t) * pose1.joint_pose[i].posZ + t * pose2.joint_pose[i].posZ;

		result.joint_pose[i].scaleX = (1-t) * pose1.joint_pose[i].scaleX + t * pose2.joint_pose[i].scaleX;
		result.joint_pose[i].scaleY = (1-t) * pose1.joint_pose[i].scaleY + t * pose2.joint_pose[i].scaleY;
		result.joint_pose[i].scaleZ = (1-t) * pose1.joint_pose[i].scaleZ + t * pose2.joint_pose[i].scaleZ;


		// XYZâÒì]äpÅÀquaternion
		D3DXQUATERNION q1, q2, qlerp;
		D3DXMATRIX rotX_mat, rotY_mat, rotZ_mat, tmp_M, M;
		{
			D3DXMatrixRotationX(&rotX_mat, pose1.joint_pose[i].rotX);
			D3DXMatrixRotationY(&rotY_mat, pose1.joint_pose[i].rotY);
			D3DXMatrixRotationZ(&rotZ_mat, pose1.joint_pose[i].rotZ);

			tmp_M = rotX_mat * rotY_mat * rotZ_mat;

			D3DXQuaternionRotationMatrix( &q1, &tmp_M);
		}
		{
			D3DXMatrixRotationX(&rotX_mat, pose2.joint_pose[i].rotX);
			D3DXMatrixRotationY(&rotY_mat, pose2.joint_pose[i].rotY);
			D3DXMatrixRotationZ(&rotZ_mat, pose2.joint_pose[i].rotZ);

			tmp_M = rotX_mat * rotY_mat * rotZ_mat;

			D3DXQuaternionRotationMatrix( &q2, &tmp_M);
		}

		// SLERP
		D3DXQuaternionSlerp( &qlerp, &q1, &q2, t);

		// quaternionÅÀâÒì]çsóÒ
		D3DXMatrixRotationQuaternion( &M, &qlerp);

		// âÒì]çsóÒÅÀXYZâÒì]äpÅ@Åihttp://www2.teu.ac.jp/clab/kondo/research/cadcgtext/Chap4/Chap405.htmlÅj
		result.joint_pose[i].rotY = -asin(M._13);
		result.joint_pose[i].rotZ = atan2(M._12, M._11);
		result.joint_pose[i].rotX = atan2(M._23, M._33);
	}

	return result;
}


FacialPose imas::FacialPoseLerp( FacialPose& pose1, FacialPose& pose2, float t )
{
	FacialPose result;

	result.pose.resize( pose1.pose.size() );

	assert( pose1.pose.size() >= 136 );
	assert( pose2.pose.size() >= 136 );

	for(int i=83; i<136; i++)
	{
		result.pose[i].posX = (1-t) * pose1.pose[i].posX + t * pose2.pose[i].posX;
		result.pose[i].posY = (1-t) * pose1.pose[i].posY + t * pose2.pose[i].posY;
		result.pose[i].posZ = (1-t) * pose1.pose[i].posZ + t * pose2.pose[i].posZ;

		result.pose[i].scaleX = (1-t) * pose1.pose[i].scaleX + t * pose2.pose[i].scaleX;
		result.pose[i].scaleY = (1-t) * pose1.pose[i].scaleY + t * pose2.pose[i].scaleY;
		result.pose[i].scaleZ = (1-t) * pose1.pose[i].scaleZ + t * pose2.pose[i].scaleZ;


		// XYZâÒì]äpÅÀquaternion
		D3DXQUATERNION q1, q2, qlerp;
		D3DXMATRIX rotX_mat, rotY_mat, rotZ_mat, tmp_M, M;
		{
			D3DXMatrixRotationX(&rotX_mat, pose1.pose[i].rotX);
			D3DXMatrixRotationY(&rotY_mat, pose1.pose[i].rotY);
			D3DXMatrixRotationZ(&rotZ_mat, pose1.pose[i].rotZ);

			tmp_M = rotX_mat * rotY_mat * rotZ_mat;

			D3DXQuaternionRotationMatrix( &q1, &tmp_M);
		}
		{
			D3DXMatrixRotationX(&rotX_mat, pose2.pose[i].rotX);
			D3DXMatrixRotationY(&rotY_mat, pose2.pose[i].rotY);
			D3DXMatrixRotationZ(&rotZ_mat, pose2.pose[i].rotZ);

			tmp_M = rotX_mat * rotY_mat * rotZ_mat;

			D3DXQuaternionRotationMatrix( &q2, &tmp_M);
		}

		// SLERP
		D3DXQuaternionSlerp( &qlerp, &q1, &q2, t);

		// quaternionÅÀâÒì]çsóÒ
		D3DXMatrixRotationQuaternion( &M, &qlerp);

		// âÒì]çsóÒÅÀXYZâÒì]äpÅ@Åihttp://www2.teu.ac.jp/clab/kondo/research/cadcgtext/Chap4/Chap405.htmlÅj
		result.pose[i].rotY = -asin(M._13);
		result.pose[i].rotZ = atan2(M._12, M._11);
		result.pose[i].rotX = atan2(M._23, M._33);
	}


	return result;
}