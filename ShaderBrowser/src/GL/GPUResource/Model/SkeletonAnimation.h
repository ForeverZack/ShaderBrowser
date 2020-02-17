#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Common/Components/Reference.h"


using namespace common;

namespace browser
{
	class Transform;
};

namespace customGL
{
    class Skeleton;
    
    // 骨骼动画序列帧数据
    class SkeletonAnimationFrame
    {
    public:
        SkeletonAnimationFrame(int frame, float t);
        ~SkeletonAnimationFrame();
        
    public:
        // 第几帧
        int frame_index;
        // 当前时间
        float time;
        // 发生变换的骨骼位置信息
        std::unordered_map<unsigned int, glm::vec3> positions;
        // 发生变换的骨骼旋转信息
        std::unordered_map<unsigned int, glm::quat> rotations;
        // 发生变换的骨骼缩放信息
        std::unordered_map<unsigned int, glm::vec3> scales;
    };
    
    
    // 骨骼动画数据
    class SkeletonAnimation : public Reference
    {
    public:
        // 默认骨骼动画帧频
        static const unsigned int DEFAULT_SKELETON_ANIMATION_FRAME_RATE;
        
    public:
        SkeletonAnimation(aiAnimation* animation);
        ~SkeletonAnimation();
        
    public:
        // 方案一：在加载模型时将骨骼动画转为序列帧，帧数根据设置进行插值(windows下渲染耗时略有缩小，但加载过程太过耗时)
        // 将animation转为序列帧数据
        void convertAnimation2Frames(unsigned int frameRate, aiNode* rootNode, Skeleton* skeleton);
        // 获取骨骼数据
        void getBonesTransform(float elapsedTime, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale, bool interpolateAnimation = true);
        
        // 方案二：正常计算(采用多线程的方式)
        // 获取骨骼数据
        void computeBonesTransform(aiNode* rootNode, Skeleton* skeleton, float elapsedTime, const std::vector<browser::Transform*>& allBones, bool interpolateAnimation = true, bool applyRootMotion = false);
        
    protected:
        // 原始动画数据
        aiAnimation* m_pAniamtion;
        // 帧数
        unsigned int m_uFrameCount;
        // 每秒帧数、
        unsigned int m_uFramePerSec;
        // 动画序列帧
        std::vector<shared_ptr<SkeletonAnimationFrame>> m_vFrames;
        
    };
    
    
}

