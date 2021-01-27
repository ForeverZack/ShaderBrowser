#pragma once

#include <glad/glad.h>
#include "Common/Components/Reference.h"
#include "Common/Tools/Utils.h"
#include "GL/GPUResource/BaseGPUResource.h"

using namespace common;

namespace customGL
{
     class GPUOperateUniformBufferCommand;
    
	/*
		std140:
		
		每个变量都有一个基线对齐(base alignment)，它等于在一个uniform块中这个变量所占的空间（包含边距），这个基线对齐是使用std140布局原则计算出来的。然后
		，我们为每个变量计算出它的对齐偏移(aligned offset)，这是一个变量从块（block）开始处的字节偏移量。变量对齐的字节偏移一定等于它的基线对齐的倍数。
		准确的布局规则可以在OpenGL的uniform缓冲规范中得到，但我们会列出最常见的规范。GLSL中每个变量类型比如int、float和bool被定义为4字节，每4字节被表示为N。
		类型					布局规范
		像int和bool这样的标量	每个标量的基线为N
		向量					每个向量的基线是2N或4N大小。这意味着vec3的基线为4N
		标量与向量数组			每个元素的基线与vec4的相同
		矩阵					被看做是存储着大量向量的数组，每个元素的基数与vec4相同
		结构体					根据以上规则计算其各个元素，并且间距必须是vec4基线的倍数
		
		layout (std140) uniform ExampleBlock
		{
							 // base alignment ----------  // aligned offset
			float value;     // 4                          // 0
			vec3 vector;     // 16                         // 16 (必须是16的倍数，因此 4->16)
			mat4 matrix;     // 16                         // 32  (第 0 行)
							 // 16                         // 48  (第 1 行)
							 // 16                         // 64  (第 2 行)
							 // 16                         // 80  (第 3 行)
			float values[3]; // 16 (数组中的标量与vec4相同)//96 (values[0])
							 // 16                        // 112 (values[1])
							 // 16                        // 128 (values[2])
			bool boolean;    // 4                         // 144
			int integer;     // 4                         // 148
		};
		
		注意:std140既可以用于UBO，也可用于SSBO；而std430仅可用于SSBO！！！
	*/
	
	class UniformBuffer : public BaseGPUResource
	{
	public:
		static UniformBuffer* create();

	public:
		UniformBuffer();
		~UniformBuffer();
		 friend class GPUOperateUniformBufferCommand;

    public:
        // 设置数据
		void setData(const string& varname, const float& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::vec2& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::vec3& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::vec4& value, bool updateGpuData = true);
		void setData(const string& varname, const int& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::ivec2& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::ivec3& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::ivec4& value, bool updateGpuData = true);
		void setData(const string& varname, const bool& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::bvec2& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::bvec3& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::bvec4& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::mat2& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::mat2x3& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::mat2x4& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::mat3& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::mat3x2& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::mat3x4& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::mat4& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::mat4x2& value, bool updateGpuData = true);
		void setData(const string& varname, const glm::mat4x3& value, bool updateGpuData = true);



	protected:
		// 创建gpu资源
		virtual void createGPUResource();
		// 更新gpu资源
		virtual void updateGPUResource();
		// 删除gpu资源
		virtual void deleteGPUResource();
       
		// 
		template <typename T>
		void setBufferData(const string& varname, const T& value, bool updateGpuData)
		{
			auto itor = m_mVariableDatas.find(varname);
			if (itor == m_mVariableDatas.end())
			{
				BufferData bufferData;
				bufferData.setData(value);
				m_mVariableDatas.emplace(varname, std::move(bufferData));
			}
			else
			{
				BufferData& bufferData = itor->second;
				bufferData.setData(value);
			}

			if (updateGpuData)
			{
				updateGPUResource();
			}
		}

	private:
        // vbo
		GLuint m_uVBO;
        
        // gpu data info
		bool m_bGpuDataInit;
        std::vector<string> m_vVariableNames;
        std::vector<GLint> m_vVariableIndices;
        std::vector<GLint> m_vVariableTypes;

		// cpu data
        std::unordered_map<string, BufferData> m_mVariableDatas;
	};
}

