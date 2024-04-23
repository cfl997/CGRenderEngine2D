#include "CGItem.h"


using namespace CGData;

struct CGItem::PrivateData
{
	int DeviceId;
	ItemData m_itemData;
	DWORD color;

	glm::mat4 modelMatrix;
};

CGItem::CGItem() :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.modelMatrix = glm::mat4{ 1 };
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

void CGData::CGItem::setColor(DWORD color)
{
	m_priv->color = color;
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

void CGData::CGItem::build(int Device)
{
}

void CGData::CGItem::Render(int device, const glm::mat4& matrix)
{

}

