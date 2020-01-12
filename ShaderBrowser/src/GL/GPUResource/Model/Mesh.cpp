#include "Mesh.h"
#include "GL/GPUResource/Shader/GLProgram.h"
#include "GL/GPUResource/Shader/Material.h"
#include "Browser/System/RenderSystem.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateMeshCommand.h"
#include "GL/System/GPUOperateSystem.h"

using namespace browser;

namespace customGL
{
	// 默认网格名称
	const char* Mesh::DEFAULT_MESH_NAME = "Default";

	Mesh* Mesh::create(int length, const std::string& meshName /*= DEFAULT_MESH_NAME*/, MeshType type /*= MeshType::CommonMesh*/)
    {
		Mesh* mesh = new Mesh(meshName, type);
        
		mesh->init(length);
        mesh->createMeshOnGPU();
        
        return mesh;
    }

	Mesh* Mesh::createAsync(int length, const std::string& meshName /*= DEFAULT_MESH_NAME*/, MeshType type /*= MeshType::CommonMesh*/)
	{
		Mesh* mesh = new Mesh(meshName, type, true);

		mesh->init(length);

		return mesh;
	}
    
	Mesh::Mesh(const std::string& meshName /*= DEFAULT_MESH_NAME*/, MeshType type /*= MeshType::CommonMesh*/, bool isRetain/* = false*/)
        : m_uVAO(0)
        , m_uMeshId(0)
		, m_uIndicesVBO(0)
        , m_eMeshType(type)
        , m_bGenVAO(false)
        , m_uVertexCount(0)
        , m_uIndexCount(0)
        , m_sMaterialName(Material::DEFAULT_MATERIAL_NAME)
		, m_sMeshName(meshName)
        , m_uPropertiesDirty(0)
        , m_bCreated(false)
	{
		this->autorelease();
		if (isRetain)
		{
			this->retain();
		}

        // 清空
        m_mTextures.clear();
	}

	Mesh::~Mesh()
	{
		deleteGPUResource();
	}
    
    Mesh* Mesh::clone()
    {
        Mesh* mesh = new Mesh(m_sMeshName, m_eMeshType);
        
        mesh->init(m_uVertexCount);
        mesh->createMeshOnGPU();
        
        if(m_vVertices.size() > 0)
        {
            mesh->setVertices(&m_vVertices[0]);
        }
        if(m_vIndices.size() > 0)
        {
            mesh->setIndices(&m_vIndices[0], m_uIndexCount);
        }
        if(m_vTexcoords1.size() > 0)
        {
            mesh->setUVs(&m_vTexcoords1[0]);
        }
        if(m_vColors.size() > 0)
        {
            mesh->setColors(&m_vColors[0]);
        }
        if(m_vNormals.size() > 0)
        {
            mesh->setNormals(&m_vNormals[0]);
        }
        if(m_vTangents.size() > 0)
        {
            mesh->setTangents(&m_vTangents[0]);
        }
        if(m_vBoneIndices.size() > 0)
        {
            mesh->setBoneIndices(m_vBoneIndices);
        }
        if(m_vBoneWeights.size() > 0)
        {
            mesh->setBoneWeights(m_vBoneWeights);
        }
        
        
        return mesh;
    }
    
    void Mesh::createMeshOnGPU()
    {
		createGPUResource();
        updateGPUResource();
    }
    
    void Mesh::init(int length)
    {
        /*
         调用resize()函数后，所有的空间都已经初始化了，所以可以直接访问。
         而reserve()函数预分配出的空间没有被初始化，所以不可访问。调用resize(n)后，容器的size即为n。至于是否影响capacity，取决于调整后的容器的size是否大于capacity。
         调用reserve(n)后，若容器的capacity<n，则重新分配内存空间，从而使得capacity等于n。如果capacity>=n呢？capacity无变化。
         */
        m_uVertexCount = length;
		m_vVertices.clear();
		m_vVertices.resize(length);

        if(m_eMeshType == MeshType::MeshWithBone)
        {
            initBonesData();
        }
        
        m_eResouceState = GRS_DataLoaded;
    }
    
    void Mesh::initBonesData()
    {
        m_vBoneIndices.resize(m_uVertexCount);
        m_vBoneWeights.resize(m_uVertexCount);
    }
    
    void Mesh::addTexture(const std::string& uniformName, Texture2D* texture)
    {
        auto itor = m_mTextures.find(uniformName);
        if(itor == m_mTextures.end())
        {
            TextureData textureData;
            textureData.texture = texture;
            textureData.uniformName = uniformName;
            
            m_mTextures.emplace(uniformName, std::move(textureData));
        }
        else
        {
            (itor->second).texture = texture;
        }
    }
    
    void Mesh::setTexture(const std::string& uniformName, Texture2D* texture)
    {
        addTexture(uniformName, texture);
    }

	void Mesh::setVertices(void* data)
	{
        // 填充数据
		fillVertexsParam(GLProgram::VERTEX_ATTR_POSITION, data);

        // 更新GPU顶点数据
        updateVertices();
	}
    
    void Mesh::setIndices(GLushort* data, unsigned int length)
    {
        // 填充数据
        m_uIndexCount = length;
        m_vIndices.resize(length);
        for(int i=0; i<length; ++i)
        {
            m_vIndices[i] = data[i];
        }
        
        // 更新GPU顶点索引数据
        updateIndices();
    }
    
    void Mesh::setUVs(void* data)
    {
        // 填充数据
        fillVertexsParam(GLProgram::VERTEX_ATTR_TEX_COORD, data);
        
        // 更新GPU纹理坐标数据
        updateUVs();
    }
    
    void Mesh::setColors(void* data)
    {
        // 填充数据
        fillVertexsParam(GLProgram::VERTEX_ATTR_COLOR, data);
        
        // 更新GPU顶点颜色数据
        updateColors();
    }
    
    void Mesh::setNormals(void* data)
    {
        // 填充数据
        fillVertexsParam(GLProgram::VERTEX_ATTR_NORMAL, data);
        
        // 更新GPU顶点法线数据
        updateNormals();
    }
    
    void Mesh::setTangents(void* data)
    {
        // 填充数据
        fillVertexsParam(GLProgram::VERTEX_ATTR_TANGENT, data);
        
        // 更新GPU顶点切线数据
        updateTangents();
    }
    
    void Mesh::setBoneIndices(const std::vector<glm::uvec4>& boneIndices)
    {
        // 填充数据
        m_vBoneIndices = boneIndices;
        
        // 更新GPU顶点骨骼索引
        updateBoneIndices();
    }
    
    void Mesh::setBoneWeights(const std::vector<glm::vec4>& boneWeights)
    {
        // 填充数据
        m_vBoneWeights = boneWeights;
        
        // 更新GPU顶点骨骼权重
        updateBoneWeights();
    }
    
    void Mesh::updateVertices()
    {
        if (m_bCreated)
        {
            BROWSER_CLEAR_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_POSITION)
            
            auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
            cmd->setMesh(this);
            cmd->setVertexAttribute(GLProgram::VERTEX_ATTR::VERTEX_ATTR_POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4));
            cmd->setData(m_vVertices);
            cmd->ready(GPUOperateType::GOT_Update);
            GPUOperateSystem::getInstance()->addCommand(cmd);
        }
        else
        {
            BROWSER_SET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_POSITION)
        }
    }
    
    void Mesh::updateIndices()
    {
        if (m_bCreated)
        {
            BROWSER_CLEAR_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_MAXCOUNT)
            
            auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
            cmd->setMesh(this);
            cmd->setData(m_vIndices);
            cmd->ready(GPUOperateType::GOT_UpdateProperties_1);
            GPUOperateSystem::getInstance()->addCommand(cmd);
        }
        else
        {
            BROWSER_SET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_MAXCOUNT)
        }
    }
    
    void Mesh::updateUVs()
    {
        if (m_bCreated)
        {
            BROWSER_CLEAR_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_TEX_COORD)
            
            auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
            cmd->setMesh(this);
            cmd->setVertexAttribute(GLProgram::VERTEX_ATTR::VERTEX_ATTR_TEX_COORD, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4));
            cmd->setData(m_vTexcoords1);
            cmd->ready(GPUOperateType::GOT_Update);
            GPUOperateSystem::getInstance()->addCommand(cmd);
        }
        else
        {
            BROWSER_SET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_TEX_COORD)
        }
    }
    
    void Mesh::updateColors()
    {
        if (m_bCreated)
        {
            BROWSER_CLEAR_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_COLOR)
            
            auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
            cmd->setMesh(this);
            cmd->setVertexAttribute(GLProgram::VERTEX_ATTR::VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4));
            cmd->setData(m_vColors);
            cmd->ready(GPUOperateType::GOT_Update);
            GPUOperateSystem::getInstance()->addCommand(cmd);
        }
        else
        {
            BROWSER_SET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_COLOR)
        }
    }
    
    void Mesh::updateNormals()
    {
        if (m_bCreated)
        {
            BROWSER_CLEAR_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_NORMAL)
            
            auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
            cmd->setMesh(this);
            cmd->setVertexAttribute(GLProgram::VERTEX_ATTR::VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3));
            cmd->setData(m_vNormals);
            cmd->ready(GPUOperateType::GOT_Update);
            GPUOperateSystem::getInstance()->addCommand(cmd);
        }
        else
        {
            BROWSER_SET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_NORMAL)
        }
    }
    
    void Mesh::updateTangents()
    {
        if (m_bCreated)
        {
            BROWSER_CLEAR_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_TANGENT)
            
            auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
            cmd->setMesh(this);
            cmd->setVertexAttribute(GLProgram::VERTEX_ATTR::VERTEX_ATTR_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3));
            cmd->setData(m_vTangents);
            cmd->ready(GPUOperateType::GOT_Update);
            GPUOperateSystem::getInstance()->addCommand(cmd);
        }
        else
        {
            BROWSER_SET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_TANGENT)
        }
    }
    
    void Mesh::updateBoneIndices()
    {
        if (m_bCreated)
        {
            BROWSER_CLEAR_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_BONE_IDS)
            
            auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
            cmd->setMesh(this);
            cmd->setVertexAttribute(GLProgram::VERTEX_ATTR::VERTEX_ATTR_BONE_IDS, 4, GL_UNSIGNED_INT, GL_FALSE, sizeof(glm::uvec4));
            cmd->setData(m_vBoneIndices);
            cmd->ready(GPUOperateType::GOT_Update);
            GPUOperateSystem::getInstance()->addCommand(cmd);
        }
        else
        {
            BROWSER_SET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_BONE_IDS)
        }
    }
    
    void Mesh::updateBoneWeights()
    {
        if (m_bCreated)
        {
            BROWSER_CLEAR_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_BONE_WEIGHTS)
            
            auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
            cmd->setMesh(this);
            cmd->setVertexAttribute(GLProgram::VERTEX_ATTR::VERTEX_ATTR_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4));
            cmd->setData(m_vBoneWeights);
            cmd->ready(GPUOperateType::GOT_Update);
            GPUOperateSystem::getInstance()->addCommand(cmd);
        }
        else
        {
            BROWSER_SET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_BONE_WEIGHTS)
        }
    }
    
	void Mesh::addColorProperty(const std::string& propertyName, const glm::vec4& value)
	{
		auto itor = m_mColorProperties.find(propertyName);
		if (itor == m_mColorProperties.end())
		{
			m_mColorProperties.emplace(propertyName, value);
		}
		else
		{
			itor->second = value;
		}
	}

    void Mesh::fillVertexsParam(GLuint location, void* data)
    {
        switch (location) {
            case GLProgram::VERTEX_ATTR_POSITION:
                {
                    // 位置属性
                    ANALYSIS_ARRAY_DATA_VERTEX(data);
                }
                break;
                
            case GLProgram::VERTEX_ATTR_COLOR:
                {
                    // 颜色属性
					m_vColors.resize(m_uVertexCount);
                    ANALYSIS_ARRAY_DATA_VEC4(m_vColors, data);
                }
                break;
                
            case GLProgram::VERTEX_ATTR_TEX_COORD:
                {
                    // 纹理uv属性
					m_vTexcoords1.resize(m_uVertexCount);
                    ANALYSIS_ARRAY_DATA_TEXCOORD(m_vTexcoords1, data);
                }
                break;
                
            case GLProgram::VERTEX_ATTR_NORMAL:
                {
                    // 法线属性
					m_vNormals.resize(m_uVertexCount);
                    ANALYSIS_ARRAY_DATA_VEC3(m_vNormals, data);
                }
                break;
                
            case GLProgram::VERTEX_ATTR_TANGENT:
                {
                    // 切线属性
					m_vTangents.resize(m_uVertexCount);
                    ANALYSIS_ARRAY_DATA_VEC3(m_vTangents, data);
                }
                break;
                
                
            default:
                BROWSER_WARNING(true, "Can not find the location in vertex attribute, warning in function MeshFilter::analysisAttribute.");
                break;
        }
    }

	void Mesh::createGPUResource()
	{
		BROWSER_ASSERT(m_eResouceState == GRS_DataLoaded, "Mesh state must be GRS_DataLoaded, then it can be created on gpu");

        m_bCreated = true;
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
        cmd->setMesh(this);
        cmd->ready(GPUOperateType::GOT_Create);
        GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void Mesh::updateGPUResource()
	{
        if (BROWSER_GET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_POSITION))
        {
            updateVertices();
        }
        if (BROWSER_GET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_MAXCOUNT))
        {
            updateIndices();
        }
        if (BROWSER_GET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_TEX_COORD))
        {
            updateUVs();
        }
        if (BROWSER_GET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_COLOR))
        {
            updateColors();
        }
        if (BROWSER_GET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_NORMAL))
        {
            updateNormals();
        }
        if (BROWSER_GET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_TANGENT))
        {
            updateTangents();
        }
        if (BROWSER_GET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_BONE_IDS))
        {
            updateBoneIndices();
        }
        if (BROWSER_GET_BIT(m_uPropertiesDirty, GLProgram::VERTEX_ATTR::VERTEX_ATTR_BONE_WEIGHTS))
        {
            updateBoneWeights();
        }
	}

	void Mesh::deleteGPUResource()
	{
        BROWSER_ASSERT(m_eResouceState == GRS_Loaded, "Mesh state must be GRS_Loaded, then it can be deleted on gpu");
        
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
        cmd->setMesh(this);
        cmd->ready(GPUOperateType::GOT_Delete);
        GPUOperateSystem::getInstance()->addCommand(cmd);
	}

}
