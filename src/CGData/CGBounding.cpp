#include "CGBounding.h"
#include "CGItem.h"

using namespace CGData;

CGData::CGBoundingBox::CGBoundingBox(CGItem* item)
{
	m_box2D.min = glm::vec2{ FLT_MAX };
	m_box2D.max = glm::vec2{ -FLT_MIN };

	if (item == nullptr)
		return;

	auto data = item->getItemData();
	for (auto& data : data->vertexes)
	{
		if (data.vertex.x < m_box2D.min.x)
		{
			m_box2D.min.x = data.vertex.x;
		}
		if (data.vertex.y < m_box2D.min.y)
		{
			m_box2D.min.y = data.vertex.y;
		}
		if (data.vertex.x > m_box2D.max.x)
		{
			m_box2D.max.x = data.vertex.x;
		}
		if (data.vertex.y > m_box2D.max.y)
		{
			m_box2D.max.y = data.vertex.y;
		}
	}

}

CGData::CGBoundingBox::~CGBoundingBox()
{

}

bool CGData::CGBoundingBox::Bounding2D(BoundingBox2D* boundingBox)
{
	boundingBox->min = m_box2D.min;
	boundingBox->max = m_box2D.max;
	return true;
}

CGData::CGBoundingShape::CGBoundingShape(CGItem* item)
{
	m_shape2D.radius = -1.;
	m_shape2D.center = glm::vec2{ 0. };
	if (item == nullptr)
		return;

	CGBoundingBox bbox(item);
	BoundingBox2D bbox2D;
	bbox.Bounding2D(&bbox2D);

	m_shape2D.center = (bbox2D.min + bbox2D.max) / 2.f;
	auto Data = item->getItemData();
	for (auto& data : Data->vertexes)
	{
		m_shape2D.radius = glm::distance(glm::vec2{ data.vertex }, m_shape2D.center);
	}
}

CGData::CGBoundingShape::~CGBoundingShape()
{
}

bool CGData::CGBoundingShape::Bounding2D(BoundingShape2D* shape2D)
{
	shape2D->center = m_shape2D.center;
	shape2D->radius = m_shape2D.radius;
	return true;;
}
