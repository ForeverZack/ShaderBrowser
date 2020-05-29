#include "PointLight.h"
#include "Common/System/Cache/TextureCache.h"
#include "Browser/System/TransformSystem.h"

namespace browser
{
    PointLight* PointLight::create(const std::string& name, BaseEntity* parent/* = nullptr*/)
    {
        if (!parent)
        {
            // 没有父节点就绑定到场景节点上
            parent = TransformSystem::getInstance()->getScene();
        }

        // entity
        BaseEntity* entity = BaseEntity::create(name);
        parent->addChild(entity);
        // 点光源组件
        PointLight* pointLight = new PointLight();
        entity->addComponent(pointLight);

        return pointLight;
    }
    
	PointLight::PointLight()
        : m_fRange(10.0f)
    {
        // 设置光源类型
        m_eType = LightType::Point;
		// 光源数据
		m_vLightData.resize(6);	// vec4:[1] + (float+position):[1] + mat4:[4]

        // 光照衰减纹理
        m_pLightTexture0 = TextureCache::getInstance()->getTexture("texture/default/light_atten_distance.png");
	}

	PointLight::~PointLight()
	{
		BROWSER_LOG("~PointLight");
	}

	void PointLight::onInspectorGUI(InspectorPanel* inspector)
	{
		inspector->addPropertyText("Light Type: Point");

		inspector->addPropertyInputFloat("Intensity", &m_fIntensity, [=](float intensity)
		{
			setIntensity(intensity);
		}, false);

		inspector->addPropertyColor4("Color", &m_oColor, [=](const glm::vec4& color)
		{
			setColor(color);
		}, false);

		inspector->addPropertyInputFloat("Range", &m_fRange, [=](float range)
			{
				setRange(range);
			}, false);
	}

	void PointLight::serialize()
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

	void PointLight::updateLight()
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
                float s = 1 / m_fRange;
                glm::mat4 scaleMat(s, 0, 0, 0,
                                   0, s, 0, 0,
                                   0, 0, s, 0,
                                   0, 0, 0, 1);
				// scaleMat * object2Light
                m_oLightMatrix = scaleMat * glm::inverse(transform->getModelMatrix());
                BROWSER_LOG_MAT4(m_oLightMatrix);
            }
        }
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
			sprintf(&uniformName[0], SHADER_UNIFORM_POINT_COLOR, index);
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
			sprintf(&uniformName[0], SHADER_UNIFORM_POINT_INTENSITY, index);
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
            sprintf(&uniformName[0], SHADER_UNIFORM_POINT_POSITION, index);
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
            sprintf(&uniformName[0], SHADER_UNIFORM_POINT_LIGHTMATRIX, index);
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
		}

		// 重置脏标记
		resetLightDirty();
	}

	bool PointLight::isLightDirty()
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

	bool PointLight::isLightSystemDirty()
	{
		return false;
	}
	
    void PointLight::setRange(float range)
    {
        m_fRange = range;
        setDirty(LightPropertyType::LPT_LightMatrix);
    }
    
}
