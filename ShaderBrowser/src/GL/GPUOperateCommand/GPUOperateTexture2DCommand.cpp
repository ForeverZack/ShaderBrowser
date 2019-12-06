#include "GPUOperateTexture2DCommand.h"
#include "GL/GPUResource/Texture/Texture2D.h"

namespace customGL
{
	GPUOperateTexture2DCommand::GPUOperateTexture2DCommand()
    {
        m_eCommandType = GOCT_Texture2D;
        m_eOperateType = GOT_Update;
	}
    
    GPUOperateTexture2DCommand::~GPUOperateTexture2DCommand()
    {
        
    }
    
    void GPUOperateTexture2DCommand::ready(GPUOperateType operateType)
    {
        BROWSER_ASSERT(m_pTexture2D, "GPUOperateTexture2DCommand does not have Texture2D object, please check your program in function GPUOperateTexture2DCommand::ready");
        
        BaseGPUOperateCommand::ready(operateType);
        
        // 逻辑线程调用，防止autorelease先执行
        m_pTexture2D->retain();
    }
    
    void GPUOperateTexture2DCommand::execute()
    {        
        switch(m_eOperateType)
        {
        case GPUOperateType::GOT_Create:
            {
                // 创建纹理
            }
            break;
            
        case GPUOperateType::GOT_Update:
            {
                // 更新纹理
            }
            break;
                
        case GPUOperateType::GOT_Delete:
            {
                // 删除纹理
            }
            break;
        }
        
    }
    
    void GPUOperateTexture2DCommand::finish()
    {
        // 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
        AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_pTexture2D);
    }
    

}
