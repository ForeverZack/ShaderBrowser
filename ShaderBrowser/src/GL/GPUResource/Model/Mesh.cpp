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
        
        return mesh;
    }

	Mesh* Mesh::createRetain(int length, const std::string& meshName /*= DEFAULT_MESH_NAME*/, MeshType type /*= MeshType::CommonMesh*/)
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
	{
		this->autorelease();
		if (isRetain)
		{
			this->retain();
		}

        // 清空
        m_mVertexAttribDeclarations.clear();
        m_mTextures.clear();
	}

	Mesh::~Mesh()
	{
		deleteGPUResource();

        // 删除vao
        glDeleteVertexArrays(1, &m_uVAO);
        
        // 删除vbo
        glDeleteBuffers(MESH_VERTEX_ATTR_COUNT, m_uVBOs);
        
        for (auto itor=m_mVertexAttribDeclarations.begin(); itor!=m_mVertexAttribDeclarations.end(); ++itor)
        {
            BROWSER_SAFE_RELEASE_POINTER(itor->second);
        }
        m_mVertexAttribDeclarations.clear();
	}
    
    void Mesh::setupVAO()
    {
		createGPUResource();

        if (!m_bGenVAO)
        {
            m_bGenVAO = true;
            // 生成vao
            glGenVertexArrays(1, &m_uVAO);
			// 生成vbo
			glGenBuffers(MESH_VERTEX_ATTR_COUNT, m_uVBOs);
			glGenBuffers(1, &m_uIndicesVBO);

        }

		// 设置vao
        browser::RenderSystem::getInstance()->setupVAO(m_uVAO, m_uVBOs, m_mVertexAttribDeclarations);
        
		// 绑定数据
		// 1.绑定对应的vao
		glBindVertexArray(m_uVAO);

		// 2.传递顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[GLProgram::VERTEX_ATTR_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, getVertexCount() * sizeof(glm::vec4), &m_vVertices[0], GL_STATIC_DRAW);

		// 3.传递索引数组
        if (m_uIndexCount > 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uIndicesVBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*getIndexCount(), &m_vIndices[0], GL_STATIC_DRAW);
		}
        
        // uv
        if (m_vTexcoords1.size() > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[GLProgram::VERTEX_ATTR_TEX_COORD]);
            glBufferData(GL_ARRAY_BUFFER, getVertexCount() * sizeof(glm::vec2), &m_vTexcoords1[0], GL_STATIC_DRAW);
        }
        
        // colors
        if(m_vColors.size() > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[GLProgram::VERTEX_ATTR_COLOR]);
            glBufferData(GL_ARRAY_BUFFER, getVertexCount() * sizeof(glm::vec4), &m_vColors[0], GL_STATIC_DRAW);
        }
        
        // normal
        if (m_vNormals.size() > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[GLProgram::VERTEX_ATTR_NORMAL]);
            glBufferData(GL_ARRAY_BUFFER, getVertexCount() * sizeof(glm::vec3), &m_vNormals[0], GL_STATIC_DRAW);
        }
        
        // tangents
        if (m_vTangents.size() > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[GLProgram::VERTEX_ATTR_TANGENT]);
            glBufferData(GL_ARRAY_BUFFER, getVertexCount() * sizeof(glm::vec3), &m_vTangents[0], GL_STATIC_DRAW);
        }
                                                  
        // boneIndices
        if (m_vBoneIndices.size() > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[GLProgram::VERTEX_ATTR_BONE_IDS]);
            glBufferData(GL_ARRAY_BUFFER, getVertexCount() * sizeof(glm::uvec4), &m_vBoneIndices[0], GL_STATIC_DRAW);
        }
        
        // boneWeights
        if (m_vBoneWeights.size() > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[GLProgram::VERTEX_ATTR_BONE_WEIGHTS]);
            glBufferData(GL_ARRAY_BUFFER, getVertexCount() * sizeof(glm::vec4), &m_vBoneWeights[0], GL_STATIC_DRAW);
        }

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
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
    
    void Mesh::setIndicesInfo(GLushort* data, unsigned int length)
    {
        m_uIndexCount = length;
        
        m_vIndices.resize(length);
        for(int i=0; i<length; ++i)
        {
            m_vIndices[i] = data[i];
        }
    }
    
    void Mesh::setIndicesInfo(std::function<void(std::vector<GLushort>&, unsigned int&)> setFunc)
    {
        setFunc(m_vIndices, m_uIndexCount);
    }
    
    void Mesh::addVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* data)
    {
        BROWSER_ASSERT(m_uVertexCount>0, "MeshFilter cannot add vertex attribute before init, break in function MeshFilter::addVertexAttribute.");
        
        // 移除旧值
        auto itor = m_mVertexAttribDeclarations.find(location);
        if (itor != m_mVertexAttribDeclarations.end())
        {
            m_mVertexAttribDeclarations.erase(itor);
        }
        
        // 填充数据
		if (data)
		{
			fillVertexsParam(location, data);
		}
        
        // 将顶点属性记录下来，用来设置vao
        VertexAttribDeclaration* declaration = Utils::createVertexAttribDeclaration(location, size, type, normalized, stride);
        m_mVertexAttribDeclarations.emplace(location, declaration);
    }

	void Mesh::setVertices(void* data)
	{
		fillVertexsParam(GLProgram::VERTEX_ATTR_POSITION, data);

		auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateMeshCommand>(GPUOperateCommandType::GOCT_Mesh);
		cmd->setMesh(this);
		cmd->setVertexAttribute(GLProgram::VERTEX_ATTR::VERTEX_ATTR_POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4));
		cmd->setData(m_vVertices);
		cmd->ready(GPUOperateType::GOT_Update);
		GPUOperateSystem::getInstance()->addCommand(cmd);
	}
    
    VertexAttribDeclaration* Mesh::getVertexAttribDeclaration(GLuint location)
    {
        const auto itr = m_mVertexAttribDeclarations.find(location);
        if (itr!=m_mVertexAttribDeclarations.end())
        {
            return itr->second;
        }
        return nullptr;
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
					m_eResouceState = GRS_DataLoaded;
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

	}

	void Mesh::updateGPUResource()
	{

	}

	void Mesh::deleteGPUResource()
	{

	}

}
