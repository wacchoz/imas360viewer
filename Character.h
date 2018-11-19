#ifndef _CHARACTER_HEADER_
#define _CHARACTER_HEADER_

#include <string>
#include <vector>
#include <map>

#include <GL/glut.h>

#include "BNAFile.h"
#include "NUD.h"
#include "NUT.h"


namespace imas{

class Character
{
private:

	imas::NUD m_NUD;
	imas::NUT m_NUT;

	std::map<int, GLuint> m_ID;	// GIDX ⇒ OpenGL用ID

public:
	bool Load( std::string filename )
	{
		imas::BNAFile bnafile;
		if( ! bnafile.Load(filename) ) return false;

		// NUDファイル読み込み
		{
			File* pFile = bnafile.GetFileByFilter(".nud");
			if( pFile == NULL ) return false;
			if( ! m_NUD.Load( pFile ) ) return false;
		}
		// NUTファイル読み込み
		{
			File* pFile = bnafile.GetFileByFilter(".nut");
			if( pFile == NULL ) return false;
			if( ! m_NUT.Load( pFile ) ) return false;
		}

		// テクスチャ割り当て
		for(int i=0; i < m_NUT.texture_data.size(); i++)
		{
			GLuint ID;
			glGenTextures(1, & ID);
			glBindTexture(GL_TEXTURE_2D, ID);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			gluBuild2DMipmaps(
				GL_TEXTURE_2D,
				GL_RGBA,
				m_NUT.texture_data[i].width,
				m_NUT.texture_data[i].height,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				& m_NUT.texture_data[i].decompressed_texture[0] );

			m_ID[ m_NUT.texture_data[i].GIDX ] = ID;
		}

		return true;
	}

	void Render()
	{
		std::vector<imas::MeshGroup>::iterator mg;
		for(mg = m_NUD.meshgroup.begin(); mg != m_NUD.meshgroup.end(); ++mg)
		{
			std::vector<imas::Mesh>::iterator s;
			for(s=(*mg).mesh.begin(); s!= (*mg).mesh.end(); ++s)
			{
				glBindTexture(GL_TEXTURE_2D, m_ID[ (*s).texture[0].GIDX ]);

				glEnableClientState( GL_VERTEX_ARRAY);
				glEnableClientState( GL_NORMAL_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				glVertexPointer( 3, GL_FLOAT, 0, & (*s).vertex.position[0].x );
				glNormalPointer( GL_FLOAT, 0, & (*s).vertex.normal[0].x );
				glTexCoordPointer(2, GL_FLOAT, 0, & (*s).vertex.uv[0].x);
				glDrawElements( GL_TRIANGLE_STRIP, (*s).index.index_degenerate_triangle.size(), GL_UNSIGNED_SHORT, & (*s).index.index_degenerate_triangle[0]);

				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState( GL_NORMAL_ARRAY);
				glDisableClientState( GL_VERTEX_ARRAY);
			}
		}
	}

};

}	// namespace

#endif