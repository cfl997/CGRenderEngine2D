#ifndef _CGITEMIMAGE_H_
#define _CGITEMIMAGE_H_

#include "CGItem.h"

namespace CGData
{
	class CGEffector;
	static const float g_ImageWHProportion = 10.f;
	class CGDATA_API CGItemImage : public CGItem
	{
		typedef CGItem Super;
	public:
		CGItemImage(int Device, const std::wstring& path);
		CGItemImage(int Device, int width, int height);
		~CGItemImage();

		CGItemType(CGItemImage);

	public:
		const int width()const;
		const int height()const;
		const int worldWidth()const;
		const int worldHeight()const;

		const int TextureID()const;
	public:
		void updateData(const std::wstring& path);
		void updateData(void* data, int width, int height);
	public:
		void Effector(CGEffector* effector = nullptr);
		void build(int Device)override;

		void Render(int device, const glm::mat4& matrix)override;
	private:
		void clearResourse();
		void loadPathData();
	private:
		struct PrivateData;
		PrivateData* m_priv = nullptr;
	};


}
#endif // !_CGITEMIMAGE_H_
