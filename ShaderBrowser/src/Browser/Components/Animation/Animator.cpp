#include "Animator.h"
#include "GL/Model.h"
#include <chrono>

namespace browser
{
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
    
	Animator::Animator()
        : BaseComponent("Animator")
        , m_oSrcModel(nullptr)
        , m_bIsPlaying(false)
        , m_bUseGPU(true)
        , m_fBlendTimer(0)
        , m_fBlendDuration(0)
        , m_bApplyRootMotion(false)
	{
		// 组件所属系统
		m_eBelongSystem = SystemType::Animation;
        
        // 清除
        m_mVertices.clear();
        m_mBonesPosition.clear();
        m_mBonesRotation.clear();
        m_mBonesScale.clear();
	}

	Animator::~Animator()
	{
		BROWSER_LOG("~Animator");
	}
    
    void Animator::onInspectorGUI(InspectorPanel* inspector)
    {
        if(m_oSrcModel)
        {
            // 是否允许根节点的运动
            inspector->addPropertyCheckbox("Apply Root Motion", m_bApplyRootMotion, [=](bool isEnable)
               {
                   m_bApplyRootMotion = isEnable;
               });
            
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
            m_mBonesPosition.reserve(m_oSrcModel->getBoneNum());
            m_mBonesRotation.reserve(m_oSrcModel->getBoneNum());
            m_mBonesScale.reserve(m_oSrcModel->getBoneNum());
            m_mVertices.clear();
            const std::vector<Mesh*>& meshes = m_oSrcModel->getMeshes();
            for(auto itor=meshes.begin(); itor!=meshes.end(); ++itor)
            {
                std::vector<VertexData> vertices;
                vertices.resize((*itor)->getVertexCount());
                m_mVertices.emplace((*itor), std::move(vertices));
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
            m_mBonesPosition.reserve(m_oSrcModel->getBoneNum());
            m_mBonesRotation.reserve(m_oSrcModel->getBoneNum());
            m_mBonesScale.reserve(m_oSrcModel->getBoneNum());
            m_mVertices.clear();
            const std::vector<Mesh*>& meshes = m_oSrcModel->getMeshes();
            for(auto itor=meshes.begin(); itor!=meshes.end(); ++itor)
            {
                std::vector<VertexData> vertices;
                vertices.resize((*itor)->getVertexCount());
                m_mVertices.emplace((*itor), std::move(vertices));
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
    
    void Animator::updateAnimation(float deltaTime)
    {
        if(m_bIsPlaying)
        {
            if(!m_oCurAnimation.animation)
            {
                return;
            }
            
            // 重置
            m_mTransformations.clear();
            m_vBonesMatrix.clear();
            m_mBonesPosition.clear();
            m_mBonesRotation.clear();
            m_mBonesScale.clear();
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
            
            //auto timeRec = std::chrono::steady_clock::now();
            // 计算骨骼变换矩阵
            if(m_oBefAnimation.animation)
            {
                // 混合
                float blendWeight = m_fBlendTimer / m_fBlendDuration;
                m_oSrcModel->blendBonesTransform(m_oBefAnimation.animation, m_oBefAnimation.elapsed, m_oBefAnimation.interpolate, m_oCurAnimation.animation, m_oCurAnimation.elapsed, m_oCurAnimation.interpolate, blendWeight, m_mTransformations, m_vBonesMatrix);
            }
            else
            {
                // 只有一个动画在播放
                m_oSrcModel->computeBonesTransform(m_oCurAnimation.animation, m_oCurAnimation.elapsed, m_mTransformations, m_vBonesMatrix, m_mBonesPosition, m_mBonesRotation, m_mBonesScale, m_oCurAnimation.interpolate, m_bApplyRootMotion);
            }
            
            //float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
            //timeRec = std::chrono::steady_clock::now();
            //BROWSER_LOG("========computeBonesTransform==========="+ std::to_string(deltaTime) + "ms");
            
            // 更新骨骼的Transform
            dispatchEventToChildren(ComponentEvent::Animator_UpdateBonesTransform, new AnimatorUpdateBonesTransformMessage(m_oSrcModel->getBonesIdMapPointer(), &m_mBonesPosition, &m_mBonesRotation, &m_mBonesScale));
            
            // cpu计算顶点位置
            if(!m_bUseGPU)
            {
                // 复制原始顶点信息
                const std::vector<Mesh*>& meshes = m_oSrcModel->getMeshes();
                for(auto itor=meshes.begin(); itor!=meshes.end(); ++itor)
                {
                    const std::vector<VertexData>& vertices = (*itor)->getVertices();
                    m_mVertices[(*itor)].assign(vertices.begin(), vertices.end());
                }
                // 根据骨骼矩阵，计算新的顶点位置、法线和切线
                glm::mat4 skinning;
                bool hasSkinning;
                for(auto itor=m_mVertices.begin(); itor!=m_mVertices.end(); ++itor)
                {
                    std::vector<VertexData>& vertices = itor->second;
                    for(int i=0; i<vertices.size(); ++i)
                    {
                        VertexData& vertex = vertices[i];
                        
                        // 计算蒙皮矩阵（顶点的骨骼变换矩阵，每个顶点最多被4个骨骼控制）
                        skinning = m_vBonesMatrix[vertex.boneIndices[0]] * vertex.boneWeights[0]
                        + m_vBonesMatrix[vertex.boneIndices[1]] * vertex.boneWeights[1]
                        + m_vBonesMatrix[vertex.boneIndices[2]] * vertex.boneWeights[2]
                        + m_vBonesMatrix[vertex.boneIndices[3]] * vertex.boneWeights[3];
                        
                        
                        //BROWSER_LOG_MAT4(skinning);
                        // 计算变换后的顶点位置、法线等信息
                        if (skinning != GLM_MAT4_ZERO)
                        {
                            vertex.position = skinning * vertex.position;
                            vertex.normal = skinning * glm::vec4(vertex.normal, 0.0f);
                            vertex.tangent = skinning * glm::vec4(vertex.tangent, 0.0f);
                        }
                        
                    }
                }
            }
            
            //deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
            //BROWSER_LOG("========cpu compute position===========" + std::to_string(deltaTime) + "ms");
            
        }
    }

	
    
}
