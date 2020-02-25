#include "LightSystem.h"
#include "GL/GPUResource/Shader/GLProgram.h"
#include "Common/System/Cache/MaterialCache.h"

namespace browser
{
	LightSystem::LightSystem()
        : m_uDirectionalDirty(0)
		, m_uPointDirty(0)
		, m_uSpotDirty(0)
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

			case BaseLight::LightType::Point:
				{
					// 点光源
					m_vPointLights.push_back(light);
					BROWSER_SET_BIT(m_uPointDirty, LightChangeType::LCT_NewLight);
				}
				break;

			case BaseLight::LightType::Spot:
				{
					// 聚光灯
					m_vSpotLights.push_back(light);
					BROWSER_SET_BIT(m_uSpotDirty, LightChangeType::LCT_NewLight);
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

			case BaseLight::LightType::Point:
				{
					// 点光源
					for (auto itor = m_vPointLights.begin(); itor != m_vPointLights.end(); ++itor)
					{
						if ((*itor) == light)
						{
							m_vPointLights.erase(itor);
							BROWSER_SET_BIT(m_uPointDirty, LightChangeType::LCT_DeleteLight);
							break;
						}
					}
				}
				break;

			case BaseLight::LightType::Spot:
				{
					// 聚光灯
					for (auto itor = m_vSpotLights.begin(); itor != m_vSpotLights.end(); ++itor)
					{
						if ((*itor) == light)
						{
							m_vSpotLights.erase(itor);
							BROWSER_SET_BIT(m_uSpotDirty, LightChangeType::LCT_DeleteLight);
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

			// 数量发生变化
			if (BROWSER_GET_BIT(m_uDirectionalDirty, LightChangeType::LCT_NewLight) || BROWSER_GET_BIT(m_uDirectionalDirty, LightChangeType::LCT_DeleteLight))
			{
				// 处理已有材质
				MaterialCache::getInstance()->operateAllMaterials([&](Material* material)  -> void
				{
					material->setUniformInt(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_DIRECTIONAL_LIGHT_NUM], m_vDirectionalLights.size());
				});
			}
			// 数据发生变化
			BaseLight* directional_light = nullptr;
			for (int i = 0; i < m_vDirectionalLights.size(); ++i)
			{
				directional_light = m_vDirectionalLights[i];
				if (directional_light->isLightDirty())
				{
					directional_light->updateAllMaterialsLight(i);
				}
			}
			// 重置脏标记
			m_uDirectionalDirty = 0;
        }
		// 处理新增材质
		{
			// 数量发生变化
			{
				// 处理已有材质
				MaterialCache::getInstance()->operateAllNewMaterials([&](Material* material)  -> void
				{
					material->setUniformInt(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_DIRECTIONAL_LIGHT_NUM], m_vDirectionalLights.size());
				});
			}
			// 数据发生变化
			BaseLight* directional_light = nullptr;
			for (int i = 0; i < m_vDirectionalLights.size(); ++i)
			{
				directional_light = m_vDirectionalLights[i];
				directional_light->updateAllNewMaterialsLight(i);
			}
		}

        // TODO: 点光源和聚光灯
		// 重置脏标记
		m_uPointDirty = 0;
		m_uSpotDirty = 0;
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

					case BaseLight::LightType::Point:
						{

						}
						break;

					case BaseLight::LightType::Spot:
						{

						}
						break;
                }
                
            }
            break;
        }
    }

}
