#ifndef _CGRENDER_H_
#define _CGRENDER_H_


#include "CGRender_Exports.h"
#include "CGRenderMacros.h"
#include "CGRenderAPI.h"
#include "CGglm.h"
#include "CGPath.h"


#if defined(_MSC_VER) || defined(WIN32) || defined(WIN64)
#ifdef _DEBUG
#pragma comment(lib,"CGRenderD.lib")
#else
#pragma comment(lib,"CGRender.lib")
#endif
#endif

#endif // !_CGRENDERGL_H_
