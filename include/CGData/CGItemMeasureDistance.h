#ifndef _CGITEMMEASUREDISTANCE_H_
#define _CGITEMMEASUREDISTANCE_H_

#include "CGItem.h"
#include "../CGRender/CGglm.h"

namespace CGData
{
	class CGDATA_API CGItemMeasureDistance :public CGItem
	{
	public:
		CGItemMeasureDistance();
		~CGItemMeasureDistance();

		CGItemType(CGItemMeasureDistance);

	public:
		void build(int Device)override;

		void Render(int device, const glm::mat4& matrix)override;


	private:
		struct PrivateData;
		PrivateData* m_priv = nullptr;
	};

}

#endif // !_CGITEMMEASUREDISTANCE_H_
