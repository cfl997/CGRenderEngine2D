project(CGData)
set(LIB_NAME CGData)



SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CGRender_ROOT_DIR}/bin64/")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CGRender_ROOT_DIR}/bin64/")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CGRender_ROOT_DIR}/bin64/")

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DCGDATA_EXPORTS)


#find_package(Freetype REQUIRED)

include_directories(
	${CGRender_ROOT_DIR}/include
	${CGRender_ROOT_DIR}/include/CGRender
	${CGRender_ROOT_DIR}/include/CGData
	${CGRender_ROOT_DIR}/include/CGCuda
)

set(COMMON_HEAD_PUBLIC
	${CGRender_ROOT_DIR}/include/CGData/CGData.h
	${CGRender_ROOT_DIR}/include/CGData/CGItem.h
	${CGRender_ROOT_DIR}/include/CGData/CGObject.h
	${CGRender_ROOT_DIR}/include/CGData/CGData_Exports.h
	${CGRender_ROOT_DIR}/include/CGData/CGItemRectangle.h
	${CGRender_ROOT_DIR}/include/CGData/CGLayer.h
	${CGRender_ROOT_DIR}/include/CGData/CGItemTex16.h
	${CGRender_ROOT_DIR}/include/CGData/CGItemText.h
	${CGRender_ROOT_DIR}/include/CGData/CGItemImage.h
	${CGRender_ROOT_DIR}/include/CGData/CGItemMeasureDistance.h
	${CGRender_ROOT_DIR}/include/CGData/CGBounding.h
	${CGRender_ROOT_DIR}/include/CGData/CGEffector.h
)

SET(COMMON_HEAD_FILES
)


SET(COMMON_SOURCE_FILES
	CGItem.cpp
	CGObject.cpp
	CGItemRectangle.cpp
	CGLayer.cpp
	CGItemTex16.cpp
	CGItemText.cpp
	CGItemImage.cpp
	CGItemMeasureDistance.cpp
	CGBounding.cpp
	CGEffector.cpp
)




if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_definitions("-DCGData_DEBUG")
else()
endif()


include_directories(
	${CGRender_ROOT_DIR}/include/CGRenderGL
	${CGRender_ROOT_DIR}/third/glm-1.0.1
	${CGRender_ROOT_DIR}/third/freetype-2.13.0/Debug/include/freetype2
)
link_directories(
	${CGRender_ROOT_DIR}/lib/win64/
	)

ADD_LIBRARY(${LIB_NAME} SHARED
	${COMMON_SOURCE_FILES} 
	${COMMON_HEAD_FILES} 
	${COMMON_HEAD_PUBLIC}
)

if(CMAKE_BUILD_TYPE MATCHES "Debug") 
target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/freetype-2.13.0/Debug/lib/freetyped.lib)
else()
target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/freetype-2.13.0/Release/lib/freetype.lib)
endif()

add_dependencies(${PROJECT_NAME} CGCuda)

source_group(${CG_HEADER_FILES} FILES ${COMMON_HEAD_FILES})
source_group(${CG_SOURCE_FILES} FILES ${COMMON_SOURCE_FILES})
source_group("Public" FILES ${COMMON_HEAD_PUBLIC})
