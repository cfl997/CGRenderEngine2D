#ifndef _CGITEMTEXT_H_
#define _CGITEMTEXT_H_


#include "CGItem.h"

namespace CGData
{
	class CGDATA_API CGItemText :public CGItem
	{
	public:
		CGItemText(int deviceID);
		~CGItemText();
		CGItemType(CGItemText);

		void Pixel(uint32_t pixel);
		uint32_t Pixel();

		void setText(const std::wstring& text);
		void lableCode(long labelCode);
		long lableCode();
		void OriginPos(glm::vec2 pos);

	public:
		virtual void build(int Device)override;
		virtual void Render(int device, const glm::mat4& matrix)override;

	private:

		struct PrivateData;
		PrivateData* m_priv = nullptr;
	};


}
#endif // !_CGITEMTEXT_H_
