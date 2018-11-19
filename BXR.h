#ifndef _BXR_H_
#define _BXR_H_

#include "File.h"

#include <vector>
#include <string>


namespace imas{

class BXR
{
public:
	struct BLOCK1
	{
		int offsetSymbol;
		std::string symbol;		// ダンス内の場合はroot/beat/data/start/words/camera/faces
	};

	std::vector<BLOCK1> tag_mainScript;

	struct BLOCK2
	{
		int offsetSymbol;	
		std::string symbol;		// ダンス内の場合はparam*/main_vocal_part/start_dance/start_title/wait/font/position_start/scale_x/rate* 等
	};

	std::vector<BLOCK2> tag_subScript;

	struct BLOCK3
	{
		int before;
		int dataTag;
		int indexSubScript;
		int next;
		int nextTicks;
		int offsetSymbol;		// 数値/camera_a/face/motion/part/song/event等
		int offsetUnicode;		// 歌詞

		std::string main_symbol;		// dataTag
		std::string sub_symbol;			// offset_symbol
		std::wstring unicode;			// 歌詞
	};

	std::vector<BLOCK3> mainScript;

	struct BLOCK4
	{
		int dataTag;	// tag_subScriptへ
		int next;
		int offsetSymbol;

		std::string sub_symbol;
		std::string symbol;
	};

	std::vector<BLOCK4> subScript;

	std::vector<unsigned char> symbol;

public:

	bool Load( std::string filename )
	{
		File file;
		if( ! file.LoadFromFile(filename) ) return false;

		return Load( & file );
	}

	bool Load( File* pFile )
	{
		if( pFile->ReadByte() != 'B' ) return false;
		if( pFile->ReadByte() != 'X' ) return false;
		if( pFile->ReadByte() != 'R' ) return false;
		if( pFile->ReadByte() != '0' ) return false;

		unsigned int count1, count2, count3, count4, count5;

		count1 = pFile->ReadUInt32();
		count2 = pFile->ReadUInt32();
		count3 = pFile->ReadUInt32();
		count4 = pFile->ReadUInt32();
		count5 = pFile->ReadUInt32();

		// BLOCK1
		tag_mainScript.resize( count1 );
		for(int i=0; i<count1; i++)
		{
			tag_mainScript[i].offsetSymbol = pFile->ReadInt32();
		}

		// BLOCK2
		tag_subScript.resize( count2 );
		for(int i=0; i<count2; i++)
		{
			tag_subScript[i].offsetSymbol = pFile->ReadInt32();
		}

		// BLOCK3
		mainScript.resize( count3 );
		for(int i=0; i<count3; i++)
		{
			mainScript[i].before = pFile->ReadInt32();
			mainScript[i].next = pFile->ReadInt32();
			mainScript[i].dataTag = pFile->ReadInt32();
			mainScript[i].offsetSymbol = pFile->ReadInt32();
			mainScript[i].indexSubScript = pFile->ReadInt32();
			mainScript[i].offsetUnicode = pFile->ReadInt32();
			mainScript[i].nextTicks = pFile->ReadInt32();
		}

		// BLOCK4
		subScript.resize( count4 );
		for(int i=0; i<count4; i++)
		{
			subScript[i].next = pFile->ReadInt32();
			subScript[i].dataTag = pFile->ReadInt32();
			subScript[i].offsetSymbol = pFile->ReadInt32();
		}

		// BLOCK5
		char* offsetStart = (char*)pFile->pCurrentData();
		symbol.resize( count5 );
		for(int i=0; i<count5; i++)
		{
			symbol[i] = pFile->ReadByte();
		}

		//
		// 解釈
		//

		// BLOCK1
		for(int i=0; i<count1; i++)
		{
			tag_mainScript[i].symbol = std::string( offsetStart + tag_mainScript[i].offsetSymbol );
		}

		// BLOCK2
		for(int i=0; i<count2; i++)
		{
			tag_subScript[i].symbol = std::string( offsetStart + tag_subScript[i].offsetSymbol );
		}

		// BLOCK3
		for(int i=0; i<count3; i++)
		{
			mainScript[i].main_symbol = tag_mainScript[ mainScript[i].dataTag ].symbol;
			mainScript[i].sub_symbol = std::string( offsetStart + mainScript[i].offsetSymbol );

			// 歌詞など
			if( mainScript[i].offsetUnicode != -1 )
			{
				mainScript[i].unicode = (wchar_t*) & symbol[ mainScript[i].offsetUnicode ];

				for(int s=0; s < mainScript[i].unicode.length(); s++)	// endian変換
				{
					char* tmp = (char*) mainScript[i].unicode.c_str() + s * 2;
					char ch;
					ch = *(tmp);
					*(tmp) = *(tmp+1);
					*(tmp+1) = ch;
				}
			}
		}

		// BLOCK4
		for(int i=0; i<count4; i++)
		{
			subScript[i].sub_symbol = tag_subScript[ subScript[i].dataTag ].symbol;
			subScript[i].symbol = std::string( offsetStart + subScript[i].offsetSymbol );
		}


		return true;
	}

};


};	// namespace imas


#endif