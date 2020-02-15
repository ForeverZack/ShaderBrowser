#pragma once

#include <string>
#include <chrono>

#include "Common/Tools/BaseSingleton.h"
#include "Common/Tools/Thread/BaseThread.h"
#include "Common/Tools/Utils.h"

using namespace common;

namespace core
{
    // 逻辑核心线程(main thread)
    class LogicCore : public BaseSingleton<LogicCore>
    {
    public:
        // 逻辑线程当前状态
        enum LogicCoreState
        {
            // 没有检测到逻辑线程数据 或 逻辑线程未完成
            LCS_None = 0,
            // 逻辑线程完成
            LCS_Finish,
        };
        
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
        // 处理输入事件
        void processInput();
        // 设置当前帧状态
        void setCurFrameLogicState(LogicCoreState state);
        
    public:
        REGISTER_PROPERTY_GET(unsigned long, m_uFrameIndex, FrameIndex)
        // 设置逻辑状态
        void setLogicState(unsigned long frameIndex, LogicCoreState state);
        // 获取逻辑状态
        LogicCoreState getLogicState(unsigned long frameIndex);
        // 清除逻辑状态
        void eraseLogicState(unsigned long frameIndex);
        // 逻辑状态数量
        size_t getLogicStatesSize()
        {
            return m_mLogicStates.size();
        }
        
	private:
		// 记录时刻
		std::chrono::steady_clock::time_point m_oLastUpdate;
        // 逻辑线程当前状态
        MutexUnorderedMap<unsigned long, LogicCoreState> m_mLogicStates;
        // 当前帧
        unsigned long m_uFrameIndex;
        
    };
}
