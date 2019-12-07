#pragma once

#include <vector>
#include "Common/Tools/BaseSingleton.h"
#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/Thread/BaseThread.h"

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
        
    protected:
        // 待执行命令队列 (逻辑线程)
        MutexQueue<BaseGPUOperateCommand*> m_qWaitCommands;
        // 命令队列 (渲染线程)
        std::vector<BaseGPUOperateCommand*> m_vCommands;

    };
    

}

