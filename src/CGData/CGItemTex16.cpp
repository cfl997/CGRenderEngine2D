#include "CGItemTex16.h"

#include "CGRender.h"

using namespace CGData;

struct ImageData
{
	unsigned short width;
	unsigned short height;
	char* data;
	bool isLoad;
	int bytes;
	ImageData() :isLoad(false),
		width(0),
		height(0),
		data(0),
		bytes(0)
	{

	}
	~ImageData()
	{
		if (data)
		{
			delete data;
			data = nullptr;
		}
	}

	ImageData(
		unsigned short w,
		unsigned short h,
		char* da,
		bool is,
		int by)
	{
		width = w;
		height = h;
		{
			data = new char[by * w * h];
			std::copy(da, da + by * w * h, data);
		}
		isLoad = is;
		bytes = bytes;
	}

	ImageData& operator=(const ImageData& other)
	{
		width = other.width;
		height = other.height;
		{
			data = new char[other.bytes * other.width * other.height];
			std::copy(other.data, other.data + other.bytes * other.width * other.height, data);
		}
		isLoad = other.isLoad;
		bytes = other.bytes;
	}
};

static bool getImageData(const std::wstring& path, ImageData& imageData)
{
	FILE* fp = 0;

	_wfopen_s(&fp, path.c_str(), L"rb");
	if (!fp)
	{
		return false;
	}
	struct {
		unsigned short nO, width, height, nl, bits, energy, order, inverse, n[248];
	}header;
	size_t size = fread(&header, sizeof(header), 1, fp);
	if (header.width > 0 && header.height > 0)
	{
		header.bits = 16;
		header.energy = 0;
		header.order = 0;
		long max = (1 << header.bits) - 1;
		long bytes = (header.bits + 7) / 8;
		long bufsize = header.width * header.height * bytes;
		char* szbuf = new char[bufsize];
		//char* szbuf = new char[bufsize];
		if (1 == fread(szbuf, bufsize, 1, fp))
		{
			fclose(fp);
			imageData.width = header.width;
			imageData.height = header.height;
			imageData.bytes = bytes;
			imageData.isLoad = true;
			imageData.data = new char[bytes * header.width * header.height];
			std::copy(szbuf, szbuf + bytes * header.width * header.height, imageData.data);
			return true;
		}
	}
	fclose(fp);

	return false;
}

struct CGData::CGITtemTex16::PrivateData
{
	std::wstring path = L"";
	ImageData imageData;

	int hTex = -1;
};


CGData::CGITtemTex16::CGITtemTex16(const std::wstring& path) :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.path = path;
	getImageData(d.path, d.imageData);

}

CGITtemTex16::~CGITtemTex16()
{

}

void CGData::CGITtemTex16::build(int Device)
{
	auto& d = *m_priv;
	d.hTex = CGRender_CreateTextureFromData(Device, d.imageData.data, d.imageData.width, d.imageData.height, GLTextureType::GLTexture_Raw16);

}

void CGData::CGITtemTex16::Render(int device, const glm::mat4& matrix)
{

}
