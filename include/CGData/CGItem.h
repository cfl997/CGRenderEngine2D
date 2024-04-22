#ifndef _CGITEM_H_
#define _CGITEM_H_

#include "CGObject.h"
#include "../CGRender/CGglm.h"
#include "../CGRender/CGRenderMacros.h"

#include <Windows.h>


namespace CGData
{
	static const float g_itemZDistance = .1f;

	struct Vertex
	{
		glm::vec3 vertex;
		DWORD color;
		glm::vec2 tex;
	};

	using VertexArray = std::vector<Vertex>;

	struct ItemData
	{
		VertexArray vertexes;
		std::vector<uint32_t>indexes;
		GLPrimitiveTypes PrimitiveType;
	};

	class CGDATA_API CGItem :public CGObject
	{
		typedef CGObject super;
	public:
		CGItem();
		virtual ~CGItem();

	public:
		void setPrimitiveType(GLPrimitiveTypes type);
		GLPrimitiveTypes GetPrimitiveType();

		void setColor(DWORD color);
		const DWORD Color()const;

		ItemData* getItemData();
	public:
		glm::mat4 getModelMatrix();
		void setModelMatrix(const glm::mat4& model);
	public:
		virtual void build(int Device);
		virtual void Render(int device, const glm::mat4& matrix);

	private:
		struct PrivateData;
		PrivateData* m_priv;
	};
}
#endif // !_CGITEM_H_
