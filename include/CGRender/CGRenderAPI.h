#ifndef _CGRENDERAPI_H_
#define _CGRENDERAPI_H_

#include "CGRenderMacros.h"
#include "CGRender_Exports.h"
#include "CGglm.h"

bool GLRENDER_API CGRender_Init(const TCHAR* pluginPath);
int GLRENDER_API CGRender_CreateContext(HWND hWnd, HGLRC hGLRC, int nWidth = 0, int nHeight = 0);
bool GLRENDER_API CGRender_ResizeWindow(int Device, int nWidth, int nHeight);

bool GLRENDER_API CGRender_Present(int Device);

int GLRENDER_API CGRender_CreateTextureFromFile(int Device, const TCHAR* path, unsigned int width, unsigned int height, GLTextureType textureType);
int GLRENDER_API CGRender_CreateTextureFromData(int Device, const void* data, unsigned int width, unsigned int height, GLTextureType textureType);
int GLRENDER_API CGRender_ClearTexture(int Device, int hTexture, DWORD color);
bool GLRENDER_API CGRender_DeleteTexture(int Device, int hTexture);
bool GLRENDER_API CGRender_ResizeTexture(int Device, int hTexture, unsigned int width, unsigned int height);

bool GLRENDER_API CGRender_SetShaderTexture(int Device, int hTexture, int nRegisterIndex, ShaderType nShaderType = ShaderType::FRAGMENT);
bool GLRENDER_API CGRender_SaveTextue(int Device, int hTexture, const std::wstring& filePath);

int GLRENDER_API CGRender_CreateShader(int Device, ShaderCodeName shaderCodeName);
bool GLRENDER_API CGRender_SetShader(int Device, int shaderid, ShaderType type);

int GLRENDER_API CGRender_CreateBuffer(int Device, int nSize, int nNum, void* pBuffer, GLBufferType nType, GLPrimitiveTypes nPriType = PRIMITIVE_UNKOWN);


bool GLRENDER_API CGRender_SetViewport(int Device, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
bool GLRENDER_API CGRender_SetScissor(int Device, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
bool GLRENDER_API CGRender_SetUniformBuffer(int hDevice, int hBuffer, int nRegisterIndex, ShaderType nShaderType);

bool GLRENDER_API CGRender_SetRenderTarget(int Device, int hTexture);
int GLRENDER_API CGRender_GetRenderTarget(int Device);

bool GLRENDER_API CGRender_RenderTest(int Device, float time);
bool GLRENDER_API CGRender_Render(int hDevice, int hVertexBuffer, int hIndexBuffer, int nVertexType, int nStrart, int PrimitiveCount, int nVertexCount);
bool GLRENDER_API CGRender_Render(int hDevice, int hVertexBuffer, int hIndexBuffer, int hVertexShader, int hGeometryShader, int hPixShader, int nStart, int nVertexCount, int nVertexBase);



glm::vec3 GLRENDER_API CGRender_GetWorldPos(glm::vec2 screenPos, const glm::mat4& view, const  glm::mat4& perspective, glm::vec4 viewPort);
#endif // !_CGRENDERAPI_H_
