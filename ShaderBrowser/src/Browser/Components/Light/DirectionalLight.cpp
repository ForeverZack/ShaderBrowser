#include "DirectionalLight.h"

namespace browser
{
	DirectionalLight::DirectionalLight()
    {
        // 设置光源类型
        m_eType = LightType::Directional;
	}

	DirectionalLight::~DirectionalLight()
	{
		BROWSER_LOG("~DirectionalLight");
	}


	
    
}
