#include "CGItemMeasureDistance.h"


using namespace CGData;
struct CGItemMeasureDistance::PrivateData
{

};


CGData::CGItemMeasureDistance::CGItemMeasureDistance() :m_priv(new PrivateData)
{
	auto& d = *m_priv;

	auto& itemData = *getItemData();
	itemData.PrimitiveType = GLPrimitiveTypes::LINE_LIST_STRIP;

	Color(0xFF0000ff);
}

CGData::CGItemMeasureDistance::~CGItemMeasureDistance()
{

}

void CGData::CGItemMeasureDistance::build(int Device)
{

}

void CGData::CGItemMeasureDistance::Render(int device, const glm::mat4& matrix)
{

}


