#include "MeshSystem.h"

namespace browser
{
	MeshSystem::MeshSystem()
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::MeshFilter;
		m_bComponentMutex = true;
	}

}
