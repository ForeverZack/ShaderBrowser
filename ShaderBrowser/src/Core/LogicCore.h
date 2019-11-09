#pragma once

#include <string>
#include <chrono>

#include "Common/Tools/BaseSingleton.h"

using namespace common;

namespace core
{
    // 逻辑核心线程(main thread)
    class LogicCore : public BaseSingleton<LogicCore>
    {
    public:
		LogicCore();
        ~LogicCore();
        
    public:
		// 初始化
		void initLogic();
        // 循环
        void logicLoop(float deltaTime);

	private:
		// 记录时间(测试系统间的刷新间隔)
		void recTime(const std::string& log);
        
	private:
		// 记录时刻
		std::chrono::steady_clock::time_point m_oLastUpdate;

    };
}
