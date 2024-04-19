#include "CGObject.h"
#include <objbase.h>
#include <string>
#include <optional>

#include <time.h>


using namespace CGData;

std::wstring getuuid()
{
	GUID	ID;

	time_t ltime;
	time(&ltime);

	if (CoCreateGuid(&ID) != S_OK)
	{
		srand((unsigned)time(NULL));
		ID.Data1 = (DWORD)ltime;
		ID.Data2 = (WORD)rand();
		ID.Data3 = (WORD)rand();
		ID.Data4[2] = '0' + (BYTE)(rand() & 0x1);
		ID.Data4[3] = '0' + (BYTE)(rand() & 0x1);
		ID.Data4[4] = '0' + (BYTE)(rand() & 0x1);
		ID.Data4[5] = '0' + (BYTE)(rand() & 0x1);
		ID.Data4[6] = '0' + (BYTE)(rand() & 0x1);
		ID.Data4[7] = 0;
	}
	else
	{
		ID.Data2 = (WORD)(ltime & 0xffff);
		ID.Data3 = (WORD)((ltime >> 16) & 0xffff);
	}
	ID.Data4[0] = 'D';
	ID.Data4[1] = 'K';


	wchar_t strBufer[64];
	swprintf(strBufer, 64, L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		ID.Data1, ID.Data2, ID.Data3, ID.Data4[0], ID.Data4[1], ID.Data4[2], ID.Data4[3],
		ID.Data4[4], ID.Data4[5], ID.Data4[6], ID.Data4[7]);

	return std::wstring{ strBufer };
}

std::wstring GuidToString(const GUID& guid)
{
	TCHAR guidString[64];
	HRESULT hr = StringFromGUID2(guid, guidString, sizeof(guidString) / sizeof(wchar_t));
	std::wstring wstr(guidString);
	return wstr;
}

std::wstring GeneralGuid()
{
	return getuuid();
}

struct CGObject::PrivateData
{
	std::wstring guid;
	std::wstring name;
};


CGObject::CGObject() :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.guid = GeneralGuid();
	d.name = L"CGObject";
}

CGObject::~CGObject()
{
	delete m_priv;
	m_priv = nullptr;
}

const std::wstring&  CGData::CGObject::GUID()const noexcept
{
	return m_priv->guid;
}

const std::wstring&  CGData::CGObject::Name()const noexcept
{
	return m_priv->name;
}





