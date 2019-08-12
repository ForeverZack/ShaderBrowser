#include "AnimatorComputeProgram.h"
#include "Common/Tools/FileUtils.h"
#include <glm/gtc/type_ptr.hpp>

using namespace customGL;

namespace browser
{
	// 顶点着色器shader
#define COMPUTE_SHADER_PROGRAM_SRC "shaders/default/anim_compute_prog.comp"

	AnimatorComputeProgram* AnimatorComputeProgram::create(Model* model)
	{
		AnimatorComputeProgram* program = new AnimatorComputeProgram();
		if (!program->initComputeProgram(model))
		{
			delete program;
			return nullptr;
		}
		return program;
	}

	AnimatorComputeProgram::AnimatorComputeProgram()
		: m_iBoneCount(0)
		, m_pCurAnimation(nullptr)
		, m_oSrcModel(nullptr)
		, m_oAnimationInfo(GLM_VEC4_ZERO)
		, m_pSamplerBuffer(nullptr)
		, m_iSamplerSize(0)
	{
	}

	AnimatorComputeProgram::~AnimatorComputeProgram()
	{
		BROWSER_SAFE_RELEASE_POINTER(m_pSamplerBuffer);
	}

	bool AnimatorComputeProgram::initComputeProgram(Model* model)
	{
		m_oSrcModel = model;
		m_iBoneCount = model->getBoneNum();
		m_iSamplerSize = sizeof(glm::vec4) * m_iBoneCount * 3;
		m_pSamplerBuffer = new float[m_iSamplerSize];
		std::string comp_shader_fullpath = FileUtils::getInstance()->getAbsolutePathForFilename(COMPUTE_SHADER_PROGRAM_SRC);

		std::string addtionVertCode = "";

		// 初始化并设置计算着色器
		BaseComputeProgram::initComputeProgram(comp_shader_fullpath.c_str(), m_iBoneCount);
		addComputeBuffer("result_buffers", m_iSamplerSize, GL_READ_WRITE, BufferType::TextureBuffer, GL_RGBA32F);
		setupVBOs();

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

	void AnimatorComputeProgram::setCurrentAnimationData(aiAnimation* animation)
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

	void AnimatorComputeProgram::play(float sampleUnscaled, std::unordered_map<unsigned int, glm::vec3>& bonesPosition, std::unordered_map<unsigned int, glm::quat>& bonesRotation, std::unordered_map<unsigned int, glm::vec3>& bonesScale)
	{
		m_oAnimationInfo[0] = sampleUnscaled;
		setUniformV4f("animation_info", m_oAnimationInfo);

		executeComputeProgram();

		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// 获取Feedback数据
		getOutputDataFromVBOs(0, m_pSamplerBuffer, m_iSamplerSize);
		for (int i = 0; i < m_iBoneCount; ++i)
		{
			            //// test 和model::computeBonesTransform的正确结果对比
			            //BROWSER_LOG("===========================")
			            //std::cout<<"model::compute position= "<<bonesPosition[i][0]<<", "<<bonesPosition[i][1]<<", "<<bonesPosition[i][2]<<endl;
			            //std::cout<<"feedback position = "<< m_pSamplerBuffer[12*i]<<", "<< m_pSamplerBuffer[12*i+1]<<", "<< m_pSamplerBuffer[12*i+2]<<", "<< m_pSamplerBuffer[12*i+3]<<endl;
			
			            //std::cout<<"model::compute rotation= "<<bonesRotation[i].x<<", "<<bonesRotation[i].y<<", "<<bonesRotation[i].z<<", "<<bonesRotation[i].w<<endl;
			            //std::cout<<"feedback rotation = "<< m_pSamplerBuffer[12*i+7]<<", "<< m_pSamplerBuffer[12*i+4]<<", "<< m_pSamplerBuffer[12*i+5]<<", "<< m_pSamplerBuffer[12*i+6]<<endl;
			
			            //std::cout<<"model::compute sacle= "<<bonesScale[i][0]<<", "<<bonesScale[i][1]<<", "<<bonesScale[i][2]<<endl;
			            //std::cout<<"feedback sacle = "<< m_pSamplerBuffer[12*i+8]<<", "<< m_pSamplerBuffer[12*i+9]<<", "<< m_pSamplerBuffer[12*i+10]<<", "<< m_pSamplerBuffer[12*i+11]<<endl;


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


			bonesPosition[i] = std::move(glm::vec3(m_pSamplerBuffer[12*i], m_pSamplerBuffer[12*i + 1], m_pSamplerBuffer[12*i + 2]));	//	position: 0,1,2,3
			bonesRotation[i] = std::move(glm::quat(m_pSamplerBuffer[12 * i + 7], m_pSamplerBuffer[12*i+4], m_pSamplerBuffer[12*i + 5], m_pSamplerBuffer[12*i + 6]));	// rotation: 4,5,6,7
			bonesScale[i] = std::move(glm::vec3(m_pSamplerBuffer[12*i+8], m_pSamplerBuffer[12*i + 9], m_pSamplerBuffer[12*i + 10]));	// scale: 8,9,10,11
		}

		//        BROWSER_LOG("=======end=======")
		//        BROWSER_LOG("=======end======11====")
	}


}
