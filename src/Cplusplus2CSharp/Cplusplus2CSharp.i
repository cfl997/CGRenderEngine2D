

/* File : example.i */
%include <windows.i>

%module CGRender2D


%{

#ifdef CGRENDERVIEW_EXPORTS
#define CGRENDERVIEW_API __declspec(dllexport)
#else
#define CGRENDERVIEW_API __declspec(dllimport)
#endif // CGRENDERVIEW_EXPORTS
//#include "../../include/CGRenderView/testswig.h"

#include "../../include/CGRenderView/RenderView.h"
#include "../../include/CGRenderView/CGWindow.h"
#include "../../include/CGRenderView/CGWindowsWindos.h"



//namespace CGRender
//{
//	enum WindowType
//	{
//		Window_Main = 0,
//		Window_One = 1,
//		Window_unknow = 0xff
//	};
//}
using namespace CGRender;

%}

/* Let's just grab the original header file here */
%include "../../include/CGRenderView/testswig.h"
%include <windows.i>
%include "../../include/CGRenderView/RenderView.h"
%include "../../include/CGRenderView/CGWindowsWindos.h"