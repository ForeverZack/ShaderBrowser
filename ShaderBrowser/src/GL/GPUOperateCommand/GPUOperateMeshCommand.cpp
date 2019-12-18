#include "GPUOperateMeshCommand.h"
#include "GL/GPUResource/Model/Mesh.h"

namespace customGL
{
	GPUOperateMeshCommand::GPUOperateMeshCommand()
        : m_pMesh(nullptr)
    {
        m_eCommandType = GOCT_Mesh;
        m_eOperateType = GOT_Update;
	}
    
	GPUOperateMeshCommand::~GPUOperateMeshCommand()
    {
        
    }
    
    void GPUOperateMeshCommand::ready(GPUOperateType operateType)
    {
        BROWSER_ASSERT(m_pMesh, "GPUOperateMeshCommand does not have operate object, please check your program in function GPUOperateMeshCommand::ready");
        
        BaseGPUOperateCommand::ready(operateType);
        
        // 逻辑线程调用，防止autorelease先执行
        m_pMesh->retain();
    }
    
    void GPUOperateMeshCommand::execute()
    {        
        switch(m_eOperateType)
        {
        case GPUOperateType::GOT_Create:
            {
                // 创建网格
                createMesh();
            }
            break;
            
        case GPUOperateType::GOT_Update:
            {
                // 更新网格数据
                updateMesh();
            }
            break;
                
        case GPUOperateType::GOT_Delete:
            {
                // 删除网格
                deleteMesh();
            }
            break;
        }
        
    }
    
    // 结束执行 (渲染线程调用)
    void GPUOperateMeshCommand::finish()
    {
        // 清除
      
        
        // 回收命令
        BaseGPUOperateCommand::finish();
        
        // 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
        AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_pMesh);
    }
    
    void GPUOperateMeshCommand::createMesh()
    {
		
    }
    
    void GPUOperateMeshCommand::updateMesh()
    {
       
    }
    
    void GPUOperateMeshCommand::deleteMesh()
    {
  //      if (m_pTexture->m_uTextureId != 0)
  //      {
  //          glDeleteTextures(1, &m_pTexture->m_uTextureId);
  //      }

		// 设置GPU删除标记
		m_pMesh->setGPUDeleted(true);
    }
    
    

}
