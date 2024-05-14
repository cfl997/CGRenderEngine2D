#ifndef _CGITEMSHAPE_H_
#define _CGITEMSHAPE_H_

#include "CGItem.h"
#include "../CGRender/CGglm.h"


namespace CGData
{
	enum RectangleType
	{
		Normal = 0,
		Window = 1,
	};
	class CGDATA_API CGItemRectangle :public CGItem
	{
	public:
		CGItemRectangle(int Device, RectangleType type = RectangleType::Normal);
		~CGItemRectangle();

		RectangleType type();

		CGItemType(CGItemRectangle);
	public:

		void setPos(glm::vec3 pos);
		void setDirection(glm::vec3 direction);

		void Width(float width);
		const float Width()const;
		void Height(float height);
		const float Height()const;
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
