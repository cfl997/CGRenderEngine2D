#include "CGItemRectangle.h"
#include "CGRender.h"

#include "CGItemText.h"

using namespace CGData;

struct CGItemRectangle::PrivateData
{
	RectangleType type;

	glm::vec3 ltpos;
	glm::vec3 xDirection;
	float width;
	float height;

	int indexBufferId = -1;
	int vertexBufferId = -1;
	int vsShader;
	int fsShader;

	//test
	int uniformBufferid = -1;

	CGItemText* itemText = nullptr;
};


CGItemRectangle::CGItemRectangle(int Device, RectangleType type) :m_priv(new PrivateData)
{
	ContextID(Device);
	auto& d = *m_priv;
	d.type = type;

	auto& itemData = *getItemData();
	itemData.PrimitiveType = GLPrimitiveTypes::LINE_LIST_STRIP;

	Color(0xFF0000ff);



	//cfl-test

	d.ltpos = glm::vec3{ 0,0,g_itemZDistance };

	d.width = 40.;
	d.height = 30.;



	itemData.indexes.clear();
	itemData.indexes.push_back(0);
	itemData.indexes.push_back(1);
	itemData.indexes.push_back(1);
	itemData.indexes.push_back(2);
	itemData.indexes.push_back(2);
	itemData.indexes.push_back(3);
	itemData.indexes.push_back(3);
	itemData.indexes.push_back(0);
	d.indexBufferId = CGRender_CreateBuffer(Device, sizeof(uint32_t), itemData.indexes.size(), itemData.indexes.data(), GLBufferType::IndexBuffer, GetPrimitiveType());

	glm::mat4 a = glm::mat4{ 1 };
	d.uniformBufferid = CGRender_CreateBuffer(Device, sizeof(glm::mat4), 1, &a, GLBufferType::uniformBuffer);


	d.vsShader = CGRender_CreateShader(Device, ShaderCodeName::VS_POS_COLOR_TEX_viewMatrix);
	d.fsShader = CGRender_CreateShader(Device, ShaderCodeName::FS_COLOR);
}

CGItemRectangle::~CGItemRectangle()
{
	auto& d = *m_priv;
	if (d.vertexBufferId > 0)
		CGRender_DeleteBuffer(ContextID(), d.vertexBufferId);
	if (d.indexBufferId > 0)
		CGRender_DeleteBuffer(ContextID(), d.indexBufferId);
	if (d.uniformBufferid > 0)
		CGRender_DeleteBuffer(ContextID(), d.uniformBufferid);
	if (d.itemText)
	{
		delete d.itemText;
		d.itemText = nullptr;
	}

	if (m_priv)
	{
		delete m_priv;
		m_priv = nullptr;
	}
}

RectangleType CGData::CGItemRectangle::rectType()
{
	auto& d = *m_priv;
	return d.type;
}

void CGData::CGItemRectangle::setPos(glm::vec3 pos)
{
	m_priv->ltpos = pos;
	auto& d = *m_priv;
	if (d.itemText)
	{
		d.itemText->OriginPos(pos);
	}
}

void CGData::CGItemRectangle::setPos(glm::vec2 pos)
{
	setPos({ pos.x,pos.y,g_itemZDistance });
}

glm::vec3 CGData::CGItemRectangle::getPos()
{
	auto& d = *m_priv;
	return d.ltpos;
}
void CGData::CGItemRectangle::setDirection(glm::vec3 direction)
{
	m_priv->xDirection = glm::normalize(direction);
}

void CGData::CGItemRectangle::Width(float width)
{
	m_priv->width = width;
}

const float CGData::CGItemRectangle::Width() const
{
	return m_priv->width;
}

void CGData::CGItemRectangle::Height(float height)
{
	m_priv->height = height;
}

const float CGData::CGItemRectangle::Height() const
{
	return m_priv->height;
}

void CGData::CGItemRectangle::Text(const std::wstring& text)
{
	auto& d = *m_priv;

	if (d.itemText == nullptr)
		d.itemText = new CGItemText(ContextID());

	d.itemText->Color(Color());
	d.itemText->setText(text);
	d.itemText->Pixel(30);
	d.itemText->OriginPos(d.ltpos);
}
void CGData::CGItemRectangle::Label(long labelCode)
{
	auto& d = *m_priv;
	if (d.itemText == nullptr)
		d.itemText = new CGItemText(ContextID());

	d.itemText->lableCode(labelCode);
	d.itemText->OriginPos(d.ltpos);
}

long CGData::CGItemRectangle::Label()
{
	auto& d = *m_priv;
	if (d.itemText == nullptr)
		return -1;

	return d.itemText->lableCode();
}

void CGData::CGItemRectangle::build(int Device)
{
	auto& d = *m_priv;
	glm::vec3 xDistance = g_XNormal * d.width;
	glm::vec3 yDistance = g_YNormal * d.height;


	glm::vec3 lbPos = d.ltpos - yDistance;
	glm::vec3 rbPos = lbPos + xDistance;

	glm::vec3 rtPos = d.ltpos + xDistance;

	auto& data = *getItemData();

	data.vertexes.clear();
	data.vertexes.push_back({ d.ltpos,Color(),{0,0} });
	data.vertexes.push_back({ rtPos,Color(),{0,0} });
	data.vertexes.push_back({ rbPos,Color(),{0,0} });
	data.vertexes.push_back({ lbPos,Color(),{0,0} });

	auto sizea = sizeof(glm::vec3);



	//d.vertexBufferId = CGRender_CreateBuffer(getGLContextID(), sizeof(CGData::Vertex), data.vertexes.size(), data.vertexes.data(), GLBufferType::VertexBuffer, GetPrimitiveType());
	if (d.vertexBufferId < 1)
		d.vertexBufferId = CGRender_CreateBuffer(Device, sizeof(CGData::Vertex), data.vertexes.size(), &data.vertexes[0], GLBufferType::VertexBuffer, GetPrimitiveType());
	else
		CGRender_ModifyBuffer(Device, d.vertexBufferId, data.vertexes.size() * sizeof(CGData::Vertex), &data.vertexes[0]);
}

void CGData::CGItemRectangle::Render(int device, const glm::mat4& matrix)
{
	build(device);
	auto& d = *m_priv;

	glm::mat4 a = matrix;


	CGRender_ModifyBuffer(device, d.uniformBufferid, sizeof(glm::mat4) * 1, &a);

	CGRender_SetShader(device, d.vsShader, ShaderType::VERTEX);
	CGRender_SetShader(device, d.fsShader, ShaderType::FRAGMENT);
	CGRender_SetShaderTexture(device, 0, 0, ShaderType::FRAGMENT);

	CGRender_SetUniformBuffer(device, d.uniformBufferid, 0, ShaderType::VERTEX);

	CGRender_Render(device, d.vertexBufferId, d.indexBufferId, 0, 0, 0, 0);
	/*
	* ceshi
	*/

	if (d.itemText == nullptr)
		return;
	return;//todo ¹Ø±ÕÎÄ×Ö
	d.itemText->Render(device, matrix);
}
