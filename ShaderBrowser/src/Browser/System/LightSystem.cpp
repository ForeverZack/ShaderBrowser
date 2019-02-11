#include "LightSystem.h"

namespace browser
{
	LightSystem::LightSystem()
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::Light;
		m_bComponentMutex = true;
	}

}
