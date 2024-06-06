#pragma once



#ifdef _DEBUG
// CUDA´íÎó¼ì²éºê
#define CUDA_CHECK_RETURN(value) { \
    cudaError_t _m_cudaStat = value; \
    if (_m_cudaStat != cudaSuccess) { \
        fprintf(stderr, "Error %s at line %d in file %s\n", \
                cudaGetErrorString(_m_cudaStat), __LINE__, __FILE__); \
        assert(0); \
    } \
}
#else
#define CUDA_CHECK_RETURN(value) value

#endif // DEBUG