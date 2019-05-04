#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "Browser/Components/BaseComponent.h"
#include "Common/Tools/Utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/scene.h>

using namespace std;
using namespace common;
using namespace customGL;

namespace browser
{
    // 播放动画数据结构
    class AnimationPlayData
    {
    public:
        AnimationPlayData();
        
        void setData(const std::string& aniName, aiAnimation* anim, float dur, float elap, float spe, bool inter, bool rep);
        void empty();
        
        // 重载赋值运算符
        AnimationPlayData& operator=(const AnimationPlayData& data);
        
    public:
        // 当前正在播放的动画
        std::string name;
        aiAnimation* animation;
        // 当前动画持续时间
        float duration;
        // 播放时间
        float elapsed;
        // 播放速度
        float speed;
        // 是否插值处理动画
        bool interpolate;
        // 是否循环
        bool repeat;
    };
    
    
	class Animator : public BaseComponent
	{

        
    public:
        // 默认动画名称前缀
        static const std::string DEFAULT_ANIMATION_NAME;
        
	public:
		Animator();
		~Animator();

	public:
        // 播放动画
        void play(const std::string& animName, bool repeat = false, float speed = 1.0f, bool interpolate = true);
        void play(int animIdx, bool repeat = false, float speed = 1.0f, bool interpolate = true);
        // 混合动画
        void blendTo(const std::string& animName, bool repeat = false, float speed = 1.0f, bool interpolate = true, float blendDuration = 0.2f);
        void blendTo(int animIdx, bool repeat = false, float speed = 1.0f, bool interpolate = true, float blendDuration = 0.2f);
        
        // 刷新动画
        void updateAnimation(float deltaTime);

        
	protected:
		// 处理组件事件
		virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        
        // 重载属性面板显示方法
        virtual void onInspectorGUI(InspectorPanel* inspector);

        
		REGISTER_PROPERTY_CONSTREF_GET(std::vector<glm::mat4>, m_vBonesMatrix, BonesMatrix)
		REGISTER_PROPERTY_GET(bool, m_bIsPlaying, IsPlaying)
		REGISTER_PROPERTY_GET_SET(bool, m_bUseGPU, UseGPU)
        const std::unordered_map<Mesh*, glm::vec4*>& getVertices()
        {
            return m_mVertices;
        }
        
	protected:
        // 源模型文件
        Model* m_oSrcModel;
        // 是否初始化
        bool m_bHasInit;
        
        // 顶点数组
        std::unordered_map<Mesh*, glm::vec4*> m_mVertices;
        std::unordered_map<Mesh*, glm::uvec4*> m_mBoneIndices;
        std::unordered_map<Mesh*, glm::vec4*> m_mBoneWeights;
        std::unordered_map<Mesh*, glm::vec3*> m_mNormals;
        std::unordered_map<Mesh*, glm::vec3*> m_mTangents;

        
        // 当前正在播放的动画
        AnimationPlayData m_oCurAnimation;
        // 上一个在播放的动画
        AnimationPlayData m_oBefAnimation;
        
        // 混合计时器
        float m_fBlendTimer;
        // 混合需要时间
        float m_fBlendDuration;
        // 是否正在播放
        bool m_bIsPlaying;
        // 是否使用gpu计算顶点位置
        bool m_bUseGPU;
        // 是否允许根节点的运动
        bool m_bApplyRootMotion;
        
        // 动画中的aiNode的变换矩阵
        std::unordered_map<aiNode*, aiMatrix4x4> m_mTransformations;
        // 骨骼变换矩阵
        std::vector<glm::mat4> m_vBonesMatrix;
        // TODO: 骨骼节点的变换后的位置,需要传给transform
        std::unordered_map<unsigned int, glm::vec3> m_mBonesPosition;
        std::unordered_map<unsigned int, glm::quat> m_mBonesRotation;
        std::unordered_map<unsigned int, glm::vec3> m_mBonesScale;
    };
}

