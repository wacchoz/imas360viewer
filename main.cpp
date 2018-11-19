#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <shlwapi.h>

using namespace std;

// OpenGL
#include <GL/glut.h>
#include <GL/glui.h>
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glui32.lib")
#pragma comment(lib, "squish.lib")	// http://code.google.com/p/libsquish/

// matrix and quaternion calculation
#pragma comment(lib, "d3dx9.lib")	//  DirectX SDK

void init();
void display();
void reshape(int width, int height);
void idle();
void gluiCallbackExit(int num);
void gluiCallbackReset(int num);

// Cg
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cgGL.lib")

// other
#pragma comment(lib, "shlwapi.lib")		// PathRemoveFileSpec()

// im@s
#include "DanceScene.h"

DanceScene g_dancescene;

// global parameter of GLUI
GLUI_Rotation *g_view_rot;
GLUI_Translation *g_view_transXY;
GLUI_Translation *g_view_transZ;
GLUI_RadioGroup  *radio;
int g_radioVal=0;		// render_mode
int g_bEnablePhysics = 1;
int g_bShowRig = 0;
int g_bShowInfo = 1;
int g_bImasCamera = 1;

float g_rotate[16] = {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f
};

float g_trans_XY[2] = { 0.0f, 0.0f };
float g_trans_Z[] = { 0.0f };


int main( int argc, char* argv[] )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowPosition( 300, 50 );
	glutInitWindowSize( 720*16/9, 720 );
	glutCreateWindow("im@s - OpenGL version");

	std::string ini_path;
	if( argc == 2 )
	{
		ini_path = argv[1];
		cout << "using setting file : " << argv[1] << endl;
	}else
	{
		// path of settings.ini
		PathRemoveFileSpec( argv[0] );
		ini_path = argv[0];
		ini_path = ini_path.append("\\settings.ini");
		cout << "using default setting file : " << ini_path << endl;
	}

	// Initialize dance scene
	DanceScene::Input input;

	char str[MAX_PATH];
	GetPrivateProfileString("path", "CharaBNA1", NULL, str, MAX_PATH, ini_path.c_str() ); 	
	input.chara_filename[0] = str;
	GetPrivateProfileString("path", "CharaBNA2", NULL, str, MAX_PATH, ini_path.c_str() ); 	
	input.chara_filename[1] = str;
	GetPrivateProfileString("path", "CharaBNA3", NULL, str, MAX_PATH, ini_path.c_str() ); 	
	input.chara_filename[2] = str;
	GetPrivateProfileString("path", "staticmotBNA", NULL, str, MAX_PATH, ini_path.c_str() ); 	
	input.staticmot_file = str;
	GetPrivateProfileString("path", "danceBNA", NULL, str, MAX_PATH, ini_path.c_str() ); 	
	input.dancefile = str;

	std::string songname = fullPath2FileName( input.dancefile );
	input.song = songname.substr(0, 3);

	GetPrivateProfileString("path", "aix", NULL, str, MAX_PATH, ini_path.c_str() ); 	
	input.songfile = str;
	
	if( ! g_dancescene.Init( input ) ) return EXIT_FAILURE;


	// Initialize GLUI
	GLUI* glui = GLUI_Master.create_glui( "control" );
	g_view_rot = glui->add_rotation( "Rotation", g_rotate );
	g_view_transXY = glui->add_translation( "TransXY With CTRL key", GLUI_TRANSLATION_XY, g_trans_XY );
	g_view_transZ = glui->add_translation( "TransZ With CTRL key", GLUI_TRANSLATION_Z, g_trans_Z );

	GLUI_Panel* pPanel = glui->add_panel("render mode");
	GLUI_RadioGroup* pRadioGroup = glui->add_radiogroup_to_panel( pPanel, &g_radioVal );
	glui->add_radiobutton_to_group( pRadioGroup, "mesh");
	glui->add_radiobutton_to_group( pRadioGroup, "wireframe");
	glui->add_radiobutton_to_group( pRadioGroup, "skeleton");
	glui->add_checkbox( "Show Rig", & g_bShowRig );
	glui->add_checkbox( "Enable Physics", & g_bEnablePhysics );

	glui->add_checkbox( "Show info", & g_bShowInfo );
	glui->add_checkbox( "Use im@s camera", & g_bImasCamera );
	glui->add_button( "Reset", 0, gluiCallbackReset );
	glui->add_button( "Exit", 0, gluiCallbackExit );

	// GLUT
	init();
	glutReshapeFunc( reshape );
	glutDisplayFunc( display );
	GLUI_Master.set_glutIdleFunc( idle );

	// main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}

void init()
{
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat position[] = { 5.0f, 5.0f, 15.0f, 0.0f };

	glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
	glLightfv( GL_LIGHT0, GL_POSITION, position );

	glFrontFace( GL_CCW );
	glEnable( GL_CULL_FACE );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_DEPTH_TEST );
	glShadeModel(GL_SMOOTH);

    glTexParameteri( GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// default camera setting
	gluLookAt( 0.0f, 10.0f, 40.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}


void display()
{
	static int startTime = glutGet(GLUT_ELAPSED_TIME);

	float frame = (int)( (glutGet(GLUT_ELAPSED_TIME) - startTime ) / (1000.0f / 59.94f) );

	g_dancescene.Update( frame , g_bEnablePhysics);

	switch( g_radioVal )
	{
	case 0:
		g_dancescene.Display(g_rotate, g_trans_XY, g_trans_Z, DanceScene::MESH, g_bShowRig, g_bShowInfo, g_bImasCamera);
		break;
	case 1:
		g_dancescene.Display(g_rotate, g_trans_XY, g_trans_Z, DanceScene::WIREFRAME, g_bShowRig, g_bShowInfo, g_bImasCamera);
		break;
	case 2:
		g_dancescene.Display(g_rotate, g_trans_XY, g_trans_Z, DanceScene::SKELETON, g_bShowRig, g_bShowInfo, g_bImasCamera);
		break;
	}
}


void reshape( int width, int height )
{
	g_dancescene.Reshape( width, height );
}

void idle()
{
	glutPostRedisplay();
}

void gluiCallbackExit( int num )
{
	exit(0);
}

void gluiCallbackReset( int num )
{
	g_view_rot->reset();
	g_view_transXY->set_x( 0.0f );
	g_view_transXY->set_y( 0.0f );
	g_view_transZ->set_z( 0.0f );
}


