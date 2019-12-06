#pragma once

#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"


namespace customGL
{
    class Texture2D;
    
    // 2D纹理操作命令
	class GPUOperateTexture2DCommand : public BaseGPUOperateCommand
	{
	public:
		GPUOperateTexture2DCommand();
        ~GPUOperateTexture2DCommand();

    public:
        // 准备执行 (逻辑线程调用)
        virtual void ready(GPUOperateType operateType);
        // 执行 (渲染线程调用)
        virtual void execute();
        // 结束执行 (渲染线程调用)
        virtual void finish();
        
    protected:
        // 创建纹理
        void createTexture2D();
        
        // 更新纹理
        void updateTexture2D();
        
        // 删除纹理
        void deleteTexture2D();
        
	protected:
        // 纹理对象
        Texture2D* m_pTexture2D;
	};

}

