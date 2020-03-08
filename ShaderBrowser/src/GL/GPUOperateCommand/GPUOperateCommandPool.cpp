#include "GPUOperateCommandPool.h"

namespace customGL
{
    GPUOperateCommandPool::GPUOperateCommandPool()
    {
        m_mCommandsPool.clear();
    }
    
    GPUOperateCommandPool::~GPUOperateCommandPool()
    {
        
    }
    
    void GPUOperateCommandPool::pushCommand(BaseGPUOperateCommand* cmd)
    {
		std::unique_lock<std::mutex> lock(m_oMutex);

        GPUOperateCommandType type = cmd->getCommandType();
        if (m_mCommandsPool.find(type) == m_mCommandsPool.end())
        {
            std::list<BaseGPUOperateCommand*> pool;
            pool.clear();
            m_mCommandsPool.emplace(type, std::move(pool));
        }
        
        std::list<BaseGPUOperateCommand*>& pool = m_mCommandsPool[type];
        pool.push_back(cmd);
    }

}

