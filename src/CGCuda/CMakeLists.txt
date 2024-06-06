project(CGCuda)

enable_language(CUDA)
find_package(CUDAToolkit REQUIRED)
include_directories(${CUDAToolkit_INCLUDE_DIRS} ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

add_definitions(-DCGCUDA_EXPORTS)

include_directories(
    ${CGRender_ROOT_DIR}/include
    ${CGRender_ROOT_DIR}/include/CGRender
    ${CGRender_ROOT_DIR}/include/CGCuda
    ${CGRender_ROOT_DIR}/third/glm-1.0.1
)

link_directories(
    ${CGRender_ROOT_DIR}/lib/win64/
)

set(common_header
    ${CGRender_ROOT_DIR}/include/CGCuda/CGCuda.h
    ${CGRender_ROOT_DIR}/include/CGCuda/CGCuda.cuh
)
set(common_src
    CGCuda.cu
    CGCudaHelper.cpp
    CGCudaHelper.h
)
set(cuda_image
    ImgCudaOperator/ImgCudaOperator.h
)

add_library(${PROJECT_NAME} SHARED
    ${common_header} 
    ${common_src} 
    ${cuda_image} 
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CGRender_ROOT_DIR}/include
    ${CGRender_ROOT_DIR}/include/CGRender
)

target_compile_definitions(${PROJECT_NAME} PRIVATE CGCUDA_EXPORTSaaaaaa)

#target_link_libraries(${PROJECT_NAME} ${CUDAToolkit_LIBRARIES})
target_link_libraries(${PROJECT_NAME} "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.5/lib/x64/cudart.lib")
target_link_libraries(${PROJECT_NAME} "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.5/lib/x64/cuda.lib")

source_group("cuda" FILES ${common_src} ${common_header})

# …Ë÷√CUDA±‡“Î—°œÓ
set_target_properties(${PROJECT_NAME} PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
)
