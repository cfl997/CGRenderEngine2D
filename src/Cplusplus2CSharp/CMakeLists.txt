cmake_minimum_required(VERSION 3.18)
project(Cplusplus2CSharp VERSION 1.0 LANGUAGES CXX)

#add_library(swig_warning_options INTERFACE)
## 设置 C# 生成目录
#
set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0122 NEW)
#
if ( MSVC )
  set(SWIG_DIR "${CGRender_ROOT_DIR}/third/swig/Lib")
  set(SWIG_Source_DIR "${CGRender_ROOT_DIR}/third/swig/Source/Include")
  include_directories(${SWIG_DIR})
  include_directories(${SWIG_Source_DIR})
  set(SWIG_EXECUTABLE "${CGRender_ROOT_DIR}/third/swig/swig.exe")  
endif (MSVC)
#


include_directories(
	${CGRender_ROOT_DIR}/include
	${CGRender_ROOT_DIR}/include/CGRender
	${CGRender_ROOT_DIR}/include/CGRenderView
	${CGRender_ROOT_DIR}/include/CGData
)
include_directories(
	"${CGRender_ROOT_DIR}/third/glfw-3.4/include/"
	${CGRender_ROOT_DIR}/third/glm-1.0.1	
)
link_directories(
	${CGRender_ROOT_DIR}/lib/win64/
	)

###############################################################################
#                                S W I G                                      #
###############################################################################

find_package(SWIG REQUIRED)

include(${SWIG_USE_FILE})


message("swig use include :" ${SWIG_USE_FILE})
message("swig use include :" ${SWIG_DIR})

## Set .Net project directory
set(CGRenderView_include_DIR "${CGRender_ROOT_DIR}/include/CGRenderView")
set(CGRenderView_src_DIR "${CGRender_ROOT_DIR}/src/CGRenderView")
set(Cplusplus2CSharp_DIR "${CGRender_ROOT_DIR}/include/Cplusplus2CSharp")

## Add swig flags here
#set(CMAKE_SWIG_FLAGS "")


set_property(SOURCE Cplusplus2CSharp.i PROPERTY CPLUSPLUS ON)
#set_source_files_properties(Cplusplus2CSharp.i PROPERTIES SWIG_FLAGS "-includeall")
#
set(swg_source_file
    #${CGRenderView_include_DIR}/testswig.h ${CGRenderView_src_DIR}/testswig.cpp
    ${CGRenderView_include_DIR}/RenderView.h ${CGRenderView_src_DIR}/RenderView.cpp
    ${CGRenderView_include_DIR}/CGWindowsWindos.h ${CGRenderView_src_DIR}/CGWindowsWindos.cpp
    ${CGRenderView_include_DIR}/CGWindow.h 

    ${CGRenderView_src_DIR}/Event/CGRenderEventFactory.h
    ${CGRenderView_src_DIR}/Event/CGRenderEventManager.h ${CGRenderView_src_DIR}/Event/CGRenderEventManager.cpp
    ${CGRenderView_src_DIR}/Event/CGRenderEvent.h ${CGRenderView_src_DIR}/Event/CGRenderEvent.cpp
    ${CGRenderView_src_DIR}/Event/CGRenderEventRectangle.h ${CGRenderView_src_DIR}/Event/CGRenderEventRectangle.cpp
    ${CGRenderView_src_DIR}/Event/CGRenderEventRectWindow.h ${CGRenderView_src_DIR}/Event/CGRenderEventRectWindow.cpp
    #${CGRenderView_include_DIR}/CGWindowsWindos.h ${CGRenderView_src_DIR}/CGWindowsWindos.cpp
    #${CGRenderView_include_DIR}/CGWindowsWindos.h ${CGRenderView_src_DIR}/CGWindowsWindos.cpp
    #${CGRenderView_include_DIR}/CGWindowsWindos.h ${CGRenderView_src_DIR}/CGWindowsWindos.cpp

    ${CGRenderView_include_DIR}/CGRenderView_Export.h 
    ${CGRenderView_src_DIR}/CGCamera.h ${CGRenderView_src_DIR}/CGCamera.cpp

)

swig_add_library(Cplusplus2CSharp 
  TYPE SHARED
  LANGUAGE CSharp
  SOURCES Cplusplus2CSharp.i ${swg_source_file}
  OUTPUT_DIR ${Cplusplus2CSharp_DIR}
  OUTFILE_DIR ${CGRenderView_include_DIR}
  )
#
#

set_target_properties( Cplusplus2CSharp
    PROPERTIES
    # These copy animal.dll on Windows to .Net project directory
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CGRender_ROOT_DIR}/bin64/
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CGRender_ROOT_DIR}/bin64/
    
    # Set address of C++ headers

    #INCLUDE_DIRECTORIES ${swig_include_file} 

)


if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/glfw-3.4/lib/Debug/glfw3.lib )
else()
	target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/glfw-3.4/lib/Release/glfw3.lib )
endif()

