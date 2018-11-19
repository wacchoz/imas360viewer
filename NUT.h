#ifndef _NUT_HEADER_
#define _NUT_HEADER_

#include "File.h"
#include <vector>
#include <map>
#include <string.h>

#include "squish/squish.h"


namespace imas{


struct TextureData
{
	//	int texture_data_size;	// ヘッダを含むサイズ
	int unknown0;
	//	int image_data_size;		// ヘッダを含まないサイズ
	//	int header_size;
	int unknown1;
	int nMipmap;
	int pixel_type;
	
	int width;
	int height;

	int unknown2;
	int unknown3;
	int unknown4;
	int unknown5;
	int unknown6;
	int unknown7;

	std::vector<int> mipmap_size;

	// eXt
	int unknown8;
	int unknown9;
	int unknown10;

	// GIDX
	int unknown11;
	int GIDX;
	int unknown12;

	// imageの実データ
	std::vector<unsigned char> raw_texture;

	// imageの展開データ(squishで展開)
	std::vector<unsigned char> decompressed_texture;


	bool Load( File* pFile )
	{
		unsigned int topPos = pFile->CurrentPosition();

		// texture header
		int texture_data_size = pFile->ReadUInt32();
		unknown0 = pFile->ReadUInt32();				// always 0
		int image_data_size = pFile->ReadUInt32();
		int header_size = pFile->ReadUInt16();
		unknown1 = pFile->ReadUInt16();				// always 0
		nMipmap = pFile->ReadUInt16();
		pixel_type = pFile->ReadUInt16();
		width = pFile->ReadUInt16();
		height = pFile->ReadUInt16();

		unknown2 = pFile->ReadUInt32();				// always 0
		unknown3 = pFile->ReadUInt32();				// always 0
		unknown4 = pFile->ReadUInt32();				// always 0
		unknown5 = pFile->ReadUInt32();				// always 0
		unknown6 = pFile->ReadUInt32();				// always 0
		unknown7 = pFile->ReadUInt32();				// always 0

		if( nMipmap > 1 )
		{
			mipmap_size.resize( nMipmap );

			for(int i=0; i<nMipmap; i++)
			{
				mipmap_size[i] = pFile->ReadUInt32();
			}
		}
		
		// ここに16byte alignmentのためのpaddingあるため、飛ばす

		pFile->Skip( (16-pFile->CurrentPosition()%16)%16 );


		// EXT

		if( pFile->ReadByte() != 'e' ) return false;
		if( pFile->ReadByte() != 'X' ) return false;
		if( pFile->ReadByte() != 't' ) return false;
		if( pFile->ReadByte() != 0 ) return false;

		unknown8 = pFile->ReadUInt32();				// always 32
		unknown9 = pFile->ReadUInt32();				// always 16
		unknown10 = pFile->ReadUInt32();			// always 0

		// GIDX 

		if( pFile->ReadByte() != 'G' ) return false;
		if( pFile->ReadByte() != 'I' ) return false;
		if( pFile->ReadByte() != 'D' ) return false;
		if( pFile->ReadByte() != 'X' ) return false;

		unknown11 = pFile->ReadUInt32();			// always 16
		GIDX = pFile->ReadUInt32();
		unknown12 = pFile->ReadUInt32();			// always 0

		raw_texture.resize( image_data_size );
		pFile->ReadByteArray( & raw_texture[0], image_data_size );

		Decompress();	// 展開

		return true;
	}
	
	void Decompress()
	{

		if( pixel_type == 0 || pixel_type == 1 || pixel_type == 2)	// DXT1,DXT3,DXT5
		{
			int decompress_type;
			switch(pixel_type){
				case 0:
					decompress_type = squish::kDxt1; break;
				case 1:
					decompress_type = squish::kDxt3; break;
				case 2:
					decompress_type = squish::kDxt5; break;
			}
			// エンディアン
			std::vector<unsigned char> tmp_texture = raw_texture;
			for(int k = 0; k < tmp_texture.size() / 2; k++){
				unsigned char tmp;
				tmp = tmp_texture[k*2];
				tmp_texture[k*2] = tmp_texture[k*2+1];
				tmp_texture[k*2+1]=tmp;
			}
			// squishで展開
			decompressed_texture.resize( width * height * 4);
			squish::DecompressImage(& decompressed_texture[0], width, height, &tmp_texture[0], decompress_type);
		}
		else if(pixel_type==19 || pixel_type==20)	//  ARGB32
		{
			decompressed_texture.resize( width * height * 4);
			for(int k = 0; k < width * height; k++)
			{
				int r, g, b, a;
				a = decompressed_texture[k*4+0];
				r = decompressed_texture[k*4+1];
				g = decompressed_texture[k*4+2];
				b = decompressed_texture[k*4+3];

				decompressed_texture[k*4+0] = r;
				decompressed_texture[k*4+1] = g;
				decompressed_texture[k*4+2] = b;
				decompressed_texture[k*4+3] = a;
			}
		}
	}

	void Write( File* pFile )
	{
		int header_size = 4+4+4+2+2+2+2+2+2+4+4+4+4+4+4;	// 48
		if( mipmap_size.size() > 1) header_size += ((mipmap_size.size()-1)/16+1)*16;
		header_size += 16+16;	// eXt + GIDX

		int image_data_size = raw_texture.size();
		int texture_data_size = header_size + image_data_size;

		pFile->WriteUInt32( texture_data_size );
		pFile->WriteUInt32( unknown0 );
		pFile->WriteUInt32( image_data_size );
		pFile->WriteUInt16( header_size );
		pFile->WriteUInt16( unknown1 );
		pFile->WriteUInt16( nMipmap );
		pFile->WriteUInt16( pixel_type );
		pFile->WriteUInt16( width );
		pFile->WriteUInt16( height );

		pFile->WriteUInt32( unknown2 );
		pFile->WriteUInt32( unknown3 );
		pFile->WriteUInt32( unknown4 );
		pFile->WriteUInt32( unknown5 );
		pFile->WriteUInt32( unknown6 );
		pFile->WriteUInt32( unknown7 );

		if( mipmap_size.size() > 1 )
		{
			for(int i=0; i<mipmap_size.size(); i++)
			{
				pFile->WriteUInt32( mipmap_size[i] );
			}
			pFile->WriteNullArray( (16 - mipmap_size.size() % 16) % 16 ); 
		}

		pFile->WriteByteArray((unsigned char*) "eXt\0", 4);
		pFile->WriteUInt32( unknown8 );
		pFile->WriteUInt32( unknown9 );
		pFile->WriteUInt32( unknown10 );

		pFile->WriteByteArray((unsigned char*) "GIDX", 4);
		pFile->WriteUInt32( unknown11 );
		pFile->WriteUInt32( GIDX );
		pFile->WriteUInt32( unknown12 );

		pFile->WriteByteArray( &raw_texture[0], raw_texture.size() );
	}

};


struct NUT
{
public:
	std::vector<TextureData> texture_data;

	int unknown0;
	int unknown1;
	int unknown2;
	int unknown3;
	int unknown4;

public:
	bool Load( std::string filename )
	{
		File file;
		if( ! file.LoadFromFile(filename) ) return false;

		return Load( & file );
	}

	bool Load( File* pFile )
	{
		if( pFile->ReadUInt32() != 'NTXR' ) return false;

		unknown0 = pFile->ReadUInt16();				// always 256

		int texture_count;
		texture_count = pFile->ReadUInt16();

		unknown1 = pFile->ReadUInt16();				// always 0
		unknown2 = pFile->ReadUInt16();				// always 0
		unknown3 = pFile->ReadUInt16();				// always 0
		unknown4 = pFile->ReadUInt16();				// always 0

		texture_data.resize( texture_count );
		for(int i=0; i<texture_count; i++)
		{
			texture_data[i].Load( pFile );
		}

		return true;
	}

	void Write( File& file )
	{
		file.SetEndianness( File::BIG_ENDIAN );

		file.WriteByteArray((unsigned char*) "NTXR", 4);
		file.WriteUInt16( unknown0 );
		file.WriteUInt16( texture_data.size() );
		file.WriteUInt16( unknown1 );
		file.WriteUInt16( unknown2 );
		file.WriteUInt16( unknown3 );
		file.WriteUInt16( unknown4 );

		for(int i=0; i<texture_data.size(); i++)
		{
			texture_data[i].Write( & file );
		}
	}
		
	void Write( std::string filename )
	{
		File out(filename);

		Write( out );

		out.WriteToFile();
	}

	void ExportDDS();

};


struct DDS_HEADER
{
	int dwMagic;		// 'DDS '

	int dwSize;			// 構造体のサイズ。常に124
	int dwFlags;		// 有効なフィールドを示すフラグ
	int dwHeight;		// イメージの高さ
	int dwWidth;		// イメージの幅
	int dwPitchOrLinearSize;		// 圧縮フォーマットの場合はイメージの総バイト数
	int dwDepth;
	int dwMipMapCount;
	int dwReserved1[11];

	struct DDPIXELFORMAT
	{
		int dwSize;
		int dwFlags;
		int dwFourCC;
		int dwRGBBitCount;
		int dwRBitMask;
		int dwGBitMask;
		int dwBBitMask;
		int dwRGBAlphaBitMask;
	} ddpfPixelFormat;

	struct DDCAPS2
	{
		int dwCaps1;
		int dwCaps2;
		int Reserved[2];
	} ddsCaps;

	int dwReserved2;
};

void NUT::ExportDDS()
{
	for(int i=0; i< texture_data.size(); i++)
	{
		DDS_HEADER header;

		header.dwMagic = ' SDD';
		header.dwSize = 124;
		header.dwFlags = 0x000A1007;
		header.dwHeight = texture_data[i].height;
		header.dwWidth = texture_data[i].width;
		header.dwPitchOrLinearSize = 0;
		header.dwDepth = 0;
		header.dwMipMapCount = texture_data[i].nMipmap;
		header.dwReserved1[0] = 0;
		header.dwReserved1[1] = 0;
		header.dwReserved1[2] = 0;
		header.dwReserved1[3] = 0;
		header.dwReserved1[4] = 0;
		header.dwReserved1[5] = 0;
		header.dwReserved1[6] = 0;
		header.dwReserved1[7] = 0;
		header.dwReserved1[8] = 0;
		header.dwReserved1[9] = 0;
		header.dwReserved1[10] = 0;
		header.dwReserved1[11] = 0;

		header.ddpfPixelFormat.dwSize = 32;
		header.ddpfPixelFormat.dwFlags = 0x000004;

		header.ddpfPixelFormat.dwRGBBitCount = 0;
		header.ddpfPixelFormat.dwRBitMask = 0;
		header.ddpfPixelFormat.dwGBitMask = 0;
		header.ddpfPixelFormat.dwBBitMask = 0;
		header.ddpfPixelFormat.dwRGBAlphaBitMask = 0;

		switch(texture_data[i].pixel_type){
			case 0:
				header.ddpfPixelFormat.dwFourCC = '1TXD';
				break;
			case 1:
				header.ddpfPixelFormat.dwFourCC = '3TXD';
				break;
			case 2:
				header.ddpfPixelFormat.dwFourCC = '5TXD';
				break;
			case 19:
				// RGB32
			case 20:
				// ARGB32
				header.dwFlags = 0x0002100F;
				header.ddpfPixelFormat.dwFlags = 0x00000041;
				header.ddpfPixelFormat.dwFourCC = 0;

				header.ddpfPixelFormat.dwRGBBitCount = 32;
				header.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
				header.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
				header.ddpfPixelFormat.dwBBitMask = 0x000000FF;
				header.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
		}


		header.ddsCaps.dwCaps1 = 0x00401008;
		header.ddsCaps.dwCaps2 = 0;
		header.ddsCaps.Reserved[0] = 0;
		header.ddsCaps.Reserved[1] = 0;

		header.dwReserved2 = 0;

		std::vector< unsigned char > dds_data;

		dds_data.resize( sizeof(header) + texture_data[i].raw_texture.size() );
		memcpy(&dds_data[0], &header, sizeof(header));
		memcpy(&dds_data[0]+sizeof(header), &texture_data[i].raw_texture[0], texture_data[i].raw_texture.size());

		int pixel_type = texture_data[i].pixel_type;
		if( pixel_type == 0 || pixel_type==1 || pixel_type==2)
		{
			int tmp;
			for(int k=sizeof(header); k< sizeof(header) + texture_data[i].raw_texture.size(); k+=2)
			{
				tmp = dds_data[k];
				dds_data[k] =dds_data[k+1];
				dds_data[k+1]=tmp;
			}
		}else
		{
			int tmp;
			for(int k=sizeof(header); k< sizeof(header) + texture_data[i].raw_texture.size(); k+=4)
			{
				tmp = dds_data[k];
				dds_data[k] =dds_data[k+3];
				dds_data[k+3]=tmp;

				tmp = dds_data[k+1];
				dds_data[k+1] =dds_data[k+2];
				dds_data[k+2]=tmp;
			}
		}

		char str[30];
		sprintf_s(str, sizeof(str), "%X.dds", texture_data[i].GIDX);

		FILE* fp;
		errno_t err = fopen_s(&fp, str, "wb");
		if( err ) return;
		fwrite( &dds_data[0], dds_data.size(), 1, fp);
		fclose( fp );
	}
}

};	// namespace imas

#endif