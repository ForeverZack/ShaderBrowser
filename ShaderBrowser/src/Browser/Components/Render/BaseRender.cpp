#include "BaseRender.h"
#include "Pass.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "Common/Tools/Utils.h"

namespace browser
{
	BaseRender* BaseRender::createBaseRender(const std::string& materialName /*= Material::DEFAULT_MATERIAL_NAME*/, const std::string& programeName /*= GLProgram::DEFAULT_GLPROGRAM_NAME*/)
	{
		BaseRender* renderer = new BaseRender();
		renderer->init(materialName, programeName);

		return renderer;
	}

	BaseRender::BaseRender()
        : BaseComponent("Base Render")
	{
		// 组件所属系统
		m_eBelongSystem = SystemType::RenderSystem;
        
        // 清理
        m_mMaterials.clear();
	}

	BaseRender::~BaseRender()
	{
	}

	void BaseRender::onInspectorGUI(InspectorPanel* inspector)
	{
		// 材质列表(临时)
		for (auto itor = m_mMaterials.begin(); itor != m_mMaterials.end(); ++itor)
		{
			inspector->addPropertyText("Materials : " + itor->first);
		}
	}

	void BaseRender::init(const std::string& materialName, const std::string& programName)
	{
        addMaterial(materialName, programName);
	}
    
    void BaseRender::changeMeshMaterial(Mesh* mesh, const std::string& programName)
    {
        BROWSER_WARNING(m_mMaterials.find(mesh->getMaterialName())==m_mMaterials.end(), "The material has already exist, and you are trying to change it, please confirm your program in function BaseRender::changeMeshMaterial");
        
        Material* material = createMaterial(programName, mesh->getMaterialName());
        addMaterial(material);
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

	Material* BaseRender::createMaterial(const std::string& programName, const std::string& materialName /*= Material::DEFAULT_MATERIAL_NAME*/)
	{
		// 缓存中获取glProgram
		GLProgram* program = GLProgramCache::getInstance()->getGLProgram(programName);
		// （注意！！！Pass中会存储uniform的值，所以要考虑好pass能不能复用，可以给场景物体一个isStatic选项，让他们可以合批noMVP。cocos中只有使用了noMVP的才能复用，所以暂时不对pass做cache处理）
		Pass* pass = Pass::createPass(program);

		Material* material = Material::createMaterial(materialName);
		material->addPass(pass);

		return material;
	}

	void BaseRender::addMaterial(const std::string& materialName, const std::string& programName)
	{
		Material* material = createMaterial(programName, materialName);
		addMaterial(material);
	}
    
    void BaseRender::addMaterial(Material* material)
    {
		const std::string& name = material->getMaterialName();
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
        case ComponentEvent::Render_AddComponent:
            {
                // Entity添加了Render组件
                BROWSER_LOG("Render_AddComponent Message received");
                // TODO: 从MeshFilter中读取mesh数据，来生成Material
                
            }
            break;
        case ComponentEvent::MeshFilter_AddComponent:
            {
                // Entity添加了MeshFilter组件
                BROWSER_LOG("MeshFilter_AddComponent Message received");
				// TODO: 从MeshFilter中读取mesh数据，来生成Material
            }
            break;

		case ComponentEvent::MeshFilter_AddMesh:
			{
				// MeshFilter组件添加Mesh
				BROWSER_LOG("MeshFilter_AddMesh Message received");
				// TODO: 读取mesh数据，来生成Material
			}
			break;
                
        default:
            break;
        }
    }
    
}
