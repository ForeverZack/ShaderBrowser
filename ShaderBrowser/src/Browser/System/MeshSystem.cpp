#include "MeshSystem.h"

namespace browser
{
	static MeshSystem* m_oInstance = nullptr;

	MeshSystem* MeshSystem::getInstance()
	{
		if (!m_oInstance)
		{
			m_oInstance = new MeshSystem();
		}
		return m_oInstance;
	}

	MeshSystem::MeshSystem()
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::Mesh;
		m_bComponentMutex = true;
	}

}
