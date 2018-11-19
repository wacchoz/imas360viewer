#include <stdlib.h>
#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")		// PathRemoveFileSpec()

// OpenGL関係
#include <GL/glut.h>
#include <GL/glui.h>
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glui32.lib")
#pragma comment(lib, "squish.lib")

void init();
void display();
void reshape(int width, int height);
void idle();
void gluiCallbackExit(int num);
void gluiCallbackReset(int num);

// Cg関係
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cgGL.lib")

#include "Shader.h"

ToonShader g_ToonShader;
OutlineShader g_OutlineShader;

// im@s関係
#include "character.h"


// グローバル変数
imas::Character g_character;

int g_width, g_height;
int g_bWireframe = 0;

// GLUI関係グローバル変数
GLUI_Rotation *g_view_rot;
GLUI_Translation *g_view_transXY;
GLUI_Translation *g_view_transZ;

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
	glutInitWindowSize( 720, 720 );
	glutCreateWindow("im@s - OpenGL version");

	if( argc == 1 )
	{
		printf("Error: No argument");
		return EXIT_FAILURE;
	}
	else
	{
		PathRemoveFileSpec( argv[0] );
		SetCurrentDirectory( argv[0] );
		std::string filename( argv[1] );
		// BNAファイル読み込み。Characterクラスへ読み込み
		if( ! g_character.Load( filename ) ) return EXIT_FAILURE;
	}


	// GLUI関係初期化
	GLUI *glui = GLUI_Master.create_glui( "control" );
	g_view_rot = glui->add_rotation( "Rotation", g_rotate );
	g_view_transXY = glui->add_translation( "TransXY With CTRL key", GLUI_TRANSLATION_XY, g_trans_XY );
	g_view_transZ = glui->add_translation( "TransZ With CTRL key", GLUI_TRANSLATION_Z, g_trans_Z );
	glui->add_checkbox( "wireframe", & g_bWireframe );
	glui->add_button( "Reset", 0, gluiCallbackReset );
	glui->add_button( "Exit", 0, gluiCallbackExit );

	// Cg関係初期化
	g_ToonShader.Init();
	g_OutlineShader.Init();

	// 以下OpenGL関係
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

//	glBindTexture(GL_TEXTURE_2D, 0);
//	glEnable(GL_TEXTURE_2D);

    // テクスチャを拡大・縮小する方法の指定
    glTexParameteri( GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	// ブレンド方法の設定
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// 視点設定
	gluLookAt( 0.0f, 10.0f, 25.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}

void display()
{
	// 背景のクリア
	glClearColor( 0.5f, 0.5f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

		glTranslatef( g_trans_XY[0], g_trans_XY[1], g_trans_Z[0] );	//平行移動
		glMultMatrixf( g_rotate );	// 回転

		
		// ワイヤフレーム切り替え
		if( g_bWireframe == 0 )
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			g_character.Render( & g_OutlineShader,  imas::Character::RENDER_OUTLINE );
			g_character.Render( & g_ToonShader, imas::Character::RENDER_MESH );

		}else
		{
			glLineWidth(1.0f);
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

			g_character.Render( & g_ToonShader, imas::Character::RENDER_LINE );
		}
		

		// 座標軸描画
		glDisable(GL_LIGHTING);
		glLineWidth(3.0f);
		glColor3f(1.0f,  0.0f,  0.0f);
		glBegin(  GL_LINES  );
			glVertex3f(0.0f,  0.0f,  0.0f);
			glVertex3f(5.0f,  0.0f,  0.0f);
		glEnd();

		glColor3f(0.0f,  1.0f,  0.0f);
		glBegin(  GL_LINES  );
			glVertex3f(0.0f,  0.0f,  0.0f);
			glVertex3f(0.0f,  5.0f,  0.0f);
		glEnd();

		glColor3f(0.0f,  0.0f,  1.0f);
		glBegin(  GL_LINES  );
			glVertex3f(0.0f,  0.0f,  0.0f);
			glVertex3f(0.0f,  0.0f,  5.0f);
		glEnd();


	glPopMatrix();
	
	glutSwapBuffers();
	glutPostRedisplay();

	// FPS表示
	{
		static int frame = 0;
		static int last_t;
		int t = glutGet(GLUT_ELAPSED_TIME); 
		if(t - last_t > 1000) { 
			printf("%g    \r", (1000.0 * frame) / (t - last_t)); 
			last_t = t; 
			frame = 0; 
		} 
		frame++; 
	}
}


void reshape(int width, int height)
{
	g_width = width;
	g_height = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)width / (double)height, 5.0, 200.0);
}

void idle()
{
	glutPostRedisplay();
}

void gluiCallbackExit(int num)
{
	exit(0);
}

void gluiCallbackReset(int num)
{
	g_view_rot->reset();
	g_view_transXY->set_x( 0.0f );
	g_view_transXY->set_y( 0.0f );
	g_view_transZ->set_z( 0.0f );
}


