#include "DirectionalLight.h"
#include "GL/GPUResource/Shader/GLProgram.h"
#include "Browser/Entitys/BaseEntity.h"
#include "Browser/System/TransformSystem.h"

using namespace common;

namespace browser
{
	DirectionalLight* DirectionalLight::create(const std::string& name, BaseEntity* parent/* = nullptr*/)
	{
		if (!parent)
		{
			// 没有父节点就绑定到场景节点上
			parent = TransformSystem::getInstance()->getScene();
		}

		// entity
		BaseEntity* directionalLightEntity = BaseEntity::create(name);
		parent->addChild(directionalLightEntity);
		// 平行光组件
		DirectionalLight* directionalLight = new DirectionalLight();
		directionalLightEntity->addComponent(directionalLight);

		return directionalLight;
	}

	DirectionalLight::DirectionalLight()
    {
        m_sComponentName = "Directional Light";
        // 设置光源类型
        m_eType = LightType::Directional;
        // 设置光源颜色
        m_oColor = glm::vec4(1, 0.9569f, 0.8392f, 1);
        
		// 设置脏标记
		m_uPropertiesDirty = LPT_Color | LPT_Intensity | LPT_LightDirection;
		dispatchEventToSystem(SystemType::Light, ComponentEvent::Light_UpdateLight, new UpdateLightMessage(this));
	}

	DirectionalLight::~DirectionalLight()
	{
		BROWSER_LOG("~DirectionalLight");
	}

    void DirectionalLight::onInspectorGUI(InspectorPanel* inspector)
    {
        //
        inspector->addPropertyText("Intensity: " + std::to_string(m_fIntensity));
    }
    
    void DirectionalLight::updateLight()
    {
		//BaseLight::updateLight();

		Transform* transform = m_oBelongEntity ? m_oBelongEntity->getComponent<Transform>() : nullptr;
		if (transform)
		{
			const glm::vec3& forward = transform->getForward();
			if (m_oLightDirection != forward)
			{
				recordDirection(forward);
			}
		}
    }
    
    void DirectionalLight::updateMaterialsLight(const std::unordered_map<unsigned int, Material*>& materials, unsigned int index/* = 0*/, bool forceUpdate/* = false*/)
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
        // 光照方向
        if (forceUpdate || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightDirection))
        {
			std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
			sprintf(&uniformName[0], SHADER_UNIFORM_DIRECTIONAL_DIRECTION, index);
			for (auto itor = materials.begin(); itor != materials.end(); ++itor)
			{
				material = itor->second;
				material->setUniformV3f(uniformName, m_oLightDirection);
			}
        }

		// 重置脏标记
		resetLightDirty();
    }

    bool DirectionalLight::isLightDirty()
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
    
    bool DirectionalLight::isLightSystemDirty()
    {
        return BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity);
    }
    
}
