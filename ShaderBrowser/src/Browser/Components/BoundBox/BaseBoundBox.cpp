#include "BaseBoundBox.h"

namespace browser
{
	BaseBoundBox::BaseBoundBox()
        : BaseComponent("Bound Box")
        , m_bRecVisibility(true)
        , m_bDynamic(true)
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
			}
			break;

		case ComponentEvent::Transform_AddComponent:
			{
//                BROWSER_LOG("Transform_AddComponent Message received");
			}
			break;
                
        case ComponentEvent::MeshFilter_AddComponent:
            {
//                BROWSER_LOG("MeshFilter_AddComponent Message received");
            }
            break;
                
        case ComponentEvent::MeshFilter_AddMesh:
            {
//                BROWSER_LOG("MeshFilter_AddMesh Message received");
            }
            break;
                
        case ComponentEvent::Animator_ParentAddComponent:
            {
            }
            break;
                
        
		}
	}

	
    
}
