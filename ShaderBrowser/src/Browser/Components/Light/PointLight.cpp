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


	void PointLight::updateLight()
	{

	}

	void PointLight::updateMaterialsLight(const std::unordered_map<unsigned int, Material*>& materials, unsigned int index/* = 0*/, bool forceUpdate/* = false*/)
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
			sprintf(&uniformName[0], SHADER_UNIFORM_DIRECTIONAL_COLOR, index);
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
			sprintf(&uniformName[0], SHADER_UNIFORM_DIRECTIONAL_INTENSITY, index);
			for (auto itor = materials.begin(); itor != materials.end(); ++itor)
			{
				material = itor->second;
				material->setUniformFloat(uniformName, m_fIntensity);
			}
		}

		// 重置脏标记
		resetLightDirty();
	}

	bool PointLight::isLightDirty()
	{
		if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Color)
			|| BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity)
			|| BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightDirection))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool PointLight::isLightSystemDirty()
	{
		return BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity);
	}
	
    
}
