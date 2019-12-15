#pragma once

#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"


namespace customGL
{
    class TextureBuffer;
    
    // 纹理缓存操作命令
	class GPUOperateTextureBufferCommand : public BaseGPUOperateCommand
	{
	public:
		GPUOperateTextureBufferCommand();
        ~GPUOperateTextureBufferCommand();
        
    public:
        // 准备执行 (逻辑线程调用)
        virtual void ready(GPUOperateType operateType);
        // 执行 (渲染线程调用)
        virtual void execute();
        // 结束执行 (渲染线程调用)
        virtual void finish();

    protected:
        // 创建纹理
        void createTextureBuffer();
        // 更新纹理数据
        void updateTextureBuffer();
        // 删除纹理
        void deleteTextureBuffer();
    
		REGISTER_PROPERTY_SET(TextureBuffer*, m_pTexture, TextureBuffer)
        REGISTER_PROPERTY_SET(GLenum, m_eInternalFormat, InternalFormat)
        void setData(const std::vector<glm::mat4>& data);
        void setData(const std::vector<float>& data);
        
	protected:
        // 纹理缓存对象
		TextureBuffer* m_pTexture;
        
        // 纹理数据
        union U {
            std::vector<glm::mat4> val_mat4;
            std::vector<float> val_float;
            
            U() { memset(this, 0, sizeof(*this)); }
            ~U() {}
            U& operator=(const U& other) {
                memcpy(this, &other, sizeof(*this));
                return *this;
            }
        } m_uValue;
        void* m_pData;
        unsigned int m_uSize;
        // 纹理format
		GLenum m_eInternalFormat;
	};

}

