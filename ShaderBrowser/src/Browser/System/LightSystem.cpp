#include "LightSystem.h"
#include "GL/GPUResource/Shader/GLProgram.h"

namespace browser
{
	LightSystem::LightSystem()
        : m_uDirectionalDirty(0)
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::Light;
		m_bComponentMutex = true;
	}
    
    bool LightSystem::addComponent(BaseEntity* entity, BaseComponent* component)
    {
        bool result = BaseSystem::addComponent(entity, component);
        
        BaseLight* light = static_cast<BaseLight*>(component);
        switch (light->getLightType())
        {
            case BaseLight::LightType::Directional:
                {
                    // 平行光
                    m_vDirectionalLights.push_back(light);
                    BROWSER_SET_BIT(m_uDirectionalDirty, LightChangeType::LCT_NewLight);
                }
                break;
        }
        
        return result;
    }
    
    bool LightSystem::removeComponent(BaseEntity* entity, BaseComponent* component)
    {
        BaseLight* light = static_cast<BaseLight*>(component);
        switch (light->getLightType())
        {
            case BaseLight::LightType::Directional:
                {
                    // 平行光
                    for (auto itor=m_vDirectionalLights.begin(); itor!=m_vDirectionalLights.end(); ++itor)
                    {
                        if ((*itor) == light)
                        {
                            m_vDirectionalLights.erase(itor);
                            BROWSER_SET_BIT(m_uDirectionalDirty, LightChangeType::LCT_DeleteLight);
                            break;
                        }
                    }
                }
                break;
        }
        
        return BaseSystem::removeComponent(entity, component);
    }
    
    void LightSystem::update(float deltaTime)
    {
        // 平行光
        if(m_uDirectionalDirty > 0)
        {
            // 平行光的重要性根据它的强度(Intensity)来重新排序
            std::stable_sort(std::begin(m_vDirectionalLights), std::end(m_vDirectionalLights), [](BaseLight* light1, BaseLight* light2) {
                return light1->getIntensity() > light2->getIntensity();
            });
        }
        // TODO: 
    }
    
    void LightSystem::afterUpdate(float deltaTime)
    {
        // 重置脏标记
        m_uDirectionalDirty = 0;
    }
    
    void LightSystem::updateMaterialLights(std::unordered_map<std::string, UniformValue>& uniforms)
    {
        // 平行光
        if (m_uDirectionalDirty > 0)
        {
            // 数量发生变化
            if (BROWSER_GET_BIT(m_uDirectionalDirty, LightChangeType::LCT_NewLight) || BROWSER_GET_BIT(m_uDirectionalDirty, LightChangeType::LCT_DeleteLight))
            {
                const char* uniformName = GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_BONES_MATRIX];
                auto itor = uniforms.find(uniformName);
                if (itor == uniforms.end())
                {
                    UniformValue uniformValue;
                    uniformValue.setInt(m_vDirectionalLights.size());
                    uniforms.emplace(uniformName, std::move(uniformValue));
                }
                else
                {
                    UniformValue& uniformValue = itor->second;
                    uniformValue.setInt(m_vDirectionalLights.size());
                }
            }
        }
    }
    
    void LightSystem::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
    {
        switch(event)
        {
        case ComponentEvent::Light_UpdateLight:
            {
                // 光源属性更新
                UpdateLightMessage* convertMsg = static_cast<UpdateLightMessage*>(msg);
                BaseLight* light = convertMsg->getLight();
                switch(light->getLightType())
                {
                    case BaseLight::LightType::Directional:
                        {
                            if (light->isLightSystemDirty())
                            {
                                BROWSER_SET_BIT(m_uDirectionalDirty, LightChangeType::LCT_Intensity);
                            }
                            BROWSER_SET_BIT(m_uDirectionalDirty, LightChangeType::LCT_Directional);
                        }
                        break;
                }
                
            }
            break;
        }
    }

}
