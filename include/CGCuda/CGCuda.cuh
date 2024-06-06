#ifndef _CGCUDA_CUH_
#define _CGCUDA_CUH_

#include "CGRender.h"
#include <vector>

#ifdef CGCUDA_EXPORTS
#define CGCUDA_API __declspec(dllexport)
#else
#define CGCUDA_API __declspec(dllimport)
#endif // CGCUDA_EXPORTS


bool CGCUDA_API CUDA_OpenglTexturebyShaderName(int contextid, int texture, std::vector<ShaderCodeName>vName, const CudaParameter * cudaParam);

#endif // !_CGCUDA_CUH_

