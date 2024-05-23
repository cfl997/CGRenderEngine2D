

/* File : example.i */
%include <windows.i>
%include <typemaps.i>
%include <wchar.i>
%include <csharp/wchar.i>


// knows about things like int *OUTPUT:
%include "typemaps.i"
// knows about int32_t
%include "stdint.i"
%include "arrays_csharp.i"
%include "std_string.i"
%apply int[] {int *};

// convert char * to byte array
%apply signed char[] {char* pchar}; 





%module CGRender2D



//current
%typemap(cstype) void* "IntPtr"
%typemap(imtype) void* "IntPtr"
%typemap(csin) void* "$csinput"
%typemap(csout) void* "$csoutput"



%{
#include "RenderView.h"
#include "CGWindow.h"
#include "CGWindowsWindos.h"
#include "CGRenderType.h"


enum CGRenderEventType_swig
{
	Event_Default = 0,
	Event_Rectangle = 1,
	Event_RectWindow,
	Event_MeasureDistance,
	EventType_unknow = 0xff,
};

using namespace CGRender;

#include <vector>
#include <string>
#include <memory>
#include <stdint.h>


%}




namespace CGRender
{

	enum WindowType
	{
		Window_Main = 0,
		Window_One = 1,
		Window_unknow = 0xff
	};

	class CGRenderView
	{
	public:
		CGRenderView(unsigned int width, unsigned int height, void* parent);
		~CGRenderView();
	private:
		CGRenderView() = delete;
	public:
		/*
		* window
		*/
		bool createWindow(void* parentWindow,const std::string& Title, unsigned int  windowWidth, unsigned int  windowHeight, void* share_Window, WindowType type);

		bool deleteWindow(const std::string& title);

		//std::shared_ptr<CGRender::Window>getWindowByTitle(const std::string& title);

		//bool closeWindow(const std::wstring& title);
	public:
		/*
		* event
		*/
	public:
		/*
		* render
		*/
		void Render();


	public:
		/*
		* Texture
		*/
	};

}

extern "C" int testswig_add(int a, int b);
extern "C" int testswig_char(char a[], int b);
extern "C" int testswig_string(std::string a);


//%include "CGWindowsWindos.h"