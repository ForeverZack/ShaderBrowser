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
#define MESH_VERTEX_ATTR_COUNT 7
    
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
            varName[i].z  = values[i].z;    \
        }

		// 默认网格名称
		static const char* DEFAULT_MESH_NAME;
        
    public:
        // 逻辑线程调用，无需手动调用createMeshOnGPU
		static Mesh* create(int length, const std::string& meshName = DEFAULT_MESH_NAME, MeshType type = MeshType::CommonMesh);
        
        // 非逻辑线程创建mesh的时候用的，需要手动调用createMeshOnGPU
		static Mesh* createAsync(int length, const std::string& meshName = DEFAULT_MESH_NAME, MeshType type = MeshType::CommonMesh);
        
	public:
        Mesh(const std::string& meshName = DEFAULT_MESH_NAME, MeshType type = MeshType::CommonMesh, bool isRetain = false);
		~Mesh();
		friend class GPUOperateMeshCommand;

	public:
        // 初始化
        void init(int length);
        void initBonesData();
        // 在GPU上创建Mesh
        void createMeshOnGPU();
        // 添加纹理属性
        void addTexture(const std::string& uniformName, Texture2D* texture);
        void setTexture(const std::string& uniformName, Texture2D* texture);
		// 添加材质颜色属性
		void addColorProperty(const std::string& propertyName, const glm::vec4& value);

		// 设置属性：
        // 顶点
		void setVertices(void* data);
        // 索引
        void setIndices(GLushort* data, unsigned int length);
        // 主纹理uv1
        void setUVs(void* data);
        // 顶点颜色
        void setColors(void* data);
        // 法线
        void setNormals(void* data);
        // 切线
        void setTangents(void* data);
        // 骨骼索引
        void setBoneIndices(const std::vector<glm::uvec4>& boneIndices);
        // 骨骼权重
        void setBoneWeights(const std::vector<glm::vec4>& boneWeights);

	protected:
		// 创建gpu资源
		virtual void createGPUResource();
		// 更新gpu资源
		virtual void updateGPUResource();
		// 删除gpu资源
		virtual void deleteGPUResource();
        
        // 更新属性:
        void updateVertices();
        void updateIndices();
        void updateUVs();
        void updateColors();
        void updateNormals();
        void updateTangents();
        void updateBoneIndices();
        void updateBoneWeights();

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
        REGISTER_PROPERTY_GET(glm::vec4*, &m_vTexcoords1[0], Texcoords1)
        REGISTER_PROPERTY_GET(glm::vec3*, &m_vNormals[0], Normals)
        REGISTER_PROPERTY_GET(glm::vec3*, &m_vTangents[0], Tangents)
        REGISTER_PROPERTY_REF_GET(std::vector<glm::uvec4>, m_vBoneIndices, BoneIndicesRef)
        REGISTER_PROPERTY_REF_GET(std::vector<glm::vec4>, m_vBoneWeights, BoneWeightsRef)
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
        std::vector<glm::vec4> m_vTexcoords1;
        // 法线
        std::vector<glm::vec3> m_vNormals;
        // 切线
        std::vector<glm::vec3> m_vTangents;
        // 骨骼ID索引  (一个顶点最多被4根骨骼所影响)
        std::vector<glm::uvec4> m_vBoneIndices;
        // 骨骼权重
		std::vector<glm::vec4> m_vBoneWeights;
        
        // 创建脏标记
        bool m_bCreated;
        // 属性脏标记
        unsigned int m_uPropertiesDirty;
        
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
        
        

        // vao
        unsigned int m_uVAO;
        // 是否生成vao
        bool m_bGenVAO;
        // vbo
        unsigned int m_uVBOs[MESH_VERTEX_ATTR_COUNT];
		// 索引buffer
		unsigned int m_uIndicesVBO;
	};
}

