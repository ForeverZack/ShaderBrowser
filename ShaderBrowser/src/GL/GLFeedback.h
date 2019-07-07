#pragma once

#include "GLProgram.h"

namespace customGL
{
    /*
        实现TransformFeedback的OpenGL部分
        bufferMode类型说明：
             --GL_INTERLEAVED_ATTRIBS:将所有属性写入一个缓冲区对象；
             --GL_SEPARATE_ATTRIBS: 将属性写入多个缓冲区对象，或将不同的偏移量写入缓冲区。
     */
    
    class GLFeedback : public GLProgram
	{
    public:
		static GLFeedback* create(const char* vertSrc, const char* varyings[], int length, GLenum bufferMode = GL_INTERLEAVED_ATTRIBS);

	public:
		GLFeedback();
		~GLFeedback();
    
    public:
        
        
        REGISTER_PROPERTY_GET_SET(GLuint, m_uProgram, Program)

    protected:
        // 初始化Feedback
        bool initFeedback(const char* vertSrc, const char* varyings[], int length, GLenum bufferMode = GL_INTERLEAVED_ATTRIBS);

	};
}

