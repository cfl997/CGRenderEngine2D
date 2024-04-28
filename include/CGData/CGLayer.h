#ifndef _CGLAYER_H_
#define _CGLAYER_H_

#include "CGItem.h"

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
		void setImageShader(ShaderCodeName shadercodeName);

		void Render(int device, const glm::mat4& matrix);

		void setPosition(const glm::vec3& pos);

	public:
		void ProcessMouseMoveXY(float x, float y);
		void ProcessMousePress(float x, float y);
		void ProcessMouseRelease(float x, float y);
	private:
		struct PrivateData;
		PrivateData* m_priv = nullptr;
	};

}
#endif // !_CGLAYER_H_
