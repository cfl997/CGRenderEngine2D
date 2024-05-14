#include "CGItem.h"

#include "CGRender.h"

using namespace CGData;

struct CGItem::PrivateData
{
	int DeviceId;
	ItemData m_itemData;
	DWORD color;

	glm::mat4 modelMatrix;

	bool isNeedRender = true;
};

CGItem::CGItem() :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.modelMatrix = glm::mat4{ 1 };
	d.m_itemData.PrimitiveType = GLPrimitiveTypes::TRIANGLELIST;

	NeedRender(true);
}

CGItem::~CGItem()
{
	delete m_priv;
	m_priv = nullptr;
}

void CGData::CGItem::ContextID(int Device)
{
	auto& d = *m_priv;
	d.DeviceId = Device;
}

const int CGData::CGItem::ContextID() const
{
	assert(m_priv->DeviceId > 0);
	return m_priv->DeviceId;
}

void CGData::CGItem::setPrimitiveType(GLPrimitiveTypes type)
{
	auto& d = *m_priv;
	d.m_itemData.PrimitiveType = type;
}

GLPrimitiveTypes CGData::CGItem::GetPrimitiveType()
{
	return m_priv->m_itemData.PrimitiveType;
}

void CGData::CGItem::Color(DWORD color)
{
	m_priv->color = color;
}

void CGData::CGItem::Color(CGRGBA rgba)
{
	CGRender_RGBA2DWORD(m_priv->color, rgba);
}

const DWORD CGData::CGItem::Color()const
{
	return m_priv->color;
}

ItemData* CGData::CGItem::getItemData()
{
	auto& d = *m_priv;
	return &d.m_itemData;
}

glm::mat4 CGData::CGItem::getModelMatrix()
{
	auto& d = *m_priv;
	return d.modelMatrix;
}

void CGData::CGItem::setModelMatrix(const glm::mat4& model)
{
	auto& d = *m_priv;
	d.modelMatrix = model;
}

bool CGData::CGItem::NeedRender()
{
	auto& d = *m_priv;
	return d.isNeedRender;
}

void CGData::CGItem::NeedRender(bool bneedRender)
{
	auto& d = *m_priv;
	d.isNeedRender = bneedRender;
}

void CGData::CGItem::build(int Device)
{
}

void CGData::CGItem::Render(int device, const glm::mat4& matrix)
{

}

