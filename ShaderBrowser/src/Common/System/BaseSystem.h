#ifndef COMMON_SYS_BASESYSTEM_H
#define COMMON_SYS_BASESYSTEM_H

#include <unordered_map>
#include <list>
#include "Common/Tools/Utils.h"
#include "Browser/Entitys/BaseEntity.h"
#include "Browser/Components/BaseComponent.h"
#include "Common/CommonDefine.h"

using namespace browser;

namespace common
{
	// 管理组件
	class BaseSystem
	{
	public:
		BaseSystem() 
		{
			m_iPriority = 0;
			m_bComponentMutex = false;
			m_bSortDirty = false;
			m_eSystemType = SystemType::CustomSystem;
		}
		~BaseSystem() {}

	public:
		// 添加组件
		virtual bool addComponent(BaseEntity* entity, BaseComponent* component)
		{
			auto iterator = m_mComponentsList.find(entity);
			if (iterator == m_mComponentsList.end())
			{
				std::list<BaseComponent*> list;
				list.push_back(component);
				m_mComponentsList.emplace(entity, std::move(list));
			}
			else
			{
				auto list = iterator->second;
				if (m_bComponentMutex && m_mComponentsList.size()>0)
				{
					BROWSER_LOG("Entity already has the same type component, it must be the only!");
					return false;
				}
				list.push_back(component);

				m_bSortDirty = true;
			}
            
            // 设置组件属于哪个Entity
            component->setBelongEntity(entity);

			return true;
		}
		// 移除组件
		virtual bool removeComponent(BaseEntity* entity, BaseComponent* component)
		{
			auto iterator = m_mComponentsList.find(entity);
			if (iterator == m_mComponentsList.end())
			{
				BROWSER_LOG("Entity don't have the component!");
				return false;
			}
			else
			{
				auto list = iterator->second;
				list.remove(component);

				m_bSortDirty = true;
			}
			return true;
		}
		// 移除组件
		virtual bool removeEntity(BaseEntity* entity)
		{
			auto itor = m_mComponentsList.find(entity);
			if (itor != m_mComponentsList.end())
			{
				m_mComponentsList.erase(itor);
				return true;
			}
			return false;
		}
		// 初始化
		virtual void init() {}
		// 在所有系统刷新前（用来重置一些标记等）
        virtual void beforeUpdate(float deltaTime) {}
        // 每帧刷新
		virtual void update(float deltaTime) {}

		REGISTER_PROPERTY_GET_SET(int, m_iPriority, Priority)
		REGISTER_PROPERTY_GET_SET(bool, m_bComponentMutex, ComponentMutex)
		REGISTER_PROPERTY_GET(SystemType, m_eSystemType, SystemType)
        const std::unordered_map<BaseEntity*, std::list<BaseComponent*>>& getComponentsList() const
        {
            return m_mComponentsList;
        }

	protected:
		// 实体-组件 map（这里组件用list主要是为了custom自定义的组件类型，使他们可以都放在CustomSystem中，在互斥的组件系统中，list中只允许有一个对象）
		std::unordered_map<BaseEntity*, std::list<BaseComponent*>> m_mComponentsList;
		// 是否和其他相同组件互斥
		bool m_bComponentMutex;
		// 排序脏标记
		bool m_bSortDirty;

		// 系统优先级(数值越小，优先级越高，会在前面执行刷新)
		int m_iPriority;
		// 系统类型
		SystemType m_eSystemType;

	};
}

#endif
