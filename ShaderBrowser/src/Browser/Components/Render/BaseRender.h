#ifndef COMS_RENDER_BASERENDER_H
#define COMS_RENDER_BASERENDER_H

#include <glad/glad.h>
#include <unordered_map>
#include "../../../GL/GLDefine.h"
#include "../../../Common/Tools/Utils.h"
#include "../BaseComponent.h"
#include "../Mesh/Mesh.h"

using namespace std;
using namespace customGL;
using namespace common;

namespace browser
{
	class Material;
    class Pass;

	class BaseRender : public BaseComponent
	{
	public:
		BaseRender();
		~BaseRender();

	public:
		// init
		void init();
        
        Material* getMaterialByMesh(Mesh* mesh);
        
    protected:
        // 处理组件事件
        void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        
        // 添加材质
        void addMaterial(std::string name, Material* material);
        
	private:
        // 材质map
        std::unordered_map<std::string, Material*> m_mMaterials;
        
	};
}

#endif
