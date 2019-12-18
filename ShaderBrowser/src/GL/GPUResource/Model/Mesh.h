#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include "Common/Tools/Utils.h"
#include "GL/GLDefine.h"
#include "GL/GPUResource/BaseGPUResource.h"
#include "GL/GPUOperateCommand/GPUOperateMeshCommand.h"

#include <assimp/scene.h>

namespace customGL
{
    // mesh的顶点属性数量
#define MESH_VERTEX_ATTR_COUNT 8
    
    class Mesh : public BaseGPUResource
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
		static Mesh* createRetain(int length, const std::string& meshName = DEFAULT_MESH_NAME, MeshType type = MeshType::CommonMesh);
        
	public:
        Mesh(const std::string& meshName = DEFAULT_MESH_NAME, MeshType type = MeshType::CommonMesh, bool isRetain = false);
		~Mesh();
		friend class GPUOperateMeshCommand;

	public:
        // 初始化
        void init(int length);
        void initBonesData();
        // 添加顶点属性(注意：如果重复添加相同的属性，会替换之前已有的)
        void addVertexAttribute(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* data);
        // 设置顶点索引信息
        void setIndicesInfo(GLushort* data, unsigned int length);
		void setIndicesInfo(std::function<void(std::vector<GLushort>&, unsigned int&)> setFunc);
        // 添加纹理属性
        void addTexture(const std::string& uniformName, Texture2D* texture);
        void setTexture(const std::string& uniformName, Texture2D* texture);
        // 根据location获取顶点属性
        VertexAttribDeclaration* getVertexAttribDeclaration(GLuint location);
		// 添加材质颜色属性
		void addColorProperty(const std::string& propertyName, const glm::vec4& value);
        
        // 向RenderSystem注册vao
        void setupVAO();

	protected:
		// 创建gpu资源
		virtual void createGPUResource();
		// 更新gpu资源
		virtual void updateGPUResource();
		// 删除gpu资源
		virtual void deleteGPUResource() ;

		// 设置数据
		//void analysisArrayData(std::vector<glm::vec4>& vec, void* data);
        
		REGISTER_PROPERTY_GET_SET(unsigned int, m_uMeshId, MeshId)
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
		REGISTER_PROPERTY_GET(glm::vec4*, &m_vVertices[0], Vertices22)
        REGISTER_PROPERTY_GET(glm::vec2*, &m_vTexcoords1[0], Texcoords1)
        REGISTER_PROPERTY_GET(glm::vec3*, &m_vNormals[0], Normals)
        REGISTER_PROPERTY_GET(glm::vec3*, &m_vTangents[0], Tangents)
		REGISTER_PROPERTY_REF_GET(std::vector<glm::uvec4>, m_vBoneIndices, BoneIndicesRef)
		REGISTER_PROPERTY_REF_GET(std::vector<glm::vec4>, m_vBoneWeights, BoneWeightsRef)
        
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
		// id
		unsigned int m_uMeshId;

        // 顶点属性如下：
        // 位置
        std::vector<glm::vec4> m_vVertices;
        // 索引数组
        std::vector<GLushort> m_vIndices;
        // 颜色
        std::vector<glm::vec4> m_vColors;
        // uv
        std::vector<glm::vec2> m_vTexcoords1;
        // 法线
        std::vector<glm::vec3> m_vNormals;
        // 切线
        std::vector<glm::vec3> m_vTangents;
        // 骨骼ID索引  (一个顶点最多被4根骨骼所影响)
        std::vector<glm::uvec4> m_vBoneIndices;
        // 骨骼权重
		std::vector<glm::vec4> m_vBoneWeights;
        
        // 模型类型
        MeshType m_eMeshType;
		// 模型名称
		std::string m_sMeshName;
        // 顶点个数
        unsigned int m_uVertexCount;
        // 索引个数
        unsigned int m_uIndexCount;


		// TODO: 这部分属性其实不应该属于Mesh的属性，只是读取Model时方便才用的，应该放到model的默认材质中去，后面记得改！！！
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

