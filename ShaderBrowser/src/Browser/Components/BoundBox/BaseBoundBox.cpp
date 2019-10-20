#include "BaseBoundBox.h"

namespace browser
{
	BaseBoundBox::BaseBoundBox()
        : BaseComponent("Bound Box")
        , m_bRecVisibility(true)
        , m_oMeshFilter(nullptr)
        , m_oAnimator(nullptr)
	{
		// 组件所属系统
		m_eBelongSystem = SystemType::BoundBox;
        
        // 清理
        m_vDisplayVertices.clear();
	}

	BaseBoundBox::~BaseBoundBox()
	{
		BROWSER_LOG("~BaseBoundBox");
	}

	void BaseBoundBox::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
	{
		switch (event)
		{
		case ComponentEvent::BoundBox_AddComponent:
			{
//                BROWSER_LOG("BoundBox_AddComponent Message received");
				getTransformFromMsg<BoundBoxAddComponentMessage>(msg);
                getMeshFilterFromMsg<BoundBoxAddComponentMessage>(msg);
                getAnimatorFromMsg<BoundBoxAddComponentMessage>(msg);
			}
			break;

		case ComponentEvent::Transform_AddComponent:
			{
//                BROWSER_LOG("Transform_AddComponent Message received");
				getTransformFromMsg<TransformAddComponentMessage>(msg);
			}
			break;
                
        case ComponentEvent::MeshFilter_AddComponent:
            {
//                BROWSER_LOG("MeshFilter_AddComponent Message received");
                getMeshFilterFromMsg<MeshFilterAddComponentMessage>(msg);
            }
            break;
                
        case ComponentEvent::MeshFilter_AddMesh:
            {
//                BROWSER_LOG("MeshFilter_AddMesh Message received");
                getMeshFilterFromMsg<MeshFilterAddMeshMessage>(msg);
            }
            break;
                
        case ComponentEvent::Animator_ParentAddComponent:
            {
                getAnimatorFromMsg<AnimatorAddComponentMessage>(msg);
            }
            break;
                
        
		}
	}

	
    
}
