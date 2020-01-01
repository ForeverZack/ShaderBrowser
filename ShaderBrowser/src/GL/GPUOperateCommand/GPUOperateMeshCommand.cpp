#include "GPUOperateMeshCommand.h"
#include "GL/GPUResource/Model/Mesh.h"

namespace customGL
{
	GPUOperateMeshCommand::GPUOperateMeshCommand()
        : m_pMesh(nullptr)
		, m_pData(nullptr)
		, m_uSize(0)
    {
        m_eCommandType = GOCT_Mesh;
        m_eOperateType = GOT_Update;
	}
    
	GPUOperateMeshCommand::~GPUOperateMeshCommand()
    {
        
    }
    
    void GPUOperateMeshCommand::ready(GPUOperateType operateType)
    {
        BROWSER_ASSERT(m_pMesh, "GPUOperateMeshCommand does not have operate object, please check your program in function GPUOperateMeshCommand::ready");
        
        BaseGPUOperateCommand::ready(operateType);
        
        // 逻辑线程调用，防止autorelease先执行
        m_pMesh->retain();
    }
    
    void GPUOperateMeshCommand::execute()
    {        
        switch(m_eOperateType)
        {
        case GPUOperateType::GOT_Create:
            {
                // 创建网格
                createMesh();
            }
            break;
            
        case GPUOperateType::GOT_Update:
            {
                // 更新网格数据
                updateMesh();
            }
            break;
                
        case GPUOperateType::GOT_Delete:
            {
                // 删除网格
                deleteMesh();
            }
            break;

		case GPUOperateType::GOT_UpdateProperties_1:
			{
				// 更新网格索引数据
				updateMeshIndices();
			}
			break;
		}
        
    }
    
    // 结束执行 (渲染线程调用)
    void GPUOperateMeshCommand::finish()
    {
        // 清除
		m_pData = nullptr;
		m_uSize = 0;
		if (m_eCommandType == GPUOperateType::GOT_Update)
		{
            // 清空网格数据
            clearData();
		}
        
        // 回收命令
        BaseGPUOperateCommand::finish();
        
        // 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
        AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_pMesh);
    }
    
    void GPUOperateMeshCommand::createMesh()
    {
		// 生成vao
		glGenVertexArrays(1, &m_pMesh->m_uVAO);
		// 生成vbo
		glGenBuffers(MESH_VERTEX_ATTR_COUNT, m_pMesh->m_uVBOs);
		// 生成索引buffer
		glGenBuffers(1, &m_pMesh->m_uIndicesVBO);

		// 资源加载完成
		m_pMesh->m_eResouceState = GRS_Loaded;
    }
    
    void GPUOperateMeshCommand::updateMesh()
    {
		// 1.绑定对应的vao
		glBindVertexArray(m_pMesh->m_uVAO);

		// 2.设置vao
		setupVAO();

		// 3.绑定对应vbo
		glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->m_uVBOs[m_oDeclaration.index]);
		glBufferData(GL_ARRAY_BUFFER, m_uSize, m_pData, GL_STATIC_DRAW);

		// 4.解绑
		glBindVertexArray(0);
	}
    
    void GPUOperateMeshCommand::deleteMesh()
    {
		if (m_pMesh->m_uVAO != 0)
        {
			glDeleteVertexArrays(1, &m_pMesh->m_uVAO);
        }
		if (m_pMesh->m_uIndicesVBO != 0)
		{
			glDeleteBuffers(1, &m_pMesh->m_uIndicesVBO);
		}
		glDeleteBuffers(MESH_VERTEX_ATTR_COUNT, m_pMesh->m_uVBOs);

		// 设置GPU删除标记
		m_pMesh->setGPUDeleted(true);
    }

	void GPUOperateMeshCommand::updateMeshIndices()
	{
		// 1.绑定对应的vao,vbo
		glBindVertexArray(m_pMesh->m_uVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pMesh->m_uIndicesVBO);

		// 2.更新索引数据
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_uSize, m_pData, GL_STATIC_DRAW);

		// 3.解绑
		glBindVertexArray(0);
	}

	void GPUOperateMeshCommand::setupVAO()
	{
		const VertexAttribDeclaration& declaration = m_oDeclaration;
		unsigned int* vbos = m_pMesh->m_uVBOs;

		// 1.设置顶点属性
		switch (declaration.index)
		{
		case GLProgram::VERTEX_ATTR_POSITION:	// 0.顶点位置
		case GLProgram::VERTEX_ATTR_COLOR:	// 1.顶点颜色
		case GLProgram::VERTEX_ATTR_TEX_COORD:	// 2.主纹理坐标
		case GLProgram::VERTEX_ATTR_NORMAL:		// 3.法线
		case GLProgram::VERTEX_ATTR_TANGENT:	// 4.切线
		case GLProgram::VERTEX_ATTR_BONE_WEIGHTS:	// 6.骨骼权重
			{
				glBindBuffer(GL_ARRAY_BUFFER, vbos[declaration.index]);
				glVertexAttribPointer(declaration.index, declaration.size, declaration.type, declaration.normalized, declaration.stride, (void*)0);
			}
			break;

		case GLProgram::VERTEX_ATTR_BONE_IDS:	// 5.骨骼id
			{
				// 注意！！！这里要用glVertexAttribIPointer来传递int值，不然都是float类型的，索引数组会找不到
				glBindBuffer(GL_ARRAY_BUFFER, vbos[declaration.index]);
				glVertexAttribIPointer(declaration.index, declaration.size, declaration.type, declaration.stride, (void*)0);
			}
			break;
		}

		// 开启对应layout
		glEnableVertexAttribArray(declaration.index);
	}

	void GPUOperateMeshCommand::setVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, VertexDataType dataType /*= VertexDataType::Float*/)
	{
		m_oDeclaration.index = location;
		m_oDeclaration.size = size;
		m_oDeclaration.type = type;
		m_oDeclaration.normalized = normalized;
		m_oDeclaration.stride = stride;
		m_oDeclaration.data_type = dataType;
	}
    
    void GPUOperateMeshCommand::clearData()
    {
        switch (m_eDataType)
        {
        case GOMC_DataType::Vector3:
            {
                std::vector<glm::vec3> vec;
                m_vValueVec3.swap(vec);
            }
            break;
        
        case GOMC_DataType::Vector4:
            {
                std::vector<glm::vec4> vec;
                m_vValueVec4.swap(vec);
            }
            break;
                
        case GOMC_DataType::UnsignedVector4:
            {
                std::vector<glm::uvec4> vec;
                m_vValueUVec4.swap(vec);
            }
            break;
                
        case GOMC_DataType::Float:
            {
                std::vector<float> vec;
                m_vValueFloat.swap(vec);
            }
            break;
        
        case GOMC_DataType::UnsignedShort:
            {
                std::vector<GLushort> vec;
                m_vValueUShort.swap(vec);
            }
            break;
        }
    }
    
	void GPUOperateMeshCommand::setData(const vector<glm::vec3>& data)
	{
		BROWSER_ASSERT(data.size() > 0, "Data size must bigger than 0 in function GPUOperateMeshCommand::setData(const vector<glm::vec3>& data)");

        m_eDataType = GOMC_DataType::Vector3;
		m_vValueVec3 = data;
        m_pData = &m_vValueVec3[0];
		m_uSize = sizeof(glm::vec3)*data.size();
	}

	void GPUOperateMeshCommand::setData(const vector<glm::vec4>& data)
	{
		BROWSER_ASSERT(data.size() > 0, "Data size must bigger than 0 in function GPUOperateMeshCommand::setData(const vector<glm::vec4>& data)");

        m_eDataType = GOMC_DataType::Vector4;
		m_vValueVec4 = data;
		m_pData = &m_vValueVec4[0];
		m_uSize = sizeof(glm::vec4)*data.size();
	}

	void GPUOperateMeshCommand::setData(const vector<glm::uvec4>& data)
	{
		BROWSER_ASSERT(data.size() > 0, "Data size must bigger than 0 in function GPUOperateMeshCommand::setData(const vector<glm::uvec4>& data)");

        m_eDataType = GOMC_DataType::UnsignedVector4;
		m_vValueUVec4 = data;
        m_pData = &m_vValueUVec4[0];
		m_uSize = sizeof(glm::uvec4)*data.size();
	}

	void GPUOperateMeshCommand::setData(const vector<float>& data)
	{
		BROWSER_ASSERT(data.size() > 0, "Data size must bigger than 0 in function GPUOperateMeshCommand::setData(const vector<float>& data)");

        m_eDataType = GOMC_DataType::Float;
		m_vValueFloat = data;
        m_pData = &m_vValueFloat[0];
		m_uSize = sizeof(float)*data.size();
	}

	void GPUOperateMeshCommand::setData(const vector<GLushort>& data)
	{
		BROWSER_ASSERT(data.size() > 0, "Data size must bigger than 0 in function GPUOperateMeshCommand::setData(const vector<GLushort>& data)");

        m_eDataType = GOMC_DataType::UnsignedShort;
		m_vValueUShort = data;
        m_pData = &m_vValueUShort[0];
		m_uSize = sizeof(GLushort)*data.size();
	}


}
