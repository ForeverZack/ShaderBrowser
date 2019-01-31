#pragma once

#include <vector>
#include "../../Common/System/BaseSystem.h"
#include "../../Common/Tools/BaseSingleton.h"
#include "../Components/Camera/Camera.h"

using namespace common;

namespace browser
{
	class CameraSystem : public common::BaseSystem, public BaseSingleton<CameraSystem>
	{
	public:
		CameraSystem();
		~CameraSystem() {};

	public:
		// 每帧刷新
		void update(float deltaTime);


		REGISTER_PROPERTY_GET_SET(Camera*, m_oMainCamera, MainCamera)

	private:
		// 主摄像机
		Camera* m_oMainCamera;
	};
}
