#ifndef _CGRENDER2D_H_
#define _CGRENDER2D_H_

#ifdef CGRENDERVIEW_EXPORTS
#define CGRENDERVIEW_API extern "C" __declspec(dllexport)
#else
#define CGRENDERVIEW_API extern "C" __declspec(dllimport)
#endif // CGRENDERVIEW_EXPORTS


CGRENDERVIEW_API char* CGRender2D_CreateView(unsigned int width, unsigned int height, void* parent);

CGRENDERVIEW_API void CGRender2D_ResizeWindow(const char* windowTitle, unsigned int width, unsigned int height);

CGRENDERVIEW_API void CGRender2D_addImage(const char* windowTitle, char* imagepath);

CGRENDERVIEW_API void 

#endif // !_CGRENDER2D_H_


