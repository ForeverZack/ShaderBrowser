#include "MeshFilterSystem.h"

namespace browser
{
	MeshFilterSystem::MeshFilterSystem()
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::MeshFilter;
		m_bComponentMutex = true;
	}

}
