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
		// 添加数据成员 (注意elementSize取决于自身对象类型以及下一个)
		void addVariable(const string& varname, const size_t elementSize, const size_t length);
        // 设置数据
		void setData(const string& varname, float& value, bool updateGpuData = true);
		void setData(const string& varname, glm::vec2& value, bool updateGpuData = true);
		void setData(const string& varname, glm::vec3& value, bool updateGpuData = true);
		void setData(const string& varname, glm::vec4& value, bool updateGpuData = true);
		void setData(const string& varname, int& value, bool updateGpuData = true);
		void setData(const string& varname, glm::ivec2& value, bool updateGpuData = true);
		void setData(const string& varname, glm::ivec3& value, bool updateGpuData = true);
		void setData(const string& varname, glm::ivec4& value, bool updateGpuData = true);
		void setData(const string& varname, bool& value, bool updateGpuData = true);
		void setData(const string& varname, glm::mat2& value, bool updateGpuData = true);
		void setData(const string& varname, glm::mat2x3& value, bool updateGpuData = true);
		void setData(const string& varname, glm::mat2x4& value, bool updateGpuData = true);
		void setData(const string& varname, glm::mat3& value, bool updateGpuData = true);
		void setData(const string& varname, glm::mat3x2& value, bool updateGpuData = true);
		void setData(const string& varname, glm::mat3x4& value, bool updateGpuData = true);
		void setData(const string& varname, glm::mat4& value, bool updateGpuData = true);
		void setData(const string& varname, glm::mat4x2& value, bool updateGpuData = true);
		void setData(const string& varname, glm::mat4x3& value, bool updateGpuData = true);

		REGISTER_PROPERTY_GET(GLuint, m_uVBO, Buffer)
		REGISTER_PROPERTY_GET(size_t, m_uSize, Size)

	protected:
		// 创建gpu资源
		virtual void createGPUResource();
		// 更新gpu资源
		virtual void updateGPUResource();
		// 删除gpu资源
		virtual void deleteGPUResource();
       
		// 
		void setBufferData(const string& varname, void* data, size_t dataSize)
		{
			auto itor = m_mVariableDatas.find(varname);
			BROWSER_ASSERT(itor != m_mVariableDatas.end(), "UniformBuffer's variable has not been added !");

			BufferData& bufferData = itor->second;
			bufferData.setData(data, 0, dataSize);
		}
		void setVectorBufferData(const string& varname, void* data, size_t index, size_t dataSize)
		{
			auto itor = m_mVariableDatas.find(varname);
			BROWSER_ASSERT(itor != m_mVariableDatas.end(), "UniformBuffer's variable has not been added !");

			BufferData& bufferData = itor->second;
			bufferData.setData(data, index, dataSize);
		}

	private:
        // vbo
		GLuint m_uVBO;
        
		// data
		std::vector<string> m_vVariableNames;
        std::unordered_map<string, BufferData > m_mVariableDatas;
		size_t m_uSize;
	};
}

