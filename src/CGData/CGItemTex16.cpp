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
			delete szbuf;
			szbuf = nullptr;
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
	int vertexBufferId = -1;
	int indexBufferId = -1;
	int vsShader = -1;

	//test
	int uniformBufferid = -1;
};

// 10.f,  10.f, 0.0f,    1.0f, 1.0f,   // 右上
// 10.f, -10.f, 0.0f,    1.0f, 0.0f,   // 右下
//-10.f, -10.f, 0.0f,    0.0f, 0.0f,   // 左下
//-10.f,  10.f, 0.0f,    0.0f, 1.0f    // 左上
CGData::CGITtemTex16::CGITtemTex16(int DeviceId, const std::wstring& path) :m_priv(new PrivateData)
{
	ContextID(DeviceId);

	auto& d = *m_priv;
	setPrimitiveType(GLPrimitiveTypes::TRIANGLELIST);

	d.path = path;
	getImageData(d.path, d.imageData);
	auto& data = *getItemData();

	//float Texwidth = d.imageData.width;
	//float Texheight = d.imageData.height;

	float proportion = d.imageData.width / d.imageData.height;
	//float Texwidth = 1920;
	float Texwidth = d.imageData.width / 10;
	float Texheight = Texwidth / proportion;

	data.vertexes.clear();
	data.vertexes.push_back({ {-Texwidth,	Texheight,	0},Color(),{0,	1} });
	data.vertexes.push_back({ {-Texwidth,	-Texheight,	0},Color(),{0,	0} });
	data.vertexes.push_back({ {Texwidth,	-Texheight,	0},Color(),{1,	0} });
	data.vertexes.push_back({ {Texwidth,	Texheight,	0},Color(),{1,	1} });

	data.indexes.clear();
	data.indexes.push_back(0);
	data.indexes.push_back(1);
	data.indexes.push_back(3);
	data.indexes.push_back(1);
	data.indexes.push_back(2);
	data.indexes.push_back(3);
}

CGITtemTex16::~CGITtemTex16()
{
	auto& d = *m_priv;
	CGRender_DeleteTexture(ContextID(), d.hTex);
	if (d.vertexBufferId > 0)
		CGRender_DeleteBuffer(ContextID(), d.vertexBufferId);
	d.vertexBufferId = -1;
	if (d.indexBufferId > 0)
		CGRender_DeleteBuffer(ContextID(), d.indexBufferId);
	d.indexBufferId = -1;
	if (d.uniformBufferid > 0)
		CGRender_DeleteBuffer(ContextID(), d.uniformBufferid);
	d.uniformBufferid = -1;

	if (m_priv)
	{
		delete m_priv;
		m_priv = nullptr;
	}
}

void CGData::CGITtemTex16::build(int Device)
{
	auto& d = *m_priv;
	auto data = *getItemData();

	if (d.hTex == -1)
		d.hTex = CGRender_CreateTextureFromData(Device, d.imageData.data, d.imageData.width, d.imageData.height, GLTextureType::GLTexture_Raw16);
	if (d.vertexBufferId == -1)
		d.vertexBufferId = CGRender_CreateBuffer(Device, sizeof(CGData::Vertex), data.vertexes.size(), &data.vertexes[0], GLBufferType::VertexBuffer, GetPrimitiveType());
	if (d.indexBufferId == -1)
		d.indexBufferId = CGRender_CreateBuffer(Device, sizeof(uint32_t), data.indexes.size(), data.indexes.data(), GLBufferType::IndexBuffer, GetPrimitiveType());

	d.vsShader = CGRender_CreateShader(Device, ShaderCodeName::VS_POS_COLOR_TEX_viewMatrix);
}

void CGData::CGITtemTex16::Render(int device, const glm::mat4& matrix)
{
	build(device);
	auto& d = *m_priv;
	glm::mat4 aa = matrix;
	if (d.uniformBufferid < 0)
		d.uniformBufferid = CGRender_CreateBuffer(device, sizeof(glm::mat4), 1, &aa, GLBufferType::uniformBuffer);
	else
		CGRender_ModifyBuffer(device, d.uniformBufferid, sizeof(glm::mat4) * 1, &aa);

	CGRender_SetShader(device, d.vsShader, ShaderType::VERTEX);

	CGRender_SetUniformBuffer(device, d.uniformBufferid, 0, ShaderType::VERTEX);

	CGRender_Render(device, d.vertexBufferId, d.indexBufferId, GetPrimitiveType(), 0, 0, 0);

	//CGRender_DeleteBuffer(device, uniformBufferid);

}
