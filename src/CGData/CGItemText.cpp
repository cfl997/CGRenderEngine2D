#include "CGItemText.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>

#include <iostream>
#include <map>
#include "CGRender.h"

#include <filesystem>


static const int MAX_SHORT_VALUE = 65536;

using namespace CGData;

struct CGItemText::PrivateData
{
	std::map<wchar_t, Character> Characters;
	int vertexBufferId = -1;
	int uniformBufferid = -1;

	uint32_t Pixel;


	std::wstring text = L"";

	glm::vec2 originPos = { 0.,0. };
};


//std::wstring text = L"ÖÐÎÄ13@!#@!$#$%$#^&%$#&{}:£¬2324234fdsafsa";

CGItemText::CGItemText(int deviceID) :m_priv(new PrivateData)
{
	setPrimitiveType(GLPrimitiveTypes::TRIANGLELIST);
	auto& d = *m_priv;
	ContextID(deviceID);

	CGRender_Font_Init(ContextID());
	CGRender_Font_LoadFont(ContextID());

	d.Pixel = g_FontPixelSize;
}

CGItemText::~CGItemText()
{
	if (m_priv)
	{
		delete m_priv;
		m_priv = nullptr;
	}
}

void CGData::CGItemText::Pixel(uint32_t pixel)
{
	auto& d = *m_priv;
	d.Pixel = pixel;
}

uint32_t CGData::CGItemText::Pixel()
{
	return m_priv->Pixel;
}

void CGData::CGItemText::setText(const std::wstring& text)
{
	auto& d = *m_priv;
	d.text = text;
	CGRender_Font_LoadFaces(ContextID(), text);
}

void CGData::CGItemText::OriginPos(glm::vec2 pos)
{
	auto& d = *m_priv;
	d.originPos = pos;
}

void CGData::CGItemText::build(int Device)
{
	auto& d = *m_priv;
	ItemData& data = *getItemData();
	Color(0x36ffffff);
	Color({ 0.5,0.1,0.3,1.0 });
	float scale = static_cast<float>(d.Pixel) / (float)g_FontPixelSize;
	scale /= g_globalProportion;
	CGRender_Font_GetCharacters(ContextID(), d.text, d.Characters);


	float x = d.originPos.x;
	float y = d.originPos.y;


	std::wstring::const_iterator c;
	for (c = d.text.begin(); c != d.text.end(); c++)
	{
		Character ch = d.Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;


		data.vertexes.clear();
		data.vertexes.push_back({ {xpos,     ypos + h,1},Color(),{0.0, 0.0} });
		data.vertexes.push_back({ {xpos,     ypos,    1},Color(),{0.0, 1.0} });
		data.vertexes.push_back({ {xpos + w, ypos,    1},Color(),{1.0, 1.0} });
		data.vertexes.push_back({ {xpos,     ypos + h,1},Color(),{0.0, 0.0} });
		data.vertexes.push_back({ {xpos + w, ypos,    1},Color(),{1.0, 1.0} });
		data.vertexes.push_back({ {xpos + w, ypos + h,1},Color(),{1.0, 0.0} });

		if (d.vertexBufferId == -1)
			d.vertexBufferId = CGRender_CreateBuffer(Device, sizeof(CGData::Vertex), data.vertexes.size(), &data.vertexes[0], GLBufferType::VertexBuffer, GetPrimitiveType());
		else
			CGRender_ModifyBuffer(Device, d.vertexBufferId, sizeof(CGData::Vertex) * data.vertexes.size(), &data.vertexes[0]);
		{
			int vsid = CGRender_CreateShader(Device, ShaderCodeName::VS_POS_COLOR_TEX_viewMatrix);
			CGRender_SetShader(Device, vsid, ShaderType::VERTEX);

			int shaderid = CGRender_CreateShader(Device, ShaderCodeName::FS_Text);
			CGRender_SetShader(Device, shaderid, ShaderType::FRAGMENT);
		}

		CGRender_SetShaderTexture(Device, ch.TextureID, 0);
		CGRender_Render(Device, d.vertexBufferId, 0, GetPrimitiveType(), 0, 0, 0);

		x += (ch.Advance >> 6) * scale;
	}

}


void CGData::CGItemText::Render(int device, const glm::mat4& matrix)
{
	auto& d = *m_priv;
	glm::mat4 aa = matrix;
	if (d.uniformBufferid < 0)
		d.uniformBufferid = CGRender_CreateBuffer(device, sizeof(glm::mat4), 1, &aa, GLBufferType::uniformBuffer);
	else
		CGRender_ModifyBuffer(device, d.uniformBufferid, sizeof(glm::mat4) * 1, &aa);

	CGRender_SetUniformBuffer(device, d.uniformBufferid, 0, ShaderType::VERTEX);
	build(device);



}
