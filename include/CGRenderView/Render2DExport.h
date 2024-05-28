#pragma once
#ifdef RENDER2D_EXPORTS
#define RENDER2D_API extern "C" __declspec(dllexport)
#else
#define RENDER2D_API extern "C" __declspec(dllimport)
#endif // RENDER2D_EXPORTS

/**
* @brief            创建窗体实例
* @param[in]        parent          父窗体句柄
* @param[in]        width           窗体宽度
* @param[in]        height          窗体高度
* @return           成功：实例id；失败：-1
*/
RENDER2D_API int Render2D_Create(void* parent, int width, int height);

/**
* @brief            释放窗体实例
* @param[in]        id              实例id
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_Release(int id);

/**
* @brief            重置窗体大小
* @param[in]        id              实例id
* @param[in]        width           窗体宽度
* @param[in]        height          窗体高度
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_Resize(int id, int width, int height);

/**
* @brief            设置图像模式
* @param[in]        id              实例id
* @param[in]        moed            图像模式：静态模式=0，动态模式(卷图模式)=1
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_SetMode(int id, int moed);

/**
* @brief            设置图像高度(动态模式)
* @param[in]        id              实例id
* @param[in]        height          图像高度
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_SetHeight(int id, int height);

/**
* @brief            设置卷图方向(动态模式)
* @param[in]        id              实例id
* @param[in]        direction       卷图方向：0 从左往右 1 从右往左
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_SetDirection(int id, int direction);

/**
* @brief            设置图像数据
* @param[in]        id              实例id
* @param[in]        data            图像数据(单通道16位)
* @param[in]        width           图像宽度
* @param[in]        height          图像高度
* @param[in]        dual            是否双能
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_SetImage(int id, void* data, int width, int height, bool dual);

/**
* @brief            设置操作参数
* @param[in]        id              实例id
* @param[in]        parameter       操作参数
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_SetParameter(int id, Parameter parameter);

/**
* @brief            设置图像属性
* @param[in]        id              实例id
* @param[in]        property        图像属性
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_SetProperty(int id, Property property);

/**
* @brief            设置图像标签
* @param[in]        id              实例id
* @param[in]        labels          图像标签数组
* @param[in]        count           标签个数
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_SetLabels(int id, Label* labels, int count);

/**
* @brief            获取图像标签
* @param[in]        id              实例id
* @param[in]        labels          图像标签数组
* @param[in]        capacity        数组容量
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_GetLabels(int id, Label* labels, int capacity);

/**
* @brief            刷新图像显示
* @param[in]        id              实例id
* @return           成功：0；失败：-1
*/
RENDER2D_API int Render2D_Update(int id);
