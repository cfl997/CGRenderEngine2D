#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <iostream>


#ifdef CGRenderView_DEBUG
#define GLRenderView_ENABLE_ASSERTS
#endif

#ifdef GLRenderView_ENABLE_ASSERTS
//#define GLRender_ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
//#define GLRender_ASSERT(fmt,...) (std::cout<<fmt<<##__VA_ARGS__<<__LINE__<<__TIME__<<__DATE__<<std::endl;));
//#define GLRender_LOG(fmt,...) (std::cout<<fmt<<##__VA_ARGS__<<std::endl;));

// ASSERT 宏用于输出带有定位信息的错误信息
#define GLRender_ASSERT(fmt, ...) if(!(fmt))\
    (std::cout << "ASSERT: " << fmt << " [Line: " << __LINE__ << "] " << __FILE__ << " " << __DATE__ << " " << __TIME__ << std::endl)

#define GLRender_LOG(fmt, ...) std::cout << "LOG: " << fmt<< ##__VA_ARGS__ << std::endl; 
#else
#define GLRender_ASSERT(x, ...)
#define GLRender_LOG(x, ...)

#endif

#define BIT(x) (1 << x)

#define GLRenderView_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


#define SAFE_DELETE(x) if(x)\
{\
delete x;\
x=nullptr;\
}