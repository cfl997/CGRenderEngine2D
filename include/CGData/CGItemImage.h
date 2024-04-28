#ifndef _CGITEMIMAGE_H_
#define _CGITEMIMAGE_H_

#include "CGItem.h"

namespace CGData
{
	class CGDATA_API CGItemImage : public CGItem
	{
		typedef CGItem Super;
	public:
		CGItemImage(int Device, const std::wstring& path);
		CGItemImage(int Device, int width, int height);
		~CGItemImage();

		CGItemType(CGItemImage);
	public:
		void updateData(const std::wstring& path);
		void updateData(void* data, int width, int height);
	public:
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
