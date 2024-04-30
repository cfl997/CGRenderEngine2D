#include "CGItemImage.h"

#include "CGRender.h"

using namespace CGData;

struct CGItemImage::PrivateData
{
	std::wstring path;

	int hTexture = -1;
	int width = -1;
	int height = -1;
	int type = -1;

	int vertexBufferId = -1;
	int indexBufferId = -1;
	int vsShader = -1;
	int uniformBufferid = -1;
};

CGData::CGItemImage::CGItemImage(int Device, const std::wstring& path) :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	Super::ContextID(Device);

	setPrimitiveType(GLPrimitiveTypes::TRIANGLELIST);
	d.path = path;

	loadPathData();

}

CGData::CGItemImage::CGItemImage(int Device, int width, int height) :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	Super::ContextID(Device);

	setPrimitiveType(GLPrimitiveTypes::TRIANGLELIST);

	//创建屏幕大小的一个纹理
	d.hTexture = CGRender_CreateTextureFromData(ContextID(), 0, width, height, GLTexture_Normal2DTex);



}

CGItemImage::~CGItemImage()
{
	auto& d = *m_priv;

	clearResourse();
	if (m_priv)
	{
		delete m_priv;
		m_priv = nullptr;
	}
}

const int CGData::CGItemImage::width() const
{
	auto& d = *m_priv;
	return d.width;
}

const int CGData::CGItemImage::height() const
{
	auto& d = *m_priv;
	return d.height;
}

const int CGData::CGItemImage::TextureID() const
{
	auto& d = *m_priv;
	return d.hTexture;
}

void CGData::CGItemImage::updateData(const std::wstring& path)
{
	auto& d = *m_priv;
	if (d.path == path)
		return;
	d.path = path;

	clearResourse();
	loadPathData();

}



void CGData::CGItemImage::updateData(void* data, int width, int height)
{
	std::vector<char>allBuffer;
	allBuffer.resize(1920 * 1080 * 4);
	allBuffer.assign(1920 * 1080 * 4, 0);

	std::vector<char>buffer;

	int bufferSize = width * height * 4;
	buffer.resize(bufferSize);

	//allBuffer.insert(allBuffer.begin(), buffer.begin(), buffer.end());

	// 删除allBuffer末尾的size大小的内容
	allBuffer.erase(allBuffer.end() - bufferSize, allBuffer.end());

	std::copy(buffer.begin(), buffer.end(), allBuffer.begin());


}


void CGData::CGItemImage::build(int Device)
{

}

void CGData::CGItemImage::Render(int device, const glm::mat4& matrix)
{
	build(device);
	auto& d = *m_priv;


	glm::mat4 aa = matrix;
	if (d.uniformBufferid < 0)
		d.uniformBufferid = CGRender_CreateBuffer(device, sizeof(glm::mat4), 1, &aa, GLBufferType::uniformBuffer);
	else
		CGRender_ModifyBuffer(device, d.uniformBufferid, sizeof(glm::mat4) * 1, &aa);

	if (!(d.hTexture > 0 && d.uniformBufferid > 0 && d.indexBufferId > 0 && d.vertexBufferId > 0 && d.vsShader > 0))
		return;

	CGRender_SetShaderTexture(device, d.hTexture, 0, ShaderType::FRAGMENT);

	CGRender_SetShader(device, d.vsShader, ShaderType::VERTEX);

	CGRender_SetUniformBuffer(device, d.uniformBufferid, 0, ShaderType::VERTEX);

	CGRender_Render(device, d.vertexBufferId, d.indexBufferId, GetPrimitiveType(), 0, 0, 0);
}

void CGData::CGItemImage::clearResourse()
{
	auto& d = *m_priv;
	CGRender_DeleteTexture(ContextID(), d.hTexture);
	if (d.vertexBufferId > 0)
		CGRender_DeleteBuffer(ContextID(), d.vertexBufferId);
	d.vertexBufferId = -1;
	if (d.indexBufferId > 0)
		CGRender_DeleteBuffer(ContextID(), d.indexBufferId);
	d.indexBufferId = -1;
	if (d.uniformBufferid > 0)
		CGRender_DeleteBuffer(ContextID(), d.uniformBufferid);
	d.uniformBufferid = -1;
}

void CGData::CGItemImage::loadPathData()
{
	auto& d = *m_priv;
	d.hTexture = CGRender_CreateTextureFromFile(ContextID(), d.path.c_str(), GLTexture_Normal2DTex);
	CGRender_GetTextureInfo(ContextID(), d.hTexture, &d.width, &d.height, &d.type);

	auto& data = *getItemData();

	float proportion = d.width / d.height;
	float Texwidth = d.width / 10.f;
	//float Texheight = Texwidth / proportion;
	float Texheight = d.height / 10.f;

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

	if (d.vertexBufferId == -1)
		d.vertexBufferId = CGRender_CreateBuffer(ContextID(), sizeof(CGData::Vertex), data.vertexes.size(), &data.vertexes[0], GLBufferType::VertexBuffer, GetPrimitiveType());
	if (d.indexBufferId == -1)
		d.indexBufferId = CGRender_CreateBuffer(ContextID(), sizeof(uint32_t), data.indexes.size(), data.indexes.data(), GLBufferType::IndexBuffer, GetPrimitiveType());

	d.vsShader = CGRender_CreateShader(ContextID(), ShaderCodeName::VS_POS_COLOR_TEX_viewMatrix);
}
