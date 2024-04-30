#ifndef _GTESTINCLUDE_H_
#define _GTESTINCLUDE_H_


#include "CGRender.h"
#include "CGData.h"
#include "CGRenderView.h"

#include "gtest/gtest.h"


#include <string>

#define PICTUREFILENAME L"e:/saveImageFile.png"
#define RESOURCE_FILE_PATH L"../src/gTestCGRenderView/testFiles/"

namespace gTest
{



	bool compareImage(const std::wstring& srcimage, const std::wstring& desimage);
}

#endif // !_GTESTINCLUDE_H_
