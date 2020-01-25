#pragma once

#include <chrono>
#include <string>

namespace core
{
    class Application
    {
    public:
		Application();
        ~Application();
        
	public:
		// init
		void init();
		// run
		void run();

	private:
		void mainLoop();
		// 记录时间(测试系统间的刷新间隔)
		void recTime(const std::string& log);

	private:
		// 上一次更新的时间戳
		std::chrono::steady_clock::time_point _lastUpdate;

    };
}
