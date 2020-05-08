#include "MeshRenderer.h"
#include "GL/GPUResource/Shader/Pass.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "Common/Tools/Utils.h"

namespace browser
{
	EnumComponentClass MeshRenderer::ComponentClass = EnumComponentClass::ECC_MeshRenderer;

	MeshRenderer* MeshRenderer::createBaseRender(const std::string& materialName /*= Material::DEFAULT_MATERIAL_NAME*/, const std::string& programeName /*= GLProgram::DEFAULT_GLPROGRAM_NAME*/)
	{
		MeshRenderer* renderer = new MeshRenderer();
        renderer->init(materialName, programeName);
		return renderer;
	}
    
    MeshRenderer* MeshRenderer::createBaseRender(Material* material)
    {
        MeshRenderer* renderer = new MeshRenderer();
        renderer->init(material);
        return renderer;
    }

	MeshRenderer::MeshRenderer()
        : BaseComponent("Base Render")
        , m_eRendererType(RendererType::RendererType_Mesh)
		, m_bCastShadows(false)
		, m_bReceiveShadows(false)
	{
		// 组件所属系统
		m_eBelongSystem = SystemType::RenderSystem;
        
        // 清理
        m_vMaterials.clear();
	}

	MeshRenderer::~MeshRenderer()
	{
	}

	void MeshRenderer::onInspectorGUI(InspectorPanel* inspector)
	{
		// 材质列表(临时)
		for (auto itor = m_vMaterials.begin(); itor != m_vMaterials.end(); ++itor)
		{
			inspector->addPropertyText("Materials : " + (*itor)->getMaterialName());
            inspector->addPropertyText("    sharedId : " + std::to_string((*itor)->getSharedId()));
		}
	}

	void MeshRenderer::init(const std::string& materialName, const std::string& programName)
	{
        addMaterial(materialName, programName);
	}
    
    void MeshRenderer::init(Material* material)
    {
        addMaterial(material);
    }
    
	Material* MeshRenderer::changeMaterial(int index, const std::string& materialName, const std::string& programName)
    {
        BROWSER_WARNING(0<=index && index<m_vMaterials.size(), "The material index is out of range, and you are trying to change it, please confirm your program in function MeshRenderer::changeMaterial");
        
        Material* material = createMaterial(programName, materialName);
		changeMaterial(index, material);
		return material;
    }
    
	Material* MeshRenderer::changeMaterial(int index, Material* material)
    {
        BROWSER_WARNING(0<=index && index<m_vMaterials.size(), "The material index is out of range, and you are trying to change it, please confirm your program in function MeshRenderer::changeMaterial");
        
		int size = m_vMaterials.size();
        if (index < m_vMaterials.size())
        {
            m_vMaterials[index]->release();
            m_vMaterials[index] = material;
            material->retain();
        }
        else
        {
            addMaterial(material);
        }
		// 重置脏标记
		material->setTransformDirty(true);

		return material;
	}
    
    Material* MeshRenderer::getMaterialByIndex(int index /*= 0*/)
    {
        BROWSER_ASSERT(index<m_vMaterials.size(), "The material index is out of materils range in function MeshRenderer::getMaterialByMeshIndex");
        
        return m_vMaterials[index];
    }

	Material* MeshRenderer::createMaterial(const std::string& programName, const std::string& materialName /*= Material::DEFAULT_MATERIAL_NAME*/)
	{
		Material* material = Material::createMaterial(programName, materialName);

		return material;
	}

	void MeshRenderer::addMaterial(const std::string& materialName, const std::string& programName)
	{
		Material* material = createMaterial(programName, materialName);
		addMaterial(material);
	}
    
    bool MeshRenderer::checkMaterialExist(Material* material)
    {
        for(auto itor=m_vMaterials.begin(); itor!=m_vMaterials.end(); ++itor)
        {
            if(material == (*itor))
            {
                return true;
            }
        }
        
        return false;
    }
    
    void MeshRenderer::addMaterial(Material* material)
    {
		const std::string& name = material->getMaterialName();
        m_vMaterials.push_back(material);
        material->retain();
    }
    
    void MeshRenderer::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
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
