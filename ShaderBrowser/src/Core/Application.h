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
		// ��¼ʱ��(����ϵͳ���ˢ�¼��)
		void recTime(const std::string& log);

	private:
		// ��һ�θ��µ�ʱ���
		std::chrono::steady_clock::time_point _lastUpdate;

    };
}
