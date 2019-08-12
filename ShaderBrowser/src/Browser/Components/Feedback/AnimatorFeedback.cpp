#include "AnimatorFeedback.h"
#include "Common/Tools/FileUtils.h"
#include <glm/gtc/type_ptr.hpp>

using namespace customGL;

namespace browser
{
    // 顶点着色器shader
#define VERTEX_SHADER_PROGRAM_SRC "shaders/default/anim_feedback.vert"
    // varyings
    // 名称
    static const char* VARYINGS[] = {"result_position", "result_rotation", "result_scale"};
    // 数量
    static const int VARYINGS_NUM = 3;
    
    AnimatorFeedback* AnimatorFeedback::create(Model* model)
    {
        AnimatorFeedback* feedback = new AnimatorFeedback();
        if (!feedback->initFeedback(model))
        {
            delete feedback;
            return nullptr;
        }
        return feedback;
    }
    
	AnimatorFeedback::AnimatorFeedback()
        : m_iBoneCount(0)
        , m_pCurAnimation(nullptr)
        , m_oSrcModel(nullptr)
        , m_oAnimationInfo(GLM_VEC4_ZERO)
        , m_pSamplerPosition(nullptr)
        , m_pSamplerRotation(nullptr)
        , m_pSamplerScale(nullptr)
        , m_iSamplerSize(0)
    {
	}

	AnimatorFeedback::~AnimatorFeedback()
	{
        BROWSER_SAFE_RELEASE_POINTER(m_pSamplerPosition);
        BROWSER_SAFE_RELEASE_POINTER(m_pSamplerRotation);
        BROWSER_SAFE_RELEASE_POINTER(m_pSamplerScale);
	}
    
    bool AnimatorFeedback::initFeedback(Model* model)
    {
        m_oSrcModel = model;
        m_iBoneCount = model->getBoneNum();
        m_iSamplerSize = sizeof(glm::vec4) * m_iBoneCount;
        m_pSamplerPosition = new float[m_iSamplerSize];
        m_pSamplerRotation = new float[m_iSamplerSize];
        m_pSamplerScale = new float[m_iSamplerSize];
        std::string vert_shader_fullpath = FileUtils::getInstance()->getAbsolutePathForFilename(VERTEX_SHADER_PROGRAM_SRC);
        
        std::string addtionVertCode = "";
        
        BaseFeedback::initFeedback(vert_shader_fullpath.c_str(), VARYINGS, VARYINGS_NUM, addtionVertCode, GL_SEPARATE_ATTRIBS);
        
        // 注意，由于这里的骨骼id都是我自己创建的，我知道他是从0开始连续的，所以这里我直接生成了一个骨骼id的数组传给gpu了。
        std::vector<int> boneIds;
        boneIds.resize(m_iBoneCount);
        for(int i=0; i<m_iBoneCount; ++i)
        {
            boneIds[i] = i;
        }
        addVertexAttribute(0, 1, GL_INT, GL_FALSE, 0, (void*)0, &boneIds[0], sizeof(int)*m_iBoneCount, VertexDataType::Int);
        addFeedbackBuffer(sizeof(glm::vec4)*m_iBoneCount, VARYINGS[0], BufferType::TextureBuffer); // position
        addFeedbackBuffer(sizeof(glm::vec4)*m_iBoneCount, VARYINGS[1], BufferType::TextureBuffer); // rotation
        addFeedbackBuffer(sizeof(glm::vec4)*m_iBoneCount, VARYINGS[2], BufferType::TextureBuffer); // scale
        setupVAOandVBOs();
        
        // 设置骨骼数量
        setUniformInt("bone_count", m_iBoneCount);
        // 设置采样用到的samplerBuffer
        // position_keys
        GLuint position_keys = m_oSrcModel->getGpuAnimSamplerBuffer(ModelGpuAnimationData::ModelGpuAnimBufferType::PositionKeys);
        setUniformSamplerBuffer("position_keys", position_keys);
        // rotation_keys
        GLuint rotation_keys = m_oSrcModel->getGpuAnimSamplerBuffer(ModelGpuAnimationData::ModelGpuAnimBufferType::RotationKeys);
        setUniformSamplerBuffer("rotation_keys", rotation_keys);
        // rotation_times
        GLuint rotation_times = m_oSrcModel->getGpuAnimSamplerBuffer(ModelGpuAnimationData::ModelGpuAnimBufferType::RotationTimes);
        setUniformSamplerBuffer("rotation_times", rotation_times);
        // scale_keys
        GLuint scale_keys = m_oSrcModel->getGpuAnimSamplerBuffer(ModelGpuAnimationData::ModelGpuAnimBufferType::ScaleKeys);
        setUniformSamplerBuffer("scale_keys", scale_keys);
        
        
        return true;
    }
    
    void AnimatorFeedback::setCurrentAnimationData(aiAnimation* animation)
    {
        if (m_pCurAnimation == animation)
        {
            return;
        }
        
        m_pCurAnimation = animation;

        // contains_bones
        const std::vector<int>& contains_bones = m_oSrcModel->getContainsBones(animation);
        setUniformIntV("contains_bones", contains_bones.size(), &contains_bones[0]);
        // trans_bone_keyframe_count
        const std::vector<glm::ivec3>& trans_bone_keyframe_count = m_oSrcModel->getTransBonesKeyframeCount(animation);
        setUniformIVec3V("trans_bone_keyframe_count", trans_bone_keyframe_count.size(), glm::value_ptr(trans_bone_keyframe_count[0]));
        // keys_offset
        const glm::ivec3& keys_offset = m_oSrcModel->getKeysOffset(animation);
        setUniformIVec3("keys_offset", keys_offset);
    }
    
    void AnimatorFeedback::play(float sampleUnscaled, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale)
    {
        m_oAnimationInfo[0] = sampleUnscaled;
        setUniformV4f("animation_info", m_oAnimationInfo);
        
        flushAsPoints(m_iBoneCount);
        
        // 获取Feedback数据
        getOutputDataFromVBOs("result_position", m_pSamplerPosition, m_iSamplerSize);
        getOutputDataFromVBOs("result_rotation", m_pSamplerRotation, m_iSamplerSize);
        getOutputDataFromVBOs("result_scale", m_pSamplerScale, m_iSamplerSize);
        for(int i=0; i<m_iBoneCount; ++i)
        {
//            // test 和model::computeBonesTransform的正确结果对比
//            BROWSER_LOG("===========================")
//            std::cout<<"model::compute position= "<<bonesPosition[i][0]<<", "<<bonesPosition[i][1]<<", "<<bonesPosition[i][2]<<endl;
//            std::cout<<"feedback position = "<<m_pSamplerPosition[4*i]<<", "<<m_pSamplerPosition[4*i+1]<<", "<<m_pSamplerPosition[4*i+2]<<", "<<m_pSamplerPosition[4*i+3]<<endl;
//
//            std::cout<<"model::compute rotation= "<<bonesRotation[i].x<<", "<<bonesRotation[i].y<<", "<<bonesRotation[i].z<<", "<<bonesRotation[i].w<<endl;
//            std::cout<<"feedback rotation = "<<m_pSamplerRotation[4*i]<<", "<<m_pSamplerRotation[4*i+1]<<", "<<m_pSamplerRotation[4*i+2]<<", "<<m_pSamplerRotation[4*i+3]<<endl;
//
//            std::cout<<"model::compute sacle= "<<bonesScale[i][0]<<", "<<bonesScale[i][1]<<", "<<bonesScale[i][2]<<endl;
//            std::cout<<"feedback sacle = "<<m_pSamplerScale[4*i]<<", "<<m_pSamplerScale[4*i+1]<<", "<<m_pSamplerScale[4*i+2]<<", "<<m_pSamplerScale[4*i+3]<<endl;
            

//            // 检查buffer数据是否正确
//            BROWSER_LOG_VEC3(glm::vec3(m_pSamplerScale[4*i], m_pSamplerScale[4*i+1], m_pSamplerScale[4*i+2]))
            
//            const std::vector<glm::vec4>& position_keys = m_oSrcModel->getGpuAnimData()->getPositionKeys();
//            const std::vector<int>& contains_bones = m_oSrcModel->getContainsBones(m_pCurAnimation);
//            const std::vector<glm::ivec3>& trans_bone_keyframe_count = m_oSrcModel->getTransBonesKeyframeCount(m_pCurAnimation);
//            int startUV = 0;
//            {
//                const glm::ivec3& keys_offset = m_oSrcModel->getKeysOffset(m_pCurAnimation);
//
//
//                int index = 0;
//                int bufferIndex = contains_bones[i];
//                startUV = keys_offset[0];
//                for (int j=0; j<m_iBoneCount; ++j)
//                {
//                    index = contains_bones[j];
//                    if (index!=-1 && index<bufferIndex)
//                    {
//                        startUV += trans_bone_keyframe_count[index][0];
//                    }
//                }
//            }
//            int index = contains_bones[i];
//            int count = trans_bone_keyframe_count[index][0];
//            glm::vec4 leftFrame, rightFrame;
//            {
//                if (count == 1)
//                {
//                    leftFrame = rightFrame = position_keys[startUV];
//                }
//                else
//                {
//                    // 时间超过最后一个关键帧时间，则停止在最后一个关键帧
//                    glm::vec4 keyValue = position_keys[startUV+count-1];
//                    if (keyValue[3] < m_oAnimationInfo[0])
//                    {
//                        leftFrame = rightFrame = keyValue;
//                    }
//                    else
//                    {
//                        // 检测关键帧位置
//                        for (int k=0; k<count; ++k)
//                        {
//                            keyValue = position_keys[startUV+k];
//                            if(keyValue[3] >= m_oAnimationInfo[0])
//                            {
//                                leftFrame = position_keys[startUV+k-1];
//                                rightFrame = keyValue;
//                                break;
//                            }
//                        }
//                    }
//                }
//            }
//            std::cout<<"leftFrame = "<<leftFrame[0]<<", "<<leftFrame[1]<<", "<<leftFrame[2]<<", "<<leftFrame[3]<<endl;
//            std::cout<<"rightFrame = "<<rightFrame[0]<<", "<<rightFrame[1]<<", "<<rightFrame[2]<<", "<<leftFrame[3]<<endl;
//            {
//                aiVectorKey* src_keys;
//                int numPositionKeys;
//                aiNodeAnim* channel;
//                const std::unordered_map<std::string, unsigned int>* boneIdMap = m_oSrcModel->getBonesIdMapPointer();
//                for(int j=0; j<m_pCurAnimation->mNumChannels; ++j)
//                {
//                    channel = m_pCurAnimation->mChannels[j];
//                    auto boneItor = (*boneIdMap).find(channel->mNodeName.C_Str());
//                    if (boneItor!=(*boneIdMap).end() && boneItor->second==i)
//                    {
//                        src_keys = channel->mPositionKeys;
//                        numPositionKeys = channel->mNumPositionKeys;
//                        break;
//                    }
//                }
//                for (int j=0; j<numPositionKeys; ++j)
//                {
//                    if (m_oAnimationInfo[0] <= src_keys[j].mTime)
//                    {
//                        rightFrame = glm::vec4(src_keys[j].mValue.x, src_keys[j].mValue.y, src_keys[j].mValue.z, src_keys[j].mTime);
//                        if (j != 0)
//                        {
//                            leftFrame = glm::vec4(src_keys[j-1].mValue.x, src_keys[j-1].mValue.y, src_keys[j-1].mValue.z, src_keys[j-1].mTime);
//                        }
//                        else
//                        {
//                            leftFrame = rightFrame;
//                        }
//                        break;
//                    }
//                }
//
//                std::cout<<"src leftFrame = "<<leftFrame[0]<<", "<<leftFrame[1]<<", "<<leftFrame[2]<<", "<<leftFrame[3]<<endl;
//                std::cout<<"src rightFrame = "<<rightFrame[0]<<", "<<rightFrame[1]<<", "<<rightFrame[2]<<", "<<leftFrame[3]<<endl;
//            }
            
            
            bonesPosition[i] = std::move(glm::vec3(m_pSamplerPosition[4*i], m_pSamplerPosition[4*i+1], m_pSamplerPosition[4*i+2]));
            bonesRotation[i] = std::move(glm::quat(m_pSamplerRotation[4*i+3], m_pSamplerRotation[4*i], m_pSamplerRotation[4*i+1], m_pSamplerRotation[4*i+2]));
            bonesScale[i] = std::move(glm::vec3(m_pSamplerScale[4*i], m_pSamplerScale[4*i+1], m_pSamplerScale[4*i+2]));
        }
        
//        BROWSER_LOG("=======end=======")
//        BROWSER_LOG("=======end======11====")
    }
    

}
