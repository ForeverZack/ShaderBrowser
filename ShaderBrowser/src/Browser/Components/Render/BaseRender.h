#ifndef COMS_RENDER_BASERENDER_H
#define COMS_RENDER_BASERENDER_H

#include <glad/glad.h>
#include <unordered_map>
#include "../../../GL/GLDefine.h"
#include "../../../Common/Tools/Utils.h"
#include "../BaseComponent.h"

using namespace std;
using namespace customGL;

namespace browser
{
	class Material;

	class BaseRender : public BaseComponent
	{
	public:
		BaseRender();
		~BaseRender();

	public:
		// init
		void init(Material* material);
        
		REGISTER_PROPERTY_GET(Material*, m_oMaterial, Material)
        
	private:
		// 材质
		Material* m_oMaterial;

	};
}

#endif
