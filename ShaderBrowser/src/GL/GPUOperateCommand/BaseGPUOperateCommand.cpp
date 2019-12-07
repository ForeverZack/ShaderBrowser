#include "BaseGPUOperateCommand.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"

namespace customGL
{
    BaseGPUOperateCommand::BaseGPUOperateCommand()
        : m_eCommandType(GOCT_Undefined)
        , m_eOperateType(GOT_Update)
        , m_bIsReady(false)
    {
    }
    
    void BaseGPUOperateCommand::ready(GPUOperateType operateType)
    {
        m_bIsReady = true;
        m_eOperateType = operateType;
    }
    
    void BaseGPUOperateCommand::finish()
    {
        m_bIsReady = false;
        // 回收命令
        GPUOperateCommandPool::getInstance()->pushCommand(this);
    }

}

