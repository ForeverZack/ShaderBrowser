#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include "Browser/Components/BaseComponent.h"
#include "Common/Tools/Utils.h"
#include "GL/GLDefine.h"
//#include "Browser/System/RenderSystem.h"

#include <assimp/scene.h>

using namespace browser;

namespace customGL
{
    // mesh的顶点属性数量
#define MESH_VERTEX_ATTR_COUNT 8
    
    class Mesh : public Reference
	{
    public:
        enum MeshType
        {
            // 普通网格模型
            CommonMesh = 0,
            // 含骨骼的网格模型
            MeshWithBone,
        };
        
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
            varName[i].x = values[i].x;    \
            varName[i].y  = values[i].y;    \
            varName[i].z  = values[i].z;    \
        }
        
#define ANALYSIS_ARRAY_DATA_VEC4(varName, data)  \
        glm::vec4* values = (glm::vec4*)(data);  \
        for (int i=0; i<m_uVertexCount; ++i)    \
        {   \
            varName[i].x = values[i].x;    \
            varName[i].y  = values[i].y;    \
            varName[i].z  = values[i].z;    \
            varName[i].w  = values[i].w;    \
        }
        
#define ANALYSIS_ARRAY_DATA_VERTEX(data)  \
        glm::vec3* values = (glm::vec3*)(data);  \
        for (int i=0; i<m_uVertexCount; ++i)    \
        {   \
            m_vVertices[i].x = values[i].x;    \
            m_vVertices[i].y  = values[i].y;    \
            m_vVertices[i].z  = values[i].z;    \
            m_vVertices[i].w  = 1.0f;    \
        }
        
#define ANALYSIS_ARRAY_DATA_TEXCOORD(varName, data)  \
        glm::vec3* values = (glm::vec3*)(data);  \
        for (int i=0; i<m_uVertexCount; ++i)    \
        {   \
            varName[i].x = values[i].x;    \
            varName[i].y  = values[i].y;    \
        }

		// 默认网格名称
		static const char* DEFAULT_MESH_NAME;
        
    public:
        static Mesh* create(int length, const std::string& meshName = DEFAULT_MESH_NAME, MeshType type = MeshType::CommonMesh);
        
	public:
        Mesh(const std::string& meshName = DEFAULT_MESH_NAME, MeshType type = MeshType::CommonMesh);
		~Mesh();

	public:
        // 初始化
        void init(int length);
        void initBonesData();
        // 添加顶点属性(注意：如果重复添加相同的属性，会替换之前已有的)
        void addVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* data);
        // 设置顶点索引信息
        void setIndicesInfo(GLushort* data, unsigned int length);
        void setIndicesInfo(std::function<void(GLushort*&, unsigned int&)> setFunc);
        // 添加纹理属性
        void addTexture(const std::string& uniformName, Texture2D* texture);
        void setTexture(const std::string& uniformName, Texture2D* texture);
        // 根据location获取顶点属性
        VertexAttribDeclaration* getVertexAttribDeclaration(GLuint location);
		// 添加材质颜色属性
		void addColorProperty(const std::string& propertyName, const glm::vec4& value);
        
        // 向RenderSystem注册vao
        void setupVAO();
        
        REGISTER_PROPERTY_GET(MeshType, m_eMeshType, MeshType)
        REGISTER_PROPERTY_GET(unsigned int, m_uVAO, VAO)
        REGISTER_PROPERTY_GET(unsigned int, m_uVertexCount, VertexCount)
//        REGISTER_PROPERTY_CONSTREF_GET(std::vector<VertexData>, m_vVertices, Vertices)
//        REGISTER_PROPERTY_REF_GET(std::vector<VertexData>, m_vVertices, VerticesRef)
        REGISTER_PROPERTY_GET(unsigned int, m_uIndexCount, IndexCount)
//        REGISTER_PROPERTY_CONSTREF_GET(std::vector<GLushort>, m_vIndices, Indices)
        REGISTER_PROPERTY_GET_SET(std::string, m_sMaterialName, MaterialName)
		REGISTER_PROPERTY_GET(unsigned int*, m_uVBOs, VBOs)
		REGISTER_PROPERTY_CONSTREF_GET(std::string, m_sMeshName, MeshName)
        
        //
        REGISTER_PROPERTY_GET_SET(glm::vec4*, m_vVertices, Vertices22)
        REGISTER_PROPERTY_GET_SET(GLushort*, m_vIndices, Indices22)
        REGISTER_PROPERTY_GET_SET(glm::vec2*, m_vTexcoords1, Texcoords1)
        REGISTER_PROPERTY_GET_SET(glm::vec3*, m_vNormals, Normals)
        REGISTER_PROPERTY_GET_SET(glm::vec3*, m_vTangents, Tangents)
        REGISTER_PROPERTY_GET_SET(glm::uvec4*, m_vBoneIndices, BoneIndices)
        REGISTER_PROPERTY_GET_SET(glm::vec4*, m_vBoneWeights, BoneWeights)
        REGISTER_PROPERTY_REF_GET(glm::uvec4*, m_vBoneIndices, BoneIndicesRef)
        REGISTER_PROPERTY_REF_GET(glm::vec4*, m_vBoneWeights, BoneWeightsRef)
        
//        REGISTER_PROPERTY_CONSTREF_GET(std::unordered_map<std::string, TextureData>, m_mTextures, Textures);
        const std::unordered_map<std::string, TextureData>& getTextures()
        {
            return m_mTextures;
        }
		const std::unordered_map<std::string, glm::vec4>& getColorProperties()
		{
			return m_mColorProperties;
		}
        
    private:
        // 填充顶点数组的数据内容
        void fillVertexsParam(GLuint location, void* data);
        
        
        
	protected:
        // 顶点属性如下：
        // 位置
        glm::vec4* m_vVertices;
        // 索引数组
        GLushort* m_vIndices;
        // 颜色
        glm::vec4* m_vColors;
        // uv
        glm::vec2* m_vTexcoords1;
        // 法线
        glm::vec3* m_vNormals;
        // 切线
        glm::vec3* m_vTangents;
        // 骨骼ID索引  (一个顶点最多被4根骨骼所影响)
        glm::uvec4* m_vBoneIndices;
        // 骨骼权重
        glm::vec4* m_vBoneWeights;
        
        // 模型类型
        MeshType m_eMeshType;
		// 模型名称
		std::string m_sMeshName;
        // 顶点个数
        unsigned int m_uVertexCount;
        // 索引个数
        unsigned int m_uIndexCount;
		// 纹理数组
        std::unordered_map<std::string, TextureData> m_mTextures;
        // 材质名称
        std::string m_sMaterialName;
		// 材质颜色属性 (diffuse)
		std::unordered_map<std::string, glm::vec4> m_mColorProperties;
        
        

        // 顶点属性格式（设置vao时需要用到）
        std::unordered_map<GLuint, VertexAttribDeclaration*> m_mVertexAttribDeclarations;
        // vao
        unsigned int m_uVAO;
        // 是否生成vao
        bool m_bGenVAO;
        // vbo
        unsigned int m_uVBOs[MESH_VERTEX_ATTR_COUNT];
	};
}

