

/* File : example.i */
%include <windows.i>

%module CGRender2D


%{

//#ifdef CGRENDERVIEW_EXPORTS
//#define CGRENDERVIEW_API __declspec(dllexport)
//#else
//#define CGRENDERVIEW_API __declspec(dllimport)
//#endif // CGRENDERVIEW_EXPORTS
//#include "testswig.h"

#include "RenderView.h"
#include "CGWindow.h"
#include "CGWindowsWindos.h"
#include "CGRenderType.h"
//namespace CGRender
//{
//	enum WindowType
//	{
//		Window_Main = 0,
//		Window_One = 1,
//		Window_unknow = 0xff
//	};
//}

enum CGRenderEventType_swig
{
	Event_Default = 0,
	Event_Rectangle = 1,
	Event_RectWindow,
	Event_MeasureDistance,
	EventType_unknow = 0xff,
};

using namespace CGRender;

%}

/* Let's just grab the original header file here */
//%include "testswig.h"
%include <windows.i>
%include "RenderView.h"
%include "CGWindowsWindos.h"