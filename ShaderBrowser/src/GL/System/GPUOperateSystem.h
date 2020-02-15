#pragma once

#include <vector>
#include "Common/Tools/BaseSingleton.h"
#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/Thread/BaseThread.h"
#include "Core/LogicCore.h"

using namespace common;

namespace customGL
{
	// GPU操作系统
	class GPUOperateSystem : public BaseSingleton<GPUOperateSystem>
	{
	public:
		GPUOperateSystem();
        ~GPUOperateSystem();

    public:
        // 添加命令 (逻辑线程)
        void addCommand(BaseGPUOperateCommand* cmd);
        
        // 刷新 （渲染线程）
        void update();
        
    private:
        // 添加命令
        void addCommand(unsigned long frameIndex, BaseGPUOperateCommand* cmd);
        // 获取命令队列
        const std::vector<BaseGPUOperateCommand*>& getCommands(unsigned long frameIndex);
        // 清除队列
        void eraseCommands(unsigned long frameIndex);
        
    protected:
        // 待执行命令队列 (逻辑线程)
        MutexUnorderedMap<unsigned long, std::vector<BaseGPUOperateCommand*>> m_mWaitCommands;
        // 命令队列 (渲染线程)
        std::vector<BaseGPUOperateCommand*> m_vCommands;

    };
    

}

