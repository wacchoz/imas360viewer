#include "LoadBXR.h"

#include "BXR.h"
#include "Common.h"

#include <vector>

using namespace BXR;


void LoadBXR_as_DanceScript(const imas::BXR & bxr, DanceScript& script )
{
	ScriptCommand command;

	for(int i = 0; i < bxr.mainScript.size(); i++)
	{
		// root‚Í“Ç‚Ý”ò‚Î‚µ
		if( bxr.mainScript[i].main_symbol =="root" ) continue;

		// main_symbol‚ð“Ç‚ñ‚Å•ªŠòˆ—
		if( bxr.mainScript[i].main_symbol == "beat")
		{
			command.beat = atoi( bxr.mainScript[i].sub_symbol.c_str() );
		}
		else if( bxr.mainScript[i].main_symbol == "data")
		{
			if( bxr.mainScript[i].sub_symbol == "face")
			{
				int subline;
				subline = bxr.mainScript[i].indexSubScript;
				do
				{
					if( bxr.subScript[ subline ].sub_symbol == "param0" ) command.face.param0 = ::toUpper( bxr.subScript[ subline ].symbol );	// ‹­§“I‚É‘å•¶Žš‚É
				}while( bxr.subScript[ subline++ ].next != -1);
			}
			if( bxr.mainScript[i].sub_symbol == "camera_a")
			{
				int subline;
				subline = bxr.mainScript[i].indexSubScript;
				do
				{
					if( bxr.subScript[ subline ].sub_symbol == "param0" ) command.camera_a.param0 = bxr.subScript[ subline ].symbol;
					if( bxr.subScript[ subline ].sub_symbol == "param1" ) command.camera_a.param1 = bxr.subScript[ subline ].symbol;
					if( bxr.subScript[ subline ].sub_symbol == "param2" ) command.camera_a.param2 = atoi( bxr.subScript[ subline ].symbol.c_str() );
					if( bxr.subScript[ subline ].sub_symbol == "param3" ) command.camera_a.param3 = bxr.subScript[ subline ].symbol;
				}while( bxr.subScript[ subline++ ].next != -1);
			}
			if( bxr.mainScript[i].sub_symbol == "camera_b")
			{
				int subline;
				subline = bxr.mainScript[i].indexSubScript;
				do
				{
					if( bxr.subScript[ subline ].sub_symbol == "param0" ) command.camera_b.param0 = bxr.subScript[ subline ].symbol;
					if( bxr.subScript[ subline ].sub_symbol == "param1" ) command.camera_b.param1 = bxr.subScript[ subline ].symbol;
					if( bxr.subScript[ subline ].sub_symbol == "param2" ) command.camera_b.param2 = atoi( bxr.subScript[ subline ].symbol.c_str() );
					if( bxr.subScript[ subline ].sub_symbol == "param3" ) command.camera_b.param3 = bxr.subScript[ subline ].symbol;
				}while( bxr.subScript[ subline++ ].next != -1);
			}
			if( bxr.mainScript[i].sub_symbol == "camera_c")
			{
				int subline;
				subline = bxr.mainScript[i].indexSubScript;
				do
				{
					if( bxr.subScript[ subline ].sub_symbol == "param0" ) command.camera_c.param0 = bxr.subScript[ subline ].symbol;
					if( bxr.subScript[ subline ].sub_symbol == "param1" ) command.camera_c.param1 = bxr.subScript[ subline ].symbol;
					if( bxr.subScript[ subline ].sub_symbol == "param2" ) command.camera_c.param2 = atoi( bxr.subScript[ subline ].symbol.c_str() );
					if( bxr.subScript[ subline ].sub_symbol == "param3" ) command.camera_c.param3 = bxr.subScript[ subline ].symbol;
				}while( bxr.subScript[ subline++ ].next != -1);
			}
			if( bxr.mainScript[i].sub_symbol == "motion")
			{
				int subline;
				subline = bxr.mainScript[i].indexSubScript;
				do
				{
					if( bxr.subScript[ subline ].sub_symbol == "param0" ) command.motion.param0 = bxr.subScript[ subline ].symbol;
				}while( bxr.subScript[ subline++ ].next != -1);
			}
			if( bxr.mainScript[i].sub_symbol == "part")
			{
				int subline;
				subline = bxr.mainScript[i].indexSubScript;
				do
				{
					if( bxr.subScript[ subline ].sub_symbol == "param0" )
					{
						command.part.param0 = bxr.subScript[ subline ].symbol;
					}
				}while( bxr.subScript[ subline++ ].next != -1);
			}
			if( bxr.mainScript[i].sub_symbol == "song")
			{
				int subline;
				subline = bxr.mainScript[i].indexSubScript;
				do
				{
					if( bxr.subScript[ subline ].sub_symbol == "param0" ) command.song.param0 = bxr.subScript[ subline ].symbol.c_str();
				}while( bxr.subScript[ subline++ ].next != -1);
			}
			if( bxr.mainScript[i].sub_symbol == "event")
			{
				int subline;
				subline = bxr.mainScript[i].indexSubScript;
				do
				{
					if( bxr.subScript[ subline ].sub_symbol == "param0" ) command.event.param0 = bxr.subScript[ subline ].symbol;
				}while( bxr.subScript[ subline++ ].next != -1);
			}
		}
		else if( bxr.mainScript[i].main_symbol == "words")
		{
			script.lyrics.push_back( bxr.mainScript[i].unicode );
		}
		else if( bxr.mainScript[i].main_symbol == "camera")
		{
			Camera_type camera = Camera_type();
			int subline;
			subline = bxr.mainScript[i].indexSubScript;
			do
			{
				if( bxr.subScript[ subline ].sub_symbol == "rate0" ) camera.rate0 = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "rate1" ) camera.rate1 = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "rate2" ) camera.rate2 = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "rate3" ) camera.rate3 = atoi( bxr.subScript[ subline ].symbol.c_str() );
			}while( bxr.subScript[ subline++ ].next != -1);

			script.camera_type.push_back( camera );
		}
		else if( bxr.mainScript[i].main_symbol == "faces")
		{
			Faces face = Faces();
			face.face_type = ::toUpper( bxr.mainScript[i].sub_symbol );	// ‹­§“I‚É‘å•¶Žš‚É

			int subline;
			subline = bxr.mainScript[i].indexSubScript;
			do
			{
				if( bxr.subScript[ subline ].sub_symbol == "param0" ) face.param[0] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param1" ) face.param[1] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param2" ) face.param[2] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param3" ) face.param[3] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param4" ) face.param[4] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param5" ) face.param[5] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param6" ) face.param[6] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param7" ) face.param[7] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param8" ) face.param[8] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param9" ) face.param[9] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param10" ) face.param[10] = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "param11" ) face.param[11] = atoi( bxr.subScript[ subline ].symbol.c_str() );
			}while( bxr.subScript[ subline++ ].next != -1);

			// “o˜^
			script.faces.push_back( face );

		}
		else if( bxr.mainScript[i].main_symbol == "start")
		{
			int subline;
			subline = bxr.mainScript[i].indexSubScript;
			do
			{
				if( bxr.subScript[ subline ].sub_symbol == "main_vocal_part" ) script.start.main_vocal_part = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "start_dance" ) script.start.start_dance = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "start_title" ) script.start.start_title = atoi( bxr.subScript[ subline ].symbol.c_str() );
				else if( bxr.subScript[ subline ].sub_symbol == "wait" ) script.start.wait = atoi( bxr.subScript[ subline ].symbol.c_str() );
			}while( bxr.subScript[ subline++ ].next != -1);
		}


		// ‚à‚µŽŸ‚ªbeat‚©I‚í‚è‚¾‚Á‚½‚ç“o˜^
		if( i == bxr.mainScript.size() -1 || bxr.mainScript[i+1].main_symbol == "beat" )
		{
			script.command.push_back( command );

			// command‚ðˆê“x‹ó‚É‚µ‚Ä‚¨‚­
			command = ScriptCommand();
		}	
	}

}
