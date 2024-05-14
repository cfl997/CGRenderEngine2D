#include "CGItemText.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#include <iostream>
#include <map>
#include "CGRender.h"

#include <filesystem>



using namespace CGData;

struct Character {
	uint32_t    TextureID;  // 字形纹理的ID
	glm::ivec2  Size;       // 字形大小
	glm::ivec2  Bearing;    // 从基准线到字形左部/顶部的偏移值
	uint32_t    Advance;    // 原点距下一个字形原点的距离
};


struct CGItemText::PrivateData
{
	std::map<char, Character> Characters;
	int vertexBufferId = -1;
	int uniformBufferid = -1;
};


CGItemText::CGItemText(int deviceID) :m_priv(new PrivateData)
{
	setPrimitiveType(GLPrimitiveTypes::TRIANGLELIST);
	auto& d = *m_priv;
	ContextID(deviceID);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;


	std::wstring pathstr = CGPath_GetPath(CGPathType::CG_PATH_FONTS);

	//std::wstring font = pathstr + L"/arial.ttf";
	//std::wstring font = pathstr + L"/ARIALUNI.TTF";
	std::wstring font = pathstr + L"/simhei.ttf";

	FT_Face face;
	if (FT_New_Face(ft, wstr2utf8(font).c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Select_Charmap(face, FT_ENCODING_UNICODE);

	FT_Set_Pixel_Sizes(face, 30, 30);

	//if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
	//	std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	{
		//const wchar_t* text = L"曹福磊cfl997";

		//int lew_w = wcslen(text);
		//setlocale(LC_ALL, "");
		//for (unsigned char i = 0; i < lew_w; i++)
		//{
		//	if (FT_Load_Char(face, text[i], FT_LOAD_RENDER))
		//	{
		//		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		//		continue;
		//	}
		//	std::string imagename = "e:/picture/textfont" +  std::string{ ".png" };
		//	CGRender_SaveImageByVoid(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 1, imagename, 0, false);
		//}

	}
	auto fn = [&](char c)
		{


			int texture = CGRender_CreateTextureFromData(ContextID(), face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, GLTextureType::GLTexture_RED);

			//std::string imagename = "e:/picture/textfont" + c + std::string{ ".png" };
			//CGRender_SaveImageByVoid(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 1, imagename, 0, false);

			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			d.Characters.insert(std::pair<char, Character>(c, character));
		};

	{
		std::string chinese = wstr2utf8(L"文32132");
		for (int i = 0; i < chinese.size(); i++)
		{
			if (FT_Load_Char(face, chinese.c_str()[i], FT_LOAD_RENDER))
				//if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			fn(chinese.c_str()[i]);
		}
		return;
	}
	{
		for (unsigned char c = 0; c < 128; c++)
		{
			// 加载字符的字形 
			//if (FT_Load_Char(face, c, FT_LOAD_RENDER))

			wchar_t* chinese_char = L"周zhou";
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			
			//// 生成纹理
			//GLuint texture;
			//glGenTextures(1, &texture);
			//glBindTexture(GL_TEXTURE_2D, texture);

			//glTexImage2D(
			//    GL_TEXTURE_2D,
			//    0,
			//    GL_RED,
			//    face->glyph->bitmap.width,
			//    face->glyph->bitmap.rows,
			//    0,
			//    GL_RED,
			//    GL_UNSIGNED_BYTE,
			//    face->glyph->bitmap.buffer
			//);
			//// 设置纹理选项
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// 储存字符供之后使用
			int texture = CGRender_CreateTextureFromData(ContextID(), face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, GLTextureType::GLTexture_RED);

			//std::string imagename = "e:/picture/textfont" + c + std::string{ ".png" };
			//CGRender_SaveImageByVoid(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 1, imagename, 0, false);

			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			d.Characters.insert(std::pair<char, Character>(c, character));
		}
	}

}

CGItemText::~CGItemText()
{
	if (m_priv)
	{
		delete m_priv;
		m_priv = nullptr;
	}
}

using GLfloat = float;

void CGData::CGItemText::build(int Device)
{
	auto& d = *m_priv;
	ItemData& data = *getItemData();
	Color(0x36ffffff);
	Color({ 0.5,0.1,0.3,1.0 });
	float scale = 1.0;


	float x = 0.0f;
	float y = 0.0f;

	//std::string text = "1234567890qweczxvdsafpoui";
	std::string text = wstr2utf8(L"文32132");
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = d.Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		//GLfloat vertices[6][4] = {
		//	{ xpos,     ypos + h,   0.0, 0.0 },
		//	{ xpos,     ypos,       0.0, 1.0 },
		//	{ xpos + w, ypos,       1.0, 1.0 },
		//	{ xpos,     ypos + h,   0.0, 0.0 },
		//	{ xpos + w, ypos,       1.0, 1.0 },
		//	{ xpos + w, ypos + h,   1.0, 0.0 }
		//};

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
