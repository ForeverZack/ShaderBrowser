#include "BaseComponent.h"
#include "../Entitys/BaseEntity.h"

namespace browser
{
	BaseComponent::BaseComponent()
		: m_eBelongSystem(SystemType::CustomSystem)
		, m_oBelongEntity(nullptr)
	{
        this->autorelease();
	}

	BaseComponent::~BaseComponent()
	{

	}
    
    void BaseComponent::dispatchEvent(ComponentEvent event, BaseComponentMessage* msg)
    {
        if (m_oBelongEntity)
        {
            m_oBelongEntity->deliverComponentMessage(event, msg);
        }
    }

    
}
