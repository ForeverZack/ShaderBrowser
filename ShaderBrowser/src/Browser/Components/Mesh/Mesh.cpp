#include "Mesh.h"
#include "./../../../GL/GLProgram.h"
#include "./../../System/RenderSystem.h"
#include "../Render/Material.h"

using namespace customGL;

namespace browser
{
	Mesh* Mesh::create(int length)
    {
		Mesh* mesh = new Mesh();
        
		mesh->init(length);
        
        return mesh;
    }
    
	Mesh::Mesh()
        : m_uVAO(0)
        , m_bGenVAO(false)
        , m_uVertexCount(0)
        , m_uIndexCount(0)
        , m_sMaterialName(Material::DEFAULT_MATERIAL_NAME)
	{
        // 清空
        m_mVertexAttribDeclarations.clear();
        m_vIndices.clear();
        m_vVertices.clear();
        m_vTextures.clear();
	}

	Mesh::~Mesh()
	{
        // 删除vao
        glDeleteVertexArrays(1, &m_uVAO);
	}
    
    void Mesh::setupVAO()
    {
        if (!m_bGenVAO)
        {
            m_bGenVAO = true;
            // 生成vao
            glGenVertexArrays(1, &m_uVAO);
        }

		// 设置vao
        RenderSystem::getInstance()->setupVAO(m_uVAO, m_mVertexAttribDeclarations);
    }
    
    void Mesh::init(int length)
    {
        m_uVertexCount = length;
        m_vVertices.resize(length);
        for(int i=0; i<length; ++i)
        {
            VertexData data;
            m_vVertices[i] = data;
        }
    }
    
    void Mesh::addTexture(const std::string& uniformName, Texture2D* texture)
    {
        TextureData data = Utils::createTextureData(uniformName, texture);
        m_vTextures.push_back(data);
    }
    
    void Mesh::setIndicesInfo(GLushort* data, unsigned int length)
    {
        m_uIndexCount = length;
        m_vIndices.assign(data, data+length);
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
        fillVertexsParam(location, data);
        
        // 将顶点属性记录下来，用来设置vao
        VertexAttribDeclaration* declaration = Utils::createVertexAttribDeclaration(location, size, type, normalized, stride);
        m_mVertexAttribDeclarations.emplace(location, declaration);
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
    
    void Mesh::fillVertexsParam(GLuint location, void* data)
    {
        switch (location) {
            case GLProgram::VERTEX_ATTR_POSITION:
                {
                    // 位置属性
                    ANALYSIS_ARRAY_DATA_VEC4(position, data);
                }
                break;
                
            case GLProgram::VERTEX_ATTR_COLOR:
                {
                    // 颜色属性
                    ANALYSIS_ARRAY_DATA_VEC4(color, data);
                }
                break;
                
            case GLProgram::VERTEX_ATTR_TEX_COORD:
                {
                    // 纹理属性
                    ANALYSIS_ARRAY_DATA_VEC2(uv_main, data);
                }
                break;
                
                
            default:
                BROWSER_WARNING(true, "Can not find the location in vertex attribute, warning in function MeshFilter::analysisAttribute.");
                break;
        }
    }


}
