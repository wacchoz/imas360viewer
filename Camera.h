#ifndef _CAMERA_HEADER_
#define _CAMERA_HEADER_

#include "BNAFile.h"
#include "NUM.h"
#include "d3dx9math.h"

#include <GL/glut.h>


namespace imas{


struct CameraPose
{
	D3DXVECTOR3 eye;
	D3DXVECTOR3 target;
	D3DXVECTOR3 up;
	float fov;
};


class CameraMotion
{
private:
	std::vector< CameraPose > camera_pose;
	float m_current_key;

public:

	bool Load(imas::BNAFile* pBNAFile)
	{
		imas::NUM numCamera;

		File* pFile = pBNAFile->GetFileByFilter( "dan_tvc_3ca_" );
		if( pFile == NULL ) return false;
		if( ! numCamera.Load( pFile ) ) return false;


		camera_pose.resize( numCamera.key.size() );

		for(int i=0; i < numCamera.key.size(); i++)
		{
			camera_pose[i].eye.x = numCamera.key[i].ch[0].data1 / 1000.0f * 10.0f;
			camera_pose[i].eye.y = numCamera.key[i].ch[0].data2 / 1000.0f * 10.0f;
			camera_pose[i].eye.z = numCamera.key[i].ch[0].data3 / 1000.0f * 10.0f;

			camera_pose[i].target.x = numCamera.key[i].ch[1].data1 / 1000.0f * 10.0f;
			camera_pose[i].target.y = numCamera.key[i].ch[1].data2 / 1000.0f * 10.0f;
			camera_pose[i].target.z = numCamera.key[i].ch[1].data3 / 1000.0f * 10.0f;

			camera_pose[i].up.x = numCamera.key[i].ch[2].data1 / 1000.0f * 10.0f;
			camera_pose[i].up.y = numCamera.key[i].ch[2].data2 / 1000.0f * 10.0f;
			camera_pose[i].up.z = numCamera.key[i].ch[2].data3 / 1000.0f * 10.0f;

			camera_pose[i].fov = numCamera.key[i].ch[3].data1 / 1000.0f / 10.0f;
		}

		m_current_key = 0;

		return true;
	};

	bool Load(std::string filename)
	{
		imas::BNAFile bna;
		if( ! bna.Load( filename ) ) return false;

		return  Load( & bna );
	}

	void Update( float key )
	{
		m_current_key = key;
	}

	void ApplyCamera( float key, double aspectRatio )
	{
		if( key < 0 ) key = 0;
		if( key >= camera_pose.size() ) key = camera_pose.size() - 1;

		float fraction = key - (int) key;
		CameraPose camerapose;

		camerapose.fov = (1 - fraction) * camera_pose[(int)key].fov + fraction * camera_pose[(int)key+1].fov;
		camerapose.eye = (1 - fraction) * camera_pose[(int)key].eye + fraction * camera_pose[(int)key+1].eye;
		camerapose.target = (1 - fraction) * camera_pose[(int)key].target + fraction * camera_pose[(int)key+1].target;
		camerapose.up = (1 - fraction) * camera_pose[(int)key].up + fraction * camera_pose[(int)key+1].up;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( camerapose.fov * 180.0f / D3DX_PI, aspectRatio, 5.0, 200.0);
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt(	camerapose.eye.x, camerapose.eye.y, camerapose.eye.z,
					camerapose.target.x, camerapose.target.y, camerapose.target.z,
					camerapose.up.x, camerapose.up.y, camerapose.up.z);
	}

	void ApplyCamera( float aspectRatio )
	{
		ApplyCamera( m_current_key, aspectRatio );
	}
};


}	// namespace


#endif