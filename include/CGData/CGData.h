#ifndef _CGDATA_H_
#define _CGDATA_H_


#include "CGObject.h"
#include "CGItem.h"
#include "CGLayer.h"
#include "CGItemRectangle.h"
#include "CGItemTex16.h"
#include "CGItemText.h"
#include "CGItemImage.h"

#if defined(_MSC_VER) || defined(WIN32) || defined(WIN64)
#ifdef _DEBUG
#pragma comment(lib,"CGDataD.lib")
#else
#pragma comment(lib,"CGData.lib")
#endif
#endif

#endif // !_CGDATA_H_
