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
        // 等待渲染线程初始化完毕，可能实际上不需要等待，但会有一定的风险，为了是为了安全起见
        while(!m_bRenderInited);
        
		while (!RenderCore::getInstance()->shouldCloseWindow())
		{
			mainLoop();
		}
        // 结束glfw
        glfwTerminate();
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
        m_fDeltaTime = deltaTime;
        _lastUpdate = now;
        
        //timePoint = std::chrono::steady_clock::now();
		unsigned long finishedFrameCount = LogicCore::getInstance()->getFinishedFrameCount();
		while(finishedFrameCount >= RenderCore::getInstance()->getFrameIndex()+LOGIC_RENDER_CORE_FRAME_INTERVAL) ;
		//recTime("========logic wait=======" + std::to_string(LogicCore::getInstance()->getFrameIndex()) + "===");
        
		LogicCore::getInstance()->logicLoop(deltaTime);
        
        glfwPollEvents();   // 负责更新窗口和事件
        
		recTime(std::to_string(LogicCore::getInstance()->getFrameIndex()-1) + "=====Logic=======");

        
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
