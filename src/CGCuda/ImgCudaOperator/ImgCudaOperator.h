#pragma once

#ifdef IMGCUDAOPERATOR_EXPORTS
#define IMGCUDAOPERATOR_API __declspec(dllexport)
#else
#define IMGCUDAOPERATOR_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// IMGCUDAOPERATOR_API void CreateDefultConfig(const char * config_file);
	// IMGCUDAOPERATOR_API void LoadConfig(const char* config_file);
	// IMGCUDAOPERATOR_API void SaveConfig(const char* config_file);
	// 
	// enum platform {
	// 	CPU,
	// 	GPU
	// };
	// 
	// IMGCUDAOPERATOR_API void SetEnablePlatform(platform p);

	struct image {
		int width;
		int height;
		void* data_ptr;
	};
	typedef image* image_ptr;

	// 反色
	IMGCUDAOPERATOR_API void OptInverseColor(image_ptr src);
	// 灰度扫描
	// 彩图（三色图）
	// 可变吸收率
	// 窗宽窗位
	IMGCUDAOPERATOR_API void OptWindowLevel(image_ptr src, int window_width, int window_level);
	IMGCUDAOPERATOR_API void OptWindowLevelRemap(image_ptr src, void* remap);
	// 超级穿透
	// 边缘增强
	// 超级增强
	// 直方图均衡
	IMGCUDAOPERATOR_API void OptHistogramEqualization(image_ptr src);

#ifdef __cplusplus
}
#endif
