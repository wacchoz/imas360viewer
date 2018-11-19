#ifndef _MPK_H_
#define _MPK_H_

#include "File.h"
#include "NUM.h"

#include <vector>

namespace imas{

class MPK
{

public:
	std::vector<NUM> num;

private:
	struct OFFSET_INDEX
	{
		int index;			// 0xFFFFFFFF はdummy
		unsigned int offset_address;
	};
	std::vector<OFFSET_INDEX> offset_index;

	enum MOTION_TYPE{ NORMAL_IDLE = 0, IDLE = 1, TRANSITION = 2, SPECIAL = 3, YES = 1000, NO = 1001};

	struct NUM_INFO
	{
		int motion_ID;	// SPECIAL以外で使用
		MOTION_TYPE motion_type;
		int transition_end; // TRANSITIONのとき、begin->end。SPECIALのとき、begin->special->end
		int transition_begin;
		int special_ID;
	};
	std::vector<NUM_INFO> num_info;

public:

	bool Load( File* file ){

		if( file->ReadByte() != 'M' ) return false;
		if( file->ReadByte() != 'P' ) return false;
		if( file->ReadByte() != 'K' ) return false;
		file->ReadByte();

		int index_count = file->ReadUInt32();

		// index/offset
		offset_index.resize( index_count );
		for(int i=0; i<index_count; i++)
		{
			offset_index[i].index = file->ReadInt32();
			offset_index[i].offset_address = file->ReadUInt32();
		}


		// NUM_INFO
		int info_count = ( index_count - 1 ) / 3;
		num_info.resize( info_count );
		for(int i=0; i<info_count; i++)
		{
			num_info[i].motion_ID = file->ReadInt32();
			num_info[i].motion_type = (MOTION_TYPE) file->ReadInt32();
			num_info[i].transition_end = file->ReadInt32();
			num_info[i].transition_begin = file->ReadInt32();
			num_info[i].special_ID = file->ReadInt32();
		}

		num.resize( index_count - 1 );

		for(int i=1; i<index_count; i++){
			if(offset_index[i].index != 0xFFFFFFFF)
			{
				file->Jump( offset_index[i].offset_address );
				num[i-1].Load( file );	// 1個目はオフセットテーブルへのオフセットなので無視
			}
			else
			{
				//	num[i] = NULL;
			}
		}

		return true;
	};

	bool Load( std::string infile )
	{
		File file;

		if( ! file.LoadFromFile(infile.c_str()) )
		{
			printf("Failed to open %s\n", infile.c_str());
			return false;
		}

		if( ! this->Load(&file) )
		{
			printf("Failed to load %s\n", infile.c_str());
			return false;
		}
		return true;
	}

	/////////////未検証！！！！！！！！！！！ OFFSET計算していない
	void Write( File& file )
	{
		file.WriteByte('M');
		file.WriteByte('P');
		file.WriteByte('K');
		file.WriteByte(0);

		int index_count = offset_index.size();

		file.WriteUInt32( index_count );
		for(int i=0; i<index_count; i++)
		{
			file.WriteUInt32( offset_index[i].index );
			file.WriteUInt32( offset_index[i].offset_address );
		}

		int info_count = ( index_count - 1 ) / 3;
		for(int i=0; i<info_count; i++)
		{
			file.WriteInt32( num_info[i].motion_ID );
			file.WriteInt32( num_info[i].motion_type );
			file.WriteInt32( num_info[i].transition_end );
			file.WriteInt32( num_info[i].transition_begin );
			file.WriteInt32( num_info[i].special_ID );
		}

		for(int i=1; i<index_count; i++){
			if(offset_index[i].index != 0xFFFFFFFF)
			{
				num[i-1].Write( file );
			}
		}
	}

	void Write( std::string filename )
	{
		File out(filename);

		Write( out );

		out.WriteToFile();
	}
};

} // namespace

#endif