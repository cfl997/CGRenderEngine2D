project(CGRenderView)
set(LIB_NAME CGRenderView)



SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CGRender_ROOT_DIR}/bin64/")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CGRender_ROOT_DIR}/bin64/")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CGRender_ROOT_DIR}/bin64/")


add_definitions(-DCGRENDERVIEW_EXPORTS)
add_definitions(-Duse_Window_Title)#标题栏会导致 页面发生偏差
add_definitions(-DUSE_ORTHO)#使用平截头体（正射投影）


include_directories(
	${CGRender_ROOT_DIR}/include
	${CGRender_ROOT_DIR}/include/CGRender
	${CGRender_ROOT_DIR}/include/CGRenderView
	${CGRender_ROOT_DIR}/include/CGData
)

set(COMMON_HEAD_PUBLIC

	${CGRender_ROOT_DIR}/include/CGRenderView/CGRenderView_Export.h
	${CGRender_ROOT_DIR}/include/CGRenderView/CGWindowsWindos.h
	${CGRender_ROOT_DIR}/include/CGRenderView/CGWindow.h
	${CGRender_ROOT_DIR}/include/CGRenderView/CGRenderView.h
	${CGRender_ROOT_DIR}/include/CGRenderView/CGRenderType.h
	${CGRender_ROOT_DIR}/include/CGRenderView/RenderView.h
	${CGRender_ROOT_DIR}/include/CGRenderView/RenderView.h
)

SET(COMMON_HEAD_FILES
	CGCamera.h
	CGCore.h
)

set(EVENT_FILES
    Event/Event.h
    Event/WindowsEvent.h
    Event/KeyEvent.h
    Event/MouseEvent.h
    Event/CGRenderEvent.cpp
    Event/CGRenderEvent.h
    Event/CGRenderEventManager.h
    Event/CGRenderEventManager.cpp
    Event/CGRenderEventFactory.h
)


SET(COMMON_SOURCE_FILES
	CGWindowsWindos.cpp
	CGCamera.cpp
	RenderView.cpp
)

SET(ITEM_EVENT
	Event/CGRenderEventRectangle.h
	Event/CGRenderEventRectangle.cpp
	Event/CGRenderEventMeasureDistance.cpp
    Event/CGRenderEventMeasureDistance.h
)


if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_definitions("-DCGRenderView_DEBUG")
else()
endif()


include_directories(
	"${CGRender_ROOT_DIR}/third/glfw-3.4/include/"
	${CGRender_ROOT_DIR}/third/glm-1.0.1	
)
link_directories(
	${CGRender_ROOT_DIR}/lib/win64/
	)
link_directories(
	${CGRender_ROOT_DIR}/lib/win64/
)

ADD_LIBRARY(${LIB_NAME} SHARED
	${COMMON_SOURCE_FILES} 
	${COMMON_HEAD_FILES} 
	${COMMON_HEAD_PUBLIC}
	${EVENT_FILES}
	${ITEM_EVENT}
)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/glfw-3.4/lib/Debug/glfw3.lib )
	#target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/glfw-3.4/lib/Debug/glfw3dll.lib )
	#add_definitions("-DGLRender_DEBUG")
else()
	target_link_libraries(${PROJECT_NAME} ${CGRender_ROOT_DIR}/third/glfw-3.4/lib/Release/glfw3.lib )
endif()



target_link_libraries(${PROJECT_NAME} "opengl32.lib")

add_dependencies(${PROJECT_NAME} CGData)

source_group(${CG_HEADER_FILES} FILES ${COMMON_HEAD_FILES})
source_group(${CG_SOURCE_FILES} FILES ${COMMON_SOURCE_FILES})
source_group("Public" FILES ${COMMON_HEAD_PUBLIC})
source_group("Event" FILES ${EVENT_FILES})

source_group("EventItem" FILES ${ITEM_EVENT})