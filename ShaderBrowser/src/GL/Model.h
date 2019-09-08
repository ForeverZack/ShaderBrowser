#pragma once

#include <string>
#include <unordered_map>
#include <tuple>
#include <memory>
#include <glad/glad.h>
#include "Common/Tools/Utils.h"
#include "Browser/Components/Mesh/Mesh.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Texture2D.h"
#include "Common/System/Cache/TextureCache.h"
#include "GL/GLProgram.h"
#include "GL/Skeleton.h"
#include "GL/SkeletonAnimation.h"
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
    
    // 模型动画需要向gpu传递的数据
    class ModelGpuAnimationData
    {
    public:
        // 动画在shader中的缓存枚举
        enum ModelGpuAnimBufferType {
            PositionKeys = 0,
            RotationKeys,
            RotationTimes,
            ScaleKeys,
            
            Max_Buffer_Count,
        };
        
    public:
        ModelGpuAnimationData(int boneNum);
        ~ModelGpuAnimationData();
        

        // 计算缓存大小
        void calculateBufferSize(const std::vector<std::tuple<aiAnimation*, std::string>>& animations, const std::unordered_map<std::string, unsigned int>& bonesIdMap, int& pos_keys_size, int& rot_keys_size, int& scal_keys_size);
        // 在gpu中创建数据缓存
        void generateDataBuffer(const std::vector<std::tuple<aiAnimation*, std::string>>& animations, const std::unordered_map<std::string, unsigned int>& bonesIdMap, const std::vector<glm::vec4>& bonesInitPos, const std::vector<glm::vec4>& bonesInitRot, const std::vector<glm::vec4>& bonesInitScal);
        
//         // 生成数据
//        void generateData(aiAnimation* animation, int boneNum, const std::unordered_map<std::string, unsigned int>& bonesIdMap);
//         // 在gpu中创建数据缓存
//        void generateDataBuffer(const std::unordered_map<std::string, unsigned int>& bonesIdMap);
        // 获取SamplerBuffer
        GLuint getSamplerBuffer(ModelGpuAnimBufferType type);
        
        const std::vector<int>& getContainsBones(aiAnimation* animation);
        const std::vector<glm::ivec3>& getTransBoneKeyframeCount(aiAnimation* animation);
        const glm::ivec3& getKeysOffset(aiAnimation* animation);
        REGISTER_PROPERTY_GET(std::vector<glm::vec4>, position_keys, PositionKeys)
        
//        REGISTER_PROPERTY_CONSTREF_GET(std::vector<int>, contains_bones, ContainsBones)
//        REGISTER_PROPERTY_CONSTREF_GET(std::vector<glm::ivec3>, trans_bone_keyframe_count, TransBoneKeyframeCount)
    
    protected:
        // 原始数据
        int src_boneNum;
        // 关键帧列表  (0～src_bone内保存着骨骼的原始变换信息)
        std::vector<glm::vec4> position_keys;
        std::vector<glm::vec4> rotation_keys;
        std::vector<float> rotation_times;
        std::vector<glm::vec4> scale_keys;
        // 上面的骨骼变换信息包含哪些骨骼 (id：boneId ; value: -1.不包含，即该骨骼节点没有发生变换 1~MAX_BONES.在所有发生变换的骨骼信息里排第几个(用来定位采样关键帧))
//        std::vector<int> contains_bones;    // 例如：[-1, 0, -1, -1, 1] 表示该动画只有boneId为1，4的骨骼发生了变换，采样位置为0，1。
        std::unordered_map<aiAnimation*, std::vector<int>> contains_bones_vec;
        // 每个发生变换的骨骼有多少关键帧 (用上面contains_bones的value定位，0.position关键帧数量 1.rotation关键帧数量 2.scale关键帧数量)
//        std::vector<glm::ivec3> trans_bone_keyframe_count;
        std::unordered_map<aiAnimation*, std::vector<glm::ivec3>> trans_bone_keyframe_count_vec;
        // 关键帧偏移量
        std::unordered_map<aiAnimation*, glm::ivec3> keys_offsets;
        
        // gpu相关
        GLuint vbos[ModelGpuAnimBufferType::Max_Buffer_Count];
        GLuint texs[ModelGpuAnimBufferType::Max_Buffer_Count];
        
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
        // 处理gpu相关数据（必须在含有opengl的主线程进行）
        void setupGpuData();
        // 加载纹理
        void loadTextures(const std::string& directory);
		// 计算模型动画
		// interpolateAnimation：是否插值动画（false使用前一帧, true在两帧间插值）
        // applyRootMotion: 是否允许根节点移动（参考unity）
        void computeBonesTransform(aiAnimation* animation, float elapsedTime, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale, bool interpolateAnimation = true, bool applyRootMotion = false);
        // 混合模型动画
        void blendBonesTransform(aiAnimation* befAnimation, float befElapsed, bool befInterpolate, aiAnimation* animation, float elapsedTime, bool interpolate, float blendWeight, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale, bool applyRootMotion = false);
		void traverseNodeToComputeBonesTransform(aiNode* node, const aiMatrix4x4 parentMatrix, std::unordered_map<aiNode*, aiMatrix4x4>& nodeTrans, std::vector<glm::mat4>& bonesMatrix);
        GLuint getGpuAnimSamplerBuffer(ModelGpuAnimationData::ModelGpuAnimBufferType type);
        const std::vector<int>& getContainsBones(aiAnimation* animation);
        const std::vector<glm::ivec3>& getTransBonesKeyframeCount(aiAnimation* animation);
        const glm::ivec3& getKeysOffset(aiAnimation* animation);

        
        // 绑定网格模型到单个骨骼
        bool bindMeshOnSingleBone(browser::Mesh* mesh, const std::string& boneName, const glm::mat4& transformation = GLM_MAT4_UNIT);
        bool bindMeshOnSingleBone(browser::Mesh* mesh, unsigned int boneIdx, const glm::mat4& transformation = GLM_MAT4_UNIT);
        
	private:
        // 初始化
		bool initWithFile(const char* fileName, const std::vector<std::string>& animFiles, unsigned int pFlags);
		// 加载动画
		void loadAnimations(const aiScene*& scene);
        // 递归遍历模型节点
        void traverseNode(aiNode* node, const aiScene*& scene);
        // 找到模型骨骼根节点
        void findModelRootBondNode(aiNode* node);
        // 记录骨骼节点的变换
        void recordBonesInitialTransform();
        // 生成游戏内部使用的网格(aiMesh->Mesh)
        browser::Mesh* generateMesh(aiMesh* aiMesh, const aiScene*& scene);
        // 读取纹理数据
        void readTextureData(browser::Mesh* mesh, aiMaterial* material, aiTextureType type, const char* uniformName = GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0]);
        // 计算到endNode空间下的变换矩阵，用来将模型空间下的网格坐标，转换到骨骼节点下。(应该只有含有骨骼的模型中，包含普通网格模型(没有绑定骨骼，只是单纯的放在骨骼节点下，需要用代码手动绑定)时才需要用到，因为这部分网格模型的顶点坐标是模型空间的，他们的原点是RootNode。如果自己单独创建了一个不相干的模型(有自己的原点)，则不需要这个空间变换)
        void calculateTransformMatrix(aiNode* node, aiNode* endNode, aiMatrix4x4& transformation);

		// 递归遍历模型，生成Entity
		BaseEntity* traverseNodeAndCreateEntity(aiNode* node, BaseEntity* parent = nullptr, BaseEntity* root = nullptr, Animator* animator = nullptr);
        // 获取共享材质
        Material* getSharedMaterial(Mesh* mesh, const std::string& materialName, const std::string& defaultProgramName);
        
    public:
        REGISTER_PROPERTY_CONSTREF_GET(std::string, m_sDirectory, Directory);
        REGISTER_PROPERTY_CONSTREF_GET(std::string, m_sFullPath, FullPath);
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<std::string>, m_vAnimationNames, AnimationNames);
        REGISTER_PROPERTY_GET_SET(std::function<void(Model*)>, m_oSuccessCallback, SuccessCallback)
        REGISTER_PROPERTY_GET(unsigned int, m_uBoneNum, BoneNum)
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<browser::Mesh*>, m_vMeshes, Meshes)
        REGISTER_PROPERTY_GET(std::shared_ptr<ModelGpuAnimationData>, m_oGpuAnimData, GpuAnimData)
        std::unordered_map<std::string, unsigned int>* getBonesIdMapPointer()
        {
            return m_oSkeleton->getBonesIdMapPointer();
        }
        aiAnimation* getAnimation(int animIdx)
        {
            BROWSER_ASSERT(animIdx<m_vAnimations.size(), "Animation index is out of range in function Model::getAnimation(int animIdx)");
            
            const std::tuple<aiAnimation*, std::string>& animInfo = m_vAnimations[animIdx];
            aiAnimation* animation = std::get<0>(animInfo);
            if (animation)
            {
                return animation;
            }
            return nullptr;
        }
        aiAnimation* getAnimation(const std::string& animName)
        {
            for(auto itor=m_vAnimations.begin(); itor!=m_vAnimations.end(); ++itor)
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
        browser::MeshFilter* m_oMeshFilter;
        
		// assimp加载器列表(除了网格的顶点数据外，还有很多其他的数据如动画等，如果释放场景，这些数据也会被释放，所以要一直留着，等删除模型时，在移除这些数据)
		std::vector<std::shared_ptr<Assimp::Importer>> m_vImporters;
		// 记录模型根节点
		aiNode* m_oRootNode;
        // 记录模型骨骼根节点
        aiNode* m_oRootBoneNode;
        // 记录模型场景
        const aiScene* m_oScene;
        // 加载路径
        std::string m_sDirectory;
        // 文件绝对路径
        std::string m_sFullPath;
        // 加载完成标记
        bool m_bLoadSuccess;
        // 加载的纹理数量
        int m_iLoadTextureNum;
        // 模型载入成功回调
        std::function<void(Model*)> m_oSuccessCallback;
        
        // 网格模型队列
        std::vector<browser::Mesh*> m_vMeshes;
        std::vector<std::tuple<aiMesh*, aiNode*>> m_vAiMeshes;
		// 模型动画队列
		std::vector<std::tuple<aiAnimation*, std::string>> m_vAnimations;
        std::vector<std::string> m_vAnimationNames;
        std::unordered_map<aiAnimation*, SkeletonAnimation*> m_mSkeletonAnimations;
        unsigned int m_uUnnamedAnimCount;
        // 模型动画数据
        std::shared_ptr<ModelGpuAnimationData> m_oGpuAnimData;
        // 纹理队列
        std::vector<Texture2D*> m_vTextures;
        // 纹理数据(中转)
        std::vector<MeshTextureData> m_vMeshTextureData;
        // 共享材质
        std::unordered_map<browser::Mesh*, std::vector<Material*>> m_mSharedMaterials;


        // 是否有骨骼动画
        bool m_bHasSkeletonAnim;
        // 骨架信息
        Skeleton* m_oSkeleton;
		// 骨骼数量
		unsigned int m_uBoneNum;
        
		// 
		// 辅助数据：（用来显示骨骼）
		// 骨骼顶点数据 
//        std::vector<VertexData> m_vSkeletonVertices;
		// 骨骼索引
//        std::vector<glm::uvec2> m_vSkieletonIndices;
	};
}

