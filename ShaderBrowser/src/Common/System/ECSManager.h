#ifndef COMMON_SYS_ECSMANAGER_H
#define COMMON_SYS_ECSMANAGER_H

#include <vector>
#include <unordered_map>
#include <algorithm> 
#include "BaseSystem.h"
#include "../Tools/BaseSingleton.h"

namespace common
{
	class SystemQueue
	{
	public:
		static SystemQueue* create()
		{
			return new SystemQueue();
		}

		SystemQueue() {}
		~SystemQueue() {}
	public:
		void addSystem(BaseSystem* system)
		{
			m_vSystems.push_back(system);
		}
		void removeSystem(BaseSystem* system)
		{
			std::vector<BaseSystem*>::iterator iter = find(m_vSystems.begin(), m_vSystems.end(), system);
			m_vSystems.erase(iter);
		}
		void sort()
		{
			// 对BaseSystem队列进行排序
			std::sort(m_vSystems.begin(), m_vSystems.end(), [=](BaseSystem* a, BaseSystem* b)
			{
				return a->getPriority() < b->getPriority();
			});
		}

		REGISTER_PROPERTY_CONSTREF_GET(std::vector<BaseSystem*>, m_vSystems, Systems)
	private:
		// BaseSystem队列
		std::vector<BaseSystem*> m_vSystems;
	};

	class ECSManager : public BaseSingleton<ECSManager>
	{
	public:
		ECSManager();
		~ECSManager();

	public:
		// 注册system
		void registerSystem(BaseSystem* system);

		// 获取某个类型的系统
		BaseSystem* getSystem(SystemType type);

		// 根据类型初始化系统
		void initSystem(SystemType type);

		// 根据类型更新系统
		void updateSystem(SystemType type, float deltaTime);

		// 将组件添加进对应的系统
		bool addComponentToSystem(BaseEntity* entity, BaseComponent* component);

		// 从对应的系统中移除组件
		bool removeComponentFromSystem(BaseEntity* entity, BaseComponent* component);

		// 刷新
		void update(float deltaTime);

	private:
		// 系统队列
		std::unordered_map<SystemType, BaseSystem*> m_mSystems;
		// 排序脏标记（当有新的系统加入的时候，则需要排序）
		bool m_bSortDirty;
	};
}

#endif
