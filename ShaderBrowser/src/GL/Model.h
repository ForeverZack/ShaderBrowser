#pragma once

#include <string>
#include <unordered_map>
#include <tuple>
#include <glad/glad.h>
#include "Common/Tools/Utils.h"
#include "Browser/Components/Mesh/Mesh.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Texture2D.h"
#include "Common/System/Cache/TextureCache.h"
#include "GL/GLProgram.h"
#include "Browser/Entitys/BaseEntity.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace browser;

namespace customGL
{
    // assimp默认参数
	#define DEFAULT_ASSIMP_FLAG  aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights
	#define DEFAULT_ASSIMP_ANIMATION_FLAG  aiProcess_LimitBoneWeights
    
    // 模型纹理数据结构
    struct MeshTextureData
    {
        // 纹理所属mesh
        browser::Mesh* mesh;
        // 纹理名称
        std::string filename;
        // 类型
        aiTextureType type;
        // uniform名称
        std::string uniformName;
        // 是否找到该纹理
        bool bFound;
        
        MeshTextureData(browser::Mesh* mesh, std::string name, aiTextureType type, std::string uniform, bool found=true)
            : mesh(mesh)
            , filename(name)
            , type(type)
            , uniformName(uniform)
            , bFound(found)
        {
        }

		MeshTextureData(const MeshTextureData&& data)
			: mesh(data.mesh)
			, filename(data.filename)
			, type(data.type)
			, uniformName(data.uniformName)
			, bFound(data.bFound)
		{

		}
    };
    
    class Model : public Reference
	{
	public:
        // aiProcess_Triangulate: 如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形
        // aiProcess_FlipUVs: 将在处理的时候翻转y轴的纹理坐标
        // aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线
        // aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用
        // aiProcess_OptimizeMeshes：和上个选项相反，它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化
		static Model* create(const char* fileName, shared_ptr<std::vector<std::string>> animFileNames);
        static Model* createAlone(std::string fileName, const std::vector<std::string>& animFiles, std::function<void(Model*)> success, unsigned int pFlags = DEFAULT_ASSIMP_FLAG);

        
	public:
		Model();
		~Model();
        
    public:
		// 生成一个新的entity
		BaseEntity* createNewEntity(const std::string& name);
        // 设置vao
        void setupMeshesVAO();
        // 加载纹理
        void loadTextures(const std::string& directory);
		// 计算模型动画
		// interpolateAnimation：是否插值动画（false使用前一帧, true在两帧间插值）
		void computeBonesTransform(aiAnimation* animation, float elapsedTime, std::unordered_map<aiNode*, aiMatrix4x4>& nodeTrans, std::vector<glm::mat4>& bonesMatrix, float speed = 1.0f, bool interpolateAnimation = true);
		void traverseNodeToComputeBonesTransform(aiNode* node, const aiMatrix4x4 parentMatrix, std::unordered_map<aiNode*, aiMatrix4x4>& nodeTrans, std::vector<glm::mat4>& bonesMatrix);
	private:
        // 初始化
		bool initWithFile(const char* fileName, const std::vector<std::string>& animFiles, unsigned int pFlags);
		// 加载动画
		void loadAnimations(const aiScene*& scene);
        // 递归遍历模型节点
        void traverseNode(aiNode* node, const aiScene*& scene);
        // 生成游戏内部使用的网格(aiMesh->Mesh)
        browser::Mesh* generateMesh(aiMesh* aiMesh, const aiScene*& scene, unsigned int boneOffset);
        // 读取纹理数据
        void readTextureData(browser::Mesh* mesh, aiMaterial* material, aiTextureType type, const char* uniformName = GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0]);

		// 递归遍历模型，生成Entity
		BaseEntity* traverseNodeAndCreateEntity(aiNode* node, BaseEntity* parent, BaseEntity* root);
        
    public:
        REGISTER_PROPERTY_CONSTREF_GET(std::string, m_sDirectory, Directory);
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<std::string>, m_vAnimationNames, AnimationNames);
        REGISTER_PROPERTY_GET_SET(std::function<void(Model*)>, m_oSuccessCallback, SuccessCallback)
        REGISTER_PROPERTY_GET(unsigned int, m_uBoneNum, BoneNum)
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<browser::Mesh*>, m_vMeshes, Meshes)
        aiAnimation* getAnimation(const std::string& animName)
        {
            for(auto itor=m_mAnimations.begin(); itor!=m_mAnimations.end(); ++itor)
            {
                const std::tuple<aiAnimation*, std::string>& animInfo = (*itor);
                const std::string& name = std::get<1>(animInfo);
                if(name == animName)
                {
                    return std::get<0>(animInfo);
                }
            }
            return nullptr;
        }
        
    private:
        // 生成的MeshFilter组件
        MeshFilter* m_oMeshFilter;
        
		// assimp加载器列表(除了网格的顶点数据外，还有很多其他的数据如动画等，如果释放场景，这些数据也会被释放，所以要一直留着，等删除模型时，在移除这些数据)
		std::vector<std::shared_ptr<Assimp::Importer>> m_vImporters;
		// 记录模型根节点
		aiNode* m_oRootNode;
        // 记录模型场景
        const aiScene* m_oScene;
        // 加载路径
        std::string m_sDirectory;
        // 加载完成标记
        bool m_bLoadSuccess;
        // 加载的纹理数量
        int m_iLoadTextureNum;
        // 模型载入成功回调
        std::function<void(Model*)> m_oSuccessCallback;
        
        // 网格模型队列
        std::vector<browser::Mesh*> m_vMeshes;
		// 模型动画队列
		std::vector<std::tuple<aiAnimation*, std::string>> m_mAnimations;
        std::vector<std::string> m_vAnimationNames;
        // 纹理队列
        std::vector<Texture2D*> m_vTextures;
        // 纹理数据(中转)
        std::vector<MeshTextureData> m_vMeshTextureData;


        // 是否有骨骼动画
        bool m_bHasSkeletonAnim;
		// 骨骼数量
		unsigned int m_uBoneNum;
		// 骨骼颜色？？
		std::vector<float> m_vBonesColor;
		// 骨骼id列表
		std::unordered_map<std::string, unsigned int> m_mBonesIdMap;
		// 骨骼列表
		std::vector<aiBone*> m_vBones;
		// 当前已记录的骨骼数量
		unsigned int m_uRecBoneOffset;
		// 
		// 辅助数据：（用来显示骨骼）
		// 骨骼顶点数据 
		std::vector<VertexData> m_vSkeletonVertices;
		// 骨骼索引
		std::vector<glm::uvec2> m_vSkieletonIndices;
	};
}

