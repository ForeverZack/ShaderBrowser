#pragma once

#include "BaseFeedback.h"
#include "GL/Model.h"

using namespace common;
using namespace customGL;

namespace browser
{

	class AnimatorFeedback : public BaseFeedback
	{
    public:
        static AnimatorFeedback* create(Model* model);
        
	public:
		AnimatorFeedback();
		~AnimatorFeedback();
        
	public:
        
        // 初始化
        bool initFeedback(Model* model);
        
        // 设置当前正在播放的动画
        void setCurrentAnimationData(aiAnimation* animation);
        // 播放
        void play(float sampleUnscaled);
        void play(float sampleUnscaled, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale);
        
	protected:
        // 骨骼数量
        int m_iBoneCount;
        // 当前正在播放的动画
        aiAnimation* m_pCurAnimation;
        // 原模型
        Model* m_oSrcModel;
        // 数据采集
        float* m_pSamplerPosition;
        float* m_pSamplerRotation;
        float* m_pSamplerScale;
        // 采集数据大小
        int m_iSamplerSize;
        
        // 动画数据
        glm::vec4 m_oAnimationInfo;
    };
}

