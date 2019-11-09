#include "Application.h"
#include "Core/LogicCore.h"
#include "Core/RenderCore.h"

namespace core
{
	Application::Application()
	{
		init();
	}
    
	Application::~Application()
	{

	}

	void Application::init()
	{
		// 创建窗口
		RenderCore::getInstance()->createWindow();

		// 初始化
		LogicCore::getInstance()->initLogic();
		RenderCore::getInstance()->initRender();
	}

	void Application::run()
	{
		while (!RenderCore::getInstance()->shouldCloseWindow())
		{
			mainLoop();
		}

		RenderCore::getInstance()->destoryWindow();
	}

	auto timePoint = std::chrono::steady_clock::now();
	void Application::recTime(const std::string& log)
	{
		return;

		auto now = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - timePoint).count() / 1000.0f;
		timePoint = now;
		BROWSER_LOG(log + std::to_string(deltaTime) + "ms");
	}

	void Application::mainLoop()
	{
		// 计算deltaTime
		auto now = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastUpdate).count() / 1000000.0f;
		deltaTime = deltaTime < 0 ? 0 : deltaTime;
		_lastUpdate = now;

		// 2.render
		recTime("====start=====");
		LogicCore::getInstance()->logicLoop(deltaTime);
		recTime("=====Logic=======");
		RenderCore::getInstance()->renderLoop(deltaTime);
		recTime("=====Render=======");
		//    BROWSER_LOG(deltaTime);


		// 锁帧
		//    float fps = ;
		//    if (deltaTime < 1.0f/60.0f)
		//    {
		//        sleep((1.0f/60.0f - deltaTime));
		////        deltaTime = 1.0f/60.0f;
		//        fps = 60;
		//    }
	}
	

}
