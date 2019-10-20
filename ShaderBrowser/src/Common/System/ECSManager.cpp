#include "ECSManager.h"

namespace common
{
	ECSManager::ECSManager()
	{	

	}

	ECSManager::~ECSManager()
	{

	}

	void ECSManager::registerSystem(BaseSystem* system)
	{
		BROWSER_ASSERT(system, "ECSManager::registerSystem() : system doesn't exist!");

		m_bSortDirty = true;
		auto itor = m_mSystems.find(system->getSystemType());
		if (itor == m_mSystems.end())
		{
			m_mSystems.emplace(system->getSystemType(), system);
		}
		else
		{
			BROWSER_LOG("This type of system has already been registed!");
		}
	}

	BaseSystem* ECSManager::getSystem(SystemType type)
	{
		auto itor = m_mSystems.find(type);
		if (itor != m_mSystems.end())
		{
			return itor->second;
		}

		BROWSER_LOG("This type of system has not been registed!");

		return nullptr;
	}

	void ECSManager::initSystem(SystemType type)
	{
		BaseSystem* system = getSystem(type);
		if (system)
		{
			system->init();
		}
	}
    
    void ECSManager::beforeUpdateSystem(SystemType type, float deltaTime)
    {
        BaseSystem* system = getSystem(type);
        if (system)
        {
            system->beforeUpdate(deltaTime);
        }
    }

	void ECSManager::updateSystem(SystemType type, float deltaTime)
	{
		BaseSystem* system = getSystem(type);
		if (system)
		{
			system->update(deltaTime);
		}
	}
    
    bool ECSManager::isSystemFinish(SystemType type)
    {
        BaseSystem* system = getSystem(type);
        return system->getIsFinish();
    }

	bool ECSManager::addComponentToSystem(BaseEntity* entity, BaseComponent* component)
	{
		BaseSystem* system = getSystem(component->getBelongSystem());
		if (system)
		{
			return system->addComponent(entity, component);
		}
		BROWSER_LOG("ECSManager::addComponentToSystem() : fail to add component to system");
		return false;
	}

	bool ECSManager::removeEntityFromSystem(BaseEntity* entity, SystemType type)
	{
		BaseSystem* system = getSystem(type);
		if (system)
		{
			system->removeEntity(entity);
			return true;
		}
		return false;
	}

	bool ECSManager::removeComponentFromSystem(BaseEntity* entity, BaseComponent* component)
	{
		BaseSystem* system = getSystem(component->getBelongSystem());
		if (system)
		{
			return system->removeComponent(entity, component);
		}
		BROWSER_LOG("ECSManager::removeComponentFromSystem() : fail to remove component from system");
		return false;
	}

	void ECSManager::update(float deltaTime)
	{
	}


}
