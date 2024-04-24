#ifndef _CGITEMTEX16_H_
#define _CGITEMTEX16_H_


#include "CGItem.h"

namespace CGData
{

	class CGDATA_API CGITtemTex16 : public CGItem
	{
		typedef CGItem super;
	public:
		CGITtemTex16(int DeviceId,const std::wstring&path);
		~CGITtemTex16();

		CGItemType(CGITtemTex16);
	public:
		void build(int Device)override;

		void Render(int device, const glm::mat4& matrix)override;
	private:
		struct PrivateData;
		PrivateData* m_priv = nullptr;
	};

}
#endif // !_CGITEMTEX16_H_
