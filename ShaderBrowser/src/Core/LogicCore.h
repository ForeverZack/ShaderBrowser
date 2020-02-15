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
            // None
            LCS_None = 0,
            // 逻辑线程准备
            LCS_Prepare,
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
        LogicCoreState getLogicState()
        {
            return m_eLogicState.getValue();
        }
        void setLogicState(LogicCoreState state)
        {
            m_eLogicState = state;
        }
        REGISTER_PROPERTY_GET(unsigned long, m_uFrameIndex, FrameIndex)
        // 设置逻辑状态
        void setLogicState(unsigned long frameIndex, LogicCoreState state);
        // 获取逻辑状态
        LogicCoreState getLogicState(unsigned long frameIndex);
        // 清除逻辑状态
        void eraseLogicState(unsigned long frameIndex);
        
	private:
		// 记录时刻
		std::chrono::steady_clock::time_point m_oLastUpdate;
        // 逻辑线程当前状态
        MutexVariable<LogicCoreState> m_eLogicState;
        MutexUnorderedMap<unsigned long, LogicCoreState> m_mLogicStates;
        // 当前帧
        unsigned long m_uFrameIndex;
        
    };
}
