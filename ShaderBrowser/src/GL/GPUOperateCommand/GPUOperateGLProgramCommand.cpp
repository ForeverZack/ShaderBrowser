#include "GPUOperateGLProgramCommand.h"
#include "GL/GPUResource/Shader/GLProgram.h"

namespace customGL
{
	GPUOperateGLProgramCommand::GPUOperateGLProgramCommand()
        : m_pGLProgram(nullptr)
    {
        m_eCommandType = GOCT_GLProgram;
        m_eOperateType = GOT_Update;
	}
    
    GPUOperateGLProgramCommand::~GPUOperateGLProgramCommand()
    {
        
    }
    
    void GPUOperateGLProgramCommand::ready(GPUOperateType operateType)
    {
        BROWSER_ASSERT(m_pGLProgram, "GPUOperateGLProgramCommand does not have operate object, please check your program in function GPUOperateGLProgramCommand::ready");
        
        BaseGPUOperateCommand::ready(operateType);
        
        // 逻辑线程调用，防止autorelease先执行
        m_pGLProgram->retain();
    }
    
    void GPUOperateGLProgramCommand::execute()
    {        
        switch(m_eOperateType)
        {
        case GPUOperateType::GOT_Create:
            {
                // 创建GLProgram
                createGLProgram();
            }
            break;
            
        case GPUOperateType::GOT_Update:
            {
                // 更新GLProgram
                
            }
            break;
                
        case GPUOperateType::GOT_Delete:
            {
                // 删除GLProgram
                
            }
            break;
        }
        
    }
    
    // 结束执行 (渲染线程调用)
    void GPUOperateGLProgramCommand::finish()
    {
        // 回收命令
        BaseGPUOperateCommand::finish();
        
        // 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
        AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_pGLProgram);
    }
    
    // 创建GLProgram
    void GPUOperateGLProgramCommand::createGLProgram()
    {
        // 1.创建着色器程序
        GLuint program = glCreateProgram();
        m_pGLProgram->m_uProgram = program;

        // 2.创建shader
        // 顶点着色器
        if (!m_pGLProgram->createShader(GL_VERTEX_SHADER, m_pGLProgram->m_uVertShader, m_sVertShaderPath.c_str(), true))
        {
            return;
        }
        // 片段着色器
        if (!m_pGLProgram->createShader(GL_FRAGMENT_SHADER, m_pGLProgram->m_uFragShader, m_sFragShaderPath.c_str(), true))
        {
            return;
        }

        // 3.着色器程序绑定shader
        // 顶点shader
        glAttachShader(program, m_pGLProgram->m_uVertShader);
        // 片段shader
        glAttachShader(program, m_pGLProgram->m_uFragShader);

        // 绑定预定义的顶点属性
        m_pGLProgram->bindPredefinedVertexAttribs();

        // 4.链接着色器程序
        glLinkProgram(program);

        // 5.记录uniform变量位置
        m_pGLProgram->updatePreDefinedUniformsLocation();


        // 检查着色器程序链接状态信息
        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked)
        {
//#ifdef _DEBUG
            GLsizei len;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

            GLchar* log = new GLchar[len + 1];
            glGetProgramInfoLog(program, len, &len, log);
            std::cerr << "Shader linking failed: " << log << std::endl;
            delete[] log;

//#endif /* DEBUG */
            common::BROWSER_ASSERT(linked, "shader program linked error in function GLProgram::initProgram");

            return;
        }
        
    }


}
