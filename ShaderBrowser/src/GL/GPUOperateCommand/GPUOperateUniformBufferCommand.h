#pragma once

#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"


namespace customGL
{
    class UniformBuffer;
    
    // Uniform Buffer Object操作命令
	class GPUOperateUniformBufferCommand : public BaseGPUOperateCommand
	{
	public:
		GPUOperateUniformBufferCommand();
        ~GPUOperateUniformBufferCommand();
        
    public:
        // 准备执行 (逻辑线程调用)
        virtual void ready(GPUOperateType operateType);
        // 执行 (渲染线程调用)
        virtual void execute();
        // 结束执行 (渲染线程调用)
        virtual void finish();

    protected:
        // 创建缓存
        void createBuffer();
        // 更新缓存
        void updateBuffer();
        // 删除缓存
        void deleteBuffer();
    
		REGISTER_PROPERTY_SET(UniformBuffer*, m_pBuffer, UniformBuffer)
        void setData(BufferData& data);
		void setData(const size_t size, const vector<string>& varnames, unordered_map<string, BufferData>& variables);
        
	protected:
        // 缓存对象
		UniformBuffer* m_pBuffer;
        
        // 缓存数据
		void* m_pData;
		size_t m_uSize;
	};

}

