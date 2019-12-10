#pragma once

#include <string>
#include <unordered_map>
#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"


namespace customGL
{
    class GLProgram;
    
    // GLProgram操作命令
	class GPUOperateGLProgramCommand : public BaseGPUOperateCommand
	{
	public:
		GPUOperateGLProgramCommand();
        ~GPUOperateGLProgramCommand();
        
    public:
        // 准备执行 (逻辑线程调用)
        virtual void ready(GPUOperateType operateType);
        // 执行 (渲染线程调用)
        virtual void execute();
        // 结束执行 (渲染线程调用)
        virtual void finish();

    protected:
        // 创建GLProgram
        void createGLProgram();
		// 删除GLProgram
		void deleteGLProgram();
        
        REGISTER_PROPERTY_SET(GLProgram*, m_pGLProgram, GLProgram)
        void setVertShaderPath(const char* path)
        {
            m_sVertShaderPath = path;
        }
        void setFragShaderPath(const char* path)
        {
            m_sFragShaderPath = path;
        }
        
	protected:
        // GLProgram操作对象
        GLProgram* m_pGLProgram;
        
        // 顶点着色器源码路径
        string m_sVertShaderPath;
        // 片段着色器源码路径
        string m_sFragShaderPath;
        
	};

}

