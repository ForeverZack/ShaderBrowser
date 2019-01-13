#pragma once

#include <vector>
#include <unordered_map>
#include "../BaseComponent.h"
#include "../../../Common/Tools/Utils.h"
#include "../../../GL/GLDefine.h"

using namespace common;
using namespace customGL;

namespace browser
{
	class MeshFilter : public BaseComponent
	{
	public:
#define ANALYSIS_ARRAY_DATA_VEC2(varName, data)  \
        glm::vec2* values = (glm::vec2*)(data);  \
        for (int i=0; i<m_uVertexCount; ++i)    \
        {   \
            m_vVertices[i].varName.x = values[i].x;    \
            m_vVertices[i].varName.y  = values[i].y;    \
        }
        
#define ANALYSIS_ARRAY_DATA_VEC3(varName, data)  \
        glm::vec3* values = (glm::vec3*)(data);  \
        for (int i=0; i<m_uVertexCount; ++i)    \
        {   \
            m_vVertices[i].varName.x = values[i].x;    \
            m_vVertices[i].varName.y  = values[i].y;    \
            m_vVertices[i].varName.z  = values[i].z;    \
        }
        
#define ANALYSIS_ARRAY_DATA_VEC4(varName, data)  \
        glm::vec4* values = (glm::vec4*)(data);  \
        for (int i=0; i<m_uVertexCount; ++i)    \
        {   \
            m_vVertices[i].varName.x = values[i].x;    \
            m_vVertices[i].varName.y  = values[i].y;    \
            m_vVertices[i].varName.z  = values[i].z;    \
            m_vVertices[i].varName.w  = values[i].w;    \
        }
        
    public:
        static MeshFilter* create(int length);
        
	public:
		MeshFilter();
		~MeshFilter();

	public:
        // 初始化
        void init(int length);
        // 添加顶点属性(注意：如果重复添加相同的属性，会替换之前已有的)
        void addVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* data);
        // 设置顶点索引信息
        void setIndicesInfo(GLushort* data, unsigned int length);
        // 根据location获取顶点属性
        VertexAttribDeclaration* getVertexAttribDeclaration(GLuint location);
        
        // 向RenderSystem注册vao
        void setupVAO();
        
        REGISTER_PROPERTY_GET(unsigned int, m_uVAO, VAO);
        REGISTER_PROPERTY_GET(unsigned int, m_uVertexCount, VertexCount);
        REGISTER_PROPERTY_GET(std::vector<VertexData>, m_vVertices, Vertices);
        REGISTER_PROPERTY_GET(unsigned int, m_uIndexCount, IndexCount);
        REGISTER_PROPERTY_GET(std::vector<GLushort>, m_vIndices, Indices);
        
    private:
        // 填充顶点数组的数据内容
        void fillVertexsParam(GLuint location, void* data);
        
        
        
	protected:
        // 顶点个数
        unsigned int m_uVertexCount;
        // 顶点数组
        std::vector<VertexData> m_vVertices;
        // 索引个数
        unsigned int m_uIndexCount;
        // 索引数组
        std::vector<GLushort> m_vIndices;
        
		// 纹理数组
		//std::vector<TextureData>
        

        // 顶点属性格式（设置vao时需要用到）
        std::unordered_map<GLuint, VertexAttribDeclaration*> m_mVertexAttribDeclarations;
        // vao
        unsigned int m_uVAO;
        // 是否生成vao
        bool m_bGenVAO;
        
	};
}

