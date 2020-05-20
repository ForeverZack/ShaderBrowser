#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"
#include "Browser/Components/Camera/Camera.h"

using namespace common;

namespace browser
{
	class CameraSystem : public common::BaseSystem, public BaseSingleton<CameraSystem>
	{
	public:
		CameraSystem();
		~CameraSystem() {};

	public:
		// 添加组件
		virtual bool addComponent(BaseEntity* entity, BaseComponent* component);
		// 移除组件
		virtual bool removeComponent(BaseEntity* entity, BaseComponent* component);
		// 每帧刷新
		void update(float deltaTime);


		REGISTER_PROPERTY_GET_SET(bool, m_bDirty, Dirty)
		REGISTER_PROPERTY_GET(Camera*, m_oMainCamera, MainCamera)
		REGISTER_PROPERTY_CONSTREF_GET(std::vector<Camera*>, m_vActiveCameras, ActiveCameras)

	private:
		// 相机渲染顺序发生变动
		bool m_bDirty;
		// 当前帧激活相机队列
		// 规则：相机的渲染纹理不为空的相机；渲染纹理为空的相机列表中Depth最大的相机
		std::vector<Camera*> m_vActiveCameras;

		// 主摄像机
		Camera* m_oMainCamera;
	};
}
