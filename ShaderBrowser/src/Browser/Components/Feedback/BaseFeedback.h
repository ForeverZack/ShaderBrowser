#pragma once

#include <vector>
#include <unordered_map>
#include <glad/glad.h>

#include "GL/GLDefine.h"
#include "GL/GLFeedback.h"
#include "Common/Tools/Utils.h"
#include "Browser/Components/BaseComponent.h"

using namespace common;
using namespace customGL;

namespace browser
{
    // 最大支持VertexAttr顶点属性数量
#define MAX_SUPPORT_VERTEX_ATTR_NUM 10
    // 最大支持VBO数量
#define MAX_SUPPORT_VBO_NUM 10
    
	class BaseFeedback : public BaseComponent
	{
	public:
		BaseFeedback();
		~BaseFeedback();
        
	public:
        
        // 初始化
        virtual bool initFeedback(const char* vertSrc, const char* varyings[], int length, GLenum bufferMode = GL_INTERLEAVED_ATTRIBS);
        
        // input
        void addVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* pointer, void* data, GLint dataSize, VertexDataType dataType = VertexDataType::Float);
        // output
        void addFeedbackBuffer(GLuint bindIdx, GLuint size, const string& name, FeedbackBufferType type = FeedbackBufferType::ArrayBuffer);
        // setup 设置vao和vbos
        void setupVAOandVBOs();
        
        // flush 更新数据(GL_POINTS)
        virtual void flushAsPoints();
        
        
        // 获取数据
        void getOutputDataFromVBOs(GLuint bindIdx, float* output, int size);
        void getOutputDataFromVBOs(std::string varying, float* output, int size);
        
    protected:
        // 根据varying名称，获取Feedback绑定id
        int getFeedbackBufferBindIdx(const char* varying);
        
	protected:
        // 
        // Transform feedback
        GLFeedback* m_oFeedback;
        // varyings  (输出变量名称)
        std::vector<std::string> m_vVaryings;
        
        // attr
        std::unordered_map<GLuint, VertexAttribDeclaration*> m_mVertexAttribDeclarations;
        // feedbackBuf
        std::unordered_map<GLuint, FeedbackBufferDeclaration*> m_mFeedbackBufDeclarations;
        // uniform数据
        std::unordered_map<std::string, UniformValue> m_mUniforms;
        
        // vao
        unsigned int m_uVAO;
        // 是否生成vao
        bool m_bGenVAO;
        //input
        unsigned int m_pMainVBOs[MAX_SUPPORT_VBO_NUM];
        // output
        // vbo (输出vbo，如果需要分开输出，可以在子类中自行添加)
        unsigned int m_pOutVBOs[MAX_SUPPORT_VBO_NUM];
    };
}

