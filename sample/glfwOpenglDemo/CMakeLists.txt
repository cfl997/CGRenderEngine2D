cmake_minimum_required (VERSION 3.18)

project(glfwOpenglDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)






set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(includes 
)

set(src
"main.cpp"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"${CGRender_ROOT_DIR}/include"
	${CGRender_ROOT_DIR}/include/CGRender
	${CGRender_ROOT_DIR}/include/CGRenderView
	${CGRender_ROOT_DIR}/include/CGData
	${CGRender_ROOT_DIR}/third/glm-1.0.1
)

link_directories(
	${CGRender_ROOT_DIR}/lib/win64/
)

set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "D")

add_executable(${PROJECT_NAME}  
${src}
${includes}
)
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "sample")


#add_dependencies(${PROJECT_NAME} CGRender CGData CGRenderView)
add_dependencies(${PROJECT_NAME} CGData CGRenderView)