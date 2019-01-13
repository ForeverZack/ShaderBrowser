#include "MeshFilter.h"
#include "./../../../GL/GLProgram.h"
#include "./../../System/RenderSystem.h"

using namespace customGL;

namespace browser
{
    MeshFilter* MeshFilter::create(int length)
    {
        MeshFilter* meshFilter = new MeshFilter();
        
        meshFilter->init(length);
        meshFilter->autorelease();
        
        return meshFilter;
    }
    
	MeshFilter::MeshFilter()
        : m_uVAO(0)
        , m_bGenVAO(false)
        , m_uVertexCount(0)
        , m_uIndexCount(0)
	{
        // 组件所属系统
        m_eBelongSystem = SystemType::Mesh;
        
        
        // 清空
        m_mVertexAttribDeclarations.clear();
        m_vIndices.clear();
        m_vVertices.clear();
	}

	MeshFilter::~MeshFilter()
	{
        // 删除vao
        glDeleteVertexArrays(1, &m_uVAO);
	}
    
    void MeshFilter::setupVAO()
    {
        if (!m_bGenVAO)
        {
            m_bGenVAO = true;
            // 生成vao
            glGenVertexArrays(1, &m_uVAO);
        }

        RenderSystem::getInstance()->setupVAO(m_uVAO, m_mVertexAttribDeclarations);
    }
    
    void MeshFilter::init(int length)
    {
        m_uVertexCount = length;
        m_vVertices.resize(length);
        for(int i=0; i<length; ++i)
        {
            VertexData data;
            m_vVertices[i] = data;
        }
    }
    
    void MeshFilter::setIndicesInfo(GLushort* data, unsigned int length)
    {
        m_uIndexCount = length;
        m_vIndices.assign(data, data+length);
    }
    
    void MeshFilter::addVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* data)
    {
        BROWSER_ASSERT(m_uVertexCount>0, "MeshFilter cannot add vertex attribute before init, break in function MeshFilter::addVertexAttribute.");
        
        // 填充数据
        fillVertexsParam(location, data);
        
        // 将顶点属性记录下来，用来设置vao
        VertexAttribDeclaration* declaration = new VertexAttribDeclaration();
        declaration->index = location;
        declaration->size = size;
        declaration->type = type;
        declaration->normalized = normalized;
        declaration->stride = stride;
        m_mVertexAttribDeclarations.emplace(location, declaration);
    }
    
    VertexAttribDeclaration* MeshFilter::getVertexAttribDeclaration(GLuint location)
    {
        const auto itr = m_mVertexAttribDeclarations.find(location);
        if (itr!=m_mVertexAttribDeclarations.end())
        {
            return itr->second;
        }
        return nullptr;
    }
    
    void MeshFilter::fillVertexsParam(GLuint location, void* data)
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
