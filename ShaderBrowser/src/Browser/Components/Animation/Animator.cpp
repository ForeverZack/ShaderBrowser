#include "Animator.h"
#include "GL/GPUResource/Model/Model.h"
#include <chrono>
#include "Common/System/ECSManager.h"
#include "Browser/System/AnimationSystem.h"
//#include "Browser/Components/Feedback/AnimatorFeedback.h"
//#include "Browser/Components/ComputeProgram/AnimatorComputeProgram.h"

using namespace customGL;

namespace browser
{
	EnumComponentClass Animator::ComponentClass = EnumComponentClass::ECC_Animator;

    // 默认动画名称前缀
    const std::string Animator::DEFAULT_ANIMATION_NAME = "DefaultAnim_";
    
    AnimationPlayData::AnimationPlayData()
        : animation(nullptr)
        , name("")
        , elapsed(0.0f)
    {
        
    }
    
    void AnimationPlayData::setData(const std::string& aniName, aiAnimation* anim, float dur, float elap, float spe, bool inter, bool rep)
    {
        name = aniName;
        animation = anim;
        duration = dur;
        elapsed = elap;
        speed = spe;
        interpolate = inter;
        repeat = rep;
    }
    
    void AnimationPlayData::empty()
    {
        name = "";
        animation = nullptr;
        elapsed = 0.0f;
    }
    
    AnimationPlayData& AnimationPlayData::operator=(const AnimationPlayData& data)
    {
        setData(data.name, data.animation, data.duration, data.elapsed, data.speed, data.interpolate, data.repeat);
        return *this;
    }
    
    
    Animator* Animator::create(unsigned int boneNum)
    {
        Animator* animator = new Animator();
        animator->setBoneInfo(boneNum);
        return animator;
    }
    
	Animator::Animator()
        : BaseComponent("Animator")
        , m_oSrcModel(nullptr)
        , m_pTextureBuffer(nullptr)
        , m_bIsPlaying(false)
        , m_bUseGPU(true)
        , m_fBlendTimer(0)
        , m_fBlendDuration(0)
        , m_bApplyRootMotion(false)
        , m_bDirty(true)
//        , m_oFeedback(nullptr)
//		, m_oComputeProgram(nullptr)
        , m_oRootBone(nullptr)
    {
		// 组件所属系统
		m_eBelongSystem = SystemType::Animation;
        
        // 清除
        m_mVertices.clear();
        m_mBoneIndices.clear();
        m_mBoneWeights.clear();
        m_mNormals.clear();
        m_mTangents.clear();
	}

	Animator::~Animator()
	{
		BROWSER_LOG("~Animator");
        
        m_pTextureBuffer->release();
//        m_oFeedback->release();
//		m_oComputeProgram->release();
	}
    
    void Animator::onInspectorGUI(InspectorPanel* inspector)
    {
        if(m_oSrcModel)
        {
            // 是否允许根节点的运动
			ShowGUIData& data = inspector->addPropertyCheckbox("Apply Root Motion", m_bApplyRootMotion, [=](bool isEnable)
               {
                   m_bApplyRootMotion = isEnable;
               });
			data.helpMarker = "Whether apply model's root motion when play animation";
            
            // 动画列表
            const std::vector<std::string>& animationNames = m_oSrcModel->getAnimationNames();
            if(animationNames.size() > 0)
            {
                inspector->addPropertyText("Animations:");
                for(auto itor=animationNames.begin(); itor!=animationNames.end(); ++itor)
                {
                    inspector->addPropertyText("name: " + (*itor));
                }
            }

        }
    }
    
    void Animator::play(const std::string& animName, bool repeat/* = false*/, float speed/* = 1.0f*/, bool interpolate/* = true*/)
    {
        BROWSER_ASSERT(m_oSrcModel, "Entity must have a source model, or you cannot play animation in function Animator::play");
        
        if(!m_bHasInit)
        {
            // 重置容器大小
            m_vBonesMatrix.reserve(m_oSrcModel->getBoneNum());
            m_mVertices.clear();
            m_mBoneIndices.clear();
            m_mBoneIndices.clear();
            m_mNormals.clear();
            m_mTangents.clear();
            const std::vector<Mesh*>& meshes = m_oSrcModel->getMeshes();
            for(auto itor=meshes.begin(); itor!=meshes.end(); ++itor)
            {
                m_mVertices.emplace((*itor), new glm::vec4[(*itor)->getVertexCount()]);
                m_mBoneIndices.emplace((*itor), new glm::uvec4[(*itor)->getVertexCount()]);
                m_mBoneWeights.emplace((*itor), new glm::vec4[(*itor)->getVertexCount()]);
                m_mNormals.emplace((*itor), new glm::vec3[(*itor)->getVertexCount()]);
                m_mTangents.emplace((*itor), new glm::vec3[(*itor)->getVertexCount()]);
            }
        }

        //
        auto animation = m_oSrcModel->getAnimation(animName);
		if (animation)
		{
            m_oCurAnimation.setData(animName, animation, animation->mDuration/animation->mTicksPerSecond, 0.0f, speed, interpolate, repeat);
            
            m_bIsPlaying = true;
		}


    }
    
    void Animator::play(int animIdx, bool repeat /*= false*/, float speed /*= 1.0f*/, bool interpolate /*= true*/)
    {
        BROWSER_ASSERT(m_oSrcModel, "Entity must have a source model, or you cannot play animation in function Animator::play");
        
        if(!m_bHasInit)
        {
            // 重置容器大小
            m_vBonesMatrix.reserve(m_oSrcModel->getBoneNum());
            m_mVertices.clear();
            m_mBoneIndices.clear();
            m_mBoneIndices.clear();
            m_mNormals.clear();
            m_mTangents.clear();
            const std::vector<Mesh*>& meshes = m_oSrcModel->getMeshes();
            for(auto itor=meshes.begin(); itor!=meshes.end(); ++itor)
            {
                m_mVertices.emplace((*itor), new glm::vec4[(*itor)->getVertexCount()]);
                m_mBoneIndices.emplace((*itor), new glm::uvec4[(*itor)->getVertexCount()]);
                m_mBoneWeights.emplace((*itor), new glm::vec4[(*itor)->getVertexCount()]);
                m_mNormals.emplace((*itor), new glm::vec3[(*itor)->getVertexCount()]);
                m_mTangents.emplace((*itor), new glm::vec3[(*itor)->getVertexCount()]);
            }
        }
        
        //
        auto animation = m_oSrcModel->getAnimation(animIdx);
        if (animation)
        {
            m_oCurAnimation.setData(animation->mName.C_Str(), animation, animation->mDuration/animation->mTicksPerSecond, 0.0f, speed, interpolate, repeat);
            m_bIsPlaying = true;
        }
    }
    
    void Animator::blendTo(const std::string& animName, bool repeat /*= false*/, float speed /*= 1.0f*/, bool interpolate /*= true*/, float blendDuration /*= 1.0f*/)
    {
        BROWSER_ASSERT(m_oSrcModel, "Entity must have a source model, or you cannot play animation in function Animator::blendTo");
        
        if (!m_oCurAnimation.animation)
        {
            // 当前没有在播放动画
            play(animName, repeat, speed, interpolate);
        }
        else
        {
            auto animation = m_oSrcModel->getAnimation(animName);
            if (animation == m_oCurAnimation.animation)
            {
                return;
            }
            
            // 当前正在播放其他动画
            m_oBefAnimation = m_oCurAnimation;
            play(animName, repeat, speed, interpolate);
            m_fBlendDuration = blendDuration;
            m_fBlendTimer = 0.0f;
        }
    }

    void Animator::blendTo(int animIdx, bool repeat /*= false*/, float speed /*= 1.0f*/, bool interpolate /*= true*/, float blendDuration /*= 1.0f*/)
    {
        BROWSER_ASSERT(m_oSrcModel, "Entity must have a source model, or you cannot play animation in function Animator::blendTo");
        
        if (!m_oCurAnimation.animation)
        {
            // 当前没有在播放动画
            play(animIdx, repeat, speed, interpolate);
        }
        else
        {
            auto animation = m_oSrcModel->getAnimation(animIdx);
            if(animation)
            {
                if(animation == m_oCurAnimation.animation)
                {
                    return;
                }
                
                // 当前正在播放其他动画
                m_oBefAnimation = m_oCurAnimation;
                play(animIdx, repeat, speed, interpolate);
                m_fBlendDuration = blendDuration;
                m_fBlendTimer = 0.0f;
            }
        }
    }
    
	void Animator::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
	{
		switch (event)
		{
            case Animator_AddComponent:
                {
                    AnimatorAddComponentMessage* convertMsg = static_cast<AnimatorAddComponentMessage*>(msg);
                    m_oSrcModel = convertMsg->getModel();
                }
                break;
            
		}
	}
    
    void Animator::setBoneInfo(unsigned int boneNum)
    {
        m_vAllBones.resize(boneNum);
        
        m_pTextureBuffer = TextureBuffer::create();
        m_pTextureBuffer->retain();
    }
    
    void Animator::addBone(unsigned int boneId, Transform* boneNode)
    {
        m_vAllBones[boneId] = boneNode;
    }
    
    TextureBuffer* Animator::useBonesMatrix()
    {
        if (m_bDirty && m_bIsPlaying)
        {
            m_bDirty = false;
//            Transform* bone;
//            for (int i=0; i<m_vAllBones.size(); ++i)
//            {
//                bone = m_vAllBones[i];
//                m_vBonesMatrix[i] = bone->getBoneMatrix();
//            }
            
            // 更新骨骼矩阵
            m_pTextureBuffer->setData(m_vBonesMatrix);
        }

        return m_pTextureBuffer;
    }
    
    void Animator::updateAnimation(float deltaTime)
    {
		//auto lastTimepoint = std::chrono::steady_clock::now();
		//auto lastTimepointaa = std::chrono::steady_clock::now();

        if(m_bIsPlaying)
        {
            if (!m_oCurAnimation.animation)
            {
                return;
            }
//            if (!m_oFeedback)
//            {
//                m_oFeedback = AnimatorFeedback::create(m_oSrcModel);
//                m_oFeedback->retain();
//            }
//            if (!m_oComputeProgram)
//            {
//                m_oComputeProgram = AnimatorComputeProgram::create(m_oSrcModel);
//                m_oComputeProgram->retain();
//            }

            // 重置
            m_vBonesMatrix.clear();
            m_bDirty = true;
			m_vBonesMatrix.reserve(m_oSrcModel->getBoneNum());
			m_vBonesMatrix.resize(m_oSrcModel->getBoneNum());

            // 计算时间
            m_oCurAnimation.elapsed += deltaTime * m_oCurAnimation.speed;
            if (m_oCurAnimation.elapsed > m_oCurAnimation.duration)
            {
                if (m_oCurAnimation.repeat)
                {
                    m_oCurAnimation.elapsed = fmod(m_oCurAnimation.elapsed, m_oCurAnimation.duration);
                }
                else
                {
                    m_oCurAnimation.empty();
                    m_bIsPlaying = false;
                }
            }
            if(m_oBefAnimation.animation)
            {
                m_oBefAnimation.elapsed += deltaTime * m_oBefAnimation.speed;
                if (m_oBefAnimation.elapsed > m_oBefAnimation.duration)
                {
                    if (m_oBefAnimation.repeat)
                    {
                        m_oBefAnimation.elapsed = fmod(m_oBefAnimation.elapsed, m_oBefAnimation.duration);
                    }
                    else
                    {
                        m_oBefAnimation.empty();
                    }
                }
                
                m_fBlendTimer += deltaTime;
                if (m_fBlendTimer >= m_fBlendDuration)
                {
                    m_oBefAnimation.empty();
                }
            }
            
//            auto timeRec = std::chrono::steady_clock::now();
            
            // feeback
            {
//                m_oFeedback->setCurrentAnimationData(m_oCurAnimation.animation);
            }
			// compute program
			{
//                if (m_oComputeProgram)
//                {
//                    m_oComputeProgram->setCurrentAnimationData(m_oCurAnimation.animation);
//                }
            }
            // 计算骨骼变换矩阵
            if(m_oBefAnimation.animation)
            {
                // 混合
                float blendWeight = m_fBlendTimer / m_fBlendDuration;
                m_oSrcModel->blendBonesTransform(m_oBefAnimation.animation, m_oBefAnimation.elapsed, m_oBefAnimation.interpolate, m_oCurAnimation.animation, m_oCurAnimation.elapsed, m_oCurAnimation.interpolate, blendWeight, m_vAllBones, m_bApplyRootMotion);
            }
            else
            {
                // 只有一个动画在播放
				// 方法一：在cpu计算
                m_oSrcModel->computeBonesTransform(m_oCurAnimation.animation, m_oCurAnimation.elapsed, m_vAllBones, m_oCurAnimation.interpolate, m_bApplyRootMotion);
                
				// 方法二：tranform feedback
//                m_oFeedback->play(m_oCurAnimation.animation->mTicksPerSecond * m_oCurAnimation.elapsed, m_mBonesPosition, m_mBonesRotation, m_mBonesScale);
				
				// 方法三：compute program （opengl 4.3以上才支持，mac不支持）
                //m_oComputeProgram->play(m_oCurAnimation.animation->mTicksPerSecond * m_oCurAnimation.elapsed, m_mBonesPosition, m_mBonesRotation, m_mBonesScale);
            }
			//BROWSER_LOG("=======Animator====updateAnimation 01======" + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - lastTimepointaa).count() / 1000.0f));

			//lastTimepointaa = std::chrono::steady_clock::now();
			// 更新骨骼变换信息
			updateBonesTranform();
			//BROWSER_LOG("=======Animator====updateAnimation 02======" + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - lastTimepointaa).count() / 1000.0f));

        }

		// 发送完成消息
		//lastTimepointaa = std::chrono::steady_clock::now();
		AnimationSystem::getInstance()->dispatchOneFinishedComponent();
		//BROWSER_LOG("=======Animator====updateAnimation 03======" + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - lastTimepointaa).count() / 1000.0f));
		//BROWSER_LOG("=======Animator====updateAnimation======" + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - lastTimepoint).count() / 1000.0f));
    }
    
    void Animator::updateBonesTranform()
    {
		if (m_bIsPlaying)
		{
			if (!m_oCurAnimation.animation)
			{
				return;
			}

			// 更新骨骼的Transform
			{
				// 方法一：通过消息机制来更新骨骼Transform（效率低）
	//                dispatchEventToChildren(ComponentEvent::Animator_UpdateBonesTransform, new AnimatorUpdateBonesTransformMessage(m_oSrcModel->getBonesIdMapPointer(), &m_mBonesPosition, &m_mBonesRotation, &m_mBonesScale));

				// 方法二：在这里统一处理变换
				unsigned int boneId;
				Transform* bone = nullptr;
				//for (auto boneItor = m_vAllBones.begin(); boneItor != m_vAllBones.end(); ++boneItor)
				//{
				//	bone = *boneItor;
				//	boneId = (*boneItor)->getBoneId();
				//	if (m_mBonesPosition.find(boneId) != m_mBonesPosition.end())
				//	{
				//		// 发生变换的骨骼
				//		const aiVector3D& position = m_mBonesPosition[boneId];
				//		const aiQuaternion& rotation = m_mBonesRotation[boneId];
				//		const aiVector3D& scale = m_mBonesScale[boneId];
				//		bone->setPosition(position.x, position.y, position.z);
				//		bone->setQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
				//		bone->setScale(scale.x, scale.y, scale.z);
				//	}
				//	else
				//	{
				//		// 未发生变化的骨骼节点需要重置位置
				//		bone->resetSrcModelTransform();
				//	}
				//}

				// 更新计算Tranform
				if (m_oRootBone)
				{
					// 从骨骼根节点开始遍历
					m_oRootBone->getParent()->visitSync(m_oRootBone->getParent()->getModelMatrix(), false);
				}
				// 获取骨骼矩阵
				for (int i = 0; i < m_vAllBones.size(); ++i)
				{
					bone = m_vAllBones[i];
					m_vBonesMatrix[i] = bone->getBoneMatrix();
				}
			}



			//            deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
			//            BROWSER_LOG("========deliver bone transform===========" + std::to_string(deltaTime) + "ms");

					// cpu计算顶点位置
			if (!m_bUseGPU)
			{
				// 复制原始顶点信息
				const std::vector<Mesh*>& meshes = m_oSrcModel->getMeshes();
				for (auto itor = meshes.begin(); itor != meshes.end(); ++itor)
				{
					glm::vec4* vertices = (*itor)->getVertices();
					std::vector<glm::uvec4>& boneIndices = (*itor)->getBoneIndicesRef();
					std::vector<glm::vec4>& boneWeights = (*itor)->getBoneWeightsRef();
					glm::vec3* normals = (*itor)->getNormals();
					glm::vec3* tangents = (*itor)->getTangents();

					glm::vec4* dst_vertices = m_mVertices[(*itor)];
					glm::uvec4* dst_boneIndices = m_mBoneIndices[(*itor)];
					glm::vec4* dst_boneWeights = m_mBoneWeights[(*itor)];
					glm::vec3* dst_normals = m_mNormals[(*itor)];
					glm::vec3* dst_tangents = m_mTangents[(*itor)];
					for (int i = 0; i < (*itor)->getVertexCount(); ++i)
					{
						dst_vertices[i] = vertices[i];
						dst_boneIndices[i] = boneIndices[i];
						dst_boneWeights[i] = boneWeights[i];
						dst_normals[i] = normals[i];
						dst_tangents[i] = tangents[i];
					}

				}
				// 根据骨骼矩阵，计算新的顶点位置、法线和切线
				glm::mat4 skinning;
				for (auto itor = m_mVertices.begin(); itor != m_mVertices.end(); ++itor)
				{
					glm::vec4* vertices = itor->second;
					glm::uvec4* boneIndices = m_mBoneIndices[itor->first];
					glm::vec4* boneWeights = m_mBoneWeights[itor->first];
					glm::vec3* normals = m_mNormals[itor->first];
					glm::vec3* tangents = m_mTangents[itor->first];
					for (int i = 0; i < itor->first->getVertexCount(); ++i)
					{
						glm::uvec4& vertexBoneIndices = boneIndices[i];
						glm::vec4& vertexBoneWeights = boneWeights[i];

						// 计算蒙皮矩阵（顶点的骨骼变换矩阵，每个顶点最多被4个骨骼控制）
						skinning = m_vBonesMatrix[vertexBoneIndices[0]] * vertexBoneWeights[0]
							+ m_vBonesMatrix[vertexBoneIndices[1]] * vertexBoneWeights[1]
							+ m_vBonesMatrix[vertexBoneIndices[2]] * vertexBoneWeights[2]
							+ m_vBonesMatrix[vertexBoneIndices[3]] * vertexBoneWeights[3];


						//BROWSER_LOG_MAT4(skinning);
						// 计算变换后的顶点位置、法线等信息
						if (skinning != GLM_MAT4_ZERO)
						{
							vertices[i] = skinning * vertices[i];
							normals[i] = skinning * glm::vec4(normals[i], 0.0f);
							tangents[i] = skinning * glm::vec4(tangents[i], 0.0f);
						}

					}
				}
			}

		}

    }

	
    
}
