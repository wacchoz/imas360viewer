#ifndef _FILE_HEADER_
#define _FILE_HEADER_

#include <vector>
#include <string>

#include <assert.h>

namespace{
	unsigned int ChangeEndianUInt32(unsigned int i)
	{
		return ((i<<24) & 0xff000000) | ((i<<8) & 0x00ff0000) | ((i>>8) & 0x0000ff00) | ((i>>24) & 0x000000ff);
	}

	unsigned short ChangeEndianUInt16(unsigned short i)
	{
		return ((i<<8) & 0xff00) | ((i>>8) & 0x00ff) ;
	}

	float ChangeEndianSingle(float f)
	{
		unsigned int i = * (unsigned int*) (&f);
		unsigned int j = ((i<<24) & 0xff000000) | ((i<<8) & 0x00ff0000) | ((i>>8) & 0x0000ff00) | ((i>>24) & 0x000000ff);
		return * (float*) (&j);
	}
}


class File
{
private:
	std::vector<unsigned char> m_data;
	std::string m_filename;
	std::string m_directory;

public:
	enum Endian{ BIG_ENDIAN, LITTLE_ENDIAN };
private:
	Endian m_endian;
	unsigned int m_topPos;		// 開始位置（Jump、Rewind、pDataTopに影響する）
	unsigned int m_currentPos;	// m_topPosを0としたときの現在位置

public:
	File(): m_topPos(0), m_currentPos(0){}
	~File(){}

	File(std::string filename, std::string directory = "", Endian endian = BIG_ENDIAN)
	{
		m_filename = filename;
		m_directory = directory;
		m_endian = endian;
		m_topPos = 0;
		m_currentPos = 0;
	}

	File(unsigned char* pData, int size, std::string filename = "", std::string directory = "", Endian endian = BIG_ENDIAN)
	{
		assert( pData != NULL );

		LoadFromMemory(pData, size, filename, directory);
	}

	bool LoadFromFile(std::string filename, std::string directory = "", Endian endian = BIG_ENDIAN)
	{
		m_filename = filename;
		m_directory = directory;
		m_endian = endian;
		m_topPos = 0;
		m_currentPos = 0;

		FILE* fp;
		fpos_t fposFileSize;

		errno_t err = fopen_s( &fp, & filename[0], "rb");
		if( err ) return false;

		fseek(fp, 0, SEEK_END);
		fgetpos(fp, &fposFileSize);

		m_data.resize((int)fposFileSize);

		fseek(fp, 0, SEEK_SET);
		fread(&m_data[0], 1, (size_t)fposFileSize, fp);

		fclose(fp);

		return true;
	}

	bool LoadFromMemory( unsigned char* pTop, int size, std::string filename = "", std::string directory = "", Endian endian = BIG_ENDIAN )
	{
		m_data.resize(size);
		memcpy(&m_data[0], pTop, size);
		
		m_filename = filename;
		m_directory = directory;
		m_endian = endian;
		m_topPos = 0;
		m_currentPos = 0;

		return true;
	}

	void Clear()
	{
		m_data.resize( 0 );
	}

	std::string FileName()
	{
		return m_filename;
	}

	std::string DirectoryName()
	{
		return m_directory;
	}

	void SetFileName( std::string filename )
	{
		m_filename = filename;
	}

	void SetDirectoryName( std::string directory )
	{
		m_directory = directory;
	}

	unsigned int FileSize()
	{
		return m_data.size();
	}

	// 取扱注意
	unsigned char* pDataTop()
	{
		return & m_data[ m_topPos ];
	}

	void Append(File* pFile)
	{
		m_data.insert(m_data.end(), pFile->m_data.begin(), pFile->m_data.end());
	}

	// 実ファイルへの書き出し
	void WriteToFile(std::string path)
	{
		FILE* fp;
		errno_t err = fopen_s(&fp, path.c_str(), "wb");
		if( err ) return;
		fwrite(&m_data[0], m_data.size(), 1, fp);
		fclose(fp);
	}

	void WriteToFile()
	{
		if(m_filename.length() == 0 )
		{
			printf("Error: File name is null in WriteToFile()\n");
			return;
		}

		WriteToFile( m_filename );
	}


	
public:

	/////////////////
	//  Set関係
	/////////////////

	void SetEndianness(Endian endian)
	{
		m_endian = endian;
	}

	void SetTopAddress(unsigned int topPos)
	{
		m_topPos = topPos;
	}

	/////////////////
	//  Get関係
	/////////////////
	unsigned int CurrentPosition()
	{
		// topを0としたときのCurrentPosition
		// Jumpと整合性が取れている必要あり
		return m_currentPos;
	}

	Endian Endianness()
	{
		return m_endian;
	}

	unsigned char* pCurrentData()
	{
		return pDataTop() + m_currentPos;
	}

	/////////////////
	//  Jump関係
	/////////////////

	void Jump(unsigned int offset)	// offsetはtopを0としたときの値
	{
		m_currentPos = offset;

		assert( m_topPos + m_currentPos <= FileSize() );
	}

	void Rewind()
	{
		m_currentPos = 0;
	}

	void JumpToLast()	// ファイルの最後へ移動
	{
		m_currentPos = m_data.size() - m_topPos;	// この位置にデータは無いので注意
	}

	void Skip(int offset)
	{
		m_currentPos += offset;

		assert( m_topPos + m_currentPos <= FileSize() );
	}


	//////////////
	//  Read関係
	//////////////
	unsigned char ReadByte()
	{
		assert( m_topPos + m_currentPos <= FileSize() - sizeof(char) );

		unsigned char c;
		c = * pCurrentData();
		m_currentPos += 1;
		return c;
	}

	unsigned short ReadUInt16()
	{
		assert( m_topPos + m_currentPos <= FileSize() - sizeof(short) );

		unsigned short i;
		i = * (unsigned short*) pCurrentData();
		m_currentPos += 2;

		if( m_endian == BIG_ENDIAN)
			return ChangeEndianUInt16(i);
		else
			return i;
	}

	short ReadInt16()
	{
		assert( m_topPos + m_currentPos <= FileSize() - sizeof(short) );

		short i;
		i = * (short*) pCurrentData();
		m_currentPos += 2;

		if( m_endian == BIG_ENDIAN)
			return ChangeEndianUInt16(i);
		else
			return i;
	}

	unsigned int ReadUInt32()
	{
		assert( m_topPos + m_currentPos <= FileSize() - sizeof(int) );

		unsigned int i;
		i = * (unsigned int*) pCurrentData();
		m_currentPos += 4;

		if( m_endian == BIG_ENDIAN)
			return ChangeEndianUInt32(i);
		else
			return i;
	}

	int ReadInt32()
	{
		assert( m_topPos + m_currentPos <= FileSize() - sizeof(int) );

		int i;
		i = * (int*) pCurrentData();
		m_currentPos += 4;

		if( m_endian == BIG_ENDIAN)
			return ChangeEndianUInt32(i);
		else
			return i;
	}

	float ReadSingle()
	{
		assert( m_topPos + m_currentPos <= FileSize() - sizeof(float) );

		float f;
		f = * (float*) pCurrentData();
		m_currentPos += 4;

		if( m_endian == BIG_ENDIAN)
			return ChangeEndianSingle(f);		
		else
			return f;
	}

	void ReadByteArray(unsigned char* data, int size)
	{
		assert( m_topPos + m_currentPos <= FileSize() - size );	// 合ってるか？？？？

		for(int i=0; i<size; i++)
		{
			data[i] = *(unsigned char*) (pCurrentData() + i);
		}
		m_currentPos += size;
	}

	//////////////
	//  Write関係
	//////////////

	void WriteByte(unsigned char c)
	{
		if( m_data.size() - ( m_topPos + CurrentPosition() ) < 1) m_data.resize( m_topPos + CurrentPosition() + 1 );
		 * (unsigned char*) (pDataTop() + m_currentPos) = c;
		m_currentPos += 1;
	}

	void WriteUInt16( unsigned short i )
	{
		if( m_data.size() - ( m_topPos + CurrentPosition() ) < 2) m_data.resize( m_topPos + CurrentPosition() + 2 );

		unsigned short a;
		if( m_endian == BIG_ENDIAN)
			a = ChangeEndianUInt16((unsigned short)i);
		else
			a = (unsigned short)i;

		 * (unsigned short*) (pDataTop() + m_currentPos) = a;
		m_currentPos += 2;
	}

	void WriteInt16( short i )
	{
		if( m_data.size() - ( m_topPos + CurrentPosition() ) < 2) m_data.resize( m_topPos + CurrentPosition() + 2 );

		unsigned short a;
		if( m_endian == BIG_ENDIAN)
			a = ChangeEndianUInt16((unsigned short)i);
		else
			a = (unsigned short) i;

		 * (unsigned short*) (pDataTop() + m_currentPos) = a;

		m_currentPos += 2;
	}

	void WriteUInt32( unsigned int i )
	{
		if( m_data.size() - ( m_topPos + CurrentPosition() ) < 4) m_data.resize( m_topPos + CurrentPosition() + 4 );
		unsigned int a;
		if( m_endian == BIG_ENDIAN)
			a = ChangeEndianUInt32((unsigned int)i);
		else
			a = (unsigned int) i;

		 * (unsigned int*) (pDataTop() + m_currentPos) = a;
		m_currentPos += 4;
	}

	void WriteInt32( int i )
	{
		if( m_data.size() - ( m_topPos + CurrentPosition() ) < 4) m_data.resize( m_topPos + CurrentPosition() + 4 );
		unsigned int a;
		if( m_endian == BIG_ENDIAN)
			a = ChangeEndianUInt32((unsigned int)i);
		else
			a = (unsigned int) i;

		 * (unsigned int*) (pDataTop() + m_currentPos) = a;
		m_currentPos += 4;
	}

	void WriteSingle( float f)
	{
		if( m_data.size() - ( m_topPos + CurrentPosition() ) < 4) m_data.resize( m_topPos + CurrentPosition() + 4 );
		float a;
		if( m_endian == BIG_ENDIAN)
			a = ChangeEndianSingle(f);
		else
			a = f;

		 * (unsigned int*) (pDataTop() + m_currentPos) = * (unsigned int*) & a;
		m_currentPos += 4;
	}

	void WriteByteArray( unsigned char* data, int size)
	{
		if( m_data.size() - ( m_topPos + CurrentPosition() ) < size ) m_data.resize( m_topPos + CurrentPosition() + size );

		for(int i=0; i<size; i++)
		 * (unsigned char*) (pDataTop() + m_currentPos + i) = * (unsigned char*) & data[i];

		m_currentPos += size;
	}

	void WriteNullArray( int count )
	{
		for(int i=0; i<count; i++)
		{
			WriteByte(0);
		}
	}
};

#endif