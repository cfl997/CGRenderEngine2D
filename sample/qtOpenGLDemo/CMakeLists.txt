cmake_minimum_required (VERSION 3.18)

project(qtOpenGLDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

#if(WIN32)
#	if (QTDIR OR DEFINED ENV{QTDIR} OR DEFINED ENV{QTDIR32} OR DEFINED ENV{QTDIR64})
#		# Qt path set by user or env var
#	else()
#		set(QTDIR "" CACHE PATH "Path to Qt (e.g. C:/Qt/5.7/msvc2015_64)")
#		message(WARNING "QTDIR variable is missing.  Please set this variable to specify path to Qt (e.g. C:/Qt/5.7/msvc2015_64)")
#	endif()
#endif()

if(MSVC)
    add_compile_definitions(_HAS_CXX17=1)
    add_compile_options(/Zc:__cplusplus)
endif()

if(DEFINED QTDIR${_lib_suffix})
	list(APPEND CMAKE_PREFIX_PATH "${QTDIR${_lib_suffix}}")
elseif(DEFINED QTDIR)
	list(APPEND CMAKE_PREFIX_PATH "${QTDIR}")
elseif(DEFINED ENV{QTDIR${_lib_suffix}})
	list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR${_lib_suffix}}")
elseif(DEFINED ENV{QTDIR})
	list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(includes 
"QtDisplayWidget.h"
)

set(src
"main.cpp"
"QtDisplayWidget.cpp"
)
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"${CGRender_ROOT_DIR}/include"
	${CGRender_ROOT_DIR}/include/CGRender
	${CGRender_ROOT_DIR}/include/CGRenderView
)

	link_directories(
		${CGRender_ROOT_DIR}/lib/win64/
		)

add_executable(${PROJECT_NAME}  
${src}
${includes}
)
target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Core)
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "sample")


#if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
#	set_target_properties(${PROJECT_NAME}
#		PROPERTIES
#		OUTPUT_NAME ${PROJECT_NAME}D
#		)
#endif()
#add_dependencies(${PROJECT_NAME} CGRender CGData)
add_dependencies(${PROJECT_NAME} CGData CGRenderView)

# 添加包含 DLL 的文件夹路径
set(EXTRA_DLL_PATH "C:/Qt/6.6.3/msvc2019_64/bin")

# 设置环境变量 PATH，告诉程序运行时去哪里找 DLL
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${EXTRA_DLL_PATH}"
)