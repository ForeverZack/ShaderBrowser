#include "Application.h"
#include "Core/LogicCore.h"
#include "Core/RenderCore.h"

namespace core
{
    Application* Application::CurrentApplication = nullptr;
    
	Application::Application()
        : m_bRenderInited(false)
        , m_fDeltaTime(0)
	{
        Application::CurrentApplication = this;
        
        // 渲染线程
        m_pRenderThread = new common::BaseThread();
        
		init();
	}
    
	Application::~Application()
	{
	}

	void Application::init()
	{
    	// 初始化
		LogicCore::getInstance()->initLogic();
        
        // 创建窗口
        RenderCore::getInstance()->createWindow();
        
        // 渲染线程run
        m_pRenderThread->addTask(std::bind(&Application::renderLoop, this));
    }
    
    void Application::initRender()
    {
        // 初始化
        RenderCore::getInstance()->initRender();
        
        m_bRenderInited = true;
    }

	void Application::run()
	{
		while (m_bRenderInited && !RenderCore::getInstance()->shouldCloseWindow())
		{
			mainLoop();
		}
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
        // logic wait (等待GPU操作命令队列拷贝完成,并开始渲染工作)
        while(LogicCore::getInstance()->getLogicState() != LogicCore::LogicCoreState::LCS_Prepare);
        
		// 计算deltaTime
		auto now = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastUpdate).count() / 1000000.0f;
		deltaTime = deltaTime < 0 ? 0 : deltaTime;
        m_fDeltaTime = deltaTime;
        _lastUpdate = now;

		// 2.render
		recTime("====start=====");
		LogicCore::getInstance()->logicLoop(deltaTime);
        // set logic mutex
        LogicCore::getInstance()->setLogicState(LogicCore::LogicCoreState::LCS_Finish);
        recTime("=====Logic=======");
		
        // wait render (在开始下一个循环前，必须确保上一帧渲染结束)
        while(RenderCore::getInstance()->getRenderState() != RenderCore::RenderCoreState::RCS_End);
        glfwPollEvents();   // 负责更新窗口和事件
        
//		RenderCore::getInstance()->renderLoop(deltaTime);
//		recTime("=====Render=======");
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
    
    void Application::renderLoop()
    {
        initRender();
        
        while(!RenderCore::getInstance()->shouldCloseWindow())
        {
            RenderCore::getInstance()->renderLoop();
        }

        RenderCore::getInstance()->destoryWindow();
    }
	

}
