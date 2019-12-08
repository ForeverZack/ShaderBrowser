#pragma once

#include "GL/GLDefine.h"
#include "Common/Components/Reference.h"
#include "Common/Tools/Utils.h"
#include "Common/System/AutoReleasePool.h"

using namespace common;

namespace customGL
{
    /*
        关于GPU操作指令的简要说明：
        1. 首先是GPU操作指令存在的意义，它是在逻辑线程中创建并压入指令队列，用来实现逻辑线程向渲染线程的通讯（逻辑线程不可以直接做GPU操作）。它记录着GPU内容中发生改变的部分(如 创建,修改,删除等)；
        2. GPU操作指令的规范: 既然是用来通讯，那么就要保证消息的独立性(即，不会因为逻辑线程中的数据变动而影响到已创建添加的命令的数据)。所以命令中的数据一定是拷贝出来的！！！！！(特殊情况自行分析)
           一定要保证线程执行安全！！！！
        3. 按照现在的设计，逻辑线程只可以比渲染线程领先1帧。必须等渲染线程拿到全部的GPU操作指令，才可以执行下一帧的逻辑循环；
     */
    
    // GPU操作命令类型(也可以理解为操作对象类型)
    enum GPUOperateCommandType
    {
        // 未定义
        GOCT_Undefined = 0,
        // 2D纹理
        GOCT_Texture2D,
        // GLProgram
        GOCT_GLProgram,
        
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

