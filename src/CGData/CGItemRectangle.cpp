#include "CGItemRectangle.h"
#include "CGRender.h"


using namespace CGData;

struct CGItemRectangle::PrivateData
{
	glm::vec3 ltpos;
	glm::vec3 xDirection;
	float width;
	float height;

	int indexBufferId;
	int vertexBufferId;
	int vsShader;
	int fsShader;
};


CGItemRectangle::CGItemRectangle() :m_priv(new PrivateData)
{
	auto& d = *m_priv;

	auto& itemData = *getItemData();
	itemData.PrimitiveType = GLPrimitiveTypes::LINE_LIST;
	itemData.PrimitiveType = GLPrimitiveTypes::LINE_LIST_STRIP;

	setColor(0xFF0000ff);



	//cfl-test

	d.ltpos = glm::vec3{ 0,0,-0.549623430 };
	d.width = 20.;
	d.height = 30.;
}

CGItemRectangle::~CGItemRectangle()
{

}

void CGData::CGItemRectangle::setPos(glm::vec3 pos)
{
	m_priv->ltpos = pos;
}

void CGData::CGItemRectangle::setDirection(glm::vec3 direction)
{
	m_priv->xDirection = glm::normalize(direction);
}

void CGData::CGItemRectangle::setWidth(float width)
{
	m_priv->width = width;
}

void CGData::CGItemRectangle::setHeight(float height)
{
	m_priv->height = height;
}

void CGData::CGItemRectangle::build(int Device)
{
	auto& d = *m_priv;
	glm::vec3 xDistance = g_XNormal * d.width;
	glm::vec3 yDistance = g_YNormal * d.height;


	glm::vec3 lbPos = d.ltpos + yDistance;
	glm::vec3 rbPos = lbPos + xDistance;

	glm::vec3 rtPos = d.ltpos + xDistance;

	auto& data = *getItemData();

	data.vertexes.clear();
	data.vertexes.push_back({ d.ltpos,Color(),{0,0} });
	data.vertexes.push_back({ rtPos,Color(),{0,0} });
	data.vertexes.push_back({ rbPos,0x00ff00,{0,0} });
	data.vertexes.push_back({ lbPos,0xff0000,{0,0} });

	auto sizea = sizeof(glm::vec3);

	//d.vertexBufferId = CGRender_CreateBuffer(getGLContextID(), sizeof(CGData::Vertex), data.vertexes.size(), data.vertexes.data(), GLBufferType::VertexBuffer, GetPrimitiveType());
	d.vertexBufferId = CGRender_CreateBuffer(Device, sizeof(CGData::Vertex), data.vertexes.size(), &data.vertexes[0], GLBufferType::VertexBuffer, GetPrimitiveType());
	{
		auto& itemData = *getItemData();
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

		d.vsShader = CGRender_CreateShader(Device, ShaderCodeName::VS_POS_COLOR_TEX_viewMatrix);
		//d.vsShader = CGRender_CreateShader(Device, ShaderCodeName::VS_POS_COLOR_TEX);
		d.fsShader = CGRender_CreateShader(Device, ShaderCodeName::FS_COLOR);
	}



}

void CGData::CGItemRectangle::Render(int device, const glm::mat4& matrix)
{
	build(device);
	auto& d = *m_priv;
	auto& itemData = *getItemData();
	struct AAA
	{
		glm::mat4 a;
	};
	AAA aaa;
	aaa.a = matrix;

	glm::mat4 move = glm::mat4{ 1 };
	//move=glm::translate(move, glm::vec3(-100, 100, 0));
	//move=glm::translate(move, glm::vec3{0});

	aaa.a = matrix * move;
	int uniformBufferid = CGRender_CreateBuffer(device, sizeof(glm::mat4), 1, &aaa, GLBufferType::uniformBuffer);

	CGRender_SetShader(device, d.vsShader, ShaderType::VERTEX);
	CGRender_SetShader(device, d.fsShader, ShaderType::FRAGMENT);

	CGRender_SetUniformBuffer(device, uniformBufferid, 0, ShaderType::VERTEX);

	CGRender_Render(device, d.vertexBufferId, d.indexBufferId, 0, 0, 0, 0);
}

