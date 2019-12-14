#pragma once

#include <vector>
#include <unordered_map>
#include <glad/glad.h>

#include "GL/GLDefine.h"
#include "GL/GLFeedback.h"
#include "Common/Tools/Utils.h"
#include "Common/Components/Reference.h"
#include "GL/GPUResource/Texture/Texture2D.h"

using namespace common;
using namespace customGL;

/*
     使用说明:
        1. 创建Feedback对象（initFeedback）
        2. 添加顶点属性声明和uniform数据作为输入数据（addVertexAttribute）
        3. 添加vbo、tbo的声明作为输出数据（addFeedbackBuffer）
        4. 创建vao和上述声明对应的buffer对象，并将作为输出的buffer对象绑定到对应的TransformFeedback的绑定点上setupVAOandVBOs）
        5. gpu绘制并更新数据（flush） 注意：如果需要更新顶点数据，则需要自己重载flush函数
        6. 从绑定到TransformFeedback上的buffer中获取结果
 */

namespace browser
{
    // 最大支持VertexAttr顶点属性数量
#define MAX_SUPPORT_VERTEX_ATTR_NUM 10
    // 最大支持VBO数量
#define MAX_SUPPORT_VBO_NUM 10
    
	class BaseFeedback : public common::Reference
	{
	public:
		BaseFeedback();
		virtual ~BaseFeedback();
        
	public:
        
        // 初始化
        bool initFeedback(const char* vertSrc, const char* varyings[], int length, GLenum bufferMode = GL_INTERLEAVED_ATTRIBS);
        bool initFeedback(const char* vertSrc, const char* varyings[], int length, const std::string& additionVertCode, GLenum bufferMode = GL_INTERLEAVED_ATTRIBS);
        
        // input
        void addVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* pointer, void* data, GLint dataSize, VertexDataType dataType = VertexDataType::Float);
        void setUniformInt(const std::string& uniformName, int value);
        void setUniformFloat(const std::string& uniformName, float value);
        void setUniformMat3(const std::string& uniformName, const glm::mat3& value);
        void setUniformMat3x4(const std::string& uniformName, const glm::mat3x4& value);
        void setUniformMat4(const std::string& uniformName, const glm::mat4& value);
        void setUniformMat4x3(const std::string& uniformName, const glm::mat4x3& value);
        void setUniformFloatV(const std::string& uniformName, int size, const float* value);
        void setUniformV2f(const std::string& uniformName, const glm::vec2& value);
        void setUniformV3f(const std::string& uniformName, const glm::vec3& value);
        void setUniformV4f(const std::string& uniformName, const glm::vec4& value);
        void setUniformTex2D(const std::string& uniformName, Texture2D* texture);
        void setUniformSamplerBuffer(const std::string& uniformName, GLuint textureId);
        void setUniformIntV(const std::string& uniformName, int count, const int* value);
        void setUniformIVec2(const std::string& uniformName, const glm::ivec2& value);
        void setUniformVec2V(const std::string& uniformName, int count, const float* value);
        void setUniformIVec2V(const std::string& uniformName, int count, const int* value);
        void setUniformIVec3(const std::string& uniformName, const glm::ivec3& value);
        void setUniformVec3V(const std::string& uniformName, int count, const float* value);
        void setUniformIVec3V(const std::string& uniformName, int count, const int* value);
        void setUniformIVec4(const std::string& uniformName, const glm::ivec4& value);
        void setUniformVec4V(const std::string& uniformName, int count, const float* value);
        void setUniformIVec4V(const std::string& uniformName, int count, const int* value);
        void setUniformMat4V(const std::string& uniformName, int count, const float* value);
        void setUniformMat4x3V(const std::string& uniformName, int count, const float* value);
        void setUniformMat3V(const std::string& uniformName, int count, const float* value);
        void setUniformMat3x4V(const std::string& uniformName, int count, const float* value);
        // output
        void addFeedbackBuffer(GLuint size, const string& name, BufferType type = BufferType::BT_ArrayBuffer, GLenum internalFormat = GL_RGBA32F);
        void clearFeedbackBuffers();
        // setup 设置vao和vbos
        void setupVAOandVBOs();
        
        // flush 更新数据(GL_POINTS)
        virtual void flushAsPoints(int length);
        
        
        // 获取数据
        void getOutputDataFromVBOs(GLuint bindIdx, float* output, int size);
        void getOutputDataFromVBOs(std::string varying, float* output, int size);
        
        
    protected:
        // 根据varying名称，获取Feedback绑定id
        int getFeedbackBufferBindIdx(const char* varying);
        // 根据当前帧获取使用第几个缓冲
        unsigned int getCurFrameTag();
        
	protected:
        // 
        // Transform feedback
        GLFeedback* m_oFeedback;
        // varyings  (输出变量名称)
        std::vector<std::string> m_vVaryings;
        
        // attr
        std::unordered_map<GLuint, VertexAttribDeclaration*> m_mVertexAttribDeclarations;
        // feedbackBuf
        GLuint m_uFeedbackBufCount;
        std::unordered_map<GLuint, FeedbackBufferDeclaration*> m_mFeedbackBufDeclarations;
        // uniform数据
        std::unordered_map<std::string, UniformValue> m_mUniforms;
        
        
        // 双缓冲使用
        unsigned long m_uCurFrameIdx;
        unsigned int m_uCurFrameTag;
        // vao
        unsigned int m_uVAOs[EXCHANGE_BUFFERS_COUNT];
        // 是否生成vao
        bool m_bGenVAO;
        //input
        unsigned int m_vMainVBOs[EXCHANGE_BUFFERS_COUNT][MAX_SUPPORT_VBO_NUM]; //vbo
        
        // output
        // vbo (输出vbo，如果需要分开输出，可以在子类中自行添加)
        unsigned int m_vOutVBOs[EXCHANGE_BUFFERS_COUNT][MAX_SUPPORT_VBO_NUM]; //vbo
        unsigned int m_vOutTexs[EXCHANGE_BUFFERS_COUNT][MAX_SUPPORT_VBO_NUM]; //texId
        unsigned int m_uOutTexNum;
    };
}

