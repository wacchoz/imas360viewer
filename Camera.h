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
	int m_current_key;

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

	void Update( int key )
	{
		m_current_key = key;
	}

	void ApplyCamera( int key, double aspectRatio )
	{
		if( key < 0 ) key = 0;
		if( key >= camera_pose.size() ) key = camera_pose.size() - 1;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( camera_pose[key].fov * 180.0f / D3DX_PI, aspectRatio, 5.0, 200.0);
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt(	camera_pose[key].eye.x, camera_pose[key].eye.y, camera_pose[key].eye.z,
					camera_pose[key].target.x, camera_pose[key].target.y, camera_pose[key].target.z,
					camera_pose[key].up.x, camera_pose[key].up.y, camera_pose[key].up.z);
	}

	void ApplyCamera( float aspectRatio )
	{
		ApplyCamera( m_current_key, aspectRatio );
	}
};


}	// namespace


#endif