#ifndef _LOAD_BXR_H_
#define _LOAD_BXR_H_

#include <string>
#include "BXR.H"

namespace BXR{

struct Camera
{
	std::string param0;	// 文字列or数値
	std::string param1;	// 文字列
	int			param2;	// 数値
	std::string param3;	// 無し
};

struct Face
{
	std::string param0;	// 文字列
};

struct Part
{
	std::string param0;	// 数値 or "end"
};

struct Song
{
	std::string	param0;	// 数値or文字列
};

struct Event
{
	std::string param0;	// 文字列
};

struct Motion
{
	std::string param0;
};

// リソース
struct Words
{
	std::string lyric;	// 歌詞

	std::string font;
	std::string position_start;
	std::string position_x, position_y;
	std::string scale_x, scale_y;
	std::string space;
};

struct Camera_type
{
	int rate0, rate1, rate2, rate3;	// 数値
};

struct Start
{
	int main_vocal_part;
	int start_dance;
	int start_title;
	int wait;
};

struct Faces
{
	std::string	face_type;	// 文字列（強制大文字）
	int			param[12];	// 数値
};

//　メインスクリプト(１コマンド分)
struct ScriptCommand
{
	int beat;	// 数値
	Face face;				// 強制大文字
	Camera camera_a, camera_b, camera_c;
	Motion motion;
	Part part;
	Song song;
	Event event;

};

// スクリプト全部
struct DanceScript
{
public:
	std::vector< ScriptCommand > command;
	std::vector< BXR::Words > words;
	std::vector< BXR::Camera_type > camera_type;
	std::vector< BXR::Faces > faces;
	BXR::Start start;
	std::vector< std::wstring > lyrics;

	void Destroy()
	{
		command.clear();
		words.clear();
		camera_type.clear();
		faces.clear();
	}

	// current_beatでのpartを返す（数値 or "end"）
	std::string GetPart( const float current_beat )
	{
		std::string part;
		for(int i=0; i < command.size(); i++)
		{
			if( command[i].beat <= current_beat && command[i].part.param0 != "" )
			{
				part = command[i].part.param0;
			}
			if( command[i].beat > current_beat ) break;
		}
		return part;
	}

	// current_beatでのsongを返す
	std::string GetSong( const float current_beat )
	{
		std::string song;
		for(int i=0; i < command.size(); i++)
		{
			if( command[i].beat <= current_beat && command[i].song.param0 != "" )
			{
				song = command[i].song.param0;
			}
			if( command[i].beat > current_beat ) break;
		}
		return song;
	}

	// current_beatでの表情文字列、前の表情文字列、表情が変わった時のbeatを返す
	void GetFace( const float current_beat, std::string & face_type, std::string & face_type_previous, int & change_face_beat )
	{
		for(int i=0; i < command.size(); i++)
		{
			if( command[i].beat <= current_beat && command[i].face.param0 != "" )
			{
				face_type_previous = face_type;

				face_type = command[i].face.param0;
				change_face_beat = command[i].beat;
			}
			if( command[i].beat > current_beat ) break;
		}
	}

	int GetFaceID( std::string face_type, int charaID )
	{
		int face_id = 0;

		for(int i=0; i < faces.size(); i++)
		{
			if( faces[i].face_type == face_type )
			{
				face_id = faces[i].param[ charaID ] * 5;

				if( faces[i].face_type.find("CLOSE_EYE") != std::string::npos ) face_id++;

				break;
			}
		}

		return face_id;
	}

};

};	// end of namespace


void LoadBXR_as_DanceScript( const imas::BXR & bxr, BXR::DanceScript& script );


#endif