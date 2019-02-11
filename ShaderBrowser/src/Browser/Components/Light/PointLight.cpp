#include "PointLight.h"

namespace browser
{
	PointLight::PointLight()
    {
        // 设置光源类型
        m_eType = LightType::Point;
	}

	PointLight::~PointLight()
	{
		BROWSER_LOG("~PointLight");
	}


	
    
}
