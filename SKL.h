#ifndef _SKL_HEADER_
#define _SKL_HEADER_

#include "File.h"

#include <vector>


namespace imas{


struct JointSKL
{
	int parentJoint;

	int unknown0;
	int correction_type;

	float rigParam[2];
	int refJoint;
	float reserved[6];

public:
	bool Load( File* pFile)
	{
		parentJoint = pFile->ReadUInt32();
		unknown0 = pFile->ReadUInt32();
		correction_type = pFile->ReadUInt32();
		rigParam[0] = pFile->ReadSingle();
		rigParam[1] = pFile->ReadSingle();
		refJoint = static_cast<int>( pFile->ReadSingle() );
		reserved[0] = pFile->ReadSingle();
		reserved[1] = pFile->ReadSingle();
		reserved[2] = pFile->ReadSingle();
		reserved[3] = pFile->ReadSingle();
		reserved[4] = pFile->ReadSingle();
		reserved[5] = pFile->ReadSingle();

		return true;
	}

	void Write( File* pFile)
	{
		pFile->WriteUInt32( parentJoint );
		pFile->WriteUInt32( unknown0 );
		pFile->WriteUInt32( correction_type );
		pFile->WriteSingle( rigParam[0] );
		pFile->WriteSingle( rigParam[1] );
		pFile->WriteSingle( static_cast<float> (refJoint) );
		pFile->WriteSingle( reserved[0] );
		pFile->WriteSingle( reserved[1] );
		pFile->WriteSingle( reserved[2] );
		pFile->WriteSingle( reserved[3] );
		pFile->WriteSingle( reserved[4] );
		pFile->WriteSingle( reserved[5] );
	}
};

struct CHAIN_JOINT_DATA
{
	int	bone_ID;
	float unknown0;
	float unknown1;
	float unknown2;
	float damping;
	float spring;
	float radius;

public:
	bool Load( File* pFile)
	{
		bone_ID = pFile->ReadUInt32();

		unknown0 = pFile->ReadSingle();
		unknown1 = pFile->ReadSingle();
		unknown2 = pFile->ReadSingle();

		damping = pFile->ReadSingle();
		spring = pFile->ReadSingle();
		radius = pFile->ReadSingle();

		return true;
	}

	void Write( File* pFile)
	{
		pFile->WriteUInt32( bone_ID );

		pFile->WriteSingle( unknown0 );
		pFile->WriteSingle( unknown1 );
		pFile->WriteSingle( unknown2 );

		pFile->WriteSingle( damping );
		pFile->WriteSingle( spring );
		pFile->WriteSingle( radius );
	}

	int WriteSize()
	{
		return 4*7;
	}
};

struct CHAINS
{
	char chain_name[16];
	float	unknown0;
	int		unknown1;
//	int joint_data_count;
	std::vector< CHAIN_JOINT_DATA > chain_joint_data_list;

public:
	bool Load( File* pFile)
	{
		memcpy(chain_name, pFile->pCurrentData(), sizeof(chain_name));
		pFile->Skip(sizeof(chain_name));

		unknown0 = pFile->ReadSingle();
		unknown1 = pFile->ReadUInt32();

		int nJoint = pFile->ReadUInt32();

		chain_joint_data_list.resize( nJoint );

		for(int i=0; i<nJoint; i++)
		{
			chain_joint_data_list[i].Load( pFile );
		}

		return true;
	}

	void Write( File* pFile)
	{
		pFile->WriteByteArray((unsigned char*)chain_name, sizeof(chain_name));

		pFile->WriteSingle( unknown0 );
		pFile->WriteUInt32( unknown1 );

		pFile->WriteUInt32( chain_joint_data_list.size() );

		for( int i=0; i< chain_joint_data_list.size(); i++)
		{
			chain_joint_data_list[i].Write( pFile );
		}
	}

	int WriteSize()
	{
		int size;
		size = sizeof(chain_name) + 4 + 4 + 4;
		for( int i=0; i< chain_joint_data_list.size(); i++)
		{
			size += chain_joint_data_list[i].WriteSize();
		}
		return size;
	}
};

struct BoundingSphere
{
	int bone_ID;
	float posX;
	float posY;
	float posZ;
	float radius;

public:
	bool Load( File* pFile)
	{
		bone_ID = pFile->ReadUInt32();

		posX = pFile->ReadSingle();
		posY = pFile->ReadSingle();
		posZ = pFile->ReadSingle();
		radius = pFile->ReadSingle();

		return true;
	}

	void Write( File* pFile)
	{
		pFile->WriteUInt32( bone_ID );

		pFile->WriteSingle( posX );
		pFile->WriteSingle( posY );
		pFile->WriteSingle( posZ );
		pFile->WriteSingle( radius );
	}

	int WriteSize()
	{
		return 4*5;
	}
};

struct BoundingCylinder
{
	int ID1;
	float posX1, posY1, posZ1;
	float radius1;
	int ID2;
	float posX2, posY2, posZ2;
	float radius2;

public:
	bool Load( File* pFile)
	{
		ID1 = pFile->ReadUInt32();

		posX1 = pFile->ReadSingle();
		posY1 = pFile->ReadSingle();
		posZ1 = pFile->ReadSingle();
		radius1 = pFile->ReadSingle();

		ID2 = pFile->ReadUInt32();
		
		posX2 = pFile->ReadSingle();
		posY2 = pFile->ReadSingle();
		posZ2 = pFile->ReadSingle();
		radius2 = pFile->ReadSingle();

		return true;
	}

	void Write( File* pFile)
	{
		pFile->WriteUInt32( ID1 );
		pFile->WriteSingle( posX1 );
		pFile->WriteSingle( posY1 );
		pFile->WriteSingle( posZ1 );
		pFile->WriteSingle( radius1 );

		pFile->WriteUInt32( ID2 );
		pFile->WriteSingle( posX2 );
		pFile->WriteSingle( posY2 );
		pFile->WriteSingle( posZ2 );
		pFile->WriteSingle( radius2 );
	}

	int WriteSize()
	{
		return (4+4+4+4+4)*2;
	}
};

struct RIG
{
	char rig_name[16];
	int unknown;
//	int chain_count;
	std::vector< CHAINS > chain_list;
//	int bounding_sphere_count;
	std::vector< BoundingSphere > sphere_list;
//	int bounding_cylinder_count;
	std::vector< BoundingCylinder > cylinder_list;

public:
	bool Load( File* pFile)
	{
		memcpy(rig_name, pFile->pCurrentData(), sizeof(rig_name));
		pFile->Skip(sizeof(rig_name));

		unknown = pFile->ReadInt32();

		// CHAIN
		int chain_count;
		chain_count = pFile->ReadUInt32();

		chain_list.resize( chain_count );

		for(int k=0; k<chain_count; k++)
		{
			chain_list[k].Load( pFile );
		}

		// SPHERE
		int bounding_sphere_count;
		bounding_sphere_count = pFile->ReadUInt32();

		sphere_list.resize( bounding_sphere_count );

		for(int m=0; m<bounding_sphere_count; m++)
		{
			sphere_list[m].Load( pFile );
		}

		// CYLINDER
		int bounding_cylinder_count;
		bounding_cylinder_count = pFile->ReadUInt32();

		cylinder_list.resize( bounding_cylinder_count );

		for(int m=0; m<bounding_cylinder_count; m++)
		{
			cylinder_list[m].Load( pFile );
		}

		return true;
	}

	void Write( File* pFile)
	{
		pFile->WriteByteArray((unsigned char*)rig_name, sizeof(rig_name));

		pFile->WriteInt32( unknown );

		// CHAIN
		pFile->WriteUInt32( chain_list.size() );

		for( int i=0; i< chain_list.size(); i++)
		{
			chain_list[i].Write( pFile );
		}

		// SPHERE
		pFile->WriteUInt32( sphere_list.size() );

		for( int i=0; i<sphere_list.size(); i++)
		{
			sphere_list[i].Write( pFile );
		}

		// CYLINDER
		pFile->WriteUInt32( cylinder_list.size() );

		for( int i=0; i<cylinder_list.size(); i++)
		{
			cylinder_list[i].Write( pFile );
		}
	}

	int WriteSize()
	{
		int size;
		size = sizeof(rig_name) + 4;
		size+=4;
		for( int i=0; i< chain_list.size(); i++)
		{
			size+=chain_list[i].WriteSize();
		}
		size+=4;
		for( int i=0; i<sphere_list.size(); i++)
		{
			size+=sphere_list[i].WriteSize();
		}
		size+=4;
		for( int i=0; i<cylinder_list.size(); i++)
		{
			size+=cylinder_list[i].WriteSize();
		}
		return size;
	}
};

struct YUR2		// —h‚êƒ‚ƒm
{
	std::vector< RIG > rig_list;

public:
	bool Load( File* pFile)
	{
		if( pFile->ReadByte() != 'Y' ) return false;
		if( pFile->ReadByte() != 'U' ) return false;
		if( pFile->ReadByte() != 'R' ) return false;
		if( pFile->ReadByte() != '2' ) return false;

		int nRig;
		nRig = pFile->ReadUInt32();

		rig_list.resize( nRig );

		for( int i=0; i<nRig; i++)
		{
			rig_list[i].Load( pFile );
		}

		return true;
	}

	void Write( File* pFile)
	{
		pFile->WriteByteArray((unsigned char*)"YUR2", 4);

		pFile->WriteUInt32( rig_list.size() );

		for( int i=0; i< rig_list.size(); i++)
		{
			rig_list[i].Write( pFile );
		}
	}

	int WriteSize()
	{
		int size=4+4;
		for( int i=0; i< rig_list.size(); i++)
		{
			size+= rig_list[i].WriteSize();
		}
		return size;
	}
};

struct SKL
{
	std::vector<JointSKL> joint;

	float unknown0;
	int rig_table_size;
	int unknown1;

	YUR2 yur2;

public:
	bool Load( File* pFile )
	{
		if( pFile->ReadByte() != 'S' ) return false;
		if( pFile->ReadByte() != 'K' ) return false;
		if( pFile->ReadByte() != 'E' ) return false;
		if( pFile->ReadByte() != '2' ) return false;

		int nJoint;
		nJoint = pFile->ReadUInt32();

		joint.resize( nJoint );

		unknown0 = pFile->ReadSingle();
		rig_table_size = pFile->ReadUInt32();
		unknown1 = pFile->ReadUInt32();

		for(int i=0; i<nJoint; i++)
		{
			joint[i].Load( pFile );
		}
		
		// read YUR2

		if( ! yur2.Load( pFile ) ) return false;

		return true;
	}

	void Write( File& file )
	{
		file.SetEndianness(File::BIG_ENDIAN);

		file.WriteByteArray((unsigned char*)"SKE2", 4);

		file.WriteUInt32( joint.size() );

		file.WriteSingle( unknown0 );
		file.WriteUInt32( 0 );	// rig_table_size temporary

		file.WriteUInt32( unknown1 );

		for( int i=0; i<joint.size(); i++)
		{
			joint[i].Write( & file );
		}

		yur2.Write( & file );

		// rig_table_size‚ð‘‚«o‚µ
		file.Rewind();
		file.Skip( 12 );
		file.WriteUInt32( yur2.WriteSize() );
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