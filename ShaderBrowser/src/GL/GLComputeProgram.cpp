#include "GLComputeProgram.h"
#include "Common/Tools/Utils.h"
#ifdef  _WIN32
	#pragma warning(disable:4996)
#endif //  _WIN32

namespace customGL
{
	GLComputeProgram* GLComputeProgram::create(const char* src, int local_size_x/* = 1*/, int local_size_y/* = 1*/, int local_size_z/* = 1*/, int num_groups_x/* = 1*/, int num_groups_y/* = 1*/, int num_groups_z/* = 1*/)
	{
		GLComputeProgram* program = new GLComputeProgram();
		if (!program->initComputeProgram(src, local_size_x, local_size_y, local_size_z, num_groups_x, num_groups_y, num_groups_z))
		{
			delete program;
			return nullptr;
		}
		return program;
	}

	GLComputeProgram::GLComputeProgram()
        : m_iLocalSizeX(1)
        , m_iLocalSizeY(1)
        , m_iLocalSizeZ(1)
        , m_iNumGroupsX(1)
        , m_iNumGroupsY(1)
        , m_iNumGroupsZ(1)
	{
	}

	GLComputeProgram::~GLComputeProgram()
	{
	}

    bool GLComputeProgram::initComputeProgram(const char* src, int local_size_x/* = 1*/, int local_size_y/* = 1*/, int local_size_z/* = 1*/, int num_groups_x/* = 1*/, int num_groups_y/* = 1*/, int num_groups_z/* = 1*/)
    {
        if (!src || strlen(src) == 0)
        {
            return false;
        }
     
        m_iLocalSizeX = local_size_x; m_iLocalSizeY = local_size_y; m_iLocalSizeZ = local_size_z;
        m_iNumGroupsX = num_groups_x; m_iNumGroupsY = num_groups_y; m_iNumGroupsZ = num_groups_z;
        
        // 1.创建着色器程序
        GLuint program = glCreateProgram();
        m_uProgram = program;

        // 设置shader中的本地工作组大小
        char localGroupDef[100];
        sprintf(localGroupDef, "layout (local_size_x=%d, local_size_y=%d, local_size_z=%d) in;", local_size_x, local_size_y, local_size_z);
        setCompLocalGroupDefCode(localGroupDef);
        
        // 2.创建shader
        // 计算着色器
        if (!createShader(GL_COMPUTE_SHADER, m_uCompShader, src, false))
        {
            return false;
        }

        // 3.着色器程序绑定shader
        // 计算着色器shader
        glAttachShader(m_uProgram, m_uCompShader);

        // 绑定预定义的顶点属性
        bindPredefinedVertexAttribs();

        // 5.链接着色器程序
        glLinkProgram(m_uProgram);

        // 6.记录uniform变量位置
        updatePreDefinedUniformsLocation();


        // 检查着色器程序链接状态信息
        GLint linked;
        glGetProgramiv(m_uProgram, GL_LINK_STATUS, &linked);
        if (!linked)
        {
//#ifdef _DEBUG
            GLsizei len;
            glGetProgramiv(m_uProgram, GL_INFO_LOG_LENGTH, &len);

            GLchar* log = new GLchar[len + 1];
            glGetProgramInfoLog(m_uProgram, len, &len, log);
            std::cerr << "Shader linking failed: " << log << std::endl;
            delete[] log;

//#endif /* DEBUG */
            common::BROWSER_ASSERT(linked, "shader program linked error in function GLComputeProgram::initComputeProgram");

            return false;
        }

        return true;
    }
    
    void GLComputeProgram::executeComputeProgram()
    {
        glDispatchCompute(m_iNumGroupsX, m_iNumGroupsY, m_iNumGroupsZ);
    }
            
    void GLComputeProgram::executeComputeProgram(int num_groups_x, int num_groups_y, int num_groups_z)
    {
        // 注意，这里全局工作组的大小是临时的，并没有记录
        glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    }
        
    
}
