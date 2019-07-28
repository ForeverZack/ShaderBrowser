#include "Model.h"
#include "Common/Tools/FileUtils.h"
#include "Common/System/Cache/MaterialCache.h"
#include "GL/Assimp.h"
#include "Rescale.h"
#include "Browser/Components/Transform/Transform.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Browser/Components/BoundBox/AABBBoundBox.h"
#include "Browser/Components/Render/BaseRender.h"
#include "Browser/Components/Render/SkinnedMeshRenderer.h"
#include "Browser/Components/Animation/Animator.h"
#include <queue>


using namespace common;

namespace customGL
{
    
    ModelGpuAnimationData::ModelGpuAnimationData(int boneNum)
        : src_boneNum(boneNum)
    {
    }
    
    ModelGpuAnimationData::~ModelGpuAnimationData()
    {
        glDeleteBuffers(ModelGpuAnimBufferType::Max_Buffer_Count, vbos);
        glDeleteTextures(ModelGpuAnimBufferType::Max_Buffer_Count, texs);
    }
    
    GLuint ModelGpuAnimationData::getSamplerBuffer(ModelGpuAnimBufferType type)
    {
        BROWSER_ASSERT(type<ModelGpuAnimBufferType::Max_Buffer_Count, "Can not find buffer in function ModelGpuAnimationData::getSamplerBuffer");
        return texs[type];
    }
    
    const std::vector<int>& ModelGpuAnimationData::getContainsBones(aiAnimation* animation)
    {
        auto itor = contains_bones_vec.find(animation);
        BROWSER_ASSERT(itor!=contains_bones_vec.end(), "Cannot find contains_bones_vec in function ModelGpuAnimationData::getContainsBones, please check your code.");
        return itor->second;
    }
    
    const std::vector<glm::ivec3>& ModelGpuAnimationData::getTransBoneKeyframeCount(aiAnimation* animation)
    {
        auto itor = trans_bone_keyframe_count_vec.find(animation);
        BROWSER_ASSERT(itor!=trans_bone_keyframe_count_vec.end(), "Cannot find trans_bone_keyframe_count_vec in function ModelGpuAnimationData::getTransBoneKeyframeCount, please check your code.");
        return itor->second;
    }
    
    const glm::ivec3& ModelGpuAnimationData::getKeysOffset(aiAnimation* animation)
    {
        auto itor = keys_offsets.find(animation);
        BROWSER_ASSERT(itor!=keys_offsets.end(), "Cannot find keys_offsets in function ModelGpuAnimationData::getKeysOffset, please check your code.");
        return itor->second;
    }
//    void ModelGpuAnimationData::generateData(aiAnimation* animation, int boneNum, const std::unordered_map<std::string, unsigned int>& bonesIdMap)
//    {
//        contains_bones.resize(boneNum);
//        contains_bones.insert(contains_bones.begin(), boneNum, -1);   // 全部重置为-1
//        trans_bone_keyframe_count.resize(animation->mNumChannels);
//        
//        int pos_keys_size=0, rot_keys_size=0, scal_keys_size=0;
//        int boneId;
//        //            std::unordered_map<std::string, unsigned int>::iterator itor;
//        aiNodeAnim* channel = nullptr;
//        for (int i=0; i<animation->mNumChannels; ++i)
//        {
//            channel = animation->mChannels[i];
//            
//            // test 测试是否按照id排序 (非按照id排序！！)
//            auto itor = bonesIdMap.find(channel->mNodeName.C_Str());
//            if (itor != bonesIdMap.end())
//            {
//                boneId = itor->second;
//                std::cout<<"insert bone id = "<<boneId<<endl;
//                
//                contains_bones[boneId] = i;
//                trans_bone_keyframe_count[i][0] = channel->mNumPositionKeys;
//                trans_bone_keyframe_count[i][1] = channel->mNumRotationKeys;
//                trans_bone_keyframe_count[i][2] = channel->mNumScalingKeys;
//                
//                pos_keys_size += channel->mNumPositionKeys;
//                rot_keys_size += channel->mNumRotationKeys;
//                scal_keys_size += channel->mNumScalingKeys;
//            }
//        }
//        
//        position_keys.resize(pos_keys_size);
//        rotation_keys.resize(rot_keys_size);
//        rotation_times.resize(rot_keys_size);
//        scale_keys.resize(scal_keys_size);
//        
//        
//        int pos_idx=0, rot_idx=0, scal_idx=0;
//        for (int i=0; i<animation->mNumChannels; ++i)
//        {
//            channel = animation->mChannels[i];
//            
//            // test 测试是否按照id排序 (非按照id排序！！)
//            auto itor = bonesIdMap.find(channel->mNodeName.C_Str());
//            if (itor != bonesIdMap.end())
//            {
//                // position
//                for (int j=0; j<channel->mNumPositionKeys; ++j)
//                {
//                    position_keys[pos_idx] = std::move(glm::vec4(channel->mPositionKeys[j].mValue.x, channel->mPositionKeys[j].mValue.y, channel->mPositionKeys[j].mValue.z, channel->mPositionKeys[j].mTime));
//                    ++pos_idx;
//                }
//                // rotation
//                for (int j=0; j<channel->mNumRotationKeys; ++j)
//                {
//                    rotation_keys[rot_idx] = std::move(glm::vec4(channel->mRotationKeys[j].mValue.x, channel->mRotationKeys[j].mValue.y, channel->mRotationKeys[j].mValue.z, channel->mRotationKeys[j].mValue.w));
//                    rotation_times[rot_idx] = channel->mRotationKeys[j].mTime;
//                    ++rot_idx;
//                }
//                // scale
//                for (int j=0; j<channel->mNumScalingKeys; ++j)
//                {
//                    scale_keys[scal_idx] = std::move(glm::vec4(channel->mScalingKeys[j].mValue.x, channel->mScalingKeys[j].mValue.y, channel->mScalingKeys[j].mValue.z, channel->mScalingKeys[j].mTime));
//                    ++scal_idx;
//                }
//            }
//            
//        }
//    }
//    
//    void ModelGpuAnimationData::generateDataBuffer(const std::unordered_map<std::string, unsigned int>& bonesIdMap)
//    {
//        // 生成数据
//        generateData(src_animation, src_boneNum, bonesIdMap);
//        
//        // 生成buffer
//        glGenTextures(ModelGpuAnimBufferType::Max_Buffer_Count, texs);
//        glGenBuffers(ModelGpuAnimBufferType::Max_Buffer_Count, vbos);
//        // position keys
//        glBindBuffer(GL_TEXTURE_BUFFER, vbos[ModelGpuAnimBufferType::PositionKeys]);
//        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4)*position_keys.size(), &position_keys[0], GL_STATIC_DRAW);
//        glBindTexture(GL_TEXTURE_BUFFER, texs[ModelGpuAnimBufferType::PositionKeys]);
//        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[ModelGpuAnimBufferType::PositionKeys]);
//        // rotation keys
//        glBindBuffer(GL_TEXTURE_BUFFER, vbos[ModelGpuAnimBufferType::RotationKeys]);
//        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4)*rotation_keys.size(), &rotation_keys[0], GL_STATIC_DRAW);
//        glBindTexture(GL_TEXTURE_BUFFER, texs[ModelGpuAnimBufferType::RotationKeys]);
//        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[ModelGpuAnimBufferType::RotationKeys]);
//        // rotation times
//        glBindBuffer(GL_TEXTURE_BUFFER, vbos[ModelGpuAnimBufferType::RotationTimes]);
//        glBufferData(GL_TEXTURE_BUFFER, sizeof(float)*rotation_times.size(), &rotation_times[0], GL_STATIC_DRAW);
//        glBindTexture(GL_TEXTURE_BUFFER, texs[ModelGpuAnimBufferType::RotationTimes]);
//        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, vbos[ModelGpuAnimBufferType::RotationTimes]);
//        // scale keys
//        glBindBuffer(GL_TEXTURE_BUFFER, vbos[ModelGpuAnimBufferType::ScaleKeys]);
//        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4)*scale_keys.size(), &scale_keys[0], GL_STATIC_DRAW);
//        glBindTexture(GL_TEXTURE_BUFFER, texs[ModelGpuAnimBufferType::ScaleKeys]);
//        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[ModelGpuAnimBufferType::ScaleKeys]);
//    }
    
    void ModelGpuAnimationData::calculateBufferSize(const std::vector<std::tuple<aiAnimation*, std::string>>& animations, const std::unordered_map<std::string, unsigned int>& bonesIdMap, int& pos_keys_size, int& rot_keys_size, int& scal_keys_size)
    {
        keys_offsets.clear();
        contains_bones_vec.clear();
        trans_bone_keyframe_count_vec.clear();
        pos_keys_size=0; rot_keys_size=0; scal_keys_size=0;
        
        int boneId;
        aiAnimation* animation = nullptr;
        aiNodeAnim* channel = nullptr;
        
        for (auto itor=animations.begin(); itor!=animations.end(); ++itor)
        {
            animation = std::get<0>(*itor);
            std::vector<int> contains_bone22;
            std::vector<glm::ivec3> trans_bone_keyframe_count22;
            contains_bone22.resize(src_boneNum);
//            contains_bone22.insert(contains_bone22.begin(), src_boneNum, -1);   // 全部重置为-1
            contains_bone22.assign(src_boneNum, -1);
            trans_bone_keyframe_count22.resize(animation->mNumChannels);
        
            for (int i=0; i<animation->mNumChannels; ++i)
            {
                channel = animation->mChannels[i];
                
                // test 测试是否按照id排序 (非按照id排序！！)
                auto boneItor = bonesIdMap.find(channel->mNodeName.C_Str());
                if (boneItor != bonesIdMap.end())
                {
                    boneId = boneItor->second;
//                    std::cout<<"insert bone id = "<<boneId<<endl;
                    
                    contains_bone22[boneId] = i;
                    trans_bone_keyframe_count22[i][0] = channel->mNumPositionKeys;
                    trans_bone_keyframe_count22[i][1] = channel->mNumRotationKeys;
                    trans_bone_keyframe_count22[i][2] = channel->mNumScalingKeys;
                    
                    pos_keys_size += channel->mNumPositionKeys;
                    rot_keys_size += channel->mNumRotationKeys;
                    scal_keys_size += channel->mNumScalingKeys;
                }
            }
            
            contains_bones_vec[animation] = std::move(contains_bone22);
            trans_bone_keyframe_count_vec[animation] = std::move(trans_bone_keyframe_count22);
        }
        
        position_keys.resize(pos_keys_size + src_boneNum);
        rotation_keys.resize(rot_keys_size + src_boneNum);
        rotation_times.resize(rot_keys_size + src_boneNum);
        scale_keys.resize(scal_keys_size + src_boneNum);
    }
    
    void ModelGpuAnimationData::generateDataBuffer(const std::vector<std::tuple<aiAnimation*, std::string>>& animations, const std::unordered_map<std::string, unsigned int>& bonesIdMap, const std::vector<glm::vec4>& bonesInitPos, const std::vector<glm::vec4>& bonesInitRot, const std::vector<glm::vec4>& bonesInitScal)
    {
        // 计算buffer大小
        int pos_keys_size, rot_keys_size, scal_keys_size;
        calculateBufferSize(animations, bonesIdMap, pos_keys_size, rot_keys_size, scal_keys_size);
        // 给骨骼初始变换赋值(拷贝)
        for (int i=0; i<src_boneNum; ++i)
        {
            position_keys[i] = bonesInitPos[i];
            rotation_keys[i] = bonesInitRot[i];
            scale_keys[i] = bonesInitScal[i];
        }
//        position_keys.assign(bonesInitPos.cbegin(), bonesInitPos.cend());
//        rotation_keys.assign(bonesInitRot.cbegin(), bonesInitRot.cend());
//        scale_keys.assign(bonesInitScal.cbegin(), bonesInitScal.cend());
        
        int pos_idx=src_boneNum, rot_idx=src_boneNum, scal_idx=src_boneNum;
        aiAnimation* animation = nullptr;
        aiNodeAnim* channel = nullptr;
        for (auto itor=animations.begin(); itor!=animations.end(); ++itor)
        {
            animation = std::get<0>(*itor);
            
            // 记录不同动画的uv偏移量 例如：第一个动画的偏移量为0, 第二个动画偏移量为第一个动画长度...
            keys_offsets[animation] = std::move(glm::ivec3(pos_idx, rot_idx, scal_idx));
            
            for (int i=0; i<animation->mNumChannels; ++i)
            {
                channel = animation->mChannels[i];
                
                // test 测试是否按照id排序 (非按照id排序！！)
                auto itor = bonesIdMap.find(channel->mNodeName.C_Str());
                if (itor != bonesIdMap.end())
                {
                    // position
                    for (int j=0; j<channel->mNumPositionKeys; ++j)
                    {
                        position_keys[pos_idx] = std::move(glm::vec4(channel->mPositionKeys[j].mValue.x, channel->mPositionKeys[j].mValue.y, channel->mPositionKeys[j].mValue.z, channel->mPositionKeys[j].mTime));
                        ++pos_idx;
                    }
                    // rotation
                    for (int j=0; j<channel->mNumRotationKeys; ++j)
                    {
                        rotation_keys[rot_idx] = std::move(glm::vec4(channel->mRotationKeys[j].mValue.x, channel->mRotationKeys[j].mValue.y, channel->mRotationKeys[j].mValue.z, channel->mRotationKeys[j].mValue.w));
                        rotation_times[rot_idx] = channel->mRotationKeys[j].mTime;
                        ++rot_idx;
                    }
                    // scale
                    for (int j=0; j<channel->mNumScalingKeys; ++j)
                    {
                        scale_keys[scal_idx] = std::move(glm::vec4(channel->mScalingKeys[j].mValue.x, channel->mScalingKeys[j].mValue.y, channel->mScalingKeys[j].mValue.z, channel->mScalingKeys[j].mTime));
                        ++scal_idx;
                    }
                }
    
            }
        }
        
        // 生成buffer
        glGenTextures(ModelGpuAnimBufferType::Max_Buffer_Count, texs);
        glGenBuffers(ModelGpuAnimBufferType::Max_Buffer_Count, vbos);
        // position keys
        glBindBuffer(GL_TEXTURE_BUFFER, vbos[ModelGpuAnimBufferType::PositionKeys]);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4)*position_keys.size(), &position_keys[0], GL_STATIC_DRAW);
        glBindTexture(GL_TEXTURE_BUFFER, texs[ModelGpuAnimBufferType::PositionKeys]);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[ModelGpuAnimBufferType::PositionKeys]);
        // rotation keys
        glBindBuffer(GL_TEXTURE_BUFFER, vbos[ModelGpuAnimBufferType::RotationKeys]);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4)*rotation_keys.size(), &rotation_keys[0], GL_STATIC_DRAW);
        glBindTexture(GL_TEXTURE_BUFFER, texs[ModelGpuAnimBufferType::RotationKeys]);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[ModelGpuAnimBufferType::RotationKeys]);
        // rotation times
        glBindBuffer(GL_TEXTURE_BUFFER, vbos[ModelGpuAnimBufferType::RotationTimes]);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(float)*rotation_times.size(), &rotation_times[0], GL_STATIC_DRAW);
        glBindTexture(GL_TEXTURE_BUFFER, texs[ModelGpuAnimBufferType::RotationTimes]);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, vbos[ModelGpuAnimBufferType::RotationTimes]);
        // scale keys
        glBindBuffer(GL_TEXTURE_BUFFER, vbos[ModelGpuAnimBufferType::ScaleKeys]);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4)*scale_keys.size(), &scale_keys[0], GL_STATIC_DRAW);
        glBindTexture(GL_TEXTURE_BUFFER, texs[ModelGpuAnimBufferType::ScaleKeys]);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[ModelGpuAnimBufferType::ScaleKeys]);
    }
    
    
    
    // ==================================== Model class ================= start ===================
    Model* Model::create(const char* fileName, shared_ptr<std::vector<std::string>> animFileNames)
    {
        Model* model = new Model();
        if (!model->initWithFile(fileName, *animFileNames, DEFAULT_ASSIMP_FLAG))
        {
            delete model;
            return nullptr;
        }
        
        return model;
    }
    
    Model* Model::createAlone(std::string fileName, const std::vector<std::string>& animFiles, std::function<void(Model*)> success, unsigned int pFlags /*= DEFAULT_ASSIMP_FLAG*/)
    {
        Model* model = new Model();
        model->m_oSuccessCallback = success;
        
        if (!model->initWithFile(fileName.c_str(), animFiles, pFlags))
        {
            delete model;
            return nullptr;
        }
        model->autorelease();
        
        return model;
    }
    
    Model::Model()
        : m_bLoadSuccess(false)
        , m_oSuccessCallback(nullptr)
        , m_iLoadTextureNum(0)
        , m_oMeshFilter(nullptr)
		, m_oRootNode(nullptr)
        , m_oRootBoneNode(nullptr)
        , m_oScene(nullptr)
        , m_bHasSkeletonAnim(false)
        , m_uUnnamedAnimCount(0)
	{
        // 清理
		m_vImporters.clear();
        m_vMeshes.clear();
        m_vAiMeshes.clear();
        m_vTextures.clear();
        m_vMeshTextureData.clear();
		m_vAnimations.clear();
        m_vAnimationNames.clear();
        m_mSharedMaterials.clear();
        m_oGpuAnimData = nullptr;
	}

	Model::~Model()
	{
        BROWSER_LOG("~Model");
        
        // release
        if (m_oMeshFilter)
        {
            m_oMeshFilter->release();
        }
        // 从cache中移除texture
        for(auto itor=m_vTextures.begin(); itor!=m_vTextures.end(); ++itor)
        {
            TextureCache::getInstance()->removeTexture(*itor);
        }
        // 删除mesh
        for(auto itor=m_vMeshes.begin(); itor!=m_vMeshes.end(); ++itor)
        {
            (*itor)->release();
        }
        m_vAiMeshes.clear();
		// 释放模型原始数据
		for (auto itor = m_vImporters.begin(); itor != m_vImporters.end(); ++itor)
		{
			const shared_ptr<Assimp::Importer>& importer = (*itor);
			importer->FreeScene();
		}
    }

	bool Model::initWithFile(const char* fileName, const std::vector<std::string>& animFiles, unsigned int pFlags)
	{
        // 加载模型（主模型文件）
		{
			// 获取文件全名和路径
			m_sFullPath = FileUtils::getInstance()->getAbsolutePathForFilename(fileName, m_sDirectory);

			std::shared_ptr<Assimp::Importer> importer = make_shared<Assimp::Importer>();
			// 设置importer的属性
            importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);    // 防止FBX自己生成枢轴，干扰Node结构树
//            importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_LIGHTS, false);
			const aiScene* scene = importer->ReadFile(m_sFullPath, pFlags);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				// 加载模型失败
				cout << "ERROR::ASSIMP::" << importer->GetErrorString() << endl;
				return false;
			}
            // 记录importer，否则场景数据会被析构
			m_vImporters.push_back(importer);
			// 初始化骨骼数据
			{
				m_uBoneNum = 0;
				m_mBonesIdMap.clear();
				m_vBones.clear();
			}
			// 加载模型网格数据
			{
                m_oScene = scene;
				m_oRootNode = scene->mRootNode;
				m_vMeshes.resize(scene->mNumMeshes);
                m_vAiMeshes.resize(scene->mNumMeshes);
				m_uRecBoneOffset = 0;
				traverseNode(scene->mRootNode, scene);
                
                m_uBoneNum = m_mBonesIdMap.size();
			}
            // 将骨骼树节点下的mesh绑定到骨骼 (在这里检测是否已经有网格模型绑定在骨骼上了。如果有，并且该网格模型没有绑定任何的骨骼，则执行网格的骨骼绑定)
            {
                aiMesh* aiMesh = nullptr;
                aiNode* meshNode = nullptr;
                aiNode* parentNode = nullptr;
                int index = 0;
                std::unordered_map<std::string, unsigned int>::iterator boneItor;
                for(auto itor=m_vAiMeshes.cbegin(); itor!=m_vAiMeshes.cend(); ++itor)
                {
                    aiMesh = std::get<0>(*itor);
                    meshNode = std::get<1>(*itor);
                    parentNode = meshNode->mParent;
                    aiMatrix4x4 globalTransformation;
                    
                    while(parentNode)
                    {
                        boneItor = m_mBonesIdMap.find(parentNode->mName.C_Str());
                        if(boneItor != m_mBonesIdMap.end())
                        {
                            unsigned int boneIdx = boneItor->second;
                            
                            browser::Mesh* mesh = m_vMeshes[index];
                            glm::vec4* boneWeights = mesh->getBoneWeights();
                            if (mesh->getVertexCount()>0 && mesh->getMeshType()==Mesh::MeshType::CommonMesh && (!boneWeights || boneWeights[0][0]==0.0f))
                            {
                                if(!boneWeights)
                                {
                                    mesh->initBonesData();
                                }
                                
                                // now：之前是将模型作为一个整体来计算变换的(boneMatrix)，其实并没有使用到Transform来计算骨骼矩阵，Transform对象的属性也不一定正确。现在改用Transform来计算，所以要绑定的网格只要将骨骼节点作为父节点即可
//                                calculateTransformMatrix(meshNode, parentNode, globalTransformation);
                                
                                // 绑定模型到骨骼
                                bindMeshOnSingleBone(mesh, boneIdx, Assimp::ConvertToGLM(globalTransformation));
                            }
                            
                            break;
                        }
                        parentNode = parentNode->mParent;
                    }
                    
                    ++index;
                }
            }
            
            // 加载模型动画数据
            loadAnimations(scene);
		}
		
		// 加载其余的动画文件（动画文件列表）
		{
			for (auto itor = animFiles.begin(); itor != animFiles.end(); ++itor)
			{
				std::shared_ptr<Assimp::Importer> importer = make_shared<Assimp::Importer>();
				// 设置importer的属性
//                importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);    // 防止FBX自己生成枢轴，干扰Node结构树
//                importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_LIGHTS, false);
                
                std::string full_path = *itor;
				const aiScene* scene = importer->ReadFile(full_path, aiProcess_GenSmoothNormals |
					aiProcess_LimitBoneWeights |
					aiProcess_FlipUVs);
				if (!scene || !scene->mRootNode)
				{
					// 加载模型失败
					cout << "ERROR::ASSIMP::" << importer->GetErrorString() << endl;
					return false;
				}
                // 记录importer，否则场景数据会被析构
                m_vImporters.push_back(importer);
                // 加载模型动画数据
                loadAnimations(scene);
			}
		}
        
        // 搜寻骨骼根节点
        findModelRootBondNode(m_oRootNode);

        // 加载创建纹理
        if (m_oSuccessCallback)
        {
            loadTextures(m_sDirectory);
        }
        
        // 记录骨骼初始变换
        recordBonesInitialTransform();
        // 创建gpu动画数据
        if (m_vAnimations.size() > 0)
        {
            m_oGpuAnimData = make_shared<ModelGpuAnimationData>(m_uBoneNum);
        }
        
		return true;
	}

	BaseEntity* Model::createNewEntity(const std::string& name)
	{
        BaseEntity* entity;
        if(m_bHasSkeletonAnim)
        {
            Animator* animator = new Animator();
            animator->setBoneInfo(m_uBoneNum);
            entity = traverseNodeAndCreateEntity(m_oRootNode, nullptr, nullptr, animator);
            entity->setModel(this);
            entity->addComponent(animator);
        }
        else
        {
            entity = traverseNodeAndCreateEntity(m_oRootNode);
            entity->setModel(this);
        }
		entity->setName(name);
		return entity;
	}

	BaseEntity* Model::traverseNodeAndCreateEntity(aiNode* node, BaseEntity* parent/* = nullptr*/, BaseEntity* root/* = nullptr*/, Animator* animator/* = nullptr*/)
	{
		BaseEntity* entity = BaseEntity::create(node->mName.C_Str());
		if (parent)
		{
			parent->addChild(entity);
		}
        if(!root)
        {
            root = entity;
        }
        entity->setModelRootEntity(root);
        
        // 设置当前node的变换
        {
            glm::vec3 position, scale;
            glm::quat rotation;
            Utils::parseMatrix(node->mTransformation, position, rotation, scale);
            auto itor = m_mBonesIdMap.find(node->mName.C_Str());
            if (itor != m_mBonesIdMap.end())
            {
                // 骨骼
                int boneId = itor->second;
                entity->setBoneInfo(boneId, Assimp::ConvertToGLM(m_vBones[boneId]->mOffsetMatrix));
                if (animator)
                {
                    animator->addBone(boneId, entity->getTransform());
                }
//                entity->setPosition(position.x, position.y, position.z);
//                entity->setQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
//                entity->setScale(scale.x, scale.y, scale.z);
            }
            entity->getTransform()->setSrcModelTransformation(position, rotation, scale);
        }
        
        bool skinned = false;
		if (node->mNumMeshes > 0)
		{
			// MeshFilter组件
            browser::MeshFilter* meshFilter = nullptr;
			// 渲染组件
			BaseRender* renderer = nullptr;
            BaseRender* skinnedRender = nullptr;
            
			for (unsigned int i = 0; i < node->mNumMeshes; ++i)
			{
				browser::Mesh* mesh = m_vMeshes[node->mMeshes[i]];
				if (mesh)
				{
                    // 检测是否需要蒙皮
                    if (mesh->getMeshType() == browser::Mesh::MeshType::MeshWithBone)
                    {
                        skinned = true;
                    }
                    
                    if(skinned)
                    {
                        if(!skinnedRender)
                        {
//                            skinnedRender = SkinnedMeshRenderer::createSkinnedMeshRenderer();
                            skinnedRender = SkinnedMeshRenderer::createSkinnedMeshRenderer(getSharedMaterial(mesh, mesh->getMaterialName(), GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME));
                            entity->addComponent(skinnedRender);
                        }
                        else
                        {
                            skinnedRender->addMaterial(getSharedMaterial(mesh, mesh->getMaterialName(), GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME));
                        }
                        static_cast<SkinnedMeshRenderer*>(skinnedRender)->addMesh(mesh);
                    }
                    else
                    {
                        if(!meshFilter)
                        {
                            meshFilter = MeshFilter::create();
                            entity->addComponent(meshFilter);
                        }
                        
                        if (!renderer)
                        {
//                            renderer = BaseRender::createBaseRender();
                            renderer = BaseRender::createBaseRender(getSharedMaterial(mesh, mesh->getMaterialName(), GLProgram::DEFAULT_GLPROGRAM_NAME));
                            entity->addComponent(renderer);
                        }
                        else
                        {
                            renderer->addMaterial(getSharedMaterial(mesh, mesh->getMaterialName(), GLProgram::DEFAULT_GLPROGRAM_NAME));
                        }
                        meshFilter->addMesh(mesh);
                    }

				}
			}
            
            if(!skinned)
            {
                // 包围盒组件
                entity->addComponent(new AABBBoundBox());
            }
            
		}
		
		// 继续遍历子节点
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			traverseNodeAndCreateEntity(node->mChildren[i], entity, root, animator);
		}

		return entity;
	}
    
    Material* Model::getSharedMaterial(Mesh* mesh, const std::string& materialName, const std::string& defaultProgramName)
    {
        Material* material = nullptr;
        
        auto mapItor = m_mSharedMaterials.find(mesh);
        if(mapItor == m_mSharedMaterials.end())
        {
            std::vector<Material*> vec;
            vec.clear();
            m_mSharedMaterials.emplace(mesh, std::move(vec));
        }
        
        const std::vector<Material*>& materials = m_mSharedMaterials[mesh];
        for(auto itor=materials.begin(); itor!=materials.end(); ++itor)
        {
            if ((*itor)->getMaterialName() == materialName)
            {
                material = (*itor);
                break;
            }
        }
        
        if (!material)
        {
            material = Material::createMaterial(defaultProgramName, materialName);
            MaterialCache::getInstance()->addSharedMaterial(material, true);
        }
        
        return material;
    }
    
    void Model::calculateTransformMatrix(aiNode* node, aiNode* endNode, aiMatrix4x4& transformation)
    {
        if(node != endNode)
        {
            transformation = node->mTransformation * transformation;
            calculateTransformMatrix(node->mParent, endNode, transformation);
        }
    }
    
    GLuint Model::getGpuAnimSamplerBuffer(ModelGpuAnimationData::ModelGpuAnimBufferType type)
    {
        BROWSER_ASSERT(m_oGpuAnimData!=nullptr, "Variable m_oGpuAnimaData is nullptr in function Model::getGpuAnimSamplerBuffer");
        return m_oGpuAnimData->getSamplerBuffer(type);
    }
    
    const std::vector<int>& Model::getContainsBones(aiAnimation* animation)
    {
        BROWSER_ASSERT(m_oGpuAnimData!=nullptr, "Variable m_oGpuAnimaData is nullptr in function Model::getContainsBones");
        return m_oGpuAnimData->getContainsBones(animation);
    }
    
    const std::vector<glm::ivec3>& Model::getTransBonesKeyframeCount(aiAnimation* animation)
    {
        BROWSER_ASSERT(m_oGpuAnimData!=nullptr, "Variable m_oGpuAnimaData is nullptr in function Model::getTransBonesKeyframeCount");
        return m_oGpuAnimData->getTransBoneKeyframeCount(animation);
    }
    
    const glm::ivec3& Model::getKeysOffset(aiAnimation* animation)
    {
        BROWSER_ASSERT(m_oGpuAnimData!=nullptr, "Variable m_oGpuAnimaData is nullptr in function Model::getKeysOffset");
        return m_oGpuAnimData->getKeysOffset(animation);
    }
    
    void Model::findModelRootBondNode(aiNode* node)
    {
        if(m_oRootBoneNode)
        {
            return;
        }
        
        if(m_mBonesIdMap.find(node->mName.C_Str())!= m_mBonesIdMap.end())
        {
            m_oRootBoneNode = node;
            return;
        }
        
        // 继续遍历子节点
        for (unsigned int i=0; i<node->mNumChildren; ++i)
        {
            findModelRootBondNode(node->mChildren[i]);
        }
    }
    
    void Model::recordBonesInitialTransform()
    {
        m_vBonesInitPosition.resize(m_uBoneNum);
        m_vBonesInitRotation.resize(m_uBoneNum);
        m_vBonesInitScale.resize(m_uBoneNum);
        
        aiNode* boneNode = nullptr;
        int index;
        for (auto itor=m_mBonesIdMap.begin(); itor!=m_mBonesIdMap.end(); ++itor)
        {
            const std::string& boneName = itor->first;
            boneNode = m_oRootNode->FindNode(boneName.c_str());
            
            index = itor->second;
            Utils::parseMatrix(boneNode->mTransformation, m_vBonesInitPosition[index], m_vBonesInitRotation[index], m_vBonesInitScale[index]);
        }
    }
    
    void Model::traverseNode(aiNode* node, const aiScene*& scene)
    {
        // 处理该节点所有的网格
		aiMesh* aiMesh = nullptr;
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh = scene->mMeshes[node->mMeshes[i]];
			browser::Mesh* mesh = generateMesh(aiMesh, scene, m_uRecBoneOffset);
			if (mesh)
			{
				m_vMeshes[node->mMeshes[i]] = mesh;
                m_vAiMeshes[node->mMeshes[i]] = make_tuple(aiMesh, node);
			}
		}
        
//        std::string nodeName(node->mName.C_Str());
//        if (FileUtils::getInstance()->getAbsolutePathForFilename("models/Fighter/fighterChar.FBX") == m_sFullPath)
//        {
//            int iii = 0;
//        }
//        if (nodeName.find("$AssimpFbx$") != -1)  // "Bip001_$AssimpFbx$_Translation"
//        {
//            auto boneIdItor = m_mBonesIdMap.find(nodeName);
//            if (boneIdItor == m_mBonesIdMap.end())
//            {
//                // 骨骼id要自己生成
//                aiBone* bone = new aiBone();
//                bone->mOffsetMatrix = node->mTransformation;
//                m_vBones.push_back(bone);
//                m_mBonesIdMap[nodeName] = m_uRecBoneOffset;
//                ++m_uRecBoneOffset;
//            }
//        }
        
        // 继续遍历子节点
        for (unsigned int i=0; i<node->mNumChildren; ++i)
        {
            traverseNode(node->mChildren[i], scene);
        }
        
    }
    
    browser::Mesh* Model::generateMesh(aiMesh* aiMesh, const aiScene*& scene, unsigned int& boneOffset)
    {
        // 根据aiMesh的id获取aiMesh
        if (aiMesh)
        {
            // 注意:如果纹理创建不成功(例如没有找到),应该有一张白色默认纹理来代替,以防程序出问题
            browser::Mesh* mesh = browser::Mesh::create(aiMesh->mNumVertices, aiMesh->mName.C_Str(), aiMesh->HasBones() ? browser::Mesh::MeshType::MeshWithBone : browser::Mesh::MeshType::CommonMesh );
            // 顶点位置
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), aiMesh->mVertices);
            // 顶点颜色
			if (aiMesh->mColors[0] && aiMesh->mColors[1])
			{
				mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), aiMesh->mColors);
			}
            // 顶点uv坐标
            if (aiMesh->mTextureCoords[0])
            {
                mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), aiMesh->mTextureCoords[0]);
            }
            // 法线
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), aiMesh->mNormals);
            // 切线
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), aiMesh->mTangents);
            // 索引信息
            mesh->setIndicesInfo([=](GLushort*& indices, unsigned int& indexCount) -> void
                 {
                     indexCount = 0;
                     for(int i=0; i<aiMesh->mNumFaces; ++i)
                     {
                         const aiFace& face = aiMesh->mFaces[i];
                         indexCount += face.mNumIndices;
                     }
                     
                     indices = new GLushort[indexCount];
                     
                     int now_index = 0;
                     for(int i=0; i<aiMesh->mNumFaces; ++i)
                     {
                         const aiFace& face = aiMesh->mFaces[i];
                         for(int j=0; j<face.mNumIndices; ++j)
                         {
                             indices[now_index] = face.mIndices[j];
                             ++now_index;
                         }
                         
                     }
                 });

			// 骨骼信息
            glm::uvec4* mesh_boneIndices = mesh->getBoneIndices();
            glm::vec4* mesh_boneWeights = mesh->getBoneWeights();
			aiBone* bone = nullptr;
			aiVertexWeight * vertexWeight = nullptr;
			unsigned int boneIdx;
            unsigned int currentBoneOffset = 0; // 当前骨骼id偏移，防止重复的骨骼
            std::unordered_map<std::string, unsigned int>::iterator boneIdItor;
			for (unsigned int i = 0; i < aiMesh->mNumBones; ++i)
			{
				bone = aiMesh->mBones[i];	// aiBone
                boneIdItor = m_mBonesIdMap.find(bone->mName.C_Str());
                if (boneIdItor != m_mBonesIdMap.end())
                {
                    // 骨骼已存在
                    boneIdx = boneIdItor->second;
                }
                else
                {
                    // 骨骼id要自己生成
                    boneIdx = boneOffset + currentBoneOffset;
                    ++currentBoneOffset;
                    m_vBones.push_back(bone);
                    m_mBonesIdMap[bone->mName.C_Str()] = boneIdx;
                }
				
				for (unsigned int w = 0; w < bone->mNumWeights; ++w)
				{
					vertexWeight = &(bone->mWeights[w]);
                    glm::uvec4& vertexBoneIndices = mesh_boneIndices[vertexWeight->mVertexId];
                    glm::vec4& vertexBoneWeights = mesh_boneWeights[vertexWeight->mVertexId];
					
					// 一个顶点最多能被4个骨骼控制，这里我们要在这个数组中找到第一个空着的骨骼信息
					for (unsigned int j = 0; j < 4; ++j)
					{
						if (vertexBoneWeights[j] != 0.0f)
						{
							continue;
						}

						vertexBoneIndices[j] = boneIdx;
						vertexBoneWeights[j] = vertexWeight->mWeight;
						break;
					}
				}
				
			}
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_BONE_IDS, 4, GL_UNSIGNED_INT, GL_FALSE, sizeof(glm::uvec4), nullptr);
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
            // 增加骨骼id偏移
            boneOffset += currentBoneOffset;

            
            // 处理材质(这里的材质指的是纹理)
            if (aiMesh->mMaterialIndex >= 0)
            {
                aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];
                // 材质名称
				mesh->setMaterialName(material->GetName().C_Str());
				// 读取材质默认属性
				aiColor4D color;
				if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS)
				{
					mesh->addColorProperty(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_ALBEDO_COLOR], Assimp::ConvertToGLM(color));
				}
                // 漫反射纹理: 主纹理   GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0]
                readTextureData(mesh, material, aiTextureType_DIFFUSE);
                // 镜面反射纹理
                readTextureData(mesh, material, aiTextureType_SPECULAR, GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE1]);
                
                
            }
            if (m_oSuccessCallback)
            {
                mesh->setupVAO();
            }
                
            return mesh;
        }
        
        return nullptr;
    }
    
    void Model::setupGpuData()
    {
        // 处理网格模型vao
        for(auto itor=m_vMeshes.begin(); itor!=m_vMeshes.end(); ++itor)
        {
            (*itor)->setupVAO();
        }
        // 处理动画数据
        if (m_oGpuAnimData != nullptr)
        {
            m_oGpuAnimData->generateDataBuffer(m_vAnimations, m_mBonesIdMap, m_vBonesInitPosition, m_vBonesInitRotation, m_vBonesInitScale);
        }
    }
    
    void Model::readTextureData(browser::Mesh* mesh, aiMaterial* material, aiTextureType type, const char* uniformName /*= GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0] */)
    {
        int textureCount = material->GetTextureCount(type);
        
        // 当加载不到模型的纹理时,用white_default代替
        if (textureCount == 0)
        {
            MeshTextureData textureData(mesh, "texture/default/default_white.png", type, uniformName, false);
            m_vMeshTextureData.push_back(std::move(textureData));
        }
        
        for(int i=0; i<textureCount; ++i)
        {
            aiString filename;
            material->GetTexture(type, i, &filename);
            
            MeshTextureData textureData(mesh, filename.C_Str(), type, uniformName);
            m_vMeshTextureData.push_back(std::move(textureData));
        }
    }

	void Model::loadAnimations(const aiScene*& scene)
	{
		if (scene->HasAnimations())
		{
			for (int i = 0; i < scene->mNumAnimations; ++i)
			{
				aiAnimation* animation = scene->mAnimations[i];
                std::string name(animation->mName.C_Str());
                if (name == "")
                {
                    name = Animator::DEFAULT_ANIMATION_NAME + std::to_string(m_uUnnamedAnimCount++);
                }
//                std::string name = Animator::DEFAULT_ANIMATION_NAME + std::to_string(m_uUnnamedAnimCount++);
				m_vAnimations.push_back(std::make_tuple(animation, name));
                m_vAnimationNames.push_back(name);
            }
            
            m_bHasSkeletonAnim = true;
		}
	}

	void Model::computeBonesTransform(aiAnimation* animation, float elapsedTime, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale, bool interpolateAnimation /*= true*/, bool applyRootMotion /*= false*/)
	{
		// 将采样范围变换到 [0, 1]
		float animSample = static_cast<float>(animation->mTicksPerSecond / animation->mDuration) * elapsedTime;
		animSample = std::min(animSample, 1.0f);

		aiMatrix4x4 transformation, scaleMat, translateMat;
		Rescale rescaler(0.0f, static_cast<float>(animation->mDuration), 0.0f, 1.0f);
		const float sampleUnscaled = rescaler.Unscale(animSample);  // 采样帧数位置(例如，第2.52帧)
        std::unordered_map<std::string, unsigned int>::iterator itor;
        unsigned int boneId;
        
//        BROWSER_LOG(sampleUnscaled)
		// 遍历骨骼变换信息，生成父节点坐标系下自身的变换矩阵
		{
			aiNodeAnim* channel = nullptr;
			aiNode* node = nullptr;
            // 遍历发生变换的骨骼节点的变换信息
			for (unsigned int i = 0; i < animation->mNumChannels; ++i)
			{
				channel = animation->mChannels[i];
				node = m_oRootNode->FindNode(channel->mNodeName);

                // 根据当前动画播放到第几帧，插值得到变换信息
				// 位移
				auto translation = Assimp::InterpolationGet<aiVector3D>(sampleUnscaled, channel->mPositionKeys, channel->mNumPositionKeys, interpolateAnimation);
//                if (!applyRootMotion && node==m_oRootBoneNode)
//                {
//                    translation = aiVector3D(0, 0, 0);
//                }
                // 旋转
				auto rotation = Assimp::InterpolationGet<aiQuaternion>(sampleUnscaled, channel->mRotationKeys, channel->mNumRotationKeys, interpolateAnimation);
				// 缩放
				auto scale = Assimp::InterpolationGet<aiVector3D>(sampleUnscaled, channel->mScalingKeys, channel->mNumScalingKeys, interpolateAnimation);
                // 记录骨骼变换
                itor = m_mBonesIdMap.find(channel->mNodeName.C_Str());
                if(itor != m_mBonesIdMap.end())
                {
                    boneId = itor->second;
                    bonesPosition[boneId] = std::move(Assimp::ConvertToGLM(translation));
                    bonesRotation[boneId] = std::move(Assimp::ConvertToGLM(rotation));
                    bonesScale[boneId] = std::move(Assimp::ConvertToGLM(scale));
                }

			}
		}
        
	}
    
    void Model::blendBonesTransform(aiAnimation* befAnimation, float befElapsed, bool befInterpolate, aiAnimation* animation, float elapsedTime, bool interpolate, float blendWeight, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale, bool applyRootMotion /*= false*/)
    {
        // 记录所有变换的node信息<位移, 旋转, 缩放>
        std::unordered_map<aiNode*, aiVector3D> translateVec;
        std::unordered_map<aiNode*, aiQuaternion> rotateVec;
        std::unordered_map<aiNode*, aiVector3D> scaleVec;
        std::unordered_map<std::string, unsigned int>::iterator itor;
        unsigned int boneId;
        
        // 遍历记录beforeAnimation骨骼变换信息
        {
            float animSample = static_cast<float>(befAnimation->mTicksPerSecond / befAnimation->mDuration) * befElapsed;
            animSample = std::min(animSample, 1.0f);

            aiMatrix4x4 transformation, scaleMat, translateMat;
            Rescale rescaler(0.0f, static_cast<float>(befAnimation->mDuration), 0.0f, 1.0f);
            const float sampleUnscaled = rescaler.Unscale(animSample);
            
        
            aiNodeAnim* channel = nullptr;
            aiNode* node = nullptr;
            for (unsigned int i = 0; i < befAnimation->mNumChannels; ++i)
            {
                channel = befAnimation->mChannels[i];
                node = m_oRootNode->FindNode(channel->mNodeName);
                // 位移
                auto translation = Assimp::InterpolationGet<aiVector3D>(sampleUnscaled, channel->mPositionKeys, channel->mNumPositionKeys, befInterpolate);
                if (!applyRootMotion && node==m_oRootBoneNode)
                {
                    translation = aiVector3D(0, 0, 0);
                }
                // 旋转
                const auto rotation = Assimp::InterpolationGet<aiQuaternion>(sampleUnscaled, channel->mRotationKeys, channel->mNumRotationKeys, befInterpolate);
                // 缩放
                const auto scale = Assimp::InterpolationGet<aiVector3D>(sampleUnscaled, channel->mScalingKeys, channel->mNumScalingKeys, befInterpolate);
                
                itor = m_mBonesIdMap.find(channel->mNodeName.C_Str());
                if(itor != m_mBonesIdMap.end())
                {
                    boneId = itor->second;
                    bonesPosition[boneId] = std::move(Assimp::ConvertToGLM(translation));
                    bonesRotation[boneId] = std::move(Assimp::ConvertToGLM(rotation));
                    bonesScale[boneId] = std::move(Assimp::ConvertToGLM(scale));
                }
            }
        }
        
        // 
        {
            // 插值函数
            const auto ipVect = Assimp::Interpolator<glm::vec3>();
            const auto ipQuat = Assimp::Interpolator<glm::quat>();
            const auto ipScale = Assimp::Interpolator<glm::vec3>();
            
            float animSample = static_cast<float>(animation->mTicksPerSecond / animation->mDuration) * befElapsed;
            animSample = std::min(animSample, 1.0f);
            
            aiMatrix4x4 transformation, scaleMat, translateMat;
            Rescale rescaler(0.0f, static_cast<float>(animation->mDuration), 0.0f, 1.0f);
            const float sampleUnscaled = rescaler.Unscale(animSample);
            
            aiNodeAnim* channel = nullptr;
            aiNode* node = nullptr;
            for (unsigned int i = 0; i < animation->mNumChannels; ++i)
            {
                channel = animation->mChannels[i];
                node = m_oRootNode->FindNode(channel->mNodeName);
                // 位移
                auto translation = Assimp::InterpolationGet<aiVector3D>(sampleUnscaled, channel->mPositionKeys, channel->mNumPositionKeys, interpolate);
                if (!applyRootMotion && node==m_oRootBoneNode)
                {
                    translation = aiVector3D(0, 0, 0);
                }
                // 旋转
                auto rotation = Assimp::InterpolationGet<aiQuaternion>(sampleUnscaled, channel->mRotationKeys, channel->mNumRotationKeys, interpolate);
                // 缩放
                auto scale = Assimp::InterpolationGet<aiVector3D>(sampleUnscaled, channel->mScalingKeys, channel->mNumScalingKeys, interpolate);
                
                
                itor = m_mBonesIdMap.find(channel->mNodeName.C_Str());
                if(itor != m_mBonesIdMap.end())
                {
                    boneId = itor->second;
                    if(bonesPosition.find(boneId) != bonesPosition.end())
                    {
                        // 如果前一个动画存在相同的骨骼有变化，则对他们进行插值
                        ipVect(bonesPosition[boneId], bonesPosition[boneId], Assimp::ConvertToGLM(translation), blendWeight);
                        ipQuat(bonesRotation[boneId], bonesRotation[boneId], Assimp::ConvertToGLM(rotation), blendWeight);
                        ipScale(bonesScale[boneId], bonesScale[boneId], Assimp::ConvertToGLM(scale), blendWeight);
                    }
                    else
                    {
                        bonesPosition[boneId] = std::move(Assimp::ConvertToGLM(translation));
                        bonesRotation[boneId] = std::move(Assimp::ConvertToGLM(rotation));
                        bonesScale[boneId] = std::move(Assimp::ConvertToGLM(scale));
                    }
                }
                


            }
        }

//        // 生成父节点坐标系下自身的变换矩阵
//        {
//            aiMatrix4x4 translateMat, rotateMat, scaleMat, transformation;
//            aiNode* node = nullptr;
//            for (auto itor=translateVec.begin(); itor!=translateVec.end(); ++itor)
//            {
//                node = itor->first;
//                aiVector3D& translation = itor->second;
//                aiQuaternion& rotation = rotateVec[node];
//                aiVector3D& scale = scaleVec[node];
//
//                // 计算单个骨骼自身坐标系下的变换矩阵
//                aiMatrix4x4::Translation(translation, translateMat);
//                aiMatrix4x4 rotateMat(rotation.GetMatrix());
//                aiMatrix4x4::Scaling(scale, scaleMat);
//                transformation = translateMat * rotateMat * scaleMat;
//
//                nodeTrans.insert(make_pair(node, transformation));
//            }
//        }
        
    }

    void Model::traverseNodeToComputeBonesTransform(aiNode* node, const aiMatrix4x4 parentMatrix, std::unordered_map<aiNode*, aiMatrix4x4>& nodeTrans, std::vector<glm::mat4>& bonesMatrix)
	{
		aiMatrix4x4 nodeMatrix;

		// 计算当前节点的变换矩阵
		auto itor = nodeTrans.find(node);
		if (itor != nodeTrans.end())
		{
			nodeMatrix = parentMatrix * itor->second;
			itor->second = nodeMatrix;
		}
		else
		{
            nodeMatrix = parentMatrix * node->mTransformation;
		}

		// 当前节点是否含有骨骼
        /* 之前从来没有注意过mOffsetMatrix，其实mOffsetMatrix很重要，是用来将网格中骨骼空间的顶点转换到模型空间中(也就是bindpose，模型在建模时的姿势)，我们这里计算的骨骼矩阵实际上是对模型空间的顶点进行变换的(顶点被绑定到骨骼上时，顶点是从模型空间直接放到了骨骼空间中（原点从模型空间的原点，变成了骨骼），所以这个时候必须要用mOffsetMatrix(记录了bindpose状态骨骼到模型空间的变换)来将顶点从骨骼空间变换回模型空间)
         */
		auto boneItor = m_mBonesIdMap.find(std::string(node->mName.C_Str()));
		if (boneItor != m_mBonesIdMap.end())
		{
            bonesMatrix[boneItor->second] = std::move(Assimp::ConvertToGLM(nodeMatrix * m_vBones[boneItor->second]->mOffsetMatrix));
		}

		// 遍历子节点
		for (int i = 0; i < node->mNumChildren; ++i)
		{
			traverseNodeToComputeBonesTransform(node->mChildren[i], nodeMatrix, nodeTrans, bonesMatrix);
		}
	}
    
    bool Model::bindMeshOnSingleBone(browser::Mesh* mesh, const std::string& boneName, const glm::mat4& transformation /*= GLM_MAT4_UNIT*/)
    {
        std::unordered_map<std::string, unsigned int>::iterator itor = m_mBonesIdMap.find(boneName);
        if(itor == m_mBonesIdMap.end())
        {
            return false;
        }
        
        return bindMeshOnSingleBone(mesh, itor->second);
    }
    
    bool Model::bindMeshOnSingleBone(browser::Mesh* mesh, unsigned int boneIdx, const glm::mat4& transformation /*= GLM_MAT4_UNIT*/)
    {
        if(boneIdx >= m_uBoneNum)
        {
            return false;
        }
        
        glm::vec4* vertices = mesh->getVertices22();
        glm::uvec4* boneIndices = mesh->getBoneIndices();
        glm::vec4* boneWeights = mesh->getBoneWeights();
        for (int i=0; i<mesh->getVertexCount(); ++i)
        {
            vertices[i] = transformation * vertices[i];
            
            boneIndices[i] = GLM_VEC4_ZERO;
            boneWeights[i] = GLM_VEC4_ZERO;
            boneIndices[i][0] = boneIdx;
            boneWeights[i][0] = 1.0f;
        }
        
        return true;
    }
    
    void Model::loadTextures(const std::string& directory)
    {
        int textureCount = m_vMeshTextureData.size();
        
        for(int i=0; i<m_vMeshTextureData.size(); ++i)
        {
            const MeshTextureData& textureData = m_vMeshTextureData[i];
            // 纹理路径
            std::string full_path;
            if (textureData.bFound)
            {
                full_path = directory + textureData.filename;
            }
            else
            {
                full_path = textureData.filename;
            }
            // 异步载入纹理
            // 注意:Lambda表达式使用的时候要注意,按引用捕获类似于传递实参或者引用,函数内部修改这个变量会传递到外边,但需要注意这个变量的生命周期,不能传递一个在调用之前被销毁的对象。而按值捕获类似于传值,内部修改并不会影响到外部,会在lambda表达式创建的时候生成一份复制,也可以保证外部的变量如果值发生变化,表达式内部该值也不会收到影响。参考下方的i和textureCount。i是一个每次循环都会变化的变量,而lambda表达式内想取到每次循环的固定值,所以按值捕获复制了一份。textureCount是一个局部变量,在lambda表达式函数被调用的时候,如果按引用捕获在调用的时候,访问到的值会不正确。
                //按值传递函数对象参数时，加上mutable修饰符后，可以修改按值传递进来的拷贝（注意是能修改拷贝，而不是值本身）
            TextureCache::getInstance()->addTextureAsync(full_path, [&, i, textureCount](Texture2D* texture) mutable -> void     // 除了itor按值捕获,其他都按引用捕获
                 {
                     if (texture)
                     {
                         // 将texture的环绕方式设为repeat
                         texture->setTexWrapParams(GL_REPEAT, GL_REPEAT);
                         // 记录texture
                         m_vTextures.push_back(texture);
                     
                         browser::Mesh* mesh = m_vMeshTextureData[i].mesh;
                         mesh->addTexture(m_vMeshTextureData[i].uniformName, texture);
                     }
                     
                     ++m_iLoadTextureNum;
                     if (m_iLoadTextureNum == textureCount)
                     {
                         // 载入模型成功
                         m_bLoadSuccess = true;
                         
                         if (m_oSuccessCallback)
                         {
                             m_oSuccessCallback(this);
                         }
                     }
                 });
        }
    }
    

}
