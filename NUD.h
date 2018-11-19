#ifndef _NUD_HEADER_
#define _NUD_HEADER_

#include "File.h"
#include <vector>
#include "d3dx9.h"

#include <assert.h>
#include <set>


namespace imas{


struct Index
{
	std::vector<short> index;	// 生データ
	std::vector<short> index_degenerate_triangle;	// indexをdegenerate triangleに変換

	int reserved;
	int unknown;

	bool Load( File* pFile)
	{
		// read Index of TRIANGLE_STRIP

		unsigned short nIndex;
		nIndex = pFile->ReadUInt16();

		reserved = pFile->ReadUInt16();
		unknown = pFile->ReadUInt32();

		index.resize( nIndex );
		for(int i = 0; i < nIndex; i++)
		{
			index[i] = pFile->ReadUInt16();
		}

		ConvertToDegenerateTriangle(); // 変換

		return true;
	}

	void ConvertToDegenerateTriangle()
	{
		for(int i=0; i< index.size(); i++)
		{
			if( index[i] == (short)(-1) )
			{
				if( index_degenerate_triangle.size() % 2 == 0 )
				{
					index_degenerate_triangle.push_back( index[i-1] );
					index_degenerate_triangle.push_back( index[i+1] );
				}else
				{
					index_degenerate_triangle.push_back( index[i-1] );
					index_degenerate_triangle.push_back( index[i+1] );
					index_degenerate_triangle.push_back( index[i+1] );
				}
			}else
			{
				index_degenerate_triangle.push_back( index[i] );
			}
		}
	}

	void Write( File* pFile )
	{
		pFile->WriteUInt16( index.size() );
		pFile->WriteUInt16( reserved );
		pFile->WriteUInt32( unknown );
		
		for(int i=0; i<index.size(); i++)
		{
			pFile->WriteUInt16( index[i] );
		}
	}

	int WriteSize()
	{
		return 2+2+4+2*index.size();
	}

};

struct Vertex
{
	unsigned short nType;

	int unknown;

	std::vector<D3DXVECTOR3> position;
	std::vector<D3DXVECTOR3> normal;
	std::vector<D3DXVECTOR2> uv;

	std::vector<int> ID1, ID2, ID3, ID4;
	std::vector<float> w1, w2, w3, w4;

	bool Load( File* pFile)
	{
		unsigned short nVertex;
		nVertex = pFile->ReadUInt16();

		nType = pFile->ReadUInt16();

		unknown = pFile->ReadUInt32();

		// メモリ確保
		position.resize( nVertex );
		normal.resize( nVertex );
		uv.resize( nVertex );

		switch( nType )
		{
			case 0xC100:		// キャラクタ
				// メモリ確保
				ID1.resize( nVertex );
				ID2.resize( nVertex );
				ID3.resize( nVertex );
				ID4.resize( nVertex );
				w1.resize( nVertex );
				w2.resize( nVertex );
				w3.resize( nVertex );
				w4.resize( nVertex );

				for(int i = 0; i < nVertex; i++)
				{
					// position
					position[i].x = pFile->ReadSingle();
					position[i].y = pFile->ReadSingle();
					position[i].z = pFile->ReadSingle();

					// normal
					normal[i].x = pFile->ReadSingle();
					normal[i].y = pFile->ReadSingle();
					normal[i].z = pFile->ReadSingle();
					
					// uv
					uv[i].x = pFile->ReadSingle();
					uv[i].y = pFile->ReadSingle();

					// bone id
					ID1[i] = pFile->ReadByte();
					ID2[i] = pFile->ReadByte();
					ID3[i] = pFile->ReadByte();
					ID4[i] = pFile->ReadByte();

					// bone weight
					w1[i] = pFile->ReadSingle();
					w2[i] = pFile->ReadSingle();
					w3[i] = pFile->ReadSingle();
					w4[i] = pFile->ReadSingle();
				}
				break;
			case 0x8300:			// 背景
				ID1.resize( nVertex );
				ID2.resize( nVertex );
				ID3.resize( nVertex );
				ID4.resize( nVertex );

				for(int i = 0; i < nVertex; i++)
				{
					// position
					position[i].x = pFile->ReadSingle();
					position[i].y = pFile->ReadSingle();
					position[i].z = pFile->ReadSingle();

					// normal
					normal[i].x = pFile->ReadSingle();
					normal[i].y = pFile->ReadSingle();
					normal[i].z = pFile->ReadSingle();
					
					// bone id
					ID1[i] = pFile->ReadByte();
					ID2[i] = pFile->ReadByte();
					ID3[i] = pFile->ReadByte();
					ID4[i] = pFile->ReadByte();

					// uv
					uv[i].x = pFile->ReadSingle();
					uv[i].y = pFile->ReadSingle();
				}

				break;
			case 0x8100:		// アクセサリ
				for(int i = 0; i < nVertex; i++)
				{
					// position
					position[i].x = pFile->ReadSingle();
					position[i].y = pFile->ReadSingle();
					position[i].z = pFile->ReadSingle();

					// normal
					normal[i].x = pFile->ReadSingle();
					normal[i].y = pFile->ReadSingle();
					normal[i].z = pFile->ReadSingle();

					// uv
					uv[i].x = pFile->ReadSingle();
					uv[i].y = pFile->ReadSingle();
				}
				break;
			default:
				assert( 0 && "unknown vertex type");
				break;
		} // switch

		return true;
	}

	void Write( File* pFile )
	{
		int nVertex = position.size();

		pFile->WriteUInt16( nVertex );
		pFile->WriteUInt16( nType );
		pFile->WriteUInt32( unknown );

		switch( nType )
		{
		case 0xC100:
			for(int i=0; i<nVertex; i++)
			{
				pFile->WriteSingle( position[i].x );
				pFile->WriteSingle( position[i].y );
				pFile->WriteSingle( position[i].z );

				pFile->WriteSingle( normal[i].x );
				pFile->WriteSingle( normal[i].y );
				pFile->WriteSingle( normal[i].z );

				pFile->WriteSingle( uv[i].x );
				pFile->WriteSingle( uv[i].y );

				pFile->WriteByte( ID1[i] );
				pFile->WriteByte( ID2[i] );
				pFile->WriteByte( ID3[i] );
				pFile->WriteByte( ID4[i] );

				pFile->WriteSingle( w1[i] );
				pFile->WriteSingle( w2[i] );
				pFile->WriteSingle( w3[i] );
				pFile->WriteSingle( w4[i] );
			}
			break;
		case 0x8300:
			for(int i=0; i<nVertex; i++)
			{
				pFile->WriteSingle( position[i].x );
				pFile->WriteSingle( position[i].y );
				pFile->WriteSingle( position[i].z );

				pFile->WriteSingle( normal[i].x );
				pFile->WriteSingle( normal[i].y );
				pFile->WriteSingle( normal[i].z );

				pFile->WriteByte( ID1[i] );
				pFile->WriteByte( ID2[i] );
				pFile->WriteByte( ID3[i] );
				pFile->WriteByte( ID4[i] );

				pFile->WriteSingle( uv[i].x );
				pFile->WriteSingle( uv[i].y );
			}
			break;
		case 0x8100:
			for(int i=0; i<nVertex; i++)
			{
				pFile->WriteSingle( position[i].x );
				pFile->WriteSingle( position[i].y );
				pFile->WriteSingle( position[i].z );

				pFile->WriteSingle( normal[i].x );
				pFile->WriteSingle( normal[i].y );
				pFile->WriteSingle( normal[i].z );

				pFile->WriteSingle( uv[i].x );
				pFile->WriteSingle( uv[i].y );
			}
			break;
		default:
			assert( 0 && "unknown vertex type" );
			break;
		}
	}

	int WriteSize()
	{
		int nVertex = position.size();

		switch(nType)
		{
		case 0xC100:
			return 2+2+4+nVertex*52;

		case 0x8300:
			return 2+2+4+nVertex*36;

		case 0x8100:
			return 2+2+4+nVertex*32;

		default:
			assert( 0 && "unknown vertex type" );
			return 0;
		}
	}
};

struct Texture_Unknown
{
	float unknown[12];

	bool Load( File* pFile )
	{
		for(int i=0; i<12; i++)
		{
			unknown[i] = pFile->ReadSingle();	// always 0.0f
		}
		return true;
	}

	void Write( File* pFile )
	{
		for(int i=0; i<12; i++)
		{
			pFile->WriteSingle( unknown[i] );
		}
	}

	int WriteSize()
	{
		return 12*4;
	}
};

struct Texture
{
	int GIDX;

	int unknownOffset;	// Write時要計算
	int materialOffset;	// Write時要計算
	int unknown0;
	int unknown1;
	int unknown2;
	int unknown3;
	int unknown4;

	Texture_Unknown texture_unknown;

	bool Load( File* pFile )
	{
		GIDX = pFile->ReadUInt32();

		unknownOffset = pFile->ReadUInt32();
		unsigned int savedPos = pFile->CurrentPosition();
		pFile->Jump( unknownOffset );
		texture_unknown.Load( pFile );
		pFile->Jump( savedPos);

		materialOffset = pFile->ReadUInt32();
		unknown0 = pFile->ReadUInt32();		// always 0
		unknown1 = pFile->ReadUInt32();		// always 02020200h or 00000200h
		unknown2 = pFile->ReadUInt32();		// always 00020000h
		unknown3 = pFile->ReadUInt32();		// always 0
		unknown4 = pFile->ReadUInt32();		// always 0

		return true;
	}

	void Write( File* pFile )
	{
		pFile->WriteUInt32( GIDX );
		pFile->WriteUInt32( unknownOffset );	// calculated in NUD::Write

		pFile->WriteUInt32( materialOffset );	// calculated in NUD::Write

		pFile->WriteUInt32( unknown0 );
		pFile->WriteUInt32( unknown1 );
		pFile->WriteUInt32( unknown2 );
		pFile->WriteUInt32( unknown3 );
		pFile->WriteUInt32( unknown4 );
	}

	int WriteSize()
	{
		return 4+4+4+4*5;	//32
	}
};

struct Material
{
	float unknown[12];

	bool Load( File* pFile )
	{
		for(int i=0; i<12; i++)
		{
			unknown[i] = pFile->ReadSingle();
		}

		return true;
	}

	void Write( File* pFile )
	{
		for(int i=0; i<12; i++)
		{
			pFile->WriteSingle( unknown[i] );
		}
	}

	int WriteSize()
	{
		return 48;
	}
};

struct Mesh
{
	unsigned int index_offset;		// Write時要計算
	unsigned int vertex_offset;		// Write時要計算
	unsigned int material_offset;	// Write時要計算

	Index index;
	Vertex vertex;
	Material material;

	unsigned int render_flag;

	int unknown0;

	int nTexture;
	int unknown1;
	int unknown2;
	int unknown3;

	int no_edge_flag;

	std::vector<Texture> texture;

	bool Load( File* pFile )
	{
		// read index
		{
			index_offset = pFile->ReadUInt32();
			unsigned int savedPos = pFile->CurrentPosition();
			pFile->Jump( index_offset );

			index.Load( pFile );

			pFile->Jump( savedPos );
		}

		// read vertex
		{
			vertex_offset = pFile->ReadUInt32();
			unsigned int savedPos = pFile->CurrentPosition();
			pFile->Jump( vertex_offset );

			vertex.Load( pFile );

			pFile->Jump( savedPos );			
		}

		// read material
		{
			material_offset = pFile->ReadUInt32();
			unsigned int savedPos = pFile->CurrentPosition();
			pFile->Jump( material_offset );

			material.Load( pFile );

			pFile->Jump( savedPos );
		}

		render_flag = pFile->ReadUInt32();
		unknown0 = pFile->ReadUInt32();		// always 0
		nTexture = pFile->ReadUInt32();

		unknown1 = pFile->ReadInt16();		// always 0
		unknown2 = pFile->ReadInt16();		// always 0
		unknown3 = pFile->ReadInt16();		// always 0

		no_edge_flag = pFile->ReadInt16();

		// read texture
		texture.resize( nTexture );
		for( int i=0; i<nTexture; i++)
		{
			texture[i].Load( pFile );
		}

		return true;
	}

	void Write( File* pFile )
	{
		pFile->WriteUInt32( index_offset );			// calculated in NUD::Write
		pFile->WriteUInt32( vertex_offset );		// calculated in NUD::Write
		pFile->WriteUInt32( material_offset );		// calculated in NUD::Write

		pFile->WriteUInt32( render_flag );
		pFile->WriteUInt32( unknown0 );
		pFile->WriteUInt32( nTexture );

		pFile->WriteUInt16( unknown1 );
		pFile->WriteUInt16( unknown2 );
		pFile->WriteUInt16( unknown3 );

		pFile->WriteInt16( no_edge_flag );

		for(int i=0; i< texture.size(); i++)
		{
			texture[i].Write( pFile );
		}
	}

	int WriteSize()	// 実データを含まないヘッダだけのサイズ
	{
		return 4+4+4
			+4+4+4+2+2+2+2
			+texture.size() * texture[0].WriteSize();	// textureサイズは固定値
	}

};

struct MeshGroup
{
	std::vector<Mesh> mesh;

	int reserved0;
	int reserved1;
	int reserved2;
	int reserved3;
	int reserved4;

	D3DXVECTOR3 sphere_center;	// Write時要計算（省略）
	float sphere_radius;		// Write時要計算（省略）

	bool Load( File* pFile )
	{
		reserved0 = pFile->ReadUInt16();	// always 0
		reserved1 = pFile->ReadUInt16();	// always 0
		reserved2 = pFile->ReadUInt16();	// always -1
		reserved3 = pFile->ReadUInt16();	// always 0

		int nMesh;
		nMesh = pFile->ReadUInt32();

		reserved4 = pFile->ReadUInt32();	// always 0
		sphere_center.x = pFile->ReadSingle();
		sphere_center.y = pFile->ReadSingle();
		sphere_center.z = pFile->ReadSingle();
		sphere_radius = pFile->ReadSingle();

		mesh.resize( nMesh );
		for(int m = 0; m < nMesh; m++)
		{
			mesh[m].Load( pFile );
		}

		return true;
	}

	void Write( File* pFile )
	{
		pFile->WriteUInt16( reserved0 );
		pFile->WriteUInt16( reserved1 );
		pFile->WriteUInt16( reserved2 );
		pFile->WriteUInt16( reserved3 );

		pFile->WriteUInt32( mesh.size() );

		pFile->WriteUInt32( reserved4 );
		pFile->WriteSingle( sphere_center.x );
		pFile->WriteSingle( sphere_center.y );
		pFile->WriteSingle( sphere_center.z );
		pFile->WriteSingle( sphere_radius );

		for(int m=0; m<mesh.size(); m++)
		{
			mesh[m].Write( pFile );
		}
	}

	int WriteSize()
	{
		int size=2+2+2+2+4+4+4+4+4+4;
		for(int m=0; m<mesh.size(); m++)
		{
			size += mesh[m].WriteSize();
		}
		return size;		
	}
};

struct NUD
{
	std::vector<MeshGroup> meshgroup;

	int reserved;
	int unknown;
	int nJoint;

	D3DXVECTOR3 sphere_center;	// Write時要計算（省略）
	float sphere_radius;		// Write時要計算（省略）

public:
	bool Load( std::string filename )
	{
		File file;
		if( ! file.LoadFromFile(filename) ) return false;

		return Load( & file );
	}

	bool Load( File* pFile )
	{
		if( pFile->ReadUInt32() != 'NUP3' ) return false;

		int filesize = pFile->ReadUInt32();
		reserved = pFile->ReadUInt16();		// always 2

		unsigned short nMeshGroup;
		nMeshGroup = pFile->ReadUInt16();

		unknown = pFile->ReadUInt16();		// always 3
		nJoint = pFile->ReadUInt16();
		sphere_center.x = pFile->ReadSingle();
		sphere_center.y = pFile->ReadSingle();
		sphere_center.z = pFile->ReadSingle();
		sphere_radius = pFile->ReadSingle();

		meshgroup.resize( nMeshGroup );
		for(int g = 0; g < nMeshGroup; g++)
		{
			meshgroup[g].Load( pFile );
		}

		return true;
	}

	void Write( File* pFile )
	{
		// 以降のヘッダ部のサイズ算出
		int size = 0;
		for(int g = 0; g < meshgroup.size(); g++)
		{
			size += meshgroup[g].WriteSize();
		}

		pFile->WriteNullArray( size + 256 );	// headerの分（＋マージン）を確保

		for(int g = 0; g < meshgroup.size(); g++)
		{
			for(int m=0; m < meshgroup[g].mesh.size(); m++)
			{
				// index
				meshgroup[g].mesh[m].index_offset = pFile->CurrentPosition();
				meshgroup[g].mesh[m].index.Write( pFile );
				pFile->WriteNullArray( 16-pFile->CurrentPosition()%16);	// 適当にalignment

				// vertex
				meshgroup[g].mesh[m].vertex_offset = pFile->CurrentPosition();
				meshgroup[g].mesh[m].vertex.Write( pFile );
				pFile->WriteNullArray( 16-pFile->CurrentPosition()%16);

				// material
				meshgroup[g].mesh[m].material_offset = pFile->CurrentPosition();
				meshgroup[g].mesh[m].material.Write( pFile );
				pFile->WriteNullArray( 16-pFile->CurrentPosition()%16);

				for(int t=0; t<meshgroup[g].mesh[m].texture.size(); t++)
				{
					meshgroup[g].mesh[m].texture[t].materialOffset = meshgroup[g].mesh[m].material_offset;

					// texture_unknown
					meshgroup[g].mesh[m].texture[t].unknownOffset = pFile->CurrentPosition();
					meshgroup[g].mesh[m].texture[t].texture_unknown.Write( pFile);
					pFile->WriteNullArray( 16-pFile->CurrentPosition()%16);
				}
			}
		}

		unsigned int lastPos = pFile->CurrentPosition();


		// header書き出し

		pFile->Rewind();
		pFile->WriteByteArray((unsigned char*)"NUP3", 4);
		
		pFile->WriteNullArray(4);	// File Size Later

		pFile->WriteUInt16( reserved );

		pFile->WriteInt16( meshgroup.size() );
		pFile->WriteUInt16( unknown );
		pFile->WriteUInt16( nJoint );
		pFile->WriteSingle( sphere_center.x );
		pFile->WriteSingle( sphere_center.y );
		pFile->WriteSingle( sphere_center.z );
		pFile->WriteSingle( sphere_radius );
		for(int g = 0; g < meshgroup.size(); g++)
		{
			meshgroup[g].Write( pFile );
		}

		// Filesize
		pFile->Jump(4);
		pFile->WriteUInt32( lastPos );

		// NUDの最後に移動しておく
		pFile->Jump( lastPos );
	}

	void Write( File& file )
	{
		file.SetEndianness(File::BIG_ENDIAN);

		Write( & file );
	}

	void Write( std::string filename )
	{
		File out(filename);

		Write( & out );

		out.WriteToFile();
	}

};


};	// namespace imas

#endif