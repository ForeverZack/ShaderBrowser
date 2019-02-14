#include "CameraSystem.h"
#include "Browser/Components/Camera/Camera.h"

namespace browser
{
	CameraSystem::CameraSystem()
        : m_oMainCamera(nullptr)
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::Camera;
		m_bComponentMutex = true;
	}

	void CameraSystem::update(float deltaTime)
	{
		Camera* camera = nullptr;
		for (auto itor = m_mComponentsList.cbegin(); itor != m_mComponentsList.cend(); ++itor)
		{
			const std::list<BaseComponent*>& cameraList = itor->second;
			camera = dynamic_cast<Camera*>(*(cameraList.begin()));
			camera->updateCamera(deltaTime);
		}
	}

}
