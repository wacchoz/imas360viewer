#ifndef _CHARACTER_HEADER_
#define _CHARACTER_HEADER_

#include <string>
#include <vector>
#include <map>

#include <GL/glut.h>

#include "BNAFile.h"
#include "NUD.h"
#include "NUT.h"

#include "Shader.h"


namespace imas{

class Character
{
public:
//private:

	imas::NUD m_NUD;
	imas::NUT m_NUT;

	std::map<int, GLuint> m_ID;	// GIDX ⇒ OpenGL用ID
	
	enum RenderType { RENDER_MESH, RENDER_OUTLINE, RENDER_LINE };


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


	void Render( Shader* pShader, RenderType render_type = RENDER_MESH )
	{
		switch( render_type )
		{
		case RENDER_OUTLINE:
			glFrontFace( GL_CW );
			glEnable(GL_CULL_FACE);
			break;
		case RENDER_MESH:
			glFrontFace( GL_CCW );
			glDisable(GL_CULL_FACE);
			break;
		case RENDER_LINE:
			glFrontFace( GL_CCW );
			glEnable(GL_CULL_FACE);
			break;
		}

		// Toon用のみ
		cgGLSetParameter1f( pShader->parameter["bSpecular"], 0.0f );


		pShader->SetModelViewMatrix();

		pShader->Enable();

		std::vector<imas::MeshGroup>::reverse_iterator mg;
		for(mg = m_NUD.meshgroup.rbegin(); mg != m_NUD.meshgroup.rend(); ++mg)
//		std::vector<imas::MeshGroup>::iterator mg;
//		for(mg = m_NUD.meshgroup.begin(); mg != m_NUD.meshgroup.end(); ++mg)
		{
			std::vector<imas::Mesh>::iterator s;
			for(s=(*mg).mesh.begin(); s!= (*mg).mesh.end(); ++s)
			{
				// Toon用parameterセット
				if( typeid( *pShader) == typeid( ToonShader ) )
				{
					cgGLSetTextureParameter(pShader->parameter["diffuseSampler"], m_ID[ (*s).texture[0].GIDX ] );
					if( (*s).texture.size()>=3)
						cgGLSetTextureParameter(pShader->parameter["specularSampler"], m_ID[ (*s).texture[2].GIDX]);

					switch( (*s).render_flag )
					{
					case 0x10211011:
					case 0x10211021:
					case 0x10211031:
					case 0x10211001:
					case 0x10212011:
						glDisable(GL_BLEND);
						cgGLSetParameter1f(pShader->parameter["bSpecular"], 1.0f);
						break;
					case 0x10210011:
					case 0x10210031:
						glDisable(GL_BLEND);
						cgGLSetParameter1f(pShader->parameter["bSpecular"], -1.0f);
						break;
					case 0x10210021:
						if((*s).unknown_flag==0)
							glDisable(GL_BLEND);
						else
							glEnable(GL_BLEND);
						cgGLSetParameter1f(pShader->parameter["bSpecular"], -1.0f);
						break;
//					case 0x10212001:	// アストロ
//						cgGLSetTextureParameter(pShader->parameter["specularSampler"], m_ID[ (*s).texture[1].GIDX]);	// 1
//						glEnable(GL_BLEND);
//						cgGLSetParameter1f(pShader->parameter["bSpecular"], 1.0f);
//						break;
					case 0x10212021:
					default:
						glEnable(GL_BLEND);
						cgGLSetParameter1f(pShader->parameter["bSpecular"], -1.0f);
					}
					cgGLEnableTextureParameter( pShader->parameter["diffuseSampler"] );
					cgGLEnableTextureParameter( pShader->parameter["specularSampler"] );
				}

				// 描画本体
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableClientState( GL_NORMAL_ARRAY );
				glEnableClientState(GL_TEXTURE_COORD_ARRAY );

				{
					glVertexPointer( 3, GL_FLOAT, 0, & (*s).vertex.position[0].x );
					glNormalPointer( GL_FLOAT, 0, & (*s).vertex.normal[0].x );
					glTexCoordPointer(2, GL_FLOAT, 0, & (*s).vertex.uv[0].x);
					glDrawElements( GL_TRIANGLE_STRIP, (*s).index.index_degenerate_triangle.size(), GL_UNSIGNED_SHORT, & (*s).index.index_degenerate_triangle[0]);
				}

				glDisableClientState( GL_TEXTURE_COORD_ARRAY );
				glDisableClientState( GL_NORMAL_ARRAY );
				glDisableClientState( GL_VERTEX_ARRAY );

				// Toon用テクスチャを無効化
				if( typeid( *pShader) == typeid( ToonShader ) )
				{
					cgGLDisableTextureParameter( pShader->parameter["diffuseSampler"] );
					cgGLDisableTextureParameter( pShader->parameter["specularSampler"] );
				}
			}
		}

		pShader->Disable();
	}
	

};

}	// namespace

#endif