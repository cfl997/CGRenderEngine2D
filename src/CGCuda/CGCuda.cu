#include "CGCuda.cuh"
#include "CGCudaHelper.h"


#include "ImgCudaOperator/ImgCudaOperator.h"

#pragma comment (lib,"ImgCudaOperator.lib")

#include "../../third/glfw-3.4/glfw-3.4/inlcude/GLFW/glfw3.h"

#include <cuda.h>

#include <cuda_runtime.h>
#include <cuda_texture_types.h>
#include <cuda_runtime_api.h>
#include <surface_functions.h>
#include <device_launch_parameters.h>

#include <cuda_gl_interop.h>

#include <driver_types.h>


#include <cuda_surface_types.h>

#include <assert.h>



void invertColor_16(cudaArray_t cuArray, int width, int height)
{
	cudaResourceDesc resDesc;
	memset(&resDesc, 0, sizeof(resDesc));
	resDesc.resType = cudaResourceTypeArray;
	resDesc.res.array.array = cuArray;


	cudaSurfaceObject_t surfObj;
	cudaCreateSurfaceObject(&surfObj, &resDesc);

	image cudaimage;
	cudaimage.width = width;
	cudaimage.height = height;
	cudaimage.data_ptr = &surfObj;


	OptInverseColor(&cudaimage);

	// 等待 CUDA 核函数执行完成
	cudaDeviceSynchronize();

	// 销毁 CUDA 表面对象
	cudaDestroySurfaceObject(surfObj);
}


bool CUDA_invertColor_16(int htexture, int width, int height, int target)
{
	cudaGraphicsResource_t cudaResources;

	// 在 CUDA 中注册纹理
	cudaError_t err = cudaGraphicsGLRegisterImage(&cudaResources, htexture, target, cudaGraphicsRegisterFlagsWriteDiscard);
	if (err != cudaSuccess) {
		return false;
	}

	// 在 CUDA 中锁定资源，获得操作纹理的指针
	cudaArray_t cuArray;
	cudaGraphicsMapResources(1, &cudaResources, 0);
	cudaGraphicsSubResourceGetMappedArray(&cuArray, cudaResources, 0, 0);

	invertColor_16(cuArray, width, height);

	// 解除资源锁定，使 OpenGL 可以利用得到的纹理对象进行纹理贴图操作
	cudaGraphicsUnmapResources(1, &cudaResources, 0);

	// 释放 CUDA 资源
	cudaGraphicsUnregisterResource(cudaResources);

	return true;
}

bool CUDA_OptWindowLevel(int htexture, int width, int height, int target, int windowWidth, int windowLevel)
{
	cudaGraphicsResource_t cudaResources;

	// 在 CUDA 中注册纹理
	cudaError_t err = cudaGraphicsGLRegisterImage(&cudaResources, htexture, target, cudaGraphicsRegisterFlagsWriteDiscard);
	if (err != cudaSuccess) {
		return false;
	}

	// 在 CUDA 中锁定资源，获得操作纹理的指针
	cudaArray_t cuArray;
	cudaGraphicsMapResources(1, &cudaResources, 0);
	cudaGraphicsSubResourceGetMappedArray(&cuArray, cudaResources, 0, 0);

	cudaResourceDesc resDesc;
	memset(&resDesc, 0, sizeof(resDesc));
	resDesc.resType = cudaResourceTypeArray;
	resDesc.res.array.array = cuArray;


	cudaSurfaceObject_t surfObj;
	cudaCreateSurfaceObject(&surfObj, &resDesc);

	image cudaimage;
	cudaimage.width = width;
	cudaimage.height = height;
	cudaimage.data_ptr = &surfObj;


	OptWindowLevel(&cudaimage, windowWidth, windowLevel);


	// 等待 CUDA 核函数执行完成
	cudaDeviceSynchronize();

	// 销毁 CUDA 表面对象
	cudaDestroySurfaceObject(surfObj);

	// 解除资源锁定，使 OpenGL 可以利用得到的纹理对象进行纹理贴图操作
	cudaGraphicsUnmapResources(1, &cudaResources, 0);

	// 释放 CUDA 资源
	cudaGraphicsUnregisterResource(cudaResources);

	return true;
}

bool CUDA_equalizeHistogram_16(int htexture, int width, int height, int target)
{
	cudaGraphicsResource_t cudaResources;

	// 在 CUDA 中注册纹理
	cudaError_t err = cudaGraphicsGLRegisterImage(&cudaResources, htexture, target, cudaGraphicsRegisterFlagsWriteDiscard);
	if (err != cudaSuccess) {
		return false;
	}

	// 在 CUDA 中锁定资源，获得操作纹理的指针
	cudaArray_t cuArray;
	cudaGraphicsMapResources(1, &cudaResources, 0);
	cudaGraphicsSubResourceGetMappedArray(&cuArray, cudaResources, 0, 0);

	cudaResourceDesc resDesc;
	memset(&resDesc, 0, sizeof(resDesc));
	resDesc.resType = cudaResourceTypeArray;
	resDesc.res.array.array = cuArray;


	cudaSurfaceObject_t surfObj;
	cudaCreateSurfaceObject(&surfObj, &resDesc);

	image cudaimage;
	cudaimage.width = width;
	cudaimage.height = height;
	cudaimage.data_ptr = &surfObj;


	OptHistogramEqualization(&cudaimage);

	// 等待 CUDA 核函数执行完成
	cudaDeviceSynchronize();

	// 销毁 CUDA 表面对象
	cudaDestroySurfaceObject(surfObj);

	// 解除资源锁定，使 OpenGL 可以利用得到的纹理对象进行纹理贴图操作
	cudaGraphicsUnmapResources(1, &cudaResources, 0);

	// 释放 CUDA 资源
	cudaGraphicsUnregisterResource(cudaResources);

	return true;
}
bool CudaAlgorithm(ShaderCodeName cudatype, int hTexture, int width, int height, int target, const CudaParameter* cudaParam)
{
	switch (cudatype)
	{
	case FS_Tex_RGBA_equalizeHistogramSource:
	{
		//return CUDA_equalizeHistogram_rgba(hTexture, width, height, target);
	}
	case FS_Tex_Red_equalizeHistogramSource:
	{
		return CUDA_equalizeHistogram_16(hTexture, width, height, target);
	}
	case FS_Tex_Red_Invert:
	{
		return CUDA_invertColor_16(hTexture, width, height, target);
	}
	case CUDA_WindowWidthLevel:
	{
		return CUDA_OptWindowLevel(hTexture, width, height, target, cudaParam->windowWidth, cudaParam->windowLevel);
	}
	default:
		break;
	}
	return false;
}
static const std::string g_cudaConfigFile = "/cudaConfig.toml";
static bool isInit = false;
#include <filesystem>

bool CGCUDA_API CUDA_ConfigInit()
{
	if (isInit)
		return false;
	isInit = true;

	auto binPath = CGPath_GetPath(CGPathType::CG_PATH_CONFIG);
	std::wstring{ binPath };

	auto configPathstr = wstr2utf8(binPath);
	configPathstr += g_cudaConfigFile;

	bool isExists = std::filesystem::exists(configPathstr);
	if (!isExists)
		CreateDefultConfig(configPathstr.c_str());

	ReloadConfig(configPathstr.c_str());

	return true;
}

bool CGCUDA_API CUDA_ReleaseMemory()
{
	AutoReleaseGlobalMemory();
	return true;
}

bool CGCUDA_API CUDA_OpenglTexturebyShaderName(int contextid, int texture, std::vector<ShaderCodeName>vName, const CudaParameter* cudaParam)
{
	int width = -1;
	int height = -1;
	int type = -1;
	CGRender_GetTextureInfo(contextid, texture, &width, &height, &type);
	CGRender_MakeCurrent(contextid);
	for (auto data : vName)
	{
		CudaAlgorithm(data, texture, width, height, 0x0DE1, cudaParam);
	}
	CGRender_UnMakeCurrent(contextid);

	return false;
}
