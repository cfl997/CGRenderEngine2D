#include "CGLayer.h"

#include <map>

#include "CGRender.h"

#include <mutex>

using namespace CGData;

struct CGLayer::PrivateData
{
	std::map<std::wstring, CGItem*>m_items;
	glm::vec3 position;
	//move
	bool isMove = false;
	glm::vec2 pressPos;

	//imageShader
	int imagefsShader = -1;

	std::recursive_mutex reMutex;



	void sortRenderItem();
	std::vector<CGItem*>m_vSortRenderItem;
};


CGLayer::CGLayer(int Device) :m_priv(new PrivateData)
{
	ContextID(Device);
	auto& d = *m_priv;
	d.position = glm::vec3{ 0 };
	d.isMove = false;
	setImageShader(ShaderCodeName::FS_Tex);
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
	std::lock_guard a(d.reMutex);
	d.m_items[item->GUID()] = item;
}

CGItem* CGData::CGLayer::getItem(const std::wstring& guid)
{
	auto& d = *m_priv;
	std::lock_guard a(d.reMutex);
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

	std::lock_guard a(d.reMutex);
	auto item1 = d.m_items[guid];
	d.m_items.erase(guid);

	delete item1;
	item1 = nullptr;


	return true;
}

bool CGData::CGLayer::removeItem(CGItemType itemType)
{
	auto& d = *m_priv;
	std::lock_guard a(d.reMutex);

	for (auto iter = d.m_items.begin(); iter != d.m_items.end(); )
	{
		if (iter->second->CGType() == itemType)
		{
			delete iter->second;
			iter->second = nullptr;
			iter = d.m_items.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	return true;
}

std::vector<CGItem*> CGData::CGLayer::getItem(CGItemType itemType)
{
	auto& d = *m_priv;
	std::vector<CGItem*>result;
	std::lock_guard a(d.reMutex);
	for (auto& data : d.m_items)
	{
		if (data.second->CGType() == itemType)
			result.emplace_back(data.second);
	}
	return result;
}

void CGData::CGLayer::setImageShader(ShaderCodeName name)
{
	auto& d = *m_priv;
	d.imagefsShader = CGRender_CreateShader(ContextID(), name);
}

void CGData::CGLayer::Render(int device, const glm::mat4& matrix)
{
	auto& d = *m_priv;
	std::lock_guard a(d.reMutex);
	if (d.position != glm::vec3{ 0 })
	{
		//__debugbreak();
	}

	glm::mat4 transform = glm::translate(g_Mat4Normal, d.position);
	glm::mat4 renderMat4 = matrix * transform;

	CGRender_SetShaderTexture(device, 0, 0);
	d.sortRenderItem();

	for (auto& data : d.m_vSortRenderItem)
	{
		if (data->CGType() == CGItemType::CGITtemTex16 ||
			data->CGType() == CGItemType::CGItemImage)
		{
			CGRender_SetShader(ContextID(), d.imagefsShader, ShaderType::FRAGMENT);
		}
		data->Render(device, renderMat4);
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

void CGLayer::PrivateData::sortRenderItem()
{
	m_vSortRenderItem.clear();
	for (auto& data : m_items)
	{
		if (data.second->CGType() == CGItemType::CGItemImage ||
			data.second->CGType() == CGItemType::CGITtemTex16)
		{
			m_vSortRenderItem.insert(m_vSortRenderItem.begin(), data.second);
			continue;
		}
		m_vSortRenderItem.insert(m_vSortRenderItem.end(), data.second);

	}
}
