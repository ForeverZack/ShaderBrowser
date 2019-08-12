#include "GLFeedback.h"
#include "Common/Tools/Utils.h"
#ifdef  _WIN32
	#pragma warning(disable:4996)
#endif //  _WIN32

namespace customGL
{
	
	GLFeedback* GLFeedback::create(const char* vertSrc, const char* varyings[], int length, GLenum bufferMode /*= GL_INTERLEAVED_ATTRIBS*/)
	{
		GLFeedback* feedback = new GLFeedback();
		if (!feedback->initFeedback(vertSrc, varyings, length, bufferMode))
		{
			delete feedback;
			return nullptr;
		}
		return feedback;
	}
    
    GLFeedback* GLFeedback::create(const char* vertSrc, const char* varyings[], int length, const std::string& addtionVertCode, GLenum bufferMode /*= GL_INTERLEAVED_ATTRIBS*/)
    {
        GLFeedback* feedback = new GLFeedback();
        feedback->setAddtionVertCode(addtionVertCode);  // 附加顶点着色器源码
        if (!feedback->initFeedback(vertSrc, varyings, length, bufferMode))
        {
            delete feedback;
            return nullptr;
        }
        return feedback;
    }

	GLFeedback::GLFeedback()
	{
        glGenTransformFeedbacks(1, &m_uFeed);
	}

	GLFeedback::~GLFeedback()
	{
        glDeleteTransformFeedbacks(1, &m_uFeed);
	}

	bool GLFeedback::initFeedback(const char* vertSrc, const char* varyings[], int length, GLenum bufferMode /*= GL_INTERLEAVED_ATTRIBS*/)
	{
		if (!vertSrc || strlen(vertSrc) == 0)
		{
			return false;
		}
        
		// 1.创建着色器程序
		GLuint program = glCreateProgram();
		m_uProgram = program;

		// 2.创建shader
		// 顶点着色器
		if (!createShader(GL_VERTEX_SHADER, m_uVertShader, vertSrc, false))
		{
			return false;
		}

		// 3.着色器程序绑定shader
		// 顶点shader
		glAttachShader(m_uProgram, m_uVertShader);

		// 绑定预定义的顶点属性
		bindPredefinedVertexAttribs();
        
        // 4.transform feedback变量设置
        glTransformFeedbackVaryings(m_uProgram, length, varyings, bufferMode);

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
            common::BROWSER_ASSERT(linked, "shader program linked error in function GLFeedback::initFeedback");

			return false;
		}

		return true;
	}
    
    
    
}
