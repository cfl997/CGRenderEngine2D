#ifndef _CGEFFECTOR_H_
#define _CGEFFECTOR_H_

#include "CGItem.h"

enum ShaderCodeName;
namespace CGData
{
	class CGDATA_API CGEffector
	{
	public:
		CGEffector(int Device);
		~CGEffector();


		void addEffector(ShaderCodeName name);
		void removeEffector(ShaderCodeName name);
		void removeAllEffctor();

	public:

		void Render(int device, int hTexture, int* desTexture);

	private:
		struct PrivateData;
		PrivateData* m_priv = nullptr;
	private:
		CGEffector() = delete;
	};


}
#endif // !_CGEFFECTOR_H_
