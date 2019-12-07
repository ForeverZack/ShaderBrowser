#pragma once

#include "BaseComputeProgram.h"
#include "GL/GPUResource/Model/Model.h"

using namespace common;
using namespace customGL;

namespace browser
{

	class AnimatorComputeProgram : public BaseComputeProgram
	{
	public:
		static AnimatorComputeProgram* create(Model* model);

	public:
		AnimatorComputeProgram();
		~AnimatorComputeProgram();

	public:

		// 初始化
		bool initComputeProgram(Model* model);

		// 设置当前正在播放的动画
		void setCurrentAnimationData(aiAnimation* animation);
		// 播放
		void play(float sampleUnscaled, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale);

	protected:
		// 骨骼数量
		int m_iBoneCount;
		// 当前正在播放的动画
		aiAnimation* m_pCurAnimation;
		// 原模型
		Model* m_oSrcModel;
		// 数据采集
		float* m_pSamplerBuffer;
		// 采集数据大小
		int m_iSamplerSize;

		// 动画数据
		glm::vec4 m_oAnimationInfo;
	};
}

