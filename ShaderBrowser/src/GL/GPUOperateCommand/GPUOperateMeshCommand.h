#pragma once

#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"


namespace customGL
{
    class Mesh;
    
    // 纹理缓存操作命令
	class GPUOperateMeshCommand : public BaseGPUOperateCommand
	{
	public:
		GPUOperateMeshCommand();
        ~GPUOperateMeshCommand();
        
    public:
        // 准备执行 (逻辑线程调用)
        virtual void ready(GPUOperateType operateType);
        // 执行 (渲染线程调用)
        virtual void execute();
        // 结束执行 (渲染线程调用)
        virtual void finish();

    protected:
        // 创建纹理
        void createMesh();
        // 更新纹理数据
        void updateMesh();
        // 删除纹理
        void deleteMesh();
    
		REGISTER_PROPERTY_SET(Mesh*, m_pMesh, Mesh)
        
	protected:
        // 网格对象
		Mesh* m_pMesh;
        
	};

}

