#pragma once

#include <vector>
#include <unordered_map>
#include <glad/glad.h>

#include "GL/GLDefine.h"
#include "GL/GLComputeProgram.h"
#include "Common/Tools/Utils.h"
#include "Common/Components/Reference.h"

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
    // 最大支持VBO数量
#define MAX_SUPPORT_VBO_NUM 10
    
	class BaseComputeProgram : public common::Reference
	{
	public:
		BaseComputeProgram();
		~BaseComputeProgram();
        
    public:

        // 初始化
        bool initComputeProgram(const char* src, int local_size_x = 1, int local_size_y = 1, int local_size_z = 1, int num_groups_x = 1, int num_groups_y = 1, int num_groups_z = 1);

		// buffers
		void addComputeBuffer(const std::string& name, GLuint size, GLenum access = GL_READ_ONLY, BufferType type = BufferType::TextureBuffer, GLenum format = GL_RGBA32F);
		// input
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
        void setUniformTex2D(const std::string& uniformName, GLuint textureId);
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
		void setUniformImageBuffer(const std::string& uniformName, GLuint textureId, GLenum access, GLenum format);
        // 设置vbos
        void setupVBOs();
        // 计算
        virtual void executeComputeProgram();
		// 获取结果
		template <typename T>
		void getOutputDataFromVBOs(GLuint index, T* output, int size)
		{
			ComputeBufferDeclaration* declaration = m_vTextureBuffers[index];
			switch (declaration->type)
			{
				case BufferType::TextureBuffer:
				{
					glBindBuffer(GL_TEXTURE_BUFFER, m_vVBOs[index]);
					glGetBufferSubData(GL_TEXTURE_BUFFER, 0, size, output);
				}
				break;
			}
		}
		template <typename T>
		void getOutputDataFromVBOs(const std::string& uniformName, T* output, int size)
		{
			getOutputDataFromVBOs(m_mBufferIds[uniformName], output, size);
		}


    protected:
		// Compute Program
		GLComputeProgram* m_oComputeProgram;
        // 输入\输出数据
        std::vector<ComputeBufferDeclaration*> m_vTextureBuffers;
        // uniform数据
        std::unordered_map<std::string, UniformValue> m_mUniforms;
		// 记录tbo数量
		unsigned int m_uTexNum;
		// 记录缓冲id
		std::unordered_map<std::string, GLuint> m_mBufferIds;

		// vbos
		GLuint m_vVBOs[MAX_SUPPORT_VBO_NUM];
		GLuint m_vTexs[MAX_SUPPORT_VBO_NUM]; //texId
    };
}

