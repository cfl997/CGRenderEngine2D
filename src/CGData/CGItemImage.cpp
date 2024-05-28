#include "CGItemImage.h"

#include "CGRender.h"
#include "CGEffector.h"

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
	int fsShader = -1;
	int uniformBufferid = -1;

	CGEffector* effector = nullptr;

	ImageRollingDrt rollingDirection = ImageRollingDrt::ImageRollingDrt_unknow;
};

CGData::CGItemImage::CGItemImage(int Device, const std::wstring& path) :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	Super::ContextID(Device);

	setPrimitiveType(GLPrimitiveTypes::TRIANGLELIST);
	d.path = path;


	loadPathData();

	createResource();

}

CGData::CGItemImage::CGItemImage(int Device, void* data, int width, int height, GLTextureType texturetype) :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	Super::ContextID(Device);

	setPrimitiveType(GLPrimitiveTypes::TRIANGLELIST);

	//创建屏幕大小的一个纹理
	d.hTexture = CGRender_CreateTextureFromData(ContextID(), data, width, height, texturetype);
	d.width = width;
	d.height = height;
	d.type = texturetype;

	createResource();

	d.vsShader = CGRender_CreateShader(ContextID(), ShaderCodeName::VS_POS_COLOR_TEX_viewMatrix);
	d.fsShader = CGRender_CreateShader(ContextID(), ShaderCodeName::FS_Tex_Raw);
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

const int CGData::CGItemImage::worldWidth() const
{
	auto& d = *m_priv;
	return d.width / g_globalProportion;
}

const int CGData::CGItemImage::worldHeight() const
{
	auto& d = *m_priv;
	return d.height / g_globalProportion;
}

const std::wstring& CGData::CGItemImage::path() const
{
	auto& d = *m_priv;
	return d.path;
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


	bool moveResult = CGRender_MoveTexturePixel(ContextID(), TextureID(), 0, height);
	assert(moveResult);

	bool uploadresult = CGRender_UploadTexture(ContextID(), TextureID(), 0, 0, width, height, data);
	assert(uploadresult);
}

void CGData::CGItemImage::Resize(int width, int height)
{
	auto& d = *m_priv;
	CGRender_DeleteTexture(ContextID(), TextureID());
	d.hTexture = CGRender_CreateTextureFromData(ContextID(), 0, width, height, GLTexture_Normal2DTex);
	return;
}


void CGData::CGItemImage::Effector(CGEffector* effector)
{
	auto& d = *m_priv;
	d.effector = effector;
}

void CGData::CGItemImage::build(int Device)
{

}

void CGData::CGItemImage::Render(int device, const glm::mat4& matrix)
{
	build(device);
	auto& d = *m_priv;

	int renderTexture = d.hTexture;

	if (!(renderTexture > 0 && d.indexBufferId > 0 && d.vertexBufferId > 0 && d.vsShader > 0))
		return;

	if (d.effector != nullptr)
	{
		d.effector->Render(device, d.hTexture, &renderTexture);
	}

	glm::mat4 mvp = matrix;
	{
		switch (d.rollingDirection)
		{
		case ImageRollingDrt::ImageRollingDrt_left2right:
		{
			mvp = glm::rotate(mvp, glm::radians(90.0f), g_ZNormal);
			break;
		}
		case ImageRollingDrt::ImageRollingDrt_right2left:
		{
			mvp = glm::rotate(mvp, glm::radians(-90.0f), g_ZNormal);
			break;
		}
		default:
			break;
		}
	}
	if (d.uniformBufferid < 0)
		d.uniformBufferid = CGRender_CreateBuffer(device, sizeof(glm::mat4), 1, &mvp, GLBufferType::uniformBuffer);
	else
		CGRender_ModifyBuffer(device, d.uniformBufferid, sizeof(glm::mat4) * 1, &mvp);



	CGRender_SetShaderTexture(device, renderTexture, 0, ShaderType::FRAGMENT);

	CGRender_SetShader(device, d.vsShader, ShaderType::VERTEX);
	CGRender_SetShader(ContextID(), d.fsShader, ShaderType::FRAGMENT);

	CGRender_SetUniformBuffer(device, d.uniformBufferid, 0, ShaderType::VERTEX);

	CGRender_Render(device, d.vertexBufferId, d.indexBufferId, GetPrimitiveType(), 0, 0, 0);

	if (renderTexture != d.hTexture)
		CGRender_DeleteTexture(device, renderTexture);
}

void CGData::CGItemImage::setRollingDirection(ImageRollingDrt direction)
{
	auto& d = *m_priv;
	d.rollingDirection = direction;
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

void CGData::CGItemImage::createResource()
{
	auto& d = *m_priv;
	auto& data = *getItemData();
	float proportion = d.width / d.height;
	float Texwidth = d.width / g_globalProportion;
	float Texheight = d.height / g_globalProportion;

	data.vertexes.clear();
	data.vertexes.push_back({ {-Texwidth / 2,	Texheight / 2,	0},Color(),{0,	1} });
	data.vertexes.push_back({ {-Texwidth / 2,	-Texheight / 2,	0},Color(),{0,	0} });
	data.vertexes.push_back({ {Texwidth / 2,	-Texheight / 2,	0},Color(),{1,	0} });
	data.vertexes.push_back({ {Texwidth / 2,	Texheight / 2,	0},Color(),{1,	1} });

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
}

void CGData::CGItemImage::loadPathData()
{
	auto& d = *m_priv;
	d.hTexture = CGRender_CreateTextureFromFile(ContextID(), d.path.c_str(), GLTexture_Normal2DTex);
	assert(d.hTexture > 0);
	CGRender_GetTextureInfo(ContextID(), d.hTexture, &d.width, &d.height, &d.type);


	d.vsShader = CGRender_CreateShader(ContextID(), ShaderCodeName::VS_POS_COLOR_TEX_viewMatrix);
	d.fsShader = CGRender_CreateShader(ContextID(), ShaderCodeName::FS_Tex);
}
