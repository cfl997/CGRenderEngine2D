#ifndef _CGLAYER_H_
#define _CGLAYER_H_

#include "CGItem.h"
#include "CGEffector.h"

namespace CGData
{
	class CGDATA_API CGLayer :public CGItem
	{
	public:
		CGLayer(int Device);
		~CGLayer();

		CGItemType(CGLayer);
	public:
		void addItem(CGItem* item);
		CGItem* getItem(const std::wstring& guid);
		bool removeItem(const std::wstring& guid);
		bool removeItem(CGItemType itemType);
		std::vector<CGItem*>getItem(CGItemType itemType);
	public:
		void addImageShader(ShaderCodeName name);
		void removeImageShader(ShaderCodeName name);
		void removeAllImageShader();

		void Render(int device, const glm::mat4& matrix);

		void resetPosition(const glm::vec3& pos);
	public:
		CGData::CGEffector* Effector();

	public:
		void ProcessMouseMoveXY(float x, float y, bool xChange, bool yChange);
		void ProcessMousePress(float x, float y);
		void ProcessMouseRelease(float x, float y);
	private:
		struct PrivateData;
		PrivateData* m_priv = nullptr;
	};

}
#endif // !_CGLAYER_H_
