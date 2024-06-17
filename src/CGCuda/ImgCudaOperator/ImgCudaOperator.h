#pragma once

#ifdef IMGCUDAOPERATOR_EXPORTS
#define IMGCUDAOPERATOR_API __declspec(dllexport)
#else
#define IMGCUDAOPERATOR_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	/// <summary>
	/// 创建指定默认配置文件，如果文件已存在则覆盖
	/// </summary>
	/// <param name="config_file">要创建的文件，例如‘config/config.toml’</param>
	/// <returns>0为成功，-1为无法打开指定路径文件</returns>
	IMGCUDAOPERATOR_API int CreateDefultConfig(const char * config_file);
	/// <summary>
	/// 从指定配置文件重新加载配置
	/// </summary>
	/// <param name="config_file">要加载的配置文件</param>
	/// <returns>0为成功，-1为无法打开指定路径文件，-2为toml解析失败，-3为转为配置失败</returns>
	IMGCUDAOPERATOR_API int ReloadConfig(const char* config_file);

	/// <summary>
	/// 自动释放全局内存
	/// </summary>
	IMGCUDAOPERATOR_API void AutoReleaseGlobalMemory();

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
	
	// 窗宽窗位
	IMGCUDAOPERATOR_API void OptWindowLevel(image_ptr src, int window_width, int window_level);
	IMGCUDAOPERATOR_API void OptWindowLevelRemap(image_ptr src, void* remap);
	// 基础增强
	IMGCUDAOPERATOR_API void OptBasicEnhance(image_ptr src, int kernel_size, float sigma, float alpha, float beta);
	// 超级增强 unsharp mask

	// 非均值滤波 anisotropic diffusion  
	
	// 直方图均衡
	IMGCUDAOPERATOR_API void OptHistogramEqualization(image_ptr src);

	enum opera_error {
		Res_Success,
		Res_UnknownException,
		Res_InvalidAlgorithm,
		Res_InvalidImage,
		Res_InvalidCallback
	};
	enum algorithm_type {
		Opt_Nothing,
		Opt_InverseColor,
		Opt_WindowLevel,
		Opt_WindowLevelRemap,
		Opt_Laplacian,
		Opt_HistogramEqualization
	};
	struct algorithm {
		algorithm_type type;
	 	struct window_level {
	 		int window_width;
	 		int window_level;
	 	}window_level;
	 	struct window_level_remap {
	 		void* remap;
	 	}window_level_remap;
	};

	IMGCUDAOPERATOR_API algorithm MakeAlgorithm(algorithm_type type);
	IMGCUDAOPERATOR_API int Opera(image_ptr src, image_ptr dst, algorithm algo);
	IMGCUDAOPERATOR_API int Operas(image_ptr src, image_ptr dst, algorithm* algos, unsigned int size);

#ifdef __cplusplus
}
#endif
