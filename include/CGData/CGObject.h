#ifndef _CGOBJECT_H_
#define _CGOBJECT_H_

#include "CGData_Exports.h"
#include <vector>
#include <string>

namespace CGData
{
	enum class CGItemType
	{
		CGObject,
		CGItem,
		CGLayer,
		CGItemRectangle,
		CGITtemTex16,
		CGItemText,
		CGDataType_unknow,
	};

#define CGItemType(x)\
virtual CGItemType CGType(){return CGItemType::x;}
}


namespace CGData
{
	class CGDATA_API CGObject
	{
	public:
		CGObject();
		~CGObject();

		CGItemType(CGObject);

		const std::wstring& GUID()const noexcept;
		const std::wstring& Name()const noexcept;
		void Name(const std::wstring& name);

	private:
		struct PrivateData;
		PrivateData* m_priv;
	};
}
#endif // !_CGOBJECT_H_
