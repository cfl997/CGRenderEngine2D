#pragma once




/*
* renderEvnetType
*/

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

enum CGRenderMode
{
	RenderMode_static = 0,
	RenderMode_rolling,
	RenderMode_unknow = 0xff,
};

enum RenderViewCallBack
{
	closeWindow = 0,
	RenderViewCallBack_unKnow = 0xf,
};