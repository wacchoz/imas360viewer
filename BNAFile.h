#ifndef _BNAFILE_HEADER_
#define _BNAFILE_HEADER_

#include "File.h"

#include <list>
#include <string>

namespace imas{


class BNAFile
{
public:
	std::list<File> m_filelist;		// listなら挿入削除でポインタが変わらないはずなので、Fileへのポインタを外に渡せる
	std::string m_filename;

public:
	bool Load( std::string filename )
	{
		m_filename = filename;

		File bna;
		if( ! bna.LoadFromFile( filename, "", File::BIG_ENDIAN ) ) return false;

		return  Load( & bna );
	}

	bool Load( File* pFile )
	{
		m_filename = pFile->FileName();

		if( pFile->ReadInt32() != 'BNA0' ) return false;

		int nFile = pFile->ReadUInt32();

		for(int i=0; i<nFile; i++)
		{
			unsigned int dirOffset, filenameOffset, dataOffset, filesize;

			// read directory name
			dirOffset = pFile->ReadUInt32();
			std::string directoryName( (char*) pFile->pDataTop() + dirOffset );

			// read file name
			filenameOffset = pFile->ReadInt32();
			std::string fileName( (char*) pFile->pDataTop() + filenameOffset );

			// read data offset
			dataOffset = pFile->ReadUInt32();

			filesize = pFile->ReadUInt32();
			
			m_filelist.push_back( File( pFile->pDataTop() + dataOffset, filesize, fileName, directoryName, File::BIG_ENDIAN) );

		}
		return true;
	}

	File* GetFile( std::string filename )
	{
		std::list<File>::iterator p;
		for( p = m_filelist.begin(); p != m_filelist.end(); ++p){
			if( (*p).FileName() == filename)
			{
				return &(*p);
			}
		}
		return NULL;
	}

	File* GetFileByFilter( std::string filter )
	{
		std::list<File>::iterator p;
		for( p = m_filelist.begin(); p != m_filelist.end(); ++p){
			if( (*p).FileName().find( filter ) != std::string::npos )
			{
				return &(*p);
			}
		}
		return NULL;
	}

	void Write( std::string filename )
	{
		File out(filename);

		out.SetEndianness(File::BIG_ENDIAN);

		int nFile = m_filelist.size();

		std::vector<unsigned int> dirOffset, filenameOffset, dataOffset, filesize;

		out.WriteNullArray( 8 + 16* nFile );	// header is written later


		std::list<File>::iterator p;

		// write directory name（duplication is not considered）
		for( p = m_filelist.begin(); p != m_filelist.end(); ++p)
		{
			dirOffset.push_back( out.CurrentPosition() );
			out.WriteByteArray( (unsigned char*) (*p).DirectoryName().c_str(), (*p).DirectoryName().length()+1);
		}

		// write file name
		for( p = m_filelist.begin(); p != m_filelist.end(); ++p)
		{
			filenameOffset.push_back( out.CurrentPosition() );
			out.WriteByteArray( (unsigned char*) (*p).FileName().c_str(), (*p).FileName().length()+1);
		}

		// write data
		for( p = m_filelist.begin(); p != m_filelist.end(); ++p)
		{
			out.WriteNullArray( 4096 - out.CurrentPosition() % 4096 );	// this large alignment is for workability of manual edit.

			dataOffset.push_back( out.CurrentPosition() );
			filesize.push_back( (*p).FileSize() );
			out.WriteByteArray( (*p).pDataTop(), (*p).FileSize());
		}

		out.WriteNullArray( 1024 - out.CurrentPosition() % 1024 );	// padding


		// write header
		out.Jump(0);

		out.WriteByteArray((unsigned char*)"BNA0", 4);
		out.WriteUInt32( m_filelist.size() );

		for( int i=0; i < nFile; i++)
		{
			out.WriteUInt32( dirOffset[i] );
			out.WriteUInt32( filenameOffset[i] );
			out.WriteUInt32( dataOffset[i] );
			out.WriteUInt32( filesize[i] );
		}

		out.WriteToFile();

	}

	void ExtractAllFiles()	// directory name is neglected
	{
		printf("Now Extracting...\n");

		std::list<File>::iterator p;
		for( p = m_filelist.begin(); p != m_filelist.end(); ++p)
		{
			printf(" - %s\n", (*p).FileName().c_str());
			(*p).WriteToFile();
		}
	}
};


};	// namespace imas

#endif