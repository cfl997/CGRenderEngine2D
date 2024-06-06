#pragma once
#ifdef RENDER2D_EXPORTS
#define RENDER2D_EXPORT __declspec(dllexport)
#else
#define RENDER2D_EXPORT __declspec(dllimport)
#endif // RENDER2D_EXPORTS

// 操作参数
typedef struct _Parameter {
	long lbutton_mode;			// 左键模式
	long rbutton_mode;			// 右键模式 0xff：（默认） 1：矩形 2：子窗口 3：测量距离
	int  windowType;			// 窗口类型 0：主窗口 1：其他窗口
} Parameter;

// 图像属性
typedef struct _Property {
	bool anticolor;				// 反色
	bool equalization;			// 直方图均衡
} Property;

// 图像标签
typedef struct _Label {
	long type;					// 标签类型
	long code;					// 标签编码
	long x, y, w, h;			// 标签区域
} Label;

class RENDER2D_EXPORT Render2D
{
public:
	Render2D();
	~Render2D();
public:
	void Create(void* parent, int width, int height);
	void Release();
	void Resize(int width, int height);
	void SetMode(int mode);
	void SetHeight(int height);
	void SetDirection(int direction);
	void SetImage(void* data, int width, int height, bool dual);
	void SetParameter(const Parameter& parameter);
	void SetProperty(const Property& property);
	void SetLabels(Label* labels, int count);
	void GetLabels(Label* labels, int capacity);
	void Update();
public:
	/*
	* 窗宽窗位更改需要调用
	*/
	void SetWindowWidthLevel(int windowWidth, int windowLevel);
private:
	void* impl;
};

