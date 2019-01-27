#include "BaseRender.h"
#include "Material.h"
#include "Pass.h"
#include "../../../Common/System/Cache/GLProgramCache.h"
#include "../../../Common/Tools/Utils.h"

namespace browser
{
	BaseRender::BaseRender()
	{
		// 组件所属系统
		m_eBelongSystem = SystemType::RenderSystem;
        
        // 清理
        m_mMaterials.clear();
        
        // 初始化
        init();
	}

	BaseRender::~BaseRender()
	{
	}

	void BaseRender::init()
	{
        GLProgram* defaultProgram = GLProgramCache::getInstance()->getGLProgram(DEFAULT_GLPROGRAM_NAME);
        Pass* defaultPass = Pass::createPass(defaultProgram);
        
        Material* material = Material::createMaterial();
        material->addPass(defaultPass);
        
        addMaterial(DEFAULT_MATERIAL_NAME, material);
	}
    
    Material* BaseRender::getMaterialByMesh(Mesh* mesh)
    {
        auto itor = m_mMaterials.find(mesh->getMaterialName());
        if (itor != m_mMaterials.end())
        {
            return itor->second;
        }
        
        return nullptr;
    }
    
    void BaseRender::addMaterial(std::string name, Material* material)
    {
        auto itor = m_mMaterials.find(name);
        if (itor != m_mMaterials.end())
        {
            // 释放上一个material
            itor->second->release();
            BROWSER_WARNING(false, "you already have a same name material in BaseRender, you must confirm your emplace operate in function BaseRender::addMaterial");
        
            // 替换
            m_mMaterials.erase(itor);
        }

        // 插入新的material
        m_mMaterials.emplace(name, material);
        material->retain();
    }
    
    void BaseRender::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
    {
        switch (event)
        {
        case ComponentEvent::MeshFilter_AddComponent:
            {
                // Entity添加了MeshFilter组件
                BROWSER_LOG("MeshFilter_AddComponent Message received");
            }
            break;
                
        case ComponentEvent::Render_AddComponent:
            {
                // Entity添加了Render组件
                BROWSER_LOG("Render_AddComponent Message received");
            }
            break;
                
        default:
            break;
        }
    }
    
}
