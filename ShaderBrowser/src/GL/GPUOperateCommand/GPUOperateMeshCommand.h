#pragma once

#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"
#include "GL/GPUResource/Shader/GLProgram.h"


namespace customGL
{
    class Mesh;
    
    // 纹理缓存操作命令
	class GPUOperateMeshCommand : public BaseGPUOperateCommand
	{
	public:
		enum GOMC_DataType
		{
			//
		};
	public:
		GPUOperateMeshCommand();
        ~GPUOperateMeshCommand();
        
    public:
        // 准备执行 (逻辑线程调用)
        virtual void ready(GPUOperateType operateType);
        // 执行 (渲染线程调用)
        virtual void execute();
        // 结束执行 (渲染线程调用)
        virtual void finish();

    protected:
        // 创建网格
        void createMesh();
        // 更新网格数据 vbo
        void updateMesh();
        // 删除网格
        void deleteMesh();
		// 更新网格索引数据
		void updateMeshIndices();

		// 设置vao
		void setupVAO();
    
		REGISTER_PROPERTY_SET(Mesh*, m_pMesh, Mesh)
		void setVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, VertexDataType dataType = VertexDataType::Float);
		void setData(const vector<glm::vec3>& data);
		void setData(const vector<glm::vec4>& data);
		void setData(const vector<glm::uvec4>& data);
		void setData(const vector<float>& data);
		void setData(const vector<GLushort>& data);

	protected:
        // 网格对象
		Mesh* m_pMesh;

		// 顶点属性
		VertexAttribDeclaration m_oDeclaration;
		// 网格数据
		std::vector<glm::vec3> val_vec3;
		std::vector<glm::vec4> val_vec4;
		std::vector<glm::uvec4> val_uvec4;
		std::vector<float> val_float;
		std::vector<GLushort> val_ushort;
        
		void* m_pData;
		GLsizeiptr m_uSize;
        
	};

}

