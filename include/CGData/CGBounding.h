#ifndef _CGBOUNDING_H_
#define _CGBOUNDING_H_

#include "CGglm.h"
#include "CGData_Exports.h"

namespace CGData
{
	struct CGDATA_API BoundingBox2D
	{
		glm::vec2 min;
		glm::vec2 max;
	};

	class CGItem;
	class CGDATA_API CGBoundingBox
	{
	public:
		CGBoundingBox(CGItem* item);
		~CGBoundingBox();

		bool Bounding2D(BoundingBox2D* box2D);
	private:
		BoundingBox2D m_box2D;
	};


	struct CGDATA_API BoundingShape2D
	{
		float radius;
		glm::vec2 center;
	};

	class CGDATA_API CGBoundingShape
	{
	public:
		CGBoundingShape(CGItem* item);
		~CGBoundingShape();

		bool Bounding2D(BoundingShape2D* shape2D);

	private:
		BoundingShape2D m_shape2D;
	};



}
#endif // !_CGBOUNDING_H_
