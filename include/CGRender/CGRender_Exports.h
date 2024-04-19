#pragma once


#ifdef GLRENDER_EXPORTS
#define GLRENDER_API __declspec(dllexport)
#else
#define  GLRENDER_API __declspec(dllimport)
#endif
