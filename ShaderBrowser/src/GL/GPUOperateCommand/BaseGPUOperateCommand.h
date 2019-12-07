#pragma once

#include "GL/GLDefine.h"
#include "Common/Components/Reference.h"
#include "Common/Tools/Utils.h"
#include "Common/System/AutoReleasePool.h"

using namespace common;

namespace customGL
{
    // GPU操作命令类型(也可以理解为操作对象类型)
    enum GPUOperateCommandType
    {
        // 未定义
        GOCT_Undefined = 0,
        // 2D纹理
        GOCT_Texture2D,
        
    };
    
    // GPU操作
    enum GPUOperateType
    {
        // 刷新数据 (GPUResource状态为Loaded，且dirty为true)
        GOT_Update = 0,
        // 创建 (GPUResource状态为DataLoaded)
        GOT_Create,
        // 删除
        GOT_Delete,
        
        // 扩展
        GOT_UpdateProperties_1,
        GOT_UpdateProperties_2,
    };
    
    
	// GPU操作命令
	class BaseGPUOperateCommand : public Reference
	{
	public:
		BaseGPUOperateCommand();
		virtual ~BaseGPUOperateCommand() {}

    public:
        // 准备执行 (逻辑线程调用)
        virtual void ready(GPUOperateType operateType);
        // 执行 (渲染线程调用)
        virtual void execute() {};
        // 结束执行 (渲染线程调用)
        virtual void finish();
        
    protected:
        // 公共方法。。。

        REGISTER_PROPERTY_GET_SET(GPUOperateCommandType, m_eCommandType, CommandType)
        REGISTER_PROPERTY_GET_SET(GPUOperateType, m_eOperateType, OperateType)
        REGISTER_PROPERTY_GET(bool, m_bIsReady, IsReady)
        
	protected:
        // 命令类型
        GPUOperateCommandType m_eCommandType;
        // 操作类型
        GPUOperateType m_eOperateType;
        // 是否Ready
        bool m_bIsReady;
    };


}

