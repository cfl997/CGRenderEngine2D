#ifndef _CGITEMSHAPE_H_
#define _CGITEMSHAPE_H_

#include "CGItem.h"
#include "../CGRender/CGglm.h"


namespace CGData
{
	class CGDATA_API CGItemRectangle :public CGItem
	{
	public:
		CGItemRectangle();
		~CGItemRectangle();

		void setPos(glm::vec3 pos);
		void setDirection(glm::vec3 direction);
		void setWidth(float width);
		void setHeight(float height);
	public:
		void build(int Device)override;

		void Render(int device, const glm::mat4 &matrix)override;

	private:
		struct PrivateData;
		PrivateData* m_priv = nullptr;
	};

}
#endif // !_CGITEMSHAPE_H_
