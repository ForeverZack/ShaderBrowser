#include "BaseFeedback.h"
#include <glad/glad.h>
#include "Browser/System/RenderSystem.h"
#include <chrono>

using namespace customGL;

namespace browser
{
	BaseFeedback::BaseFeedback()
        : m_oFeedback(nullptr)
        , m_bGenVAO(false)
        , m_uOutTexNum(0)
        , m_uFeedbackBufCount(0)
        , m_uCurFrameIdx(0)
        , m_uCurFrameTag(0)
	{
		this->autorelease();

		// clear
        m_vVaryings.clear();
        m_mUniforms.clear();
	}

	BaseFeedback::~BaseFeedback()
	{
        if (m_uVAOs[0] != 0)
        {
            glDeleteVertexArrays(EXCHANGE_BUFFERS_COUNT, m_uVAOs);
            
            for (int i=0; i<EXCHANGE_BUFFERS_COUNT; ++i)
            {
                glDeleteBuffers(m_mVertexAttribDeclarations.size(), m_vMainVBOs[i]);
                glDeleteBuffers(m_mFeedbackBufDeclarations.size(), m_vOutVBOs[i]);
                if (m_uOutTexNum > 0)
                {
                    glDeleteTextures(m_uOutTexNum, m_vOutTexs[i]);
                }
            }
        }
        
        
        // 释放feedback对象
        m_oFeedback->release();
        
        for (auto itor=m_mVertexAttribDeclarations.begin(); itor!=m_mVertexAttribDeclarations.end(); ++itor)
        {
            BROWSER_SAFE_RELEASE_POINTER(itor->second);
        }
        m_mVertexAttribDeclarations.clear();
        
        clearFeedbackBuffers();
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
    
    bool BaseFeedback::initFeedback(const char* vertSrc, const char* varyings[], int length, const std::string& additionVertCode, GLenum bufferMode /*= GL_INTERLEAVED_ATTRIBS*/)
    {
        m_oFeedback = GLFeedback::create(vertSrc, varyings, length, additionVertCode, bufferMode);
        m_oFeedback->retain();
        
        for (int i=0; i<length; ++i)
        {
            m_vVaryings.push_back(varyings[i]);
        }
        
        return true;
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
    
    void BaseFeedback::addFeedbackBuffer(GLuint size, const string& name, BufferType type/* = BufferType::ArrayBuffer*/, GLenum internalFormat /*= GL_RGBA32F*/)
    {
        BROWSER_ASSERT(!m_bGenVAO, "Feedback has already generate vao and vbos, its member m_mFeedbackBufDeclarations cannot be changed in function BaseFeedback::addFeedbackBuffer.");
     
        
        GLuint bindIdx = m_uFeedbackBufCount;
        
        // 将顶点属性记录下来，用来设置vao
        FeedbackBufferDeclaration* declaration = Utils::createFeedbackBufferDeclaration(bindIdx, size, name, type, internalFormat);
        m_mFeedbackBufDeclarations.emplace(bindIdx, declaration);
        
        // 记录tbo数量
        if (type == BufferType::TextureBuffer)
        {
            ++m_uOutTexNum;
        }
        
        ++m_uFeedbackBufCount;
    }

    void BaseFeedback::clearFeedbackBuffers()
    {
        for (auto itor=m_mFeedbackBufDeclarations.begin(); itor!=m_mFeedbackBufDeclarations.end(); ++itor)
        {
            BROWSER_SAFE_RELEASE_POINTER(itor->second);
        }
        m_mFeedbackBufDeclarations.clear();
        
        m_uFeedbackBufCount = 0;
    }
    
    void BaseFeedback::setupVAOandVBOs()
    {
        m_bGenVAO = true;
        
        // 绑定transform feedback
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_oFeedback->getFeed());
        
        // vao
        glGenVertexArrays(EXCHANGE_BUFFERS_COUNT, m_uVAOs);
        
        for (int i=0; i<EXCHANGE_BUFFERS_COUNT; ++i)
        {
            glBindVertexArray(m_uVAOs[i]);
            // input vbos
            int idx = 0;
            {
                VertexAttribDeclaration* declaration;
                glGenBuffers(m_mVertexAttribDeclarations.size(), m_vMainVBOs[i]);
                for (auto itor=m_mVertexAttribDeclarations.begin(); itor!=m_mVertexAttribDeclarations.end(); ++itor)
                {
                    declaration = itor->second;
                    // data
                    glBindBuffer(GL_ARRAY_BUFFER, m_vMainVBOs[i][idx]);
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
                    ++idx;
                }
            }
            
            
            // output vbos
            idx = 0;
            {
                int texIdx = 0;
                FeedbackBufferDeclaration* declaration;
                glGenBuffers(m_mFeedbackBufDeclarations.size(), m_vOutVBOs[i]);
                glGenTextures(m_uOutTexNum, m_vOutTexs[i]);
                for (auto itor=m_mFeedbackBufDeclarations.begin(); itor!=m_mFeedbackBufDeclarations.end(); ++itor)
                {
                    declaration = itor->second;
                    declaration->vbos[i] = m_vOutVBOs[i][idx];
                    
                    switch(declaration->type)
                    {
                        case BufferType::ArrayBuffer:
                        {
                            // data
                            glBindBuffer(GL_ARRAY_BUFFER, declaration->vbos[i]);
                            glBufferData(GL_ARRAY_BUFFER, declaration->size, nullptr, GL_DYNAMIC_COPY);
                        }
                            break;
                            
                        case BufferType::TextureBuffer:
                        {
                            // tbo
                            glBindBuffer(GL_TEXTURE_BUFFER, declaration->vbos[i]);
                            glBufferData(GL_TEXTURE_BUFFER, declaration->size, nullptr, GL_DYNAMIC_COPY);
                            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, declaration->bindIdx, declaration->vbos[i]);
                            glBindTexture(GL_TEXTURE_BUFFER,m_vOutTexs[i][texIdx]);
                            // 将缓存区关联到纹理对象上tbo
                            glTexBuffer(GL_TEXTURE_BUFFER, declaration->internalFormat, declaration->vbos[i]);
                            ++texIdx;
                        }
                            break;
                    }
                    // 绑定到Transform Feedback缓存点
                    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, declaration->bindIdx, declaration->vbos[i]);
                    
                    ++idx;
                }
            }

        }
        glBindVertexArray(0);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    }
    
    void BaseFeedback::flushAsPoints(int length)
    {
        auto timeRec = std::chrono::steady_clock::now();
        float deltaTime;
        
        // 绑定transform feedback
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_oFeedback->getFeed());
        // 绑定vao
        glBindVertexArray(m_uVAOs[getCurFrameTag()]);
        // 绑定到Transform Feedback缓存点 (由于要交换缓冲，所以每次更新Transform_Feedback_Buffer时，必须要重新将vbo绑定到对应的绑定点上)
        for (auto itor=m_mFeedbackBufDeclarations.begin(); itor!=m_mFeedbackBufDeclarations.end(); ++itor)
        {
            FeedbackBufferDeclaration* declaration = itor->second;
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, declaration->bindIdx, declaration->vbos[getCurFrameTag()]);
        }
        
//        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
//        timeRec = std::chrono::steady_clock::now();
//        BROWSER_LOG("========bind buffer=====00======"+ std::to_string(deltaTime) + "ms");
        
        // 如果有tbo存在的话，需要重新设置纹理单元
        for (auto itor = m_mUniforms.begin(); itor != m_mUniforms.end(); ++itor)
        {
            const UniformValue& uniform = itor->second;
            if (uniform.getUniformValueType() == UniformValue::UniformValueType::UniformValueType_SamplerBuffer)
            {
                setUniformSamplerBuffer(itor->first, uniform._value.samplerBuffer.textureId);
            }
        }
        
//        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
//        timeRec = std::chrono::steady_clock::now();
//        BROWSER_LOG("========set tbo=====01======"+ std::to_string(deltaTime) + "ms");
        
        m_oFeedback->useProgram();
        // 更新属性
        for (auto itor = m_mUniforms.begin(); itor != m_mUniforms.end(); ++itor)
        {
            itor->second.updateGLProgramUniformValue(m_oFeedback, itor->first);
        }
        
//        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
//        timeRec = std::chrono::steady_clock::now();
//        BROWSER_LOG("========update value=====02======"+ std::to_string(deltaTime) + "ms");
        
        // Perform feedback transform
        glEnable(GL_RASTERIZER_DISCARD);// 禁用光栅器
        
        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, length);
//        glDrawTransformFeedback(GL_POINTS, m_oFeedback->getFeed());
        glEndTransformFeedback();
        
        glDisable(GL_RASTERIZER_DISCARD);// 开启光栅器
        
        // flush
        // 一般，使用glFlush的目的是确保在调用之后，CPU没有OpenGL相关的事情需要做-命令会送到硬件执行。调用glFinish的目的是确保当返回之后，没有相关工作留下需要继续做。
        glFlush();
        
        // 解绑
        glBindVertexArray(0);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
        
//        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
//        timeRec = std::chrono::steady_clock::now();
//        BROWSER_LOG("========draw=====03======"+ std::to_string(deltaTime) + "ms========bone count====" + std::to_string(length) + "=====frameIndex====" + std::to_string(RenderSystem::getInstance()->getFrameIndex()));
    }
    
    void BaseFeedback::getOutputDataFromVBOs(GLuint bindIdx, float* output, int size)
    {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_oFeedback->getFeed());
        FeedbackBufferDeclaration* declaration = m_mFeedbackBufDeclarations[bindIdx];
        switch(declaration->type)
        {
            case BufferType::ArrayBuffer:
                {
                    glBindBuffer(GL_ARRAY_BUFFER, declaration->vbos[getCurFrameTag()]);
                    glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, output);
                }
                break;
                
            case BufferType::TextureBuffer:
                {
                    glBindBuffer(GL_TEXTURE_BUFFER, declaration->vbos[getCurFrameTag()]);
                    glGetBufferSubData(GL_TEXTURE_BUFFER, 0, size, output);
                }
                break;
        }
        
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
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
    
    unsigned int BaseFeedback::getCurFrameTag()
    {
        if (m_uCurFrameIdx != RenderSystem::getInstance()->getFrameIndex())
        {
            m_uCurFrameIdx = RenderSystem::getInstance()->getFrameIndex();
            m_uCurFrameTag = m_uCurFrameIdx % EXCHANGE_BUFFERS_COUNT;    // 0 or 1
        }
        
        return m_uCurFrameTag;
    }
    
    void BaseFeedback::setUniformInt(const std::string& uniformName, int value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setInt(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setInt(value);
        }
    }
    void BaseFeedback::setUniformFloat(const std::string& uniformName, float value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setFloat(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setFloat(value);
        }
    }
    void BaseFeedback::setUniformMat3(const std::string& uniformName, const glm::mat3& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3(value);
        }
    }
    void BaseFeedback::setUniformMat3x4(const std::string& uniformName, const glm::mat3x4& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3x4(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3x4(value);
        }
    }
    void BaseFeedback::setUniformMat4(const std::string& uniformName, const glm::mat4& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4(value);
        }
    }
    
    void BaseFeedback::setUniformMat4x3(const std::string& uniformName, const glm::mat4x3& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4x3(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4x3(value);
        }
    }
    
    void BaseFeedback::setUniformFloatV(const std::string& uniformName, int size, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setFloatV(size, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setFloatV(size, value);
        }
    }
    
    void BaseFeedback::setUniformV2f(const std::string& uniformName, const glm::vec2& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec2(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec2(value);
        }
    }
    
    void BaseFeedback::setUniformV3f(const std::string& uniformName, const glm::vec3& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec3(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec3(value);
        }
    }
    
    void BaseFeedback::setUniformV4f(const std::string& uniformName, const glm::vec4& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec4(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec4(value);
        }
    }
    
    void BaseFeedback::setUniformTex2D(const std::string& uniformName, Texture2D* texture)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setTex2D(texture);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setTex2D(texture);
        }
    }
    
    void BaseFeedback::setUniformSamplerBuffer(const std::string& uniformName, GLuint textureId)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setSamplerBuffer(textureId);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setSamplerBuffer(textureId);
        }
    }
    
    void BaseFeedback::setUniformIntV(const std::string& uniformName, int count, const int* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIntV(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIntV(count, value);
        }
    }
    
    void BaseFeedback::setUniformIVec2(const std::string& uniformName, const glm::ivec2& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec2(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec2(value);
        }
    }
    
    void BaseFeedback::setUniformVec2V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec2V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec2V(count, value);
        }
    }
    
    void BaseFeedback::setUniformIVec2V(const std::string& uniformName, int count, const int* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec2V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec2V(count, value);
        }
    }
    
    void BaseFeedback::setUniformIVec3(const std::string& uniformName, const glm::ivec3& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec3(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec3(value);
        }
    }
    
    void BaseFeedback::setUniformVec3V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec3V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec3V(count, value);
        }
    }
    
    void BaseFeedback::setUniformIVec3V(const std::string& uniformName, int count, const int* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec3V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec3V(count, value);
        }
    }
    
    void BaseFeedback::setUniformIVec4(const std::string& uniformName, const glm::ivec4& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec4(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec4(value);
        }
    }
    
    void BaseFeedback::setUniformVec4V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec4V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec4V(count, value);
        }
    }
    
    void BaseFeedback::setUniformIVec4V(const std::string& uniformName, int count, const int* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec4V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec4V(count, value);
        }
    }
    
    void BaseFeedback::setUniformMat4V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4V(count, value);
        }
    }
    
    void BaseFeedback::setUniformMat4x3V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4x3V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4x3V(count, value);
        }
    }
    
    void BaseFeedback::setUniformMat3V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3V(count, value);
        }
    }
    
    void BaseFeedback::setUniformMat3x4V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3x4V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3x4V(count, value);
        }
    }
    
    

}
