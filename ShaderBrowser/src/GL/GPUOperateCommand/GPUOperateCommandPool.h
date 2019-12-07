#pragma once

#include <unordered_map>
#include <list>
#include "Common/Tools/BaseSingleton.h"
#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"
#include "Common/Tools/Utils.h"

using namespace common;

namespace customGL
{
	// GPU操作命令池
	class GPUOperateCommandPool : public BaseSingleton<GPUOperateCommandPool>
	{
	public:
		GPUOperateCommandPool();
        ~GPUOperateCommandPool();

    public:
        // 获取空闲命令
        template <typename T>
        T* popCommand(GPUOperateCommandType type)
        {
            if (m_mCommandsPool.find(type) == m_mCommandsPool.end())
            {
                std::list<BaseGPUOperateCommand*> pool;
                pool.clear();
                m_mCommandsPool.emplace(type, std::move(pool));
            }
            
            std::list<BaseGPUOperateCommand*>& pool = m_mCommandsPool[type];
            T* cmd = nullptr;
            if (pool.size() > 0)
            {
                cmd = static_cast<T*>(*(pool.begin()));
                pool.pop_front();
            }
            else
            {
                cmd = new T();
            }
                
            return cmd;
        }
        // 回收空闲命令
        void pushCommand(BaseGPUOperateCommand* cmd);
        
    protected:
        // 命令池
        std::unordered_map<GPUOperateCommandType, std::list<BaseGPUOperateCommand*>> m_mCommandsPool;
    };
    

}

