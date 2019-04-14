#ifndef COMS_RENDER_BASERENDER_H
#define COMS_RENDER_BASERENDER_H

#include <glad/glad.h>
#include <unordered_map>
#include "GL/GLDefine.h"
#include "Common/Tools/Utils.h"
#include "Browser/Components/BaseComponent.h"
#include "Browser/Components/Mesh/Mesh.h"
#include "Material.h"

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
		static BaseRender* createBaseRender(const std::string& materialName = Material::DEFAULT_MATERIAL_NAME, const std::string& programeName = GLProgram::DEFAULT_GLPROGRAM_NAME);
    
    public:
        // 渲染器类型
        enum RendererType
        {
            // 普通渲染器
            Base = 0,
            // 蒙皮渲染器
            Skinned,
        };
        
	public:
		BaseRender();
		~BaseRender();

	public:
		// init
		void init(const std::string& materialName, const std::string& programName);
        
		// 修改模型材质shader
        void changeMeshMaterial(Mesh* mesh, const std::string& programName);
        Material* getMaterialByMesh(Mesh* mesh);
		// 添加材质
		void addMaterial(Material* material);
		void addMaterial(const std::string& materialName, const std::string& programName);
        // 检测材质是否存在
        bool checkMaterialExist(const std::string& materialName);


		// 重载属性面板显示方法
		virtual void onInspectorGUI(InspectorPanel* inspector);
        
        
        REGISTER_PROPERTY_GET(RendererType, m_eRendererType, RendererType)
        
    protected:
        // 处理组件事件
        void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        
		// 创建材质(根据shader名字)
		Material* createMaterial(const std::string& programName, const std::string& materialName = Material::DEFAULT_MATERIAL_NAME);
        
	protected:
        // 材质map	std::string绑定Mesh的MaterialName
        std::unordered_map<std::string, Material*> m_mMaterials;
        
        // 渲染器类型
        RendererType m_eRendererType;
	};
}

#endif
