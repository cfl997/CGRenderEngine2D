#pragma once



#ifdef CGDATA_EXPORTS
#define CGDATA_API __declspec(dllexport)
#else
#define CGDATA_API __declspec(dllimport)
#endif // CGDATA_EXPORTS
