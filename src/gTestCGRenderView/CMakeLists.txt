cmake_minimum_required(VERSION 3.18)

project(gTestRenderView)




include_directories(
	"${CGRender_ROOT_DIR}/include"
	${CGRender_ROOT_DIR}/include/CGRender
	${CGRender_ROOT_DIR}/include/CGRenderView
	${CGRender_ROOT_DIR}/include/CGData
	${CGRender_ROOT_DIR}/third/glm-1.0.1
	${CGRender_ROOT_DIR}/third/gTest/Debug/include
)

link_directories(
	${CGRender_ROOT_DIR}/lib/win64/
)

set(common_src
	gTestCGRenderView.cpp
	gtestInclude.cpp
)
set(common_header
	gtestInclude.h
)

add_executable(${PROJECT_NAME}
${common_src}
${common_header}
)
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/gTest/Debug/lib/gtest.lib)
	target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/gTest/Debug/lib/gtest_main.lib)
else()
	target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/gTest/Release/lib/gtest.lib)
	target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/gTest/Release/lib/gtest_main.lib)
endif()


SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "gTest")

add_dependencies(${PROJECT_NAME} CGRenderView CGData)

