#pragma once







#include <string>
#include <Windows.h>


enum ShaderCodeName
{
	VS_POS_COLOR_TEX = 0,
	VS_POS_COLOR_TEX_viewMatrix = 0,
	VS_POS_TEX_MVP3Matrix,
	VS_POS_TEX_MVPMatrix,
	FS_Tex,
	FS_COLOR,
	FS_Tex_Rotate90,
	FS_Tex_Gray,
	FSTwoTex,
	ShaderCodeCount,
	//test
	FSTestUniform,
};

enum ShaderType
{
	VERTEX = 0,		//顶点着色器
	FRAGMENT,		//片段着色器
	GEOMETRY,		//几何着色器
	COMPUTE,		//计算着色器
	ShaderType_Num,
};

enum ShaderVertexType
{
	CGVertex_POS,
	CGVertex_POS_TEX,
	CGVertex_POS_COLOR_TEX,
	CGVertex_Unknow = 0xFFFFFFFF
};

struct ShaderData
{
	std::string shaderCode;
	ShaderType shaderType;
	ShaderVertexType shaderVertexType;
};


enum GLTextureType
{
	GLTexture_Normal2DTex = 0,
	GLTexture_Raw16,
	GLTexture_Count
};

struct CreateContextIn
{
	void* hwnd;
	void* glhrc;
	int width;
	int height;
};

struct ResizeWindowIn
{
	int nWidth;
	int nHeight;
};
struct CreateTextureIn
{
	const TCHAR* path;
	const void* data;
	unsigned int width;
	unsigned int height;
	int Type;
};

struct ClearTextureIn
{
	int hTexture;
	void* pData;
};

struct SetShaderTextureIn
{
	int hTexture;
	int nRegisterIndex;
	ShaderType nShaderType;
};

struct SaveTextureIn
{
	int hTexture;
	std::wstring filePath;
};

struct SetViewport
{
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
};

struct SetUniformBufferIn
{
	int bufferid;
	int index;
	ShaderType shaderType;
};

struct CreateShaderIn
{
	ShaderCodeName shaderCodeName;
};

struct SetShaderIn
{
	ShaderType type;
	int id;
};



/*
*
*/

enum GLBufferType
{
	VertexBuffer,
	IndexBuffer,
	uniformBuffer,
};

enum GLPrimitiveTypes
{
	LINE_LIST = 0,
	LINE_LIST_STRIP = 1,
	TRIANGLELIST = 2,
	TRIANGLESTRIP = 3,
	PRIMITIVE_UNKOWN = 0xFFFFFFFF,
};

struct CreateBufferIn
{
	int nSize;
	int nNum;
	void* pBuffer;
	GLBufferType nType;
	GLPrimitiveTypes nPriType;
};

struct RenderIn
{
	int hVertexBuffer;
	int hIndexBuffer;
	int	nVertexType;
	int nStart;
	int PrimitiveCount;
	int nVertexCount;
};
struct EasyRenderIn
{
	int hVertexBuffer;
	int hIndexBuffer;
	int hVertexShader;
	int hGeometryShader;
	int hPixShader;
	int nStart;
	int nVertexCount;
	int nVertexBase;
};

