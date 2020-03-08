#include "DirectionalLight.h"
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
		BaseEntity* entity = BaseEntity::create(name);
		parent->addChild(entity);
		// 平行光组件
		DirectionalLight* light = new DirectionalLight();
		entity->addComponent(light);

		return light;
	}

	DirectionalLight::DirectionalLight()
    {
        m_sComponentName = "Directional Light";
        // 设置光源类型
        m_eType = LightType::Directional;
        // 设置光源颜色
        m_oColor = glm::vec4(1, 0.9569f, 0.8392f, 1);
        
		// 设置脏标记
		m_uPropertiesDirty = LPT_Color | LPT_Intensity | LPT_Direction | LPT_LightMatrix;
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

			if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightMatrix))
			{
				// 更新光源矩阵
				m_oLightMatrix = glm::inverse(transform->getModelMatrix());
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
        if (forceUpdate || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Direction))
        {
			std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
			sprintf(&uniformName[0], SHADER_UNIFORM_DIRECTIONAL_DIRECTION, index);
			for (auto itor = materials.begin(); itor != materials.end(); ++itor)
			{
				material = itor->second;
				material->setUniformV3f(uniformName, m_oLightDirection);
			}
        }
		// 光源矩阵
		if (forceUpdate || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightMatrix))
		{
			std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
			sprintf(&uniformName[0], SHADER_UNIFORM_DIRECTIONAL_LIGHTMATRIX, index);
			for (auto itor = materials.begin(); itor != materials.end(); ++itor)
			{
				material = itor->second;
				material->setUniformMat4(uniformName, m_oLightMatrix);
			}
		}

		// 重置脏标记
		resetLightDirty();
    }

    bool DirectionalLight::isLightDirty()
    {
        if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Color)
            || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity)
            || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Direction)
            || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightMatrix))
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
		// 对于平行光来说，光源系统只关心它的强度
        return BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity);
    }
    
}
