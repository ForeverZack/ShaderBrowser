#pragma once

#include <chrono>
#include <string>
#include "Common/Tools/Utils.h"
#include "Common/Tools/Thread/BaseThread.h"

using namespace common;

namespace core
{    
    class Application
    {
     // 定义逻辑线程最大领先渲染线程几帧
     #define LOGIC_RENDER_CORE_FRAME_INTERVAL 3
    
    public:
        // 方便访问当前应用指针
        static Application* CurrentApplication;
    
    public:
		Application();
        ~Application();
        
	public:
		// init
		void init();
        void initRender();
        // run
		void run();

	private:
		void mainLoop();
        void renderLoop();
		// 记录时间(测试系统间的刷新间隔)
		void recTime(const std::string& log);

        
    public:
        float getDeltaTime()
        {
            return m_fDeltaTime.getValue();
        }
        
	private:
		// 上一次更新的时间戳
		std::chrono::steady_clock::time_point _lastUpdate;
        
        // 渲染线程
        BaseThread* m_pRenderThread;
        // 渲染线程是否初始化
        bool m_bRenderInited;
        // deltaTime
        MutexVariable<float> m_fDeltaTime;
        
    };
}
