#ifndef _AIX_HEADER_
#define _AIX_HEADER_

/*
	[AIXF]
	--------
	[AIXP]　＼
	[AIXP]　　｜
	[AIXP]　　｜このブロックを繰り返す
	... 　　　｜
	[AIXE]　／
	--------
	[AIXP]
	[AIXP]
	[AIXP]
	...
	[AIXE]
	--------
	[AIXP]
	[AIXP]
	[AIXP]
	...
	[AIXE]
	--------


	[AIXF]
	0x00-0x03: 'AIXF'
	0x04-0x07: offset to next AIX header, from 0x08
	0x18-0x19: block count

	0x20-0x23: offset to block header
	0x24-0x27: block size
	0x28-0x2B: sample count
	0x2C-0x2F: sampling rate 
	(16bytes * block number)
	(#1)

	#1+0x10: track count

	[AIXP]
	0x00-0x03: 'AIXP'
	0x04-0x07: offset to next AIX header, from 0x08
	0x08 : track number
	0x09 : track count
	0x0A-0x0B : data size of AIXP
	0x0C-0x0F : frame number (0xffffffff: first, 0xfffffffe: last)
	0x10- : data

	[AIXE]
	0x00-0x03: 'AIXE'
	0x04-0x07: offset to next AIX header, from 0x08
*/

#include "File.h"
#include <vector>
#include "ADX.h"
#include "WAV.h"

namespace imas
{

class AIX
{
	std::vector<WAV> m_wav_concat;		// wave変換後連結

public:
	bool Load(File* aix, bool bExtractADX = false)
	{
		std::vector<std::vector<File>> m_file;	// ADXに展開し格納するため

		aix->SetEndianness( File::BIG_ENDIAN );
		//////////
		// AIXF
		//////////
		if( aix->ReadByte() != 'A' ) return false;
		if( aix->ReadByte() != 'I' ) return false;
		if( aix->ReadByte() != 'X' ) return false;
		if( aix->ReadByte() != 'F' ) return false;

		int offsetToNext;
		offsetToNext = aix->ReadInt32();
		offsetToNext += aix->CurrentPosition();

		aix->Skip(16);

		int blockcount;
		blockcount = aix->ReadInt16();

		m_file.resize( blockcount );

		aix->Skip(6);

		std::vector<unsigned int> offsetToBlockHeader;
		offsetToBlockHeader.resize( blockcount );

		for(int block = 0; block < blockcount; block++)
		{
			offsetToBlockHeader[block] = aix->ReadInt32();
			int blocksize;
			blocksize = aix->ReadInt32();
			int samplecount;
			samplecount = aix->ReadInt32();
			int samplingrate;
			samplingrate = aix->ReadInt32();
		}
		
		//////////
		// AIXP
		//////////
		for(int block = 0; block < blockcount; block++)
		{

			int frameNo;
			aix->Jump( offsetToBlockHeader[block] );
			do{
				// read
				if( aix->ReadByte() != 'A' ) return false;
				if( aix->ReadByte() != 'I' ) return false;
				if( aix->ReadByte() != 'X' ) return false;
				if( aix->ReadByte() != 'P' ) return false;

				int offsetToNext;
				offsetToNext = aix->ReadInt32();
				offsetToNext += aix->CurrentPosition();

				int trackNo;
				trackNo = aix->ReadByte();
				int trackCount;
				trackCount = aix->ReadByte();
				int datasize;
				datasize = aix->ReadInt16();
				frameNo = aix->ReadInt32();

				// write
				m_file[block].resize( trackCount );

				m_file[block][trackNo].WriteByteArray(
					aix->pCurrentData(), datasize );

				aix->Jump( offsetToNext );

			}while(frameNo != -2);
		}


		// ADXファイル書き出し
		if(	bExtractADX	)
		{
			for(int block = 0; block < blockcount; block++)
			{
				for(int track = 0; track < m_file[block].size(); track++)
				{
					char buf[MAX_PATH];
					sprintf_s(buf,sizeof(buf), "%02d%02d.adx", track, block);
					m_file[block][track].WriteToFile( buf );
				}
			}
		}


		// WAVEファイルへの変換
		m_wav_concat.resize( m_file[0].size() );

		for(int track = 0; track < m_file[0].size(); track++)
		{
			int datasize = 0, totalsize = 0;
			for(int block = 0; block < blockcount; block++)
			{
				ADX adx;
				m_file[block][track].Rewind();		// currentPosを戻す必要あり
				adx.Load( & m_file[block][track] );

				m_wav_concat[track].Append( adx.m_wav );
			}
		}

		return true;
	}

	bool Load(std::string infile, bool bExtractADX = false)
	{
		File file;
		if( ! file.LoadFromFile(infile.c_str()) )
		{
			printf("Failed to open %s\n", infile.c_str());
			return false;
		}

		if( ! this->Load(&file, bExtractADX) )
		{
			printf("Failed to load %s\n", infile.c_str());
			return false;
		}
		return true;
	}


	void Play(int number)
	{
		if(m_wav_concat.size() < number+1)
		{
			printf("Unable to play channel #%d\n", number);
			return;
		}
		m_wav_concat[number].Play();
	}


	void ExtractAllToWave(char* filename)
	{
		for(int track = 0; track < m_wav_concat.size(); track++)
		{
			char buf[MAX_PATH];
			sprintf_s(buf, sizeof(buf), "%s%02d.wav", filename, track);
			m_wav_concat[track].ExtractWav( buf );
		}
	}

};


}	// namespace

#endif