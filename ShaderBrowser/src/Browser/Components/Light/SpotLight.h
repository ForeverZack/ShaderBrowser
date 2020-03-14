#pragma once

#include <vector>
#include "Browser/Components/Light/BaseLight.h"
using namespace common;

namespace browser
{
	class SpotLight : public BaseLight
	{
	public:
		SpotLight();
		~SpotLight();

	public:
		
        
	protected:
		// 光照范围
		float m_fRange;
		// 光照角度 (<180度)
		float m_fSpotAngle;
		// 光照衰减纹理 (基于距离)
		customGL::Texture2D* m_pLightTexture0;
		// 光照衰减纹理2 (基于广角)
		customGL::Texture2D* m_pLightTextureB0;

       // 阴影属性等
        
	};
}

