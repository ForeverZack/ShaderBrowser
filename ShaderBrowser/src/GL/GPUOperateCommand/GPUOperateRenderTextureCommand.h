#pragma once

#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"


namespace customGL
{
	class RenderTexture;
	class RenderTextureAttachment;
    
    // 纹理缓存操作命令
	class GPUOperateRenderTextureCommand : public BaseGPUOperateCommand
	{
	public:
		GPUOperateRenderTextureCommand();
        ~GPUOperateRenderTextureCommand();
        
    public:
        // 准备执行 (逻辑线程调用)
        virtual void ready(GPUOperateType operateType);
        // 执行 (渲染线程调用)
        virtual void execute();
        // 结束执行 (渲染线程调用)
        virtual void finish();

    protected:
        // 创建渲染纹理
        void createRenderTexture();
        // 更新渲染纹理数据
        void updateRenderTexture();
        // 删除渲染纹理
        void deleteRenderTexture();
    
		REGISTER_PROPERTY_SET(RenderTexture*, m_pRenderTexture, RenderTexture)
		void setAttachments(std::unordered_map<GLuint, RenderTextureAttachment>* attachments)
		{
			m_mAttachments = attachments;
		}
        
	protected:
        // 渲染纹理对象
		RenderTexture* m_pRenderTexture;
		// 帧缓冲附件
		std::unordered_map<GLuint, RenderTextureAttachment>* m_mAttachments;
	};

}

