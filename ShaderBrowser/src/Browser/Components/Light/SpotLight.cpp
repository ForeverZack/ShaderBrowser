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

		// 光照衰减纹理 (基于广角)
		m_pLightTexture0 = TextureCache::getInstance()->getTexture("texture/default/light_atten_field.png");
		// 光照衰减纹理 (基于距离)
		m_pLightTextureB0 = TextureCache::getInstance()->getTexture("texture/default/light_atten_distance.png");
	}

	SpotLight::~SpotLight()
	{
		BROWSER_LOG("~SpotLight");
	}

	void SpotLight::updateLight()
	{
		Transform* transform = m_oBelongEntity ? m_oBelongEntity->getComponent<Transform>() : nullptr;
		if (transform)
		{
			const glm::vec3& globalPosition = transform->getGlobalPosition();
			if (m_oGlobalPosition != globalPosition)
			{
				recordGlobalPosition(globalPosition);
			}

			if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightMatrix))
			{
				// 更新光源矩阵
				// we want out.w = 2.0 * in.z / m_CotanHalfSpotAngle
				// c = m_CotanHalfSpotAngle
				// 1 0 0 0
				// 0 1 0 0
				// 0 0 1 0
				// 0 0 2/c 0
				// the "2" will be used to scale .xy for the cookie as in .xy/2 + 0.5 

				float halfSpotRad = glm::radians(m_fSpotAngle * 0.5f);
				float cos = cosf(halfSpotRad);
				float sin = sinf(halfSpotRad);
				float cotHalfSpotAngle = cos / sin;
				glm::mat4 halfFieldMat(1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 2/cotHalfSpotAngle,
					0, 0, 0, 0);

				float s = 1 / m_fRange;
				glm::mat4 scaleMat(s, 0, 0, 0,
					0, s, 0, 0,
					0, 0, s, 0,
					0, 0, 0, 1);

				// halfFieldMat * scaleMat * object2Light
				m_oLightMatrix = halfFieldMat * scaleMat * glm::inverse(transform->getModelMatrix());
				BROWSER_LOG_MAT4(m_oLightMatrix);
			}
		}
	}

	void SpotLight::updateMaterialsLight(const std::unordered_map<unsigned int, Material*>& materials, unsigned int index/* = 0*/, bool forceUpdate/* = false*/)
	{
		if (materials.size() <= 0)
		{
			return;
		}

		Material* material = nullptr;
		// 颜色
		if (forceUpdate || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Color))
		{
			std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
			sprintf(&uniformName[0], SHADER_UNIFORM_SPOT_COLOR, index);
			for (auto itor = materials.begin(); itor != materials.end(); ++itor)
			{
				material = itor->second;
				material->setUniformV4f(uniformName, m_oColor);
			}
		}
		// 强度
		if (forceUpdate || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity))
		{
			std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
			sprintf(&uniformName[0], SHADER_UNIFORM_SPOT_INTENSITY, index);
			for (auto itor = materials.begin(); itor != materials.end(); ++itor)
			{
				material = itor->second;
				material->setUniformFloat(uniformName, m_fIntensity);
			}
		}
		// 位置
		if (forceUpdate || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_GlobalPosition))
		{
			std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
			sprintf(&uniformName[0], SHADER_UNIFORM_SPOT_POSITION, index);
			for (auto itor = materials.begin(); itor != materials.end(); ++itor)
			{
				material = itor->second;
				material->setUniformV3f(uniformName, m_oGlobalPosition);
			}
		}
		// 光源矩阵
		if (forceUpdate || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightMatrix))
		{
			std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
			sprintf(&uniformName[0], SHADER_UNIFORM_SPOT_LIGHTMATRIX, index);
			for (auto itor = materials.begin(); itor != materials.end(); ++itor)
			{
				material = itor->second;
				material->setUniformMat4(uniformName, m_oLightMatrix);
			}
		}
		// 衰减纹理
		for (auto itor = materials.begin(); itor != materials.end(); ++itor)
		{
			material = itor->second;
			material->setUniformTex2D(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_LIGHT_TEXTURE0], m_pLightTexture0);
			material->setUniformTex2D(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_LIGHT_TEXTUREB0], m_pLightTextureB0);
		}

		// 重置脏标记
		resetLightDirty();
	}

	bool SpotLight::isLightDirty()
	{
		if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Color)
			|| BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity)
			|| BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_GlobalPosition)
			|| BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightMatrix))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool SpotLight::isLightSystemDirty()
	{
		return false;
	}

	void SpotLight::setRange(float range)
	{
		m_fRange = range;
		setDirty(LightPropertyType::LPT_LightMatrix);
	}
	
	void SpotLight::setSpotAngle(float angle)
	{
		m_fSpotAngle = angle;
		setDirty(LightPropertyType::LPT_LightMatrix);
	}
    
}
