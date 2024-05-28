#pragma once







#include <string>
#include <Windows.h>
#include <CGglm.h>

enum ShaderCodeName
{
	VS_POS_COLOR_TEX = 0,
	VS_POS_TEX,
	VS_POS_COLOR_TEX_viewMatrix,
	VS_POS_TEX_MVP3Matrix,
	VS_POS_TEX_MVPMatrix,
	VS_Count,

	FS_Tex,
	FS_COLOR,

	FS_Tex_Rotate90,
	FS_Tex_XReversal,
	FS_Tex_Gray,
	FS_Tex_Invert,

	FSTwoTex,
	FS_Text,
	FS_Count,

	//raw16
	FS_Tex_Raw,
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
	GLTexture_RED,
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

struct GetTextureInfoOut
{
	int* width;
	int* height;
	int* Type;
};

struct ClearTextureIn
{
	int hTexture;
	void* pData;
};

struct ResizeTextureIn
{
	int hTexture;
	unsigned int width;
	unsigned int height;
};


struct UploadTextureIn
{
	int hTexture;
	int xoffset;
	int yoffset;
	int width;
	int height;
	void* buffer;
};

struct MoveTextureIn
{
	int hTexture;
	int xoffset;
	int yoffset;
	bool down;
};

struct CopyTextureIn
{
	int hSrcTex;
	int srcOffsetX;
	int srcOffsetY;
	int srcWidth;
	int srcHeight;
	int hDesTex;
	int desOffsetX;
	int desOffsetY;
	int desWidth;
	int desHeight;
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

struct CGRECT
{
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
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

struct ModifyBufferIn
{
	int hBuffer;
	int buffersize;
	void* pBuffer;
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

struct RenderTextureIn
{
	int hTexture;
	const CGRECT* pDstRt;
};

//common

struct CGRGBA
{
	float R;
	float G;
	float B;
	float A;
};

//image text all item size
static const float g_globalProportion = 10.f;


//fonts
static const uint32_t g_FontPixelSize = 30;//default font size -> scale is this 
struct Character {
	uint32_t    TextureID;  // 字形纹理的ID
	glm::ivec2  Size;       // 字形大小
	glm::ivec2  Bearing;    // 从基准线到字形左部/顶部的偏移值
	uint32_t    Advance;    // 原点距下一个字形原点的距离
};