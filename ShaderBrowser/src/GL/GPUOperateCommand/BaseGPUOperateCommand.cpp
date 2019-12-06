#include "BaseGPUOperateCommand.h"

namespace customGL
{
	BaseGPUOperateCommand::BaseGPUOperateCommand()
        : m_eCommandType(GOCT_Undefined)
        , m_eOperateType(GOT_Update)
    {
		this->autorelease();
	}
    
    void BaseGPUOperateCommand::ready(GPUOperateType operateType)
    {
        m_eOperateType = operateType;
    }
    
    
}
