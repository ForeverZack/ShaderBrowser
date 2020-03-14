#include "SpotLight.h"
#include "Common/System/Cache/TextureCache.h"
#include "Browser/System/TransformSystem.h"

namespace browser
{
	SpotLight::SpotLight()
		: m_fRange(10)
		, m_fSpotAngle(30)
    {
        // 设置光源类型
        m_eType = LightType::Spot;

		// 光照衰减纹理 (基于距离)
		m_pLightTexture0 = TextureCache::getInstance()->getTexture("texture/default/light_atten_distance.png");
		// 光照衰减纹理 (基于广角)
		m_pLightTextureB0 = TextureCache::getInstance()->getTexture("texture/default/light_atten_field.png");
	}

	SpotLight::~SpotLight()
	{
		BROWSER_LOG("~SpotLight");
	}


	
    
}
