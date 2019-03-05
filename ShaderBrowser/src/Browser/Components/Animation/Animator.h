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
    
	class Animator : public BaseComponent
	{
	public:
		Animator();
		~Animator();

	public:
        // 播放动画
        void play(const std::string& animName, bool repeat = false, float speed = 1.0f, bool interpolate = true);
        
        // 刷新动画
        void updateAnimation(float deltaTime);

        
	protected:
		// 处理组件事件
		virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        
        // 重载属性面板显示方法
        virtual void onInspectorGUI(InspectorPanel* inspector);

        
        REGISTER_PROPERTY_GET(bool, m_bIsPlaying, IsPlaying)
        const std::unordered_map<Mesh*, std::vector<VertexData>>& getVertices()
        {
            return m_mVertices;
        }
        
	protected:
        // 源模型文件
        Model* m_oSrcModel;
        // 是否初始化
        bool m_bHasInit;
        
        // 顶点数组
        std::unordered_map<Mesh*, std::vector<VertexData>> m_mVertices;
        
        // 当前正在播放的动画
        std::string m_sCurAnimName;
        aiAnimation* m_oAnimation;
        // 播放时间
        float m_fElapsedTime;
        // 播放速度
        float m_fSpeed;
        // 是否插值处理动画
        bool m_bInterpolate;
        // 是否循环
        bool m_bRepeat;
        // 是否正在播放
        bool m_bIsPlaying;
        
        // 是否使用gpu计算顶点位置
        bool m_bUseGPU;
        
        // 动画中的aiNode的变换矩阵
        std::unordered_map<aiNode*, aiMatrix4x4> m_mTransformations;
        // 骨骼变换矩阵
        std::vector<glm::mat4> m_vBonesMatrix;
	};
}
