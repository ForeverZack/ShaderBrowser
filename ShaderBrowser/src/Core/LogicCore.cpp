#include "LogicCore.h"
#include "Common/Tools/FileUtils.h"
#include "Common/Tools/BinaryFileUtils.h"
#include "Common/System/ECSManager.h"
#include "Common/System/AutoReleasePool.h"
#include "Browser/System/TransformSystem.h"
#include "Browser/System/MeshSystem.h"
#include "Browser/System/CameraSystem.h"
#include "Browser/System/BoundBoxSystem.h"
#include "Browser/System/LightSystem.h"
#include "Browser/System/AnimationSystem.h"


namespace core
{
	LogicCore::LogicCore()
	{

	}

	LogicCore::~LogicCore()
	{

	}

	void LogicCore::initLogic()
	{
		// 添加搜索路径
		FileUtils::getInstance()->addSearchPath(".");
		FileUtils::getInstance()->addSearchPath("res");

		// test flatbuffer
		//    BinaryFileUtils::getInstance()->serialize();
		//    BinaryFileUtils::getInstance()->deserialize();

		// 注册基本系统
		ECSManager::getInstance()->registerSystem(browser::TransformSystem::getInstance()); // Transform
		ECSManager::getInstance()->registerSystem(browser::MeshSystem::getInstance()); // Mesh
		ECSManager::getInstance()->registerSystem(browser::CameraSystem::getInstance());	// Camera
		ECSManager::getInstance()->registerSystem(browser::BoundBoxSystem::getInstance()); // 包围盒
		ECSManager::getInstance()->registerSystem(browser::LightSystem::getInstance()); // 灯光系统
		ECSManager::getInstance()->registerSystem(browser::AnimationSystem::getInstance()); // 动画系统
		// 初始化系统
		ECSManager::getInstance()->initSystem(SystemType::Transform);    // Transform
		ECSManager::getInstance()->initSystem(SystemType::MeshFilter);    // Mesh
		ECSManager::getInstance()->initSystem(SystemType::Camera);    // Camera
		ECSManager::getInstance()->initSystem(SystemType::BoundBox);    // 包围盒
		ECSManager::getInstance()->initSystem(SystemType::Light);   // Light
		ECSManager::getInstance()->initSystem(SystemType::Animation);   // Animation
	}

	void LogicCore::logicLoop(float deltaTime)
	{
		// beforeUpdate
		ECSManager::getInstance()->beforeUpdateSystem(SystemType::Transform, deltaTime); // 在所有系统刷新前刷新transform

		
		m_oLastUpdate = std::chrono::steady_clock::now();

		ECSManager::getInstance()->updateSystem(SystemType::Transform, deltaTime);  // 更新transform
		recTime("====SystemType::Transform=====");
		ECSManager::getInstance()->updateSystem(SystemType::Animation, deltaTime);   // 更新动画系统
		recTime("====SystemType::Animation=====");

		while (!ECSManager::getInstance()->isSystemFinish(SystemType::Transform));    // Transform系统必须更新完

		ECSManager::getInstance()->updateSystem(SystemType::Camera, deltaTime);  // 更新camera
		recTime("====SystemType::Camera=====");
		ECSManager::getInstance()->updateSystem(SystemType::BoundBox, deltaTime);   // 更新BoundBox
		recTime("====SystemType::BoundBox=====");

		// auto release
		AutoReleasePool::getInstance()->update();

	}

	void LogicCore::recTime(const std::string& log)
	{
		return;

		auto now = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - m_oLastUpdate).count() / 1000.0f;
		m_oLastUpdate = now;
		BROWSER_LOG(log + std::to_string(deltaTime) + "ms");
	}


}
