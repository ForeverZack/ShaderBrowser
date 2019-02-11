#include "BaseLight.h"

namespace browser
{
	BaseLight::BaseLight()
        : m_oColor(GLM_COLOR_WHITE)
        , m_fIntensity(1.0f)
    {
		// 组件所属系统
		m_eBelongSystem = SystemType::Light;
	}

	BaseLight::~BaseLight()
	{
		BROWSER_LOG("~BaseLight");
	}

	void BaseLight::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
	{
		switch (event)
		{
		case ComponentEvent::Transform_AddComponent:
			{
//                BROWSER_LOG("Transform_AddComponent Message received");
				getTransformFromMsg<TransformAddComponentMessage>(msg);
			}
			break;
                
		}
	}

	
    
}
