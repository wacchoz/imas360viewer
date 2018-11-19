#ifndef _SHADER_HEADER_
#define _SHADER_HEADER_

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <map>

class Shader
{
protected:
	CGcontext cgContext;

	CGprofile vertexProfile;
	CGprofile fragmentProfile;

	CGprogram vertexShader;
	CGprogram fragmentShader;

	CGparameter ModelViewProj;
	CGparameter ModelView;
public:
	std::map<std::string, CGparameter> parameter;

public:
	void checkCgError()
	{
		CGerror err = cgGetError();
		if( err != CG_NO_ERROR )
		{
			printf("cg error: %s\n", cgGetErrorString(err) );
			exit(1);
		}
	}

	virtual void Init() = 0;

	void Destroy()
	{
		cgDestroyProgram(vertexShader);
		cgDestroyProgram(fragmentShader);
		cgDestroyContext(cgContext);
	}
	
	void Enable()
	{
		cgGLEnableProfile(vertexProfile);
		cgGLEnableProfile(fragmentProfile);
		cgGLBindProgram(vertexShader);
		cgGLBindProgram(fragmentShader);
	}

	void Disable()
	{
		cgGLDisableProfile(vertexProfile);
		cgGLDisableProfile(fragmentProfile);
	}

	void SetModelViewMatrix()
	{
		cgGLSetStateMatrixParameter( ModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
		cgGLSetStateMatrixParameter( ModelView, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	}
};



class ToonShader : public Shader
{
	CGparameter diffuseSampler;
	CGparameter specularSampler;
	CGparameter bSpecular;

public:
	void Init()
	{
		vertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
		if ( vertexProfile == CG_PROFILE_UNKNOWN || vertexProfile == CG_INVALID_ENUMERANT_ERROR)
		{
			printf( "Error : Invalid Profile Type\n" );
			exit(1);
		}
		fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
		if ( fragmentProfile == CG_PROFILE_UNKNOWN || fragmentProfile == CG_INVALID_ENUMERANT_ERROR)
		{
			printf( "Error : Invalid Profile Type\n" );
			exit(1);
		}

		cgContext = cgCreateContext();
		checkCgError();

		vertexShader = cgCreateProgramFromFile(cgContext, CG_SOURCE, "ToonVertex.cg", vertexProfile, "mainVS", NULL);
		checkCgError();

		fragmentShader = cgCreateProgramFromFile(cgContext, CG_SOURCE, "ToonFragment.cg", fragmentProfile, "mainPS", NULL);
		checkCgError();

		cgGLLoadProgram(vertexShader);
		checkCgError();

		cgGLLoadProgram(fragmentShader);
		checkCgError();

		ModelViewProj = cgGetNamedParameter(vertexShader, "ModelViewProj");
		checkCgError();

		ModelView = cgGetNamedParameter(vertexShader, "ModelView");
		checkCgError();

		diffuseSampler = cgGetNamedParameter(fragmentShader, "diffuseSampler");
		checkCgError();

		specularSampler = cgGetNamedParameter(fragmentShader, "specularSampler");
		checkCgError();

		bSpecular = cgGetNamedParameter(fragmentShader, "bSpecular");
		checkCgError();

		parameter["diffuseSampler"] = diffuseSampler;
		parameter["specularSampler"] = specularSampler;
		parameter["bSpecular"] = bSpecular;
	}

};


class OutlineShader : public Shader
{

public:
	void Init()
	{
		vertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
		if ( vertexProfile == CG_PROFILE_UNKNOWN || vertexProfile == CG_INVALID_ENUMERANT_ERROR)
		{
			printf( "Error : Invalid Profile Type\n" );
			exit(1);
		}
		fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
		if ( fragmentProfile == CG_PROFILE_UNKNOWN || fragmentProfile == CG_INVALID_ENUMERANT_ERROR)
		{
			printf( "Error : Invalid Profile Type\n" );
			exit(1);
		}

		cgContext = cgCreateContext();
		checkCgError();

		vertexShader = cgCreateProgramFromFile(cgContext, CG_SOURCE, "OutlineVertex.cg", vertexProfile, "mainVS2", NULL);
		checkCgError();

		fragmentShader = cgCreateProgramFromFile(cgContext, CG_SOURCE, "OutlineFragment.cg", fragmentProfile, "mainPS2", NULL);
		checkCgError();

		cgGLLoadProgram(vertexShader);
		checkCgError();

		cgGLLoadProgram(fragmentShader);
		checkCgError();

		ModelViewProj = cgGetNamedParameter(vertexShader, "ModelViewProj");
		checkCgError();

		ModelView = cgGetNamedParameter(vertexShader, "ModelView");
		checkCgError();
	}
};


#endif