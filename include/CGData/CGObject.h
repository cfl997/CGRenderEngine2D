#ifndef _CGOBJECT_H_
#define _CGOBJECT_H_

#include "CGData_Exports.h"
#include <vector>
#include <string>

namespace CGData
{

	class CGDATA_API CGObject
	{
	public:
		CGObject();
		~CGObject();

		const std::wstring& GUID()const noexcept;
		const std::wstring& Name()const noexcept;
	private:
		struct PrivateData;
		PrivateData* m_priv;
	};


}
#endif // !_CGOBJECT_H_
