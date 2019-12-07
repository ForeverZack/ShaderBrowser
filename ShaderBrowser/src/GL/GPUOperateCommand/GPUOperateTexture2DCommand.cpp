#include "GPUOperateTexture2DCommand.h"
#include "GL/GPUResource/Texture/Texture2D.h"

namespace customGL
{
	GPUOperateTexture2DCommand::GPUOperateTexture2DCommand()
        : m_pTexture(nullptr)
        , m_pImage(nullptr)
    {
        m_eCommandType = GOCT_Texture2D;
        m_eOperateType = GOT_Update;
	}
    
    GPUOperateTexture2DCommand::~GPUOperateTexture2DCommand()
    {
        
    }
    
    void GPUOperateTexture2DCommand::ready(GPUOperateType operateType)
    {
        BROWSER_ASSERT(m_pTexture, "BaseGPUOperateCommand does not have operate object, please check your program in function BaseGPUOperateCommand::ready");
        
        BaseGPUOperateCommand::ready(operateType);
        
        // 逻辑线程调用，防止autorelease先执行
        m_pTexture->retain();
    }
    
    void GPUOperateTexture2DCommand::execute()
    {        
        switch(m_eOperateType)
        {
        case GPUOperateType::GOT_Create:
            {
                // 创建纹理
                createTexture2D();
            }
            break;
            
        case GPUOperateType::GOT_Update:
            {
                // 更新纹理数据
                updateTexture2D();
            }
            break;
                
        case GPUOperateType::GOT_UpdateProperties_1:
            {
                // 更新纹理属性
                updateTexture2DProperties();
            }
            break;
                
        case GPUOperateType::GOT_Delete:
            {
                // 删除纹理
                deleteTexture2D();
            }
            break;
        }
        
    }
    
    // 结束执行 (渲染线程调用)
    void GPUOperateTexture2DCommand::finish()
    {
        // 回收命令
        BaseGPUOperateCommand::finish();
        
        // 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
        AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_pTexture);
    }
    
    void GPUOperateTexture2DCommand::createTexture2D()
    {
        // 1.创建纹理
        glGenTextures(1, &m_pTexture->m_uTextureId);
        // 2.绑定纹理
        glBindTexture(GL_TEXTURE_2D, m_pTexture->m_uTextureId);
        // 3.设置纹理环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_eFilterMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_eFilterMag);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_eWrapTypeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_eWrapTypeT);
        // 4.设置纹理数据
        /*
        第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
        第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
        第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
        第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
        下个参数应该总是被设为0（历史遗留的问题）。
        第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
        最后一个参数是真正的图像数据。
        */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_pImage->getWidth(), m_pImage->getHeight(), 0, m_pImage->getType(), GL_UNSIGNED_BYTE, m_pImage->getData());

        // 为当前绑定的纹理自动生成所有需要的多级渐远纹理
        glGenerateMipmap(GL_TEXTURE_2D);

        // 释放image数据
        m_pImage->deleteImage();
        // 资源加载完成
        m_pTexture->m_eResouceState = GRS_Loaded;
    }
    
    void GPUOperateTexture2DCommand::updateTexture2D()
    {
        glBindTexture(GL_TEXTURE_2D, m_pTexture->m_uTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_pImage->getWidth(), m_pImage->getHeight(), 0, m_pImage->getType(), GL_UNSIGNED_BYTE, m_pImage->getData());
    }
    
    void GPUOperateTexture2DCommand::updateTexture2DProperties()
    {
        glBindTexture(GL_TEXTURE_2D, m_pTexture->m_uTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_eFilterMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_eFilterMag);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_eWrapTypeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_eWrapTypeT);
    }
    
    void GPUOperateTexture2DCommand::deleteTexture2D()
    {
        if (m_pTexture->m_uTextureId)
        {
            glDeleteTextures(1, &m_pTexture->m_uTextureId);
        }
        if (m_pImage)
        {
            delete m_pImage;
        }
    }
    
    void GPUOperateTexture2DCommand::setTexParameters(GLenum wrapS, GLenum wrapT, GLenum filterMin, GLenum filterMag)
    {
        m_eWrapTypeS = wrapS;
        m_eWrapTypeT = wrapT;
        m_eFilterMin = filterMin;
        m_eFilterMag = filterMag;
    }

}
