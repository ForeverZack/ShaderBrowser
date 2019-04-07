#include "Model.h"
#include "Common/Tools/FileUtils.h"
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
//            importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);    // 防止FBX自己生成枢轴，干扰Node结构树
			const aiScene* scene = importer->ReadFile(m_sFullPath, pFlags);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				// 加载模型失败
				cout << "ERROR::ASSIMP::" << importer->GetErrorString() << endl;
				return false;
			}
            // 记录importer，否则场景数据会被析构
			m_vImporters.push_back(importer);
			// 加载模型动画数据
			loadAnimations(scene);
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
                    
                    while(parentNode)
                    {
                        boneItor = m_mBonesIdMap.find(parentNode->mName.C_Str());
                        if(boneItor != m_mBonesIdMap.end())
                        {
                            unsigned int boneIdx = boneItor->second;
                            
                            browser::Mesh* mesh = m_vMeshes[index];
                            std::vector<VertexData>& vertices = mesh->getVerticesRef();
                            if (vertices.size()>0 && vertices[0].boneWeights[0]==0.0f)
                            {
                                // 绑定模型到骨骼
                                bindMeshOnSingleBone(mesh, boneIdx);
                            }
                            
                            break;
                        }
                        parentNode = parentNode->mParent;
                    }
                    
                    ++index;
                }
            }
		}
		
		// 加载其余的动画文件（动画文件列表）
		{
			for (auto itor = animFiles.begin(); itor != animFiles.end(); ++itor)
			{
				std::shared_ptr<Assimp::Importer> importer = make_shared<Assimp::Importer>();
				// 设置importer的属性
//                importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);    // 防止FBX自己生成枢轴，干扰Node结构树
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
        
		return true;
	}

	BaseEntity* Model::createNewEntity(const std::string& name)
	{
		BaseEntity* entity = traverseNodeAndCreateEntity(m_oRootNode, nullptr, nullptr);
		entity->setName(name);
		entity->setModel(this);
        if(m_bHasSkeletonAnim)
        {
            entity->addComponent(new Animator());
        }
		return entity;
	}

	BaseEntity* Model::traverseNodeAndCreateEntity(aiNode* node, BaseEntity* parent, BaseEntity* root)
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

        bool skinned = false;
		if (node->mNumMeshes > 0)
		{
			// MeshFilter组件
			MeshFilter* meshFilter = nullptr;
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
                            skinnedRender = SkinnedMeshRenderer::createSkinnedMeshRenderer();
                            entity->addComponent(skinnedRender);
                        }
                        
                        skinnedRender->addMaterial(mesh->getMaterialName(), GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME);
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
                            renderer = BaseRender::createBaseRender();
                            entity->addComponent(renderer);
                        }
                        
                        renderer->addMaterial(mesh->getMaterialName(), GLProgram::DEFAULT_GLPROGRAM_NAME);
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
			traverseNodeAndCreateEntity(node->mChildren[i], entity, root);
		}

		return entity;
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
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mVertices);
            // 顶点颜色
			if (aiMesh->mColors[0] && aiMesh->mColors[1])
			{
				mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mColors);
			}
            // 顶点uv坐标
            if (aiMesh->mTextureCoords[0])
            {
                mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mTextureCoords[0]);
            }
            // 法线
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mNormals);
            // 切线
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mTangents);
            // 索引信息
            mesh->setIndicesInfo([=](std::vector<GLushort>& indices, unsigned int& indexCount) -> void
                 {
                     indices.clear();
                     indexCount = 0;
                     
                     for(int i=0; i<aiMesh->mNumFaces; ++i)
                     {
                         const aiFace& face = aiMesh->mFaces[i];
                         indexCount += face.mNumIndices;
                         for(int j=0; j<face.mNumIndices; ++j)
                         {
                             indices.push_back(face.mIndices[j]);
                         }
                         
                     }
                 });

			// 骨骼信息
			std::vector<VertexData>& vertices = mesh->getVerticesRef();
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
					VertexData& vertexData = vertices[vertexWeight->mVertexId];
					
					// 一个顶点最多能被4个骨骼控制，这里我们要在这个数组中找到第一个空着的骨骼信息
					for (unsigned int j = 0; j < 4; ++j)
					{
						if (vertexData.boneWeights[j] != 0.0f)
						{
							continue;
						}

						vertexData.boneIndices[j] = boneIdx;
						vertexData.boneWeights[j] = vertexWeight->mWeight;
						break;
					}
				}
				
			}
			mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_BONE_IDS, 4, GL_UNSIGNED_INT, GL_FALSE, sizeof(VertexData), nullptr);
			mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), nullptr);
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
    
    void Model::setupMeshesVAO()
    {
        for(auto itor=m_vMeshes.begin(); itor!=m_vMeshes.end(); ++itor)
        {
            (*itor)->setupVAO();
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

	void Model::computeBonesTransform(aiAnimation* animation, float elapsedTime, std::unordered_map<aiNode*, aiMatrix4x4>& nodeTrans, std::vector<glm::mat4>& bonesMatrix, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale, bool interpolateAnimation /*= true*/, bool applyRootMotion /*= false*/)
	{
		// 将采样范围变换到 [0, 1]
		float animSample = static_cast<float>(animation->mTicksPerSecond / animation->mDuration) * elapsedTime;
		animSample = std::min(animSample, 1.0f);

		aiMatrix4x4 transformation, scaleMat, translateMat;
		Rescale rescaler(0.0f, static_cast<float>(animation->mDuration), 0.0f, 1.0f);
		const float sampleUnscaled = rescaler.Unscale(animSample);
        std::unordered_map<std::string, unsigned int>::iterator itor;
        unsigned int boneId;
		// 遍历骨骼变换信息，生成父节点坐标系下自身的变换矩阵
		{
			aiNodeAnim* channel = nullptr;
			aiNode* node = nullptr;
			for (unsigned int i = 0; i < animation->mNumChannels; ++i)
			{
				channel = animation->mChannels[i];
				node = m_oRootNode->FindNode(channel->mNodeName);

				// 位移
				auto translation = Assimp::InterpolationGet<aiVector3D>(sampleUnscaled, channel->mPositionKeys, channel->mNumPositionKeys, interpolateAnimation);
                if (!applyRootMotion && node==m_oRootBoneNode)
                {
                    translation = aiVector3D(0, 0, 0);
                }
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
				// 计算单个骨骼自身坐标系下的变换矩阵
				aiMatrix4x4::Translation(translation, translateMat);
				aiMatrix4x4 rotateMat(rotation.GetMatrix());
				aiMatrix4x4::Scaling(scale, scaleMat);
				transformation = translateMat * rotateMat * scaleMat;
                

//                nodeTrans.insert(make_pair(node, transformation));
                nodeTrans[node] = std::move(transformation);
			}
		}

        // 从根节点遍历下去，得到每个节点相对于根节点的变换矩阵
		aiMatrix4x4 identity;
		traverseNodeToComputeBonesTransform(m_oRootNode, identity, nodeTrans, bonesMatrix);
	}
    
    void Model::blendBonesTransform(aiAnimation* befAnimation, float befElapsed, bool befInterpolate, aiAnimation* animation, float elapsedTime, bool interpolate, float blendWeight, std::unordered_map<aiNode*, aiMatrix4x4>& nodeTrans, std::vector<glm::mat4>& bonesMatrix, bool applyRootMotion /*= false*/)
    {
        // 记录所有变换的node信息<位移, 旋转, 缩放>
        std::unordered_map<aiNode*, aiVector3D> translateVec;
        std::unordered_map<aiNode*, aiQuaternion> rotateVec;
        std::unordered_map<aiNode*, aiVector3D> scaleVec;
        
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
                
                translateVec[node] = std::move(translation);
                rotateVec[node] = std::move(rotation);
                scaleVec[node] = std::move(scale);
            }
        }
        
        // 
        {
            // 插值函数
            const auto ipVect = Assimp::Interpolator<aiVector3D>();
            const auto ipQuat = Assimp::Interpolator<aiQuaternion>();
            const auto ipScale = Assimp::Interpolator<aiVector3D>();
            
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
                
                if(translateVec.find(node) != translateVec.end())
                {
                    // 如果前一个动画存在相同的骨骼有变化，则对他们进行插值
                    ipVect(translateVec[node], translateVec[node], translation, blendWeight);
                    ipQuat(rotateVec[node], rotateVec[node], rotation, blendWeight);
                    ipScale(scaleVec[node], scaleVec[node], scale, blendWeight);
                }
                else
                {
                    translateVec[node] = std::move(translation);
                    rotateVec[node] = std::move(rotation);
                    scaleVec[node] = std::move(scale);
                }

            }
        }

        // 生成父节点坐标系下自身的变换矩阵
        {
            aiMatrix4x4 translateMat, rotateMat, scaleMat, transformation;
            aiNode* node = nullptr;
            for (auto itor=translateVec.begin(); itor!=translateVec.end(); ++itor)
            {
                node = itor->first;
                aiVector3D& translation = itor->second;
                aiQuaternion& rotation = rotateVec[node];
                aiVector3D& scale = scaleVec[node];
                
                // 计算单个骨骼自身坐标系下的变换矩阵
                aiMatrix4x4::Translation(translation, translateMat);
                aiMatrix4x4 rotateMat(rotation.GetMatrix());
                aiMatrix4x4::Scaling(scale, scaleMat);
                transformation = translateMat * rotateMat * scaleMat;
                
                nodeTrans.insert(make_pair(node, transformation));
            }
        }
        
        // 从根节点遍历下去，得到每个节点相对于根节点的变换矩阵
        aiMatrix4x4 identity;
        traverseNodeToComputeBonesTransform(m_oRootNode, identity, nodeTrans, bonesMatrix);
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
    
    bool Model::bindMeshOnSingleBone(browser::Mesh* mesh, const std::string& boneName)
    {
        std::unordered_map<std::string, unsigned int>::iterator itor = m_mBonesIdMap.find(boneName);
        if(itor == m_mBonesIdMap.end())
        {
            return false;
        }
        
        return bindMeshOnSingleBone(mesh, itor->second);
    }
    
    bool Model::bindMeshOnSingleBone(browser::Mesh* mesh, unsigned int boneIdx)
    {
        if(boneIdx >= m_uBoneNum)
        {
            return false;
        }
        
        std::vector<VertexData>& vertices = mesh->getVerticesRef();
        for (auto itor=vertices.begin(); itor!=vertices.end(); ++itor)
        {
            VertexData& vertex = (*itor);
            vertex.boneIndices = GLM_VEC4_ZERO;
            vertex.boneWeights = GLM_VEC4_ZERO;
            vertex.boneIndices[0] = boneIdx;
            vertex.boneWeights[0] = 1.0f;
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
