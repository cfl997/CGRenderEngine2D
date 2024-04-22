#include "CGLayer.h"

#include <map>



using namespace CGData;

struct CGLayer::PrivateData
{
	std::map<std::wstring, CGItem*>m_items;
	glm::vec3 position;
	//move
	bool isMove = false;
	glm::vec2 pressPos;
};


CGLayer::CGLayer() :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.position = glm::vec3{ 0 };
	d.isMove = false;
}

CGLayer::~CGLayer()
{
	auto& d = *m_priv;
	for (auto it = d.m_items.begin(); it != d.m_items.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
			it->second = nullptr;
		}
	}
	if (m_priv)
	{
		delete m_priv;
		m_priv = nullptr;
	}
}

void CGData::CGLayer::addItem(CGItem* item)
{
	auto& d = *m_priv;
	d.m_items[item->GUID()] = item;
}

CGItem* CGData::CGLayer::getItem(const std::wstring& guid)
{
	auto& d = *m_priv;
	auto item = d.m_items.find(guid);
	if (item == d.m_items.end())
		return nullptr;
	return item->second;
}

bool CGData::CGLayer::removeItem(const std::wstring& guid)
{
	auto& d = *m_priv;
	auto item = d.m_items.find(guid);
	if (item == d.m_items.end())
		return false;

	auto item1 = d.m_items[guid];
	d.m_items.erase(guid);

	delete item1;
	item1 = nullptr;


	return true;
}

void CGData::CGLayer::Render(int device, const glm::mat4& matrix)
{
	auto& d = *m_priv;
	if (d.position != glm::vec3{ 0 })
	{
		//__debugbreak();
	}

	glm::mat4 transform = glm::translate(g_Mat4Normal, d.position);
	glm::mat4 renderMat4 = matrix * transform;
	for (auto& data : d.m_items)
	{
		data.second->Render(device, renderMat4);
	}
	//for (auto iter = d.m_items.cbegin(); iter != d.m_items.cend(); iter++)
	//{
	//	iter->second->Render(device, renderMat4);
	//}

}

void CGData::CGLayer::setPosition(const glm::vec3& pos)
{
	auto& d = *m_priv;
	d.position = pos;
}

void CGData::CGLayer::ProcessMouseMoveXY(float x, float y)
{
	auto& d = *m_priv;
	if (!d.isMove)
		return;
	glm::vec2 offset = glm::vec2{ x,y } - d.pressPos;
	//offset=glm::normalize(offset);
	//d.position += glm::vec3((glm::vec2{ x,y } - d.pressPos),0.0);
	d.position.x += offset.x;
	d.position.y += offset.y;

	//d.position = d.position - glm::vec3(offset, 0);
	d.pressPos = glm::vec2{ x,y };

}

void CGData::CGLayer::ProcessMousePress(float x, float y)
{
	auto& d = *m_priv;
	d.isMove = true;
	d.pressPos = glm::vec2{ x,y };
}

void CGData::CGLayer::ProcessMouseRelease(float x, float y)
{
	auto& d = *m_priv;
	d.position = glm::vec3{ 0 };
	d.isMove = false;
}

