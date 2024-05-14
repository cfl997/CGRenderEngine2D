#include "CGEffector.h"

using namespace CGData;

#include "CGRender.h"
#include <map>

struct CGEffector::PrivateData
{
	std::map<ShaderCodeName, int>m_fs;

	int Device = -1;
	int vertexBufferId = -1;
	int indexBufferId = -1;
	int vsShader = -1;
	GLPrimitiveTypes primitiveType = GLPrimitiveTypes::TRIANGLELIST;

	ItemData data;
};

CGEffector::CGEffector(int Device) :m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.Device = Device;


	d.data.vertexes.clear();
	//远平面z值问题
	d.data.vertexes.push_back({ {-1,1,1},{0xff00ffff},{0,	1} });
	d.data.vertexes.push_back({ {-1,-1,1},{0xff00ffff},{0,	0} });
	d.data.vertexes.push_back({ {1,-1,1},{0xff00ffff},{1,	0} });
	d.data.vertexes.push_back({ {1,1, 1},{0xff00ffff},{1,	1} });

	d.data.indexes.clear();
	d.data.indexes.push_back(0);
	d.data.indexes.push_back(1);
	d.data.indexes.push_back(3);
	d.data.indexes.push_back(1);
	d.data.indexes.push_back(2);
	d.data.indexes.push_back(3);

	if (d.vertexBufferId == -1)
		d.vertexBufferId = CGRender_CreateBuffer(Device, sizeof(CGData::Vertex), d.data.vertexes.size(), &d.data.vertexes[0], GLBufferType::VertexBuffer, d.primitiveType);
	if (d.indexBufferId == -1)
		d.indexBufferId = CGRender_CreateBuffer(Device, sizeof(uint32_t), d.data.indexes.size(), d.data.indexes.data(), GLBufferType::IndexBuffer, d.primitiveType);

	d.vsShader = CGRender_CreateShader(Device, ShaderCodeName::VS_POS_COLOR_TEX);
}

CGEffector::~CGEffector()
{
	auto& d = *m_priv;
	if (d.vertexBufferId > 0)
		CGRender_DeleteBuffer(d.Device, d.vertexBufferId);
	d.vertexBufferId = -1;
	if (d.indexBufferId > 0)
		CGRender_DeleteBuffer(d.Device, d.indexBufferId);
	d.indexBufferId = -1;
	if (m_priv)
	{
		delete m_priv;
		m_priv = nullptr;
	}
}

void CGData::CGEffector::addEffector(ShaderCodeName name)
{
	auto& d = *m_priv;
	if (!(name<ShaderCodeName::FS_Count && name>ShaderCodeName::VS_Count))
		return;
	int shader = CGRender_CreateShader(d.Device, name);
	d.m_fs[name] = shader;
}

void CGData::CGEffector::removeEffector(ShaderCodeName name)
{
	auto& d = *m_priv;
	if (!(name<ShaderCodeName::FS_Count && name>ShaderCodeName::VS_Count))
		return;
	d.m_fs.erase(name);
}

void CGData::CGEffector::removeAllEffctor()
{
	auto& d = *m_priv;
	d.m_fs.clear();
}

void CGData::CGEffector::Render(int device, int hTexture, int* desTexture)
{
	auto& d = *m_priv;

	if (d.m_fs.empty())
	{
		*desTexture = hTexture;
		return;
	}


	int oldTarget = CGRender_GetRenderTarget(device);
	CGRECT oldviewport;
	CGRECT oldscissor;
	CGRender_GetViewport(device, &oldviewport);
	CGRender_GetScissor(device, &oldscissor);


	{

		int nwidth, nheight, type;
		CGRender_GetTextureInfo(device, hTexture, &nwidth, &nheight, &type);
		int newTarget = CGRender_CreateTextureFromData(device, 0, nwidth, nheight, GLTexture_Normal2DTex);


		CGRender_SetViewport(device, 0, 0, nwidth, nheight);
		CGRender_SetScissor(device, 0, 0, nwidth, nheight);


		CGRender_SetShader(device, d.vsShader, ShaderType::VERTEX);

		int tempTarget = -1;
		if (d.m_fs.size() > 1)
			tempTarget = CGRender_CreateTextureFromData(device, 0, nwidth, nheight, GLTexture_Normal2DTex);

		for (auto iter = d.m_fs.begin(); iter != d.m_fs.end(); iter++)
		{
			int fs = iter->second;
			CGRender_SetShader(device, fs, ShaderType::FRAGMENT);

			if (iter == d.m_fs.begin())
			{
				CGRender_SetRenderTarget(device, newTarget);
				CGRender_SetShaderTexture(device, hTexture, 0, ShaderType::FRAGMENT);
				CGRender_Render(device, d.vertexBufferId, d.indexBufferId, d.primitiveType, 0, 0, 0);
				if (0)
				{
					CGRender_SaveTextue(device, newTarget, L"d:/effectnewTarget.png");
				}
				continue;
			}


			CGRender_SetRenderTarget(device, tempTarget);

			CGRender_SetShaderTexture(device, newTarget, 0, ShaderType::FRAGMENT);
			CGRender_Render(device, d.vertexBufferId, d.indexBufferId, d.primitiveType, 0, 0, 0);
			if (0)
			{
				CGRender_SaveTextue(device, tempTarget, L"d:/effecttempTarget.png");
			}

			int tmep = newTarget;
			newTarget = tempTarget;
			tempTarget = tmep;

		}
		if (tempTarget != -1)
		{
			CGRender_DeleteTexture(device, tempTarget);
		}

		*desTexture = newTarget;
		if (0)
		{
			CGRender_SaveTextue(device, newTarget, L"d:/effectnewTarget.png");
		}
	}


	CGRender_SetRenderTarget(device, oldTarget);
	CGRender_SetViewport(device, oldviewport);
	CGRender_SetScissor(device, oldscissor);
}



