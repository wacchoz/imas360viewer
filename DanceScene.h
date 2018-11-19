#ifndef _DANCE_SCENE_H_
#define _DANCE_SCENE_H_


// im@s関係
#include "GameConstant.h"
#include "character.h"
#include "Motion.h"
#include "BXR.h"
#include "LoadBXR.h"
#include "AIX.h"
#include "Camera.h"

#include "Shader.h"

#include "Font.h"

#include <locale>
#include <iostream>


class DanceScene
{
private:
	ToonShader m_ToonShader;
	OutlineShader m_OutlineShader;

	imas::Character m_character[3];

	imas::Motion m_motion[3];
	imas::Motion m_loopMotion[3];
	imas::LipMotion m_lipmotion;
	imas::BXR m_bxr;
	BXR::DanceScript m_dance_script;
	float m_BPM;
	float m_scriptDelay;
	imas::AIX m_AIX;

	imas::CameraMotion m_camera_motion;

	BitmapFont m_FontLyrics;	// 日本語フォント
	BitmapFont m_FontInfo;		// Debug用フォント

	int m_window_width;
	int m_window_height;
//	bool m_bImasCamera;			// im@s camera or User camera

private:
	struct debugInfo{
		int frame;
		float currentBeat;
		std::string face_type;
		int m_dance_script_start_wait; 
		int m_dance_script_start_start_dance;
		std::wstring lyrics;
		std::string part;
		float fps;
	} debugInfo;

public:
	struct Input{
		std::string chara_filename[3];
		std::string staticmot_file;
		std::string song;
		std::string dancefile;
		std::string songfile;
	};

public:
	bool Init(Input& input )
	{
		imas::InitializeGameConstant();

		// キャラクター読み込み
		{
			std::cout << "loading " << input.chara_filename[0] << endl;
			if( ! m_character[0].Load( input.chara_filename[0] ) )
			{
				std::cout << "Error at loading " << input.chara_filename[0] << endl;
				return false;
			}
			std::cout << "loading " << input.chara_filename[1] << endl;
			if( ! m_character[1].Load( input.chara_filename[1] ) )
			{
				std::cout << "Error at loading " << input.chara_filename[1] << endl;
				return false;
			}
			std::cout << "loading " << input.chara_filename[2] << endl;
			if( ! m_character[2].Load( input.chara_filename[2] ) )
			{
				std::cout << "Error at loading " << input.chara_filename[2] << endl;
				return false;
			}

			// staticmot読み込み
			std::cout << "loading " << input.staticmot_file << endl;
			if( ! m_character[0].LoadFacialPoseArray( input.staticmot_file ) )
			{
				std::cout << "Error at loading " << input.staticmot_file << endl;
				return false;
			}
			if( ! m_character[1].LoadFacialPoseArray( input.staticmot_file ) )
			{
				std::cout << "Error at loading " << input.staticmot_file << endl;
				return false;
			}
			if( ! m_character[2].LoadFacialPoseArray( input.staticmot_file ) )
			{
				std::cout << "Error at loading " << input.staticmot_file << endl;
				return false;
			}
		}

		// モーション読み込み
		{
			m_BPM = imas::g_bpm[input.song];

			std::cout << "loading " << input.dancefile << endl;
			if(		! m_motion[0].Load( input.dancefile, imas::Motion::CENTER, imas::Motion::MAIN )
				||	! m_loopMotion[0].Load( input.dancefile, imas::Motion::CENTER, imas::Motion::LOOP )
				||	! m_motion[1].Load( input.dancefile, imas::Motion::RIGHT, imas::Motion::MAIN )
				||	! m_loopMotion[1].Load( input.dancefile,imas:: Motion::RIGHT, imas::Motion::LOOP )
				||	! m_motion[2].Load( input.dancefile, imas::Motion::LEFT, imas::Motion::MAIN )
				||	! m_loopMotion[2].Load( input.dancefile, imas::Motion::LEFT, imas::Motion::LOOP ) )
			{
				std::cout << "Error at loading " << input.dancefile << endl;
				return false;
			}

			if( ! m_lipmotion.Load( input.dancefile ) )
			{
				std::cout << "Error at loading lipsync in" << input.dancefile << endl;
				return false;
			}

			if( ! m_camera_motion.Load( input.dancefile ) )
			{
				std::cout << "Error at loading camera motion in" << input.dancefile << endl;
				return false;
			}
		}

		// 音楽ファイル
		{
			std::cout << "loading " << input.songfile << endl;
			if( ! m_AIX.Load( input.songfile ) )
			{
				std::cout << "Error at loading " << input.songfile << endl;
				return false;
			}
		}

		// BXRスクリプト
		{
			imas::BNAFile bnafile;
			if( ! bnafile.Load( input.dancefile ) ) return false;
			File* pFile = bnafile.GetFileByFilter(".bxr");
			if( pFile == NULL ) return false;

			std::cout << "loading " << pFile->FileName() << " in " << input.dancefile << endl;
			if( ! m_bxr.Load( pFile ) )
			{
				std::cout << "Error at loading " << pFile->FileName() << endl;
				return false;
			}

			::LoadBXR_as_DanceScript( m_bxr, m_dance_script );

			m_scriptDelay = imas::g_scriptDelay[ input.song ];
		}

		// Cg関係初期化
		{
			m_ToonShader.Init();
			m_OutlineShader.Init();
		}

		// フォント
		if(		! m_FontLyrics.CreateW(L"ＭＳ　Ｐゴシック", 24)
			||	! m_FontInfo.CreateA("Arial", 24) )
		{
			std::cout << "Error at creating font" << endl;
		}

		return true;
	}


	void Update( int frame )
	{
		const int start_dance = m_dance_script.start.start_dance;

		float currentBeat = (frame - m_dance_script.start.wait ) / 360.0f * m_BPM - m_scriptDelay;

		// 音楽
		if( frame >= m_dance_script.start.wait )
		{
			static bool first = true;
			if(first)
			{
				first = false;

				m_AIX.Play(10);
				m_AIX.Play(11);
				m_AIX.Play(imas::g_chara2track[ m_character[0].GetName()]);		// センターだけ歌わせる
			}
		}

		/////////////////////////////////////////////
		// currentBeatでの顔表情を探す
		std::string face_type;
		std::string face_type_previous;
		int change_face_beat=0;

		m_dance_script.GetFace( currentBeat, face_type, face_type_previous, change_face_beat);

		for( int i=0; i<3; i++)
		{
			int face_id;
			int face_id_previous;

			// 表情を検索
			int a = imas::g_chara2mpkID[ m_character[i].GetName() ];
			face_id = m_dance_script.GetFaceID( face_type, imas::g_chara2mpkID[ m_character[i].GetName() ] );
			// 前の表情を検索
			face_id_previous = m_dance_script.GetFaceID( face_type_previous, imas::g_chara2mpkID[ m_character[i].GetName() ] );
			// 顔の表情のポーズをブレンドして更新 （3フレームで移行）
			float lerp_weight = ( (currentBeat - change_face_beat) * 360.0f / m_BPM <= 3.0f ?
									(currentBeat - change_face_beat) * 360.0f / m_BPM / 3.0f : 1.0f );
			m_character[i].UpdateFaceEmotionLERP( face_id_previous, face_id, lerp_weight );

			/////////////////////////////////////////////
			// メッシュ変更本体
			if( frame < m_dance_script.start.wait + start_dance )		// loop motion
			{
				if( frame < m_dance_script.start.wait + start_dance - 10 )
				{
					m_character[i].UpdateBody( m_loopMotion[i].GetPose( frame % m_loopMotion[i].GetSize() ));
				}
				else		// loopとdanceを10frameでつなぐ
				{
					imas::Pose lerppose, loop_pose, first_pose;
					loop_pose = m_loopMotion[i].GetPose( frame % m_loopMotion[i].GetSize() );
					first_pose = m_motion[i].GetPose( 0 );
					lerppose = imas::PoseLerp( loop_pose, first_pose, (float) (frame - (m_dance_script.start.wait + start_dance - 10)) / 10.0f );
					m_character[i].UpdateBody( lerppose );
				}
			}
			else														// dance motion
			{
				m_character[i].UpdateBody( m_motion[i].GetPose(frame - m_dance_script.start.wait - start_dance) );
			}

			// センターだけリップシンクする
			if( i== 0 )
				m_character[i].UpdateLip( m_lipmotion.GetPose(frame - m_dance_script.start.wait ) );
			else
				m_character[i].CloseLip();

			m_character[i].UpdateSkinningMatrix();
			m_character[i].UpdateMesh();
		}

		// Camera姿勢を更新
		m_camera_motion.Update( frame - m_dance_script.start.wait );	// lipと同じkey


		// Debug用情報
		{
			debugInfo.frame = frame;
			debugInfo.currentBeat = currentBeat;
			debugInfo.face_type = face_type;
			debugInfo.m_dance_script_start_wait = m_dance_script.start.wait; 
			debugInfo.m_dance_script_start_start_dance = m_dance_script.start.start_dance;

			std::string script_song = m_dance_script.GetSong( currentBeat );
			if( script_song == "off" || script_song == "" )
				debugInfo.lyrics = L"";
			else
				debugInfo.lyrics = m_dance_script.lyrics[ atoi(m_dance_script.GetSong( currentBeat ).c_str()) ];
			debugInfo.part = m_dance_script.GetPart( currentBeat );
		}
	}

	void Display(float* rotate, float* trans_XY, float* trans_Z, int bWireframe, int bShowInfo, int bImasCamera)
	{
		// 背景のクリア
		glClearColor( 0.5f, 0.5f, 1.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	

		if( bImasCamera )
		{
			m_camera_motion.ApplyCamera( (double) m_window_width / (double) m_window_height );

		}else
		{
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			gluPerspective( 30.0, (double)m_window_width / (double)m_window_height, 5.0, 200.0 );
			gluLookAt( 0.0f, 10.0f, 40.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f );

			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();
			glTranslatef( trans_XY[0], trans_XY[1], trans_Z[0] );	//平行移動
			glMultMatrixf( rotate );	// 回転
		}

		
		// ワイヤフレーム切り替え
		if( bWireframe )
		{
			glLineWidth(1.0f);
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

			for(int i=0; i<3; i++)
			{
				m_character[i].Render( & m_ToonShader, imas::Character::RENDER_LINE );
			}
//			m_character.m_Skeleton.Render();
		}
		else
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			for(int i=0; i<3; i++)
			{
				m_character[i].Render( & m_OutlineShader,  imas::Character::RENDER_OUTLINE );
				m_character[i].Render( & m_ToonShader, imas::Character::RENDER_MESH );
			}
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


		// FPS表示
		{
			static int frame = 0;
			static int last_t;
			int t = glutGet(GLUT_ELAPSED_TIME); 
			if(t - last_t > 1000) { 
				debugInfo.fps =  (1000.0 * frame) / (t - last_t); 
				last_t = t; 
				frame = 0; 
			} 
			frame++; 
		}
	
		// 文字列描画
		if( bShowInfo )
		{
			static char buf[200];
//			glPushMatrix(); // 行列を退避

			glLoadIdentity();
				glMatrixMode(GL_PROJECTION); // 2Dの並行投影を設定
				glPushMatrix();

					glLoadIdentity();
					gluOrtho2D(0, 720/4, 0, 720/4);	// ちょっとインチキ
					//ここから描画
					glPushAttrib(GL_CURRENT_BIT|GL_DEPTH_BUFFER_BIT);  // 色、Zバッファの退避
					glDisable(GL_DEPTH_TEST);
					glColor3d(1,1,1);

					sprintf_s(buf,sizeof(buf),  "fps : %4.1f", debugInfo.fps); 
					glRasterPos2i(10, 5);
					m_FontInfo.DrawStringA(buf);

					sprintf_s(buf, sizeof(buf), "frame : %4d", debugInfo.frame); 
					glRasterPos2i(10, 10);
					m_FontInfo.DrawStringA(buf);

					sprintf_s(buf, sizeof(buf), "beat  :  %4.1f", debugInfo.currentBeat); 
					glRasterPos2i(10, 15);
					m_FontInfo.DrawStringA(buf);

					sprintf_s(buf, sizeof(buf), "face  : %s", debugInfo.face_type.c_str()); 
					glRasterPos2i(10, 20);
					m_FontInfo.DrawStringA(buf);

					sprintf_s(buf, sizeof(buf), "wait  : %d", debugInfo.m_dance_script_start_wait); 
					glRasterPos2i(10, 25);
					m_FontInfo.DrawStringA(buf);

					sprintf_s(buf,sizeof(buf),  "start : %d", debugInfo.m_dance_script_start_start_dance); 
					glRasterPos2i(10, 30);
					m_FontInfo.DrawStringA(buf);

					sprintf_s(buf,sizeof(buf),  "part : %s", debugInfo.part.c_str()); 
					glRasterPos2i(10, 35);
					m_FontInfo.DrawStringA(buf);

					// 日本語用
					glRasterPos2i(50, 10);
					m_FontLyrics.DrawStringW( (wchar_t*) debugInfo.lyrics.c_str() );

					glPopAttrib();
				
				glPopMatrix(); 
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix(); // もとの状態にもどる
		}

		glutSwapBuffers();
		glutPostRedisplay();
	}

	void Reshape( int width, int height )
	{
		m_window_width = width;
		m_window_height = height;

		glViewport( 0, 0, width, height );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 30.0, (double)width / (double)height, 5.0, 200.0 );
	}
};

#endif