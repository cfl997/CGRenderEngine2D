project(Render2D)
set(LIB_NAME Render2D)



SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CGRender_ROOT_DIR}/bin64/")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CGRender_ROOT_DIR}/bin64/")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CGRender_ROOT_DIR}/bin64/")

add_definitions(-DRENDER2D_EXPORTS)

include_directories(
	${CGRender_ROOT_DIR}/include
	${CGRender_ROOT_DIR}/include/CGRender
	${CGRender_ROOT_DIR}/include/CGRenderView
	${CGRender_ROOT_DIR}/include/CGData
	${CGRender_ROOT_DIR}/include/Render2D
)

set(COMMON_HEAD_PUBLIC

	${CGRender_ROOT_DIR}/include/Render2D/Render2D.h
	${CGRender_ROOT_DIR}/include/Render2D/Render2DExport.h
)


SET(COMMON_SOURCE_FILES

	Render2D.cpp
)

include_directories(
	${CGRender_ROOT_DIR}/third/glm-1.0.1	
)
link_directories(
	${CGRender_ROOT_DIR}/lib/win64/
	)

ADD_LIBRARY(${LIB_NAME} SHARED
	${COMMON_SOURCE_FILES} 
	${COMMON_HEAD_PUBLIC}
)

#target_link_libraries(${PROJECT_NAME} "opengl32.lib")

add_dependencies(${PROJECT_NAME} CGData CGRenderView)

source_group(${CG_SOURCE_FILES} FILES ${COMMON_SOURCE_FILES})
source_group("Public" FILES ${COMMON_HEAD_PUBLIC})
