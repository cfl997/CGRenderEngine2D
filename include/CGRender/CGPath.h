#ifndef _CGPATH_H_
#define _CGPATH_H_

#include "CGRender_Exports.h"
#include <string>

enum CGPathType
{
	CG_PATH_BIN = 0,
	CG_PATH_PLUGIN,
	CG_PATH_FONTS,
	CG_PATH_COUNT,									//预定义

	CG_PATH_CUSTOM_FIRST = CG_PATH_COUNT,			//自定义
	CG_PATH_MAX_COUNT = CG_PATH_CUSTOM_FIRST + 9,
};

GLRENDER_API bool CGPath_Init();
GLRENDER_API void CGPath_Release();

GLRENDER_API void CGPath_SetPath(CGPathType type, const wchar_t* szPath);
GLRENDER_API wchar_t* CGPath_GetPath(CGPathType dwType);

GLRENDER_API std::string wstr2utf8(const std::wstring& wstr);
GLRENDER_API std::wstring utf82wstr(const std::string& wstr);

GLRENDER_API bool CGText_Init();//CGPath_Init()包含 更新数据后再次调用
GLRENDER_API const std::wstring CGText_TextByCode(long code);

#endif // !_CGPATH_H_
