#include "SpotLight.h"
#include "Common/System/Cache/TextureCache.h"
#include "Browser/System/TransformSystem.h"

namespace browser
{
    SpotLight* SpotLight::create(const std::string& name, BaseEntity* parent/* = nullptr*/)
    {
        if (!parent)
        {
            // 没有父节点就绑定到场景节点上
            parent = TransformSystem::getInstance()->getScene();
        }

        // entity
        BaseEntity* entity = BaseEntity::create(name);
        parent->addChild(entity);
        // 聚光灯组件
        SpotLight* spotLight = new SpotLight();
        entity->addComponent(spotLight);

        return spotLight;
    }
    
	SpotLight::SpotLight()
		: m_fRange(10)
		, m_fSpotAngle(30)
    {
        // 设置光源类型
        m_eType = LightType::Spot;
		// 光源数据
		m_vLightData.resize(6);	// vec4:[1] + (float+position):[1] + mat4:[4]

		// 光照衰减纹理 (基于距离)
		m_pLightTexture0 = TextureCache::getInstance()->getTexture("texture/default/light_atten_distance.png");
		// 光照衰减纹理 (基于广角)
		m_pLightTextureB0 = TextureCache::getInstance()->getTexture("texture/default/light_atten_field.png");
	}

	SpotLight::~SpotLight()
	{
		BROWSER_LOG("~SpotLight");
	}

	void SpotLight::onInspectorGUI(InspectorPanel* inspector)
	{
		inspector->addPropertyText("Light Type: Spot");

		inspector->addPropertyInputFloat("Intensity", &m_fIntensity, [=](float intensity)
			{
				setIntensity(intensity);
			}, false);

		inspector->addPropertyColor4("Color", &m_oColor, [=](const glm::vec4& color)
			{
				setColor(color);
			}, false);

		ShowGUIData& range_data = inspector->addPropertyInputFloat("Range", &m_fRange, [=](float range)
			{
				setRange(range);
			}, false);
		range_data.helpMarker = "Spot light's attenuation range";

		ShowGUIData& angle_data = inspector->addPropertySliderFloat("Spot Angle", &m_fSpotAngle, 0, 180, [=](float angle)
			{
				setSpotAngle(angle);
			}, false);
		angle_data.helpMarker = "Control spot light's field angle, it limits in [0, 180]";
	}

	void SpotLight::serialize()
	{
		//struct SpotLight
		//{
		//	vec4 color;
		//	float intensity;
		//	vec3 position;  // world-space
		//	mat4 lightMatrix;   // world-space -> light-space
		//};

		// vec4:[1] + (float+position):[1] + mat4:[4]
		m_vLightData.clear();
		// color
		m_vLightData[0] = m_oColor;
		// intensity, position
		m_vLightData[1].w = m_fIntensity;
		m_vLightData[1].x = m_oGlobalPosition.x;
		m_vLightData[1].y = m_oGlobalPosition.y;
		m_vLightData[1].z = m_oGlobalPosition.z;
		// lightMatrix
		m_vLightData[2] = m_oLightMatrix[0];	// 第1列
		m_vLightData[3] = m_oLightMatrix[1];	// 第2列
		m_vLightData[4] = m_oLightMatrix[2];	// 第3列
		m_vLightData[5] = m_oLightMatrix[3];	// 第4列
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
