#include "Animator.h"
#include "GL/Model.h"
#include <chrono>

namespace browser
{
	Animator::Animator()
        : BaseComponent("Animator")
        , m_oSrcModel(nullptr)
        , m_sCurAnimName("")
        , m_oAnimation(nullptr)
        , m_fElapsedTime(0.0f)
        , m_fSpeed(1.0f)
        , m_bInterpolate(true)
        , m_bRepeat(false)
        , m_bIsPlaying(false)
        , m_bUseGPU(true)
	{
		// 组件所属系统
		m_eBelongSystem = SystemType::Animation;
        
        // 清除
        m_mVertices.clear();
	}

	Animator::~Animator()
	{
		BROWSER_LOG("~Animator");
	}
    
    void Animator::onInspectorGUI(InspectorPanel* inspector)
    {
        if(m_oSrcModel)
        {
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
    
    void Animator::updateAnimation(float deltaTime)
    {
        if(m_bIsPlaying)
        {
            if(!m_oAnimation)
            {
                return;
            }

            // 重置
            m_mTransformations.clear();
            m_vBonesMatrix.clear();
            m_vBonesMatrix.resize(m_oSrcModel->getBoneNum());

            // 计算时间
            m_fElapsedTime += deltaTime;
			if (m_fElapsedTime > m_fCurAnimDuration)
			{
				if (m_bRepeat)
				{
					m_fElapsedTime = fmod(m_fElapsedTime, m_fCurAnimDuration);
				}
				else
				{
					m_bIsPlaying = false;
				}
			}

			//auto timeRec = std::chrono::steady_clock::now();
            // 计算骨骼变换矩阵
            m_oSrcModel->computeBonesTransform(m_oAnimation, m_fElapsedTime, m_mTransformations, m_vBonesMatrix, m_fSpeed, m_bInterpolate);
	
			//float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
			//timeRec = std::chrono::steady_clock::now();
			//BROWSER_LOG("========computeBonesTransform==========="+ std::to_string(deltaTime) + "ms");

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
						//skinning = GLM_MAT4_ZERO;
						//hasSkinning = false;
      //                  for(int j=0; j<4; ++j)
      //                  {
      //                      if(vertex.boneWeights[j] > 0.0f)
      //                      {
      //                          skinning = skinning + (m_vBonesMatrix[vertex.boneIndices[j]]*vertex.boneWeights[j]);
						//		hasSkinning = true;
      //                      }
      //                  }

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
    
    void Animator::play(const std::string& animName, bool repeat/* = false*/, float speed/* = 1.0f*/, bool interpolate/* = true*/)
    {
        BROWSER_ASSERT(m_oSrcModel, "Entity must have a source model, or you cannot play animation in function Animator::play");
        
        if(!m_bHasInit)
        {
            // 重置容器大小
            m_vBonesMatrix.reserve(m_oSrcModel->getBoneNum());
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
			m_oAnimation = animation;
			m_sCurAnimName = animName;
			m_bRepeat = repeat;
			m_fSpeed = speed;
			m_bInterpolate = interpolate;
			m_bIsPlaying = true;
			m_fElapsedTime = 0.0f;

			m_fCurAnimDuration = animation->mDuration / animation->mTicksPerSecond;
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

	
    
}
