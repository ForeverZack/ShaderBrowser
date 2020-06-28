#include "CameraSystem.h"
#include "Browser/Components/Camera/Camera.h"

namespace browser
{
	CameraSystem::CameraSystem()
        : m_bDirty(true)
		, m_oMainCamera(nullptr)
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::Camera;
		m_bComponentMutex = true;
	}

	bool CameraSystem::addComponent(BaseEntity* entity, BaseComponent* component)
	{
		m_bDirty = true;
		return BaseSystem::addComponent(entity, component);
	}

	bool CameraSystem::removeComponent(BaseEntity* entity, BaseComponent* component)
	{
		m_bDirty = true;
		return BaseSystem::removeComponent(entity, component);
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

		// 更新激活的相机队列
		// 规则：相机的渲染纹理不为空的相机；渲染纹理为空的相机列表中Depth最大的相机
		if (m_bDirty)
		{
			m_bDirty = false;

			m_vActiveCameras.clear();
			Camera* mainCamera = nullptr;
			for (auto itor = m_mComponentsList.cbegin(); itor != m_mComponentsList.cend(); ++itor)
			{
				const std::list<BaseComponent*>& cameraList = itor->second;
				camera = dynamic_cast<Camera*>(*(cameraList.begin()));
				if (camera->getRenderTexture())
				{
					m_vActiveCameras.push_back(camera);
				}
				else if(camera->getIsRenderable() && (!mainCamera || camera->getDepth()>=mainCamera->getDepth()))
				{
					mainCamera = camera;
				}
			}
			if (mainCamera)
			{
				m_vActiveCameras.push_back(mainCamera);
			}
			m_oMainCamera = mainCamera;

			// 按depth对相机进行排序
			std::stable_sort(std::begin(m_vActiveCameras), std::end(m_vActiveCameras), [](Camera* c1, Camera* c2) {
				return c1->getDepth() < c2->getDepth();
			});
		}
	}

}
