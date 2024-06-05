

/* File : example.i */
%include <windows.i>
%include <typemaps.i>
//%include <wchar.i>
//%include <csharp/wchar.i>


// knows about things like int *OUTPUT:
%include "typemaps.i"
// knows about int32_t
%include "stdint.i"
%include "arrays_csharp.i"
%include "std_string.i"
//%apply int[] {int *};

// convert char * to byte array
//%apply signed char[] {char* pchar}; 





%module CGRender2D



//current
%typemap(cstype) void* "IntPtr"
%typemap(imtype) void* "IntPtr"
%typemap(csin) void* "$csinput"
%typemap(csout) void* "$csoutput"



%{
#include "RenderView.h"
#include "CGRenderType.h"
//#include "Render2D.h"


using namespace CGRender;

#include <vector>
#include <string>
#include <memory>
#include <stdint.h>


%}


enum CGRenderEventType
{
	RenderEvent_Default = 0,
	RenderEvent_Rectangle = 1,
	RenderEvent_RectWindow,
	RenderEvent_MeasureDistance,
	CGRenderEventType_unknow = 0xff,
};

enum WindowType
{
	Window_Main = 0,
	Window_One = 1,
	Window_unknow = 0xff
};
namespace CGRender
{
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
		bool createWindow(void* parentWindow, const char* Title, unsigned int  windowWidth, unsigned int  windowHeight, void* share_Window, WindowType type);

		bool deleteWindow(const char* title);
	public:
		/*
		* c#
		* event
		*/
		bool addCGRenderEvent(CGRenderEventType EventType);
		bool addImage(const char* windowTitle, const char* imagePath);
		bool resizeWindow(const char* windowTitle, unsigned int width, unsigned int height);
	public:
		/*
		* render
		*/
		void Render();
	public:
		/*
		* Texture
		*/
	private:
		struct PrivateRenderView;
		PrivateRenderView* m_priv;
		
	};

}

//// 操作参数
//struct Parameter {
//	long lbutton_mode;			// 左键模式
//	long rbutton_mode;			// 右键模式 0xff：（默认） 1：矩形 2：子窗口 3：测量距离
//	int  windowType;			// 窗口类型 0：主窗口 1：其他窗口
//};
//
//// 图像属性
//struct Property {
//	bool anticolor;				// 反色
//	bool equalization;			// 直方图均衡
//};
//
//// 图像标签
//struct Label {
//	long type;					// 标签类型
//	long code;					// 标签编码
//	long x, y, w, h;			// 标签区域
//};
//
//class Render2D
//{
//public:
//	Render2D();
//	~Render2D();
//public:
//	void Create(void* parent, int width, int height);
//	void Release();
//	void Resize(int width, int height);
//	void SetMode(int mode);
//	void SetHeight(int height);
//	void SetDirection(int direction);
//	void SetImage(void* data, int width, int height, bool dual);
//	void SetParameter(const Parameter& parameter);
//	void SetProperty(const Property& property);
//	void SetLabels(Label* labels, int count);
//	void GetLabels(Label* labels, int capacity);
//	void Update();
//};




extern "C" int testswig_add(int a, int b);
extern "C" int testswig_char(char a[], int b);

//%include "CGWindowsWindos.h"