#include "BaseFeedback.h"
#include <glad/glad.h>

using namespace customGL;

namespace browser
{
	BaseFeedback::BaseFeedback()
        : BaseComponent("BaseFeedback")
        , m_oFeedback(nullptr)
        , m_uVAO(0)
        , m_bGenVAO(false)
	{
        // 组件所属系统
        m_eBelongSystem = SystemType::TransformFeedback;
        
        // clear
        m_vVaryings.clear();
        m_mUniforms.clear();
	}

	BaseFeedback::~BaseFeedback()
	{
        if (m_uVAO != 0)
        {
            glDeleteVertexArrays(1, &m_uVAO);
        }
        
        if (m_pMainVBOs)
        {
            glDeleteBuffers(m_mVertexAttribDeclarations.size(), m_pMainVBOs);
        }
        if (m_pOutVBOs)
        {
            glDeleteBuffers(m_mFeedbackBufDeclarations.size(), m_pOutVBOs);
        }
        
	}
    
    bool BaseFeedback::initFeedback(const char* vertSrc, const char* varyings[], int length, GLenum bufferMode /*= GL_INTERLEAVED_ATTRIBS*/)
    {
        m_oFeedback = GLFeedback::create(vertSrc, varyings, length, bufferMode);
        m_oFeedback->retain();
        
        for (int i=0; i<length; ++i)
        {
            m_vVaryings.push_back(varyings[i]);
        }
        
        return true;
    }
    
    void BaseFeedback::flushAsPoints()
    {
        glBindVertexArray(m_uVAO);
        
        // TODO: 如果有tbo存在的话，需要重新set（类似material）
        // ...
        
        m_oFeedback->useProgram();
        
        // Perform feedback transform
        glEnable(GL_RASTERIZER_DISCARD);// 禁用光栅器
        
        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, 5);
        glEndTransformFeedback();
        
        glDisable(GL_RASTERIZER_DISCARD);// 开启光栅器
        
        // flush
        glFlush();
        
        
        glBindVertexArray(0);
    }
    
    void BaseFeedback::getOutputDataFromVBOs(GLuint bindIdx, float* output, int size)
    {
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, bindIdx, size, output);
    }
    
    void BaseFeedback::getOutputDataFromVBOs(std::string varying, float* output, int size)
    {
        int bindIdx = getFeedbackBufferBindIdx(varying.c_str());
        
        BROWSER_ASSERT(bindIdx>=0, "Cannot find transform feedback buffer bind index in function BaseFeedback::getOutputDataFromVBOs");
        
        getOutputDataFromVBOs(bindIdx, output, size);
    }
    
    int BaseFeedback::getFeedbackBufferBindIdx(const char* varying)
    {
        for (auto itor=m_mFeedbackBufDeclarations.begin(); itor!=m_mFeedbackBufDeclarations.end(); ++itor)
        {
            if (itor->second->varying == varying)
            {
                return itor->first;
            }
        }
        
        return -1;
    }
    
    void BaseFeedback::addVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* pointer, void* data, GLint dataSize, VertexDataType dataType /*= VertexDataType::Float*/)
    {
        BROWSER_ASSERT(!m_bGenVAO, "Feedback has already generate vao and vbos, its member m_mVertexAttribDeclarations cannot be changed in function BaseFeedback::addVertexAttribute.");
        
        // 移除旧值
        auto itor = m_mVertexAttribDeclarations.find(location);
        if (itor != m_mVertexAttribDeclarations.end())
        {
            m_mVertexAttribDeclarations.erase(itor);
        }
        
        // 将顶点属性记录下来，用来设置vao
        VertexAttribDeclaration* declaration = Utils::createVertexAttribDeclaration(location, size, type, normalized, stride, pointer, data, dataSize, dataType);
        m_mVertexAttribDeclarations.emplace(location, declaration);
    }
    
    void BaseFeedback::addFeedbackBuffer(GLuint bindIdx, GLuint size, const string& name, FeedbackBufferType type/* = FeedbackBufferType::ArrayBuffer*/)
    {
        BROWSER_ASSERT(!m_bGenVAO, "Feedback has already generate vao and vbos, its member m_mFeedbackBufDeclarations cannot be changed in function BaseFeedback::addFeedbackBuffer.");
        
        // 移除旧值
        auto itor = m_mFeedbackBufDeclarations.find(bindIdx);
        if (itor != m_mFeedbackBufDeclarations.end())
        {
            m_mFeedbackBufDeclarations.erase(itor);
        }
        
        // 将顶点属性记录下来，用来设置vao
        FeedbackBufferDeclaration* declaration = Utils::createFeedbackBufferDeclaration(bindIdx, size, name);
        m_mFeedbackBufDeclarations.emplace(bindIdx, declaration);
    }

    void BaseFeedback::setupVAOandVBOs()
    {
        m_bGenVAO = true;
        
        // vao
        glGenVertexArrays(1, &m_uVAO);
        
        glBindVertexArray(m_uVAO);
        // input vbos
        int idx = 0;
        {
            VertexAttribDeclaration* declaration;
            glGenBuffers(m_mVertexAttribDeclarations.size(), m_pMainVBOs);
            for (auto itor=m_mVertexAttribDeclarations.begin(); itor!=m_mVertexAttribDeclarations.end(); ++itor)
            {
                declaration = itor->second;
                // data
                glBindBuffer(GL_ARRAY_BUFFER, m_pMainVBOs[idx]);
                glBufferData(GL_ARRAY_BUFFER, declaration->data_size, declaration->data, GL_STATIC_DRAW);
                // attr
                glEnableVertexAttribArray(declaration->index);
                switch(declaration->data_type)
                {
                    case VertexDataType::Int:
                        {
                            glVertexAttribIPointer(declaration->index, declaration->size, declaration->type, declaration->stride, declaration->pointer);
                        }
                        break;
                    
                    default:
                        {
                            glVertexAttribPointer(declaration->index, declaration->size, declaration->type, declaration->normalized, declaration->stride, declaration->pointer);
                        }
                        break;
                    
                }
            }
        }
        
        
        // output vbos
        idx = 0;
        {
            FeedbackBufferDeclaration* declaration;
            glGenBuffers(m_mFeedbackBufDeclarations.size(), m_pOutVBOs);
            for (auto itor=m_mFeedbackBufDeclarations.begin(); itor!=m_mFeedbackBufDeclarations.end(); ++itor)
            {
                declaration = itor->second;
                
                // data
                glBindBuffer(GL_ARRAY_BUFFER, m_pOutVBOs[idx]);
                glBufferData(GL_ARRAY_BUFFER, declaration->size, nullptr, GL_DYNAMIC_COPY);
                // 绑定到Transform Feedback缓存点
                glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, declaration->bindIdx, m_pOutVBOs[idx]);
            }
        }
        glBindVertexArray(0);
    }
    

}
