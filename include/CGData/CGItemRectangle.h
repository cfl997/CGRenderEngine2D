#ifndef _CGITEMSHAPE_H_
#define _CGITEMSHAPE_H_

#include "CGItem.h"
#include "../CGRender/CGglm.h"


namespace CGData
{
	enum RectangleType
	{
		Normal = 0,//默认绘制方式
		Window = 1,//这个只在绘制时绘制
	};
	class CGDATA_API CGItemRectangle :public CGItem
	{
	public:
		CGItemRectangle(int Device, RectangleType type = RectangleType::Normal);
		~CGItemRectangle();

		RectangleType rectType();

		CGItemType(CGItemRectangle);
	public:

		void setPos(glm::vec3 pos);
		void setPos(glm::vec2 pos);
		glm::vec3 getPos();
		void setDirection(glm::vec3 direction);

		void Width(float width);
		const float Width()const;
		void Height(float height);
		const float Height()const;
	public:
		//Text
		void Text(const std::wstring& text);
		void Label(long labelCode);
		long Label();
	public:
		void build(int Device)override;

		void Render(int device, const glm::mat4& matrix)override;

	private:
		struct PrivateData;
		PrivateData* m_priv = nullptr;
	private:
		CGItemRectangle() = delete;
	};
}
#endif // !_CGITEMSHAPE_H_
