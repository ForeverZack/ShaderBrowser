#include "SkeletonAnimation.h"
#include "GL/Skeleton.h"
#include "GL/Assimp.h"
#include "GL/AssimpConvert.h"

using namespace common;

namespace customGL
{
    // 默认骨骼动画帧频
    const unsigned int SkeletonAnimation::DEFAULT_SKELETON_ANIMATION_FRAME_RATE = 24;
    
    // 骨骼动画序列帧数据
    SkeletonAnimationFrame::SkeletonAnimationFrame(int frame, float t)
        : frame_index(frame)
        , time(t)
    {
        positions.clear();
        rotations.clear();
        scales.clear();
    }
    
    SkeletonAnimationFrame::~SkeletonAnimationFrame()
    {
        
    }
    
    
    
    // 骨骼动画
    static auto GLM_Vec3_Lerp = Assimp::Interpolator<glm::vec3>();
    static auto GLM_Quat_Lerp = Assimp::Interpolator<glm::quat>();
    
    SkeletonAnimation::SkeletonAnimation(aiAnimation* animation)
        : m_pAniamtion(animation)
    {
    }
    
    SkeletonAnimation::~SkeletonAnimation()
    {
        
    }
    
    void SkeletonAnimation::convertAnimation2Frames(unsigned int frameRate, aiNode* rootNode, Skeleton* skeleton)
    {
        // 总帧数
        m_uFrameCount = m_pAniamtion->mDuration * frameRate;
        // 帧率
        m_uFramePerSec = frameRate;

        m_vFrames.reserve(m_uFrameCount);
        m_vFrames.clear();

        // 转换序列帧
        aiNodeAnim* channel = nullptr;
        aiNode* node = nullptr;
        unsigned int boneId;
        float elapsedTime;
        shared_ptr<SkeletonAnimationFrame> skeletonAnimationFrame;
        for(float frameIdx=0; frameIdx<m_uFrameCount; ++frameIdx)
        {
            elapsedTime = 1.0f / m_uFramePerSec * frameIdx;
            skeletonAnimationFrame = make_shared<SkeletonAnimationFrame>(frameIdx, elapsedTime);
            m_vFrames.push_back(skeletonAnimationFrame);

            // 遍历骨骼变换信息，生成父节点坐标系下自身的变换矩阵
            // 遍历发生变换的骨骼节点的变换信息
            for (unsigned int i = 0; i < m_pAniamtion->mNumChannels; ++i)
            {
                channel = m_pAniamtion->mChannels[i];
                node = rootNode->FindNode(channel->mNodeName);

                // 根据当前动画播放到第几帧，插值得到变换信息
                // 位移
                auto translation = Assimp::InterpolationGet<aiVector3D>(frameIdx, channel->mPositionKeys, channel->mNumPositionKeys, false);
                //                if (!applyRootMotion && node==m_oRootBoneNode)
                //                {
                //                    translation = aiVector3D(0, 0, 0);
                //                }
                // 旋转
                auto rotation = Assimp::InterpolationGet<aiQuaternion>(frameIdx, channel->mRotationKeys, channel->mNumRotationKeys, false);
                // 缩放
                auto scale = Assimp::InterpolationGet<aiVector3D>(frameIdx, channel->mScalingKeys, channel->mNumScalingKeys, false);
                // 记录骨骼变换
                if(skeleton->isBone(channel->mNodeName.C_Str(), boneId))
                {
                    skeletonAnimationFrame->positions[boneId] = std::move(Assimp::ConvertToGLM(translation));
                    skeletonAnimationFrame->rotations[boneId] = std::move(Assimp::ConvertToGLM(rotation));
                    skeletonAnimationFrame->scales[boneId] = std::move(Assimp::ConvertToGLM(scale));
                }


            }

        }
        
        
    }
    
    void SkeletonAnimation::getBonesTransform(float elapsedTime, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale, bool interpolateAnimation/* = true*/)
    {
        BROWSER_ASSERT(m_uFrameCount>=1, "Animation frame count must be more than 1, please make sure that SkeletonAnimation has been inited in function SkeletonAnimation::getBonesTransform.");
        
        // 根据时间找到相邻两帧数据
        shared_ptr<SkeletonAnimationFrame> leftFrame, rightFrame;
        if (m_uFrameCount == 1)
        {
            leftFrame = rightFrame = m_vFrames[0];
        }
        else if(m_vFrames[m_uFrameCount-1]->time < elapsedTime)
        {
            leftFrame = rightFrame = m_vFrames[m_uFrameCount - 1];
        }
        else
        {
            for (int frameIdx=0; frameIdx<m_uFrameCount; ++frameIdx)
            {
                if(m_vFrames[frameIdx]->time >= elapsedTime)
                {
                    leftFrame = m_vFrames[(frameIdx-1)%(m_uFrameCount+1)];  // 循环
                    rightFrame = m_vFrames[frameIdx];
                    break;
                }
            }
        }

        // 根据需要进行插值
        if (interpolateAnimation)
        {
            // 插值
            const auto progress = elapsedTime - leftFrame->time;
            const auto differenz = rightFrame->time - leftFrame->time;
            const auto weight = (progress) / (differenz);
            
            unsigned int boneId;
            for(auto itor=leftFrame->positions.begin(); itor!=leftFrame->positions.end(); ++itor)
            {
                boneId = itor->first;
                GLM_Vec3_Lerp(bonesPosition[boneId], leftFrame->positions[boneId], rightFrame->positions[boneId], weight);
                GLM_Quat_Lerp(bonesRotation[boneId], leftFrame->rotations[boneId], rightFrame->rotations[boneId], weight);
                GLM_Vec3_Lerp(bonesScale[boneId], leftFrame->scales[boneId], rightFrame->scales[boneId], weight);
            }
        }
        else
        {
            bonesPosition = leftFrame->positions;
            bonesRotation = leftFrame->rotations;
            bonesScale = leftFrame->scales;
        }
    }
    
    

    
}
