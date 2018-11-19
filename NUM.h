#ifndef _NUM_HEADER_
#define _NUM_HEADER_


#include "File.h"

#include <vector>

namespace imas{


struct NUM
{
public:

	unsigned char unknown0[3];
	int unknown1;
	int unknown2[8];
	int unknown3[7];

//	int fileSize;		// Write時要計算
//	int dataSize;		// Write時要計算

	struct Ch{
		short data1;
		short data2;
		short data3;
	};

	struct Key{
		std::vector<Ch> ch;
	};

	std::vector<Key> key;

public:
	bool Load( File* pFile )
	{
		unsigned int topPosition = pFile->CurrentPosition();
		
		if( pFile->ReadByte() != 'N' ) return false;
		if( pFile->ReadByte() != 'U' ) return false;
		if( pFile->ReadByte() != 'M' ) return false;
		if( pFile->ReadByte() != 'O' ) return false;
		if( pFile->ReadByte() != 'T' ) return false;

		unknown0[0] = pFile->ReadByte();
		unknown0[1] = pFile->ReadByte();
		unknown0[2] = pFile->ReadByte();

		unknown1 = pFile->ReadUInt32();

		int fileSize;
		fileSize = pFile->ReadUInt32();

		unknown2[0] = pFile->ReadUInt32();
		unknown2[1] = pFile->ReadUInt32();
		unknown2[2] = pFile->ReadUInt32();
		unknown2[3] = pFile->ReadUInt32();
		unknown2[4] = pFile->ReadUInt32();
		unknown2[5] = pFile->ReadUInt32();
		unknown2[6] = pFile->ReadUInt32();
		unknown2[7] = pFile->ReadUInt32();

		int dataSize;
		dataSize = pFile->ReadUInt32();

		unknown3[0] = pFile->ReadUInt32();
		unknown3[1] = pFile->ReadUInt32();
		unknown3[2] = pFile->ReadUInt32();
		unknown3[3] = pFile->ReadUInt32();
		unknown3[4] = pFile->ReadUInt32();
		unknown3[5] = pFile->ReadUInt32();

		unknown3[6] = pFile->ReadUInt32();


		int nKey = pFile->ReadUInt16();
		key.resize( nKey );

		int nCh;
		nCh = pFile->ReadUInt16();

		for(int i=0; i<nKey; i++)
		{
			key[i].ch.resize( nCh );

			for(int k=0; k<nCh; k++)
			{
				key[i].ch[k].data1 = pFile->ReadInt16();
				key[i].ch[k].data2 = pFile->ReadInt16();
				key[i].ch[k].data3 = pFile->ReadInt16();
			}
		}

		// skip blank
		pFile->Jump( topPosition + fileSize );

		return true;
	}

	void Write( File* pFile )
	{
		pFile->WriteByte('N');
		pFile->WriteByte('U');
		pFile->WriteByte('M');
		pFile->WriteByte('O');
		pFile->WriteByte('T');
		pFile->WriteByte( unknown0[0] );
		pFile->WriteByte( unknown0[1] );
		pFile->WriteByte( unknown0[2] );

		pFile->WriteUInt32( unknown1 );
		pFile->WriteUInt32( 0 );			// fileSize

		pFile->WriteUInt32( unknown2[0] );
		pFile->WriteUInt32( unknown2[1] );
		pFile->WriteUInt32( unknown2[2] );
		pFile->WriteUInt32( unknown2[3] );
		pFile->WriteUInt32( unknown2[4] );
		pFile->WriteUInt32( unknown2[5] );
		pFile->WriteUInt32( unknown2[6] );
		pFile->WriteUInt32( unknown2[7] );

		pFile->WriteUInt32( 0 );		// dataSize

		pFile->WriteUInt32( unknown3[0] );
		pFile->WriteUInt32( unknown3[1] );
		pFile->WriteUInt32( unknown3[2] );
		pFile->WriteUInt32( unknown3[3] );
		pFile->WriteUInt32( unknown3[4] );
		pFile->WriteUInt32( unknown3[5] );
		pFile->WriteUInt32( unknown3[6] );

		pFile->WriteUInt16( key.size() );
		pFile->WriteUInt16( key[0].ch.size() );

		for(int i=0; i<key.size(); i++)
		{
			for(int k=0; k<key[i].ch.size(); k++)
			{
				pFile->WriteInt16( key[i].ch[k].data1 );
				pFile->WriteInt16( key[i].ch[k].data2 );
				pFile->WriteInt16( key[i].ch[k].data3 );
			}
		}

		int dataSize = 4 + 2 * 3 * key[0].ch.size() * key.size();
		int fileSize = dataSize + 80;
		fileSize = (fileSize / 16 + 1 ) * 16;	// 16byte alignment

		pFile->WriteNullArray( fileSize - pFile->CurrentPosition() );

		pFile->Jump(12);
		pFile->WriteUInt32( fileSize );

		pFile->Jump(48);
		pFile->WriteUInt32( dataSize );
	}

	void Write(File& file)
	{
		file.SetEndianness(File::BIG_ENDIAN);

		Write( & file );
	}

	void Write(std::string filename)
	{
		File out(filename);

		Write( out );

		out.WriteToFile();
	}
};


};	// namespace imas

#endif