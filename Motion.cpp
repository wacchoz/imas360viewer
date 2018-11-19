#include "Motion.h"
#include "d3dx9math.h"

using namespace imas;

// lerp by quaternion
BodyPose imas::PoseLerp( BodyPose& pose1, BodyPose& pose2, float t )
{
	BodyPose result;

	result.joint_pose.resize( pose1.joint_pose.size() );

	result.position = (1-t) * pose1.position + t * pose2.position;

	for(int i=0; i<pose1.joint_pose.size(); i++)
	{
		result.joint_pose[i].translate = (1-t) * pose1.joint_pose[i].translate + t * pose2.joint_pose[i].translate;
		result.joint_pose[i].scale = (1-t) * pose1.joint_pose[i].scale + t * pose2.joint_pose[i].scale;

		// SLERP
		D3DXQuaternionSlerp( &result.joint_pose[i].q, &pose1.joint_pose[i].q, &pose2.joint_pose[i].q, t);
	}

	return result;
}


FacialPose imas::FacialPoseLerp( FacialPose& pose1, FacialPose& pose2, float t )
{
	FacialPose result;

	result.joint_pose.resize( pose1.joint_pose.size() );

	assert( pose1.joint_pose.size() >= 136 );
	assert( pose2.joint_pose.size() >= 136 );

	for(int i=83; i<136; i++)
	{
		result.joint_pose[i].translate = (1-t) * pose1.joint_pose[i].translate + t * pose2.joint_pose[i].translate;
		result.joint_pose[i].scale = (1-t) * pose1.joint_pose[i].scale + t * pose2.joint_pose[i].scale;

		// SLERP
		D3DXQuaternionSlerp( &result.joint_pose[i].q, &pose1.joint_pose[i].q, &pose2.joint_pose[i].q, t);
	}

	return result;
}