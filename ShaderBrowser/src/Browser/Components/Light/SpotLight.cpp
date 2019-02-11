#include "SpotLight.h"

namespace browser
{
	SpotLight::SpotLight()
    {
        // 设置光源类型
        m_eType = LightType::Spot;
	}

	SpotLight::~SpotLight()
	{
		BROWSER_LOG("~SpotLight");
	}


	
    
}
