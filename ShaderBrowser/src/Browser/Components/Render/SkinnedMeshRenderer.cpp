#include "SkinnedMeshRenderer.h"
#include "Common/Tools/Utils.h"
#include "Browser/Components/BoundBox/AABBBoundBox.h"
#include "Browser/Entitys/BaseEntity.h"

namespace browser
{
    SkinnedMeshRenderer* SkinnedMeshRenderer::createSkinnedMeshRenderer(const std::string& materialName /*= Material::DEFAULT_MATERIAL_NAME*/, const std::string& programeName /*= GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME*/)
    {
        SkinnedMeshRenderer* renderer = new SkinnedMeshRenderer();
        renderer->init(materialName, programeName);
        return renderer;
    }
    
    SkinnedMeshRenderer* SkinnedMeshRenderer::createSkinnedMeshRenderer(Material* material)
    {
        SkinnedMeshRenderer* renderer = new SkinnedMeshRenderer();
        renderer->init(material);
        return renderer;
    }

	SkinnedMeshRenderer::SkinnedMeshRenderer()
	{
        // 组件名称
        m_sComponentName = "Skinned Mesh Renderer";

        // 渲染器类型
        m_eRendererType = RendererType::RendererType_Skinned;
        
        // 网格筛选器
        m_oMeshFilter = new MeshFilter();
        m_oMeshFilter->retain();
        
        // 包围盒：虽然包围盒是在skinnedMeshRenderer中创建的，但在renderer组件添加到entity上时，会把包围盒组件也添加上去
        m_oBoundBox = new AABBBoundBox();
        m_oBoundBox->retain();
	}

	SkinnedMeshRenderer::~SkinnedMeshRenderer()
	{
        // 释放网格筛选器
        m_oMeshFilter->release();
	}
    
    void SkinnedMeshRenderer::addMesh(Mesh* mesh)
    {
        m_oMeshFilter->addMesh(mesh);
    }
    
    bool SkinnedMeshRenderer::checkVisibility(Camera* camera, bool reCalculate /*= false*/)
    {
        return m_oBoundBox->checkVisibility(camera, reCalculate);
    }

	void SkinnedMeshRenderer::onInspectorGUI(InspectorPanel* inspector)
	{
		// 材质列表(临时)
		for (auto itor = m_vMaterials.begin(); itor != m_vMaterials.end(); ++itor)
		{
			inspector->addPropertyText("Materials : " + (*itor)->getMaterialName());
            inspector->addPropertyText("    sharedId : " + std::to_string((*itor)->getSharedId()));
		}
	}
    
    void SkinnedMeshRenderer::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
    {
        switch (event)
        {
        case ComponentEvent::Transform_AddComponent:
            {
                //                BROWSER_LOG("Transform_AddComponent Message received");
            }
            break;
                
        case ComponentEvent::Render_AddComponent:
            {
                //                BROWSER_LOG("Transform_AddComponent Message received");
                RenderAddComponentMessage* convert_msg = static_cast<RenderAddComponentMessage*>(msg);
                // 将SkinnedMeshRenderer的包围盒组件添加到Entity上
                convert_msg->getEntity()->addComponent(m_oBoundBox);
                m_oBoundBox->release();
            }
            break;
        default:
            break;
        }
    }

    
}
