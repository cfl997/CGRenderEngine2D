#ifndef _CGRENDERAPI_H_
#define _CGRENDERAPI_H_

#include "CGRenderMacros.h"
#include "CGRender_Exports.h"
#include "CGglm.h"

bool GLRENDER_API CGRender_Init(const TCHAR* pluginPath);
int  GLRENDER_API CGRender_CreateContext(HWND hWnd, HGLRC hGLRC, int nWidth = 0, int nHeight = 0);
bool GLRENDER_API CGRender_ResizeWindow(int Device, int nWidth, int nHeight);

bool GLRENDER_API CGRender_Present(int Device);

int  GLRENDER_API CGRender_CreateTextureFromFile(int Device, const TCHAR* path, GLTextureType textureType);
int  GLRENDER_API CGRender_CreateTextureFromData(int Device, const void* data, unsigned int width, unsigned int height, GLTextureType textureType);
bool GLRENDER_API CGRender_GetTextureInfo(int Device, int hTexture, int* nWidth, int* nHeight, int* textureType);
int  GLRENDER_API CGRender_ClearTexture(int Device, int hTexture, DWORD color);
bool GLRENDER_API CGRender_DeleteTexture(int Device, int hTexture);
bool GLRENDER_API CGRender_ResizeTexture(int Device, int hTexture, unsigned int width, unsigned int height);
bool GLRENDER_API CGRender_UploadTexture(int Device, int hTexture, int xoffset, int yoffset, int width, int height, void* buffer);
bool GLRENDER_API CGRender_MoveTexturePixel(int Device, int hTexture, int xoffset, int yoffset, bool down = true);


bool GLRENDER_API CGRender_SetShaderTexture(int Device, int hTexture, int nRegisterIndex, ShaderType nShaderType = ShaderType::FRAGMENT);
bool GLRENDER_API CGRender_SaveTextue(int Device, int hTexture, const std::wstring& filePath);

int  GLRENDER_API CGRender_CreateShader(int Device, ShaderCodeName shaderCodeName);
bool GLRENDER_API CGRender_SetShader(int Device, int shaderid, ShaderType type);

int  GLRENDER_API CGRender_CreateBuffer(int Device, int nSize, int nNum, void* pBuffer, GLBufferType nType, GLPrimitiveTypes nPriType = PRIMITIVE_UNKOWN);
bool GLRENDER_API CGRender_ModifyBuffer(int Device, int hBuffer, int bufferSize, void* pBuffer);
bool GLRENDER_API CGRender_DeleteBuffer(int Device, int hBuffer);

bool GLRENDER_API CGRender_SetViewport(int Device, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
bool GLRENDER_API CGRender_SetScissor(int Device, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
bool GLRENDER_API CGRender_SetViewport(int Device, CGRECT viewPort);
bool GLRENDER_API CGRender_SetScissor(int Device, CGRECT scissor);
bool GLRENDER_API CGRender_GetViewport(int Device, CGRECT* rect);
bool GLRENDER_API CGRender_GetScissor(int Device, CGRECT* rect);

bool GLRENDER_API CGRender_SetUniformBuffer(int hDevice, int hBuffer, int nRegisterIndex, ShaderType nShaderType);

bool GLRENDER_API CGRender_SetRenderTarget(int Device, int hTexture);
int  GLRENDER_API CGRender_GetRenderTarget(int Device);

bool GLRENDER_API CGRender_RenderTest(int Device, float time);
bool GLRENDER_API CGRender_Render(int hDevice, int hVertexBuffer, int hIndexBuffer, int nVertexType, int nStrart, int PrimitiveCount, int nVertexCount);
bool GLRENDER_API CGRender_Render(int hDevice, int hVertexBuffer, int hIndexBuffer, int hVertexShader, int hGeometryShader, int hPixShader, int nStart, int nVertexCount, int nVertexBase);


glm::vec3 GLRENDER_API CGRender_GetWorldPos(glm::vec2 screenPos, const glm::mat4& view, const  glm::mat4& perspective, glm::vec4 viewPort);

bool GLRENDER_API CGRender_SaveImageByVoid(const void* buffer, unsigned int width, unsigned int height, unsigned int pixsize,
	const std::string& fileName = "e:/saveImageByVoid.png", int stride_bytes = 0, bool isOverTurnY = true);

void GLRENDER_API CGRender_DWORD2RGBA(const DWORD& color, CGRGBA& rgba);
void GLRENDER_API CGRender_RGBA2DWORD(DWORD& color, const CGRGBA& rgba);

#endif // !_CGRENDERAPI_H_
