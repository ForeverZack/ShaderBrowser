#include "BaseComponent.h"
#include "Browser/Entitys/BaseEntity.h"

namespace browser
{
	BaseComponent::BaseComponent()
        : m_oTransform(nullptr)
        , m_eBelongSystem(SystemType::CustomSystem)
		, m_oBelongEntity(nullptr)
        , m_sComponentName("Unnamed Component")
	{
        this->autorelease();
	}
    
    BaseComponent::BaseComponent(const std::string& name)
        : m_oTransform(nullptr)
        , m_eBelongSystem(SystemType::CustomSystem)
        , m_oBelongEntity(nullptr)
        , m_sComponentName(name)
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
    
    void BaseComponent::dispatchEventToChildren(ComponentEvent event, BaseComponentMessage* msg)
    {
        if (m_oBelongEntity)
        {
            m_oBelongEntity->deliverComponentMessageToChildren(event, msg);
        }
    }

    
}
