#include "BaseComponent.h"

namespace browser
{
	BaseComponent::BaseComponent()
		: m_eBelongSystem(SystemType::CustomSystem)
		, m_oBelongEntity(nullptr)
	{

	}

	BaseComponent::~BaseComponent()
	{

	}

}