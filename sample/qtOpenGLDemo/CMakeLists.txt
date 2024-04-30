cmake_minimum_required (VERSION 3.18)

project(qtOpenGLDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# 添加编译器选项
if (MSVC)
    add_compile_options(/permissive-)
endif()
if(WIN32)
	if (QTDIR OR DEFINED ENV{QTDIR} OR DEFINED ENV{QTDIR32} OR DEFINED ENV{QTDIR64})
		# Qt path set by user or env var
	else()
		set(QTDIR "C:/Qt/6.6.3/msvc2019_64" CACHE PATH "Path to Qt (e.g. C:/Qt/5.7/msvc2015_64)")
		#message(WARNING "QTDIR variable is missing.  Please set this variable to specify path to Qt (e.g. C:/Qt/5.7/msvc2015_64)")
	endif()
endif()


# 在 Release 模式下禁用某些优化选项
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # 在这里添加你想要禁用的优化选项，例如 -O2 或者 -O3
    string(REPLACE "-O2" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REPLACE "-O3" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif()



if(MSVC)
    add_compile_definitions(_HAS_CXX17=1)
    add_compile_options(/Zc:__cplusplus)
endif()

if(DEFINED QTDIR${_lib_suffix})
	list(APPEND CMAKE_PREFIX_PATH "${QTDIR${_lib_suffix}}")
	message("aaa1" ${_lib_suffix})
elseif(DEFINED QTDIR)
	list(APPEND CMAKE_PREFIX_PATH "${QTDIR}")
	message("aaa2")
elseif(DEFINED ENV{QTDIR${_lib_suffix}})
	list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR${_lib_suffix}}")
	message("aaa3")
elseif(DEFINED ENV{QTDIR})
	list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
	message("aaa4")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


# 指定UI文件所在目录
set(ui_files
    "QtDisplayWidget.ui"
    # 添加其他UI文件，如果有多个的话
)



set(includes 
"QtDisplayWidget.h"
)

set(src
"main.cpp"
"QtDisplayWidget.cpp"
)
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6UiTools REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	"${CGRender_ROOT_DIR}/include"
	${CGRender_ROOT_DIR}/include/CGRender
	${CGRender_ROOT_DIR}/include/CGRenderView
	${CGRender_ROOT_DIR}/include/CGData
)

	link_directories(
		${CGRender_ROOT_DIR}/lib/win64/
		)

add_executable(${PROJECT_NAME}  
${src}
${includes}
${ui_files}
)
target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Core Qt6::UiTools )
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "sample")

# 使用Qt的UI文件生成器生成对应的C++代码
#qt6_wrap_ui(ui_generated_files ${ui_files})
qt6_wrap_ui(ui_generated_files ${ui_files} OPTIONS -o ${CMAKE_CURRENT_BINARY_DIR}/Generated_uis)





# 将UI文件目录添加到包含目录中，确保可以找到生成的头文件
include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated_uis)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set_target_properties(${PROJECT_NAME}
		PROPERTIES
		OUTPUT_NAME ${PROJECT_NAME}D
		)      
endif()
#add_dependencies(${PROJECT_NAME} CGRender CGData)
add_dependencies(${PROJECT_NAME} CGData CGRenderView)

# 添加包含 DLL 的文件夹路径
#set(EXTRA_DLL_PATH "C:/Qt/6.6.3/msvc2019_64/bin")

## 设置环境变量 PATH，告诉程序运行时去哪里找 DLL
#set_target_properties(${PROJECT_NAME} PROPERTIES
#    VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${EXTRA_DLL_PATH}"
#)


