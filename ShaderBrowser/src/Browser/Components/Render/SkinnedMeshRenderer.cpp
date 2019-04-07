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

	SkinnedMeshRenderer::SkinnedMeshRenderer()
	{
        // 组件名称
        m_sComponentName = "Skinned Mesh Renderer";

        // 渲染器类型
        m_eRendererType = RendererType::Skinned;
        
        // 网格筛选器
        m_oMeshFilter = new MeshFilter();
        m_oMeshFilter->retain();
        
        // 包围盒 （TODO:这里暂时使用AABB包围盒）
        m_oBoundBox = new AABBBoundBox();
        m_oBoundBox->retain();
        m_oBoundBox->setMeshFilter(m_oMeshFilter);
	}

	SkinnedMeshRenderer::~SkinnedMeshRenderer()
	{
        // 释放网格筛选器
        m_oMeshFilter->release();
        // 释放包围盒
        m_oBoundBox->release();
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
		for (auto itor = m_mMaterials.begin(); itor != m_mMaterials.end(); ++itor)
		{
			inspector->addPropertyText("Materials : " + itor->first);
		}
	}
    
    void SkinnedMeshRenderer::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
    {
        switch (event)
        {
        case ComponentEvent::Transform_AddComponent:
            {
                //                BROWSER_LOG("Transform_AddComponent Message received");
                m_oBoundBox->getTransformFromMsg<TransformAddComponentMessage>(msg);
            }
            break;
                
        case ComponentEvent::Render_AddComponent:
            {
                //                BROWSER_LOG("Transform_AddComponent Message received");
//                m_oBoundBox->getTransformFromMsg<TransformAddComponentMessage>(msg);
                RenderAddComponentMessage* convert_msg = static_cast<RenderAddComponentMessage*>(msg);
                m_oBoundBox->setTransform(convert_msg->getEntity()->getTransform());
            }
            break;
        default:
            break;
        }
    }
    
    void SkinnedMeshRenderer::updateRenderer(float deltaTime)
    {
        m_oBoundBox->updateBoundBox(deltaTime);
    }
    
    
}
