#ifndef COMS_RENDER_BASERENDER_H
#define COMS_RENDER_BASERENDER_H

#include <glad/glad.h>
#include <vector>
#include "GL/GLDefine.h"
#include "Common/Tools/Utils.h"
#include "Browser/Components/BaseComponent.h"
#include "GL/GPUResource/Model/Mesh.h"
#include "GL/GPUResource/Shader/Material.h"

using namespace std;
using namespace customGL;
using namespace common;

namespace browser
{
    class Pass;

	class BaseRender : public BaseComponent
	{
	public:
		static BaseRender* createBaseRender(const std::string& materialName = customGL::Material::DEFAULT_MATERIAL_NAME, const std::string& programeName = GLProgram::DEFAULT_GLPROGRAM_NAME);
        static BaseRender* createBaseRender(Material* material);
    
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
		virtual ~BaseRender();

	public:
		// init
		void init(const std::string& materialName, const std::string& programName);
        void init(Material* material);
        
		// 修改模型材质shader
        void changeMaterial(int index, const std::string& materialName, const std::string& programName);
        void changeMaterial(int index, Material* material);
        // 获取材质队列中对应的材质（对于一个节点包含多个mesh的情况，现在的处理方式是：mesh队列和material队列一一对应，对于多出的material将会用来对最后一个mesh进行重复渲染）
        Material* getMaterialByIndex(int index = 0);
		// 添加材质
		void addMaterial(Material* material);
		void addMaterial(const std::string& materialName, const std::string& programName);
        // 检测材质是否存在
        bool checkMaterialExist(Material* material);


		// 重载属性面板显示方法
		virtual void onInspectorGUI(InspectorPanel* inspector);
        
        
        REGISTER_PROPERTY_GET(RendererType, m_eRendererType, RendererType)
        
    protected:
        // 处理组件事件
        void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        
		// 创建材质(根据shader名字)
		Material* createMaterial(const std::string& programName, const std::string& materialName = customGL::Material::DEFAULT_MATERIAL_NAME);
        
	protected:
        // 材质map	std::string绑定Mesh的MaterialName
        std::vector<Material*> m_vMaterials;
        
        // 渲染器类型
        RendererType m_eRendererType;
	};
}

#endif
