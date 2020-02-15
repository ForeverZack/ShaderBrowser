#include "LogicCore.h"
#include "Common/Tools/FileUtils.h"
#include "Common/Tools/BinaryFileUtils.h"
#include "Common/System/ECSManager.h"
#include "Common/System/AutoReleasePool.h"
#include "Browser/System/TransformSystem.h"
#include "Browser/System/MeshFilterSystem.h"
#include "Browser/System/CameraSystem.h"
#include "Browser/System/BoundBoxSystem.h"
#include "Browser/System/LightSystem.h"
#include "Browser/System/AnimationSystem.h"
#include "Common/System/Cache/TextureCache.h"
#include "Common/System/Cache/ModelCache.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "GL/System/Input.h"
#include "Core/RenderCore.h"


namespace core
{
	LogicCore::LogicCore()
        : m_eLogicState(LogicCoreState::LCS_Prepare)
        , m_uFrameIndex(1)
	{
        setLogicState(1, LogicCoreState::LCS_Prepare);
	}

	LogicCore::~LogicCore()
	{

	}

	void LogicCore::initLogic()
	{
		// 添加搜索路径
		FileUtils::getInstance()->addSearchPath(".");
		FileUtils::getInstance()->addSearchPath("res");

		// test flatbuffer
		//    BinaryFileUtils::getInstance()->serialize();
		//    BinaryFileUtils::getInstance()->deserialize();
        
        // 加载缓存
        GLProgramCache::getInstance()->init();  // 着色器缓存

		// 注册基本系统
		ECSManager::getInstance()->registerSystem(browser::TransformSystem::getInstance()); // Transform
		ECSManager::getInstance()->registerSystem(browser::MeshFilterSystem::getInstance()); // Mesh
		ECSManager::getInstance()->registerSystem(browser::CameraSystem::getInstance());	// Camera
		ECSManager::getInstance()->registerSystem(browser::BoundBoxSystem::getInstance()); // 包围盒
		ECSManager::getInstance()->registerSystem(browser::LightSystem::getInstance()); // 灯光系统
		ECSManager::getInstance()->registerSystem(browser::AnimationSystem::getInstance()); // 动画系统
		// 初始化系统
		ECSManager::getInstance()->initSystem(SystemType::Transform);    // Transform
		ECSManager::getInstance()->initSystem(SystemType::MeshFilter);    // Mesh
		ECSManager::getInstance()->initSystem(SystemType::Camera);    // Camera
		ECSManager::getInstance()->initSystem(SystemType::BoundBox);    // 包围盒
		ECSManager::getInstance()->initSystem(SystemType::Light);   // Light
		ECSManager::getInstance()->initSystem(SystemType::Animation);   // Animation
	}

	void LogicCore::logicLoop(float deltaTime)
	{
        // input
        processInput();
		
		m_oLastUpdate = std::chrono::steady_clock::now();
        
        // 在所有系统刷新前刷新transform，重置脏标记
        ECSManager::getInstance()->beforeUpdateSystem(SystemType::Transform, deltaTime);
        // 更新transform
		ECSManager::getInstance()->updateSystem(SystemType::Transform, deltaTime);
		recTime("====SystemType::Transform=====");
        // 更新动画系统
		ECSManager::getInstance()->updateSystem(SystemType::Animation, deltaTime);
		recTime("====SystemType::Animation=====");

        // Transform系统必须更新完
		while (!ECSManager::getInstance()->isSystemFinish(SystemType::Transform));

        // 更新camera
		ECSManager::getInstance()->updateSystem(SystemType::Camera, deltaTime);
		recTime("====SystemType::Camera=====");
        
        // 更新BoundBox
		ECSManager::getInstance()->updateSystem(SystemType::BoundBox, deltaTime);
		recTime("====SystemType::BoundBox=====");
        
        // 更新渲染系统（这里只负责生成渲染命令队列，并不会有任何绘制操作）
        ECSManager::getInstance()->beforeUpdateSystem(SystemType::RenderSystem, deltaTime);
        recTime("====SystemType::Render=====");

		// temp
		TextureCache::getInstance()->update(deltaTime);
		ModelCache::getInstance()->update(deltaTime);

		// auto release
		AutoReleasePool::getInstance()->update();
        
        // frame index
        ++m_uFrameIndex;
	}

	void LogicCore::recTime(const std::string& log)
	{
		return;

		auto now = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - m_oLastUpdate).count() / 1000.0f;
		m_oLastUpdate = now;
		BROWSER_LOG(log + std::to_string(deltaTime) + "ms");
	}
    
    void LogicCore::setCurFrameLogicState(LogicCoreState state)
    {
        setLogicState(m_uFrameIndex, state);
    }
    
    void LogicCore::setLogicState(unsigned long frameIndex, LogicCoreState state)
    {
        m_mLogicStates[frameIndex] = state;
    }
    
    LogicCore::LogicCoreState LogicCore::getLogicState(unsigned long frameIndex)
    {
        if(m_mLogicStates.containsKey(frameIndex))
        {
            return m_mLogicStates[frameIndex];
        }
        else
        {
            return LogicCoreState::LCS_None;
        }
    }
    
    void LogicCore::eraseLogicState(unsigned long frameIndex)
    {
        if(m_mLogicStates.containsKey(frameIndex))
        {
            m_mLogicStates.erase(frameIndex);
        }
    }
    
    //    BaseComputeProgram* _computeProgram = nullptr;
    //
    //
    //    BaseFeedback* _feedback = nullptr;
    //    int _feedbackData[] = {0, 1, 2, 3, 4};
    //    //glm::vec4 _feedbackInput[] = {
    //    //    glm::vec4(101, 102, 100, 100),
    //    //    glm::vec4(201, 102, 100, 100),
    //    //    glm::vec4(301, 102, 100, 100),
    //    //    glm::vec4(401, 102, 100, 100),
    //    //    glm::vec4(501, 102, 100, 100),
    //    //};
    //    float _feedbackInput[] = {
    //        1001, 2002, 3003, 4004, 5005
    //    };
    //
    //    int _feedbackData2[] = {
    //        0, 1, 2, 3
    //    };
    //    glm::vec4 _feedbackPosition[] = {
    //        // 骨骼1
    //        glm::vec4(0, 0, 0, 1),
    //        glm::vec4(1, 0, 0, 2),
    //        glm::vec4(2, 0, 0, 3),
    //        glm::vec4(3, 0, 0, 4),
    //    };
    //    glm::vec4 _feedbackRotation[] = {
    //        // 骨骼1
    //        glm::vec4(0, 0, 0, 0)
    //    };
    //    float _feedbackRotationTime[] = {
    //        // 骨骼1
    //        0
    //    };
    //    glm::vec4 _feedbackScale[] = {
    //        glm::vec4(1, 1, 1, 4)
    //    };
    //    int contains_bones[] = {
    //        -1, 0, -1, -1
    //    };
    //    glm::ivec3 trans_bone_keyframe_count[] = {
    //        glm::ivec3(4, 1, 1)
    //    };
    //    glm::vec4 animation_info(0, 0, 0, 0);
    //    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    //    // ---------------------------------------------------------------------------------------------------------
        void LogicCore::processInput()
        {
            if (Input::isKeyDown(GLFW_KEY_ESCAPE))
            {
                RenderCore::getInstance()->setWindowShouldClose();
            }

    
            browser::Camera* camera = CameraSystem::getInstance()->getMainCamera();
            if (camera)
            {
                if (Input::isKeyDown(GLFW_KEY_W))
                {
                    browser::Transform* transform = camera->getBelongEntity()->getComponent<browser::Transform>();
                    transform->Translate(transform->getForward() * 0.1f);
                }
                else if (Input::isKeyDown(GLFW_KEY_S))
                {
                    browser::Transform* transform = camera->getBelongEntity()->getComponent<browser::Transform>();
                    transform->Translate(-transform->getForward() * 0.1f);
                }
                else if (Input::isKeyDown(GLFW_KEY_A))
                {
                    browser::Transform* transform = camera->getBelongEntity()->getComponent<browser::Transform>();
                    transform->Translate(transform->getLeft() * 0.1f);
                }
                else if (Input::isKeyDown(GLFW_KEY_D))
                {
                    browser::Transform* transform = camera->getBelongEntity()->getComponent<browser::Transform>();
                    transform->Translate(-transform->getLeft() * 0.1f);
                }
                else if (Input::isKeyDown(GLFW_KEY_E))
                {
                    browser::Transform* transform = camera->getBelongEntity()->getComponent<browser::Transform>();
                    transform->Rotate(glm::vec3(0, -1.0f, 0), Space::Self);
                }
                else if (Input::isKeyDown(GLFW_KEY_Q))
                {
                    browser::Transform* transform = camera->getBelongEntity()->getComponent<browser::Transform>();
                    transform->Rotate(glm::vec3(0, 1.0f, 0), Space::Self);
                }
            }
            
    //            else if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    //            {
    ////                static int playAniIndx = 2;
    ////                m_YBotEntity->getAnimator()->blendTo(playAniIndx, true, 1, true);
    //            }
    //            else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    //            {
    //    //            if (!_feedback)
    //    //            {
    //    //                _feedback = new BaseFeedback();
    //    //                const char* varyings[] = {"outValue", "outValue2"};
    //    //                const std::string vert_full_path = FileUtils::getInstance()->getAbsolutePathForFilename("shaders/default/anim_feedback_test2.vert");
    //    //                _feedback->initFeedback(vert_full_path.c_str(), varyings, 2, GL_SEPARATE_ATTRIBS);
    //    //                _feedback->retain();
    //    //
    //    //                BROWSER_LOG(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS)
    //    //
    //    //                _feedback->addVertexAttribute(0, 1, GL_INT, GL_FALSE, 0, (void*)0, _feedbackData, sizeof(_feedbackData), VertexDataType::Int);
    //    //                _feedback->addFeedbackBuffer(sizeof(_feedbackData), "outValue", FeedbackBufferType::TextureBuffer);
    //    //                _feedback->addFeedbackBuffer(sizeof(_feedbackData), "outValue2", FeedbackBufferType::TextureBuffer);
    //    //                _feedback->setupVAOandVBOs();
    //    //
    //    //                // tbo
    //    //                GLuint vbo, tex;
    //    //                glGenTextures(1, &tex);
    //    //                glGenBuffers(1, &vbo);
    //    //                glBindBuffer(GL_TEXTURE_BUFFER, vbo);
    //    //                glBufferData(GL_TEXTURE_BUFFER, sizeof(_feedbackInput), _feedbackInput, GL_STATIC_DRAW);
    //    //                glBindTexture(GL_TEXTURE_BUFFER, tex);
    //    //                // 将缓存区关联到纹理对象上tbo
    //    //                glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, vbo);
    //    //                _feedback->setUniformSamplerBuffer("testInput", tex);
    //    //            }
    //    //            _feedback->flushAsPoints(5);
    //    //
    //    //            GLfloat output[5];
    //    //            _feedback->getOutputDataFromVBOs("outValue", output, sizeof(output));
    //    //            printf("outValue === %f %f %f %f %f\n", output[0], output[1], output[2], output[3], output[4]);
    //    //            _feedback->getOutputDataFromVBOs("outValue2", output, sizeof(output));
    //    //            printf("outValue2 === %f %f %f %f %f\n", output[0], output[1], output[2], output[3], output[4]);
    //
    ////                if (!_feedback)
    ////                {
    ////                    _feedback = new BaseFeedback();
    ////                    const char* varyings[] = {"result_position", "result_rotation", "result_scale"};
    ////                    const std::string vert_full_path = FileUtils::getInstance()->getAbsolutePathForFilename("shaders/default/anim_feedback.vert");
    ////                    _feedback->initFeedback(vert_full_path.c_str(), varyings, 3, GL_SEPARATE_ATTRIBS);
    ////                    _feedback->retain();
    ////
    ////                    _feedback->addVertexAttribute(0, 1, GL_INT, GL_FALSE, 0, (void*)0, _feedbackData2, sizeof(_feedbackData2), VertexDataType::Int);
    ////                    _feedback->addFeedbackBuffer(sizeof(glm::vec4)*4, varyings[0], BufferType::TextureBuffer); //boneMatrix1
    ////                    _feedback->addFeedbackBuffer(sizeof(glm::vec4)*4, varyings[1], BufferType::TextureBuffer); //boneMatrix2
    ////                    _feedback->addFeedbackBuffer(sizeof(glm::vec4)*4, varyings[2], BufferType::TextureBuffer); //boneMatrix3
    ////                    _feedback->setupVAOandVBOs();
    ////
    ////                    // tbo
    ////                    GLuint vbos[4], texs[4];
    ////                    glGenTextures(4, texs);
    ////                    glGenBuffers(4, vbos);
    ////                    // position
    ////                    glBindBuffer(GL_TEXTURE_BUFFER, vbos[0]);
    ////                    glBufferData(GL_TEXTURE_BUFFER, sizeof(_feedbackPosition), _feedbackPosition, GL_STATIC_DRAW);
    ////                    glBindTexture(GL_TEXTURE_BUFFER, texs[0]);
    ////                    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[0]);
    ////                    _feedback->setUniformSamplerBuffer("position_keys", texs[0]);
    ////                    // rotation
    ////                    glBindBuffer(GL_TEXTURE_BUFFER, vbos[1]);
    ////                    glBufferData(GL_TEXTURE_BUFFER, sizeof(_feedbackRotation), _feedbackRotation, GL_STATIC_DRAW);
    ////                    glBindTexture(GL_TEXTURE_BUFFER, texs[1]);
    ////                    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[1]);
    ////                    _feedback->setUniformSamplerBuffer("rotation_keys", texs[1]);
    ////    //                _feedback->setUniformFloatV("rotation_times", 1, _feedbackRotationTime);
    ////                    // rotation_times
    ////                    glBindBuffer(GL_TEXTURE_BUFFER, vbos[3]);
    ////                    glBufferData(GL_TEXTURE_BUFFER, sizeof(_feedbackRotation), _feedbackRotationTime, GL_STATIC_DRAW);
    ////                    glBindTexture(GL_TEXTURE_BUFFER, texs[3]);
    ////                    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, vbos[3]);
    ////                    _feedback->setUniformSamplerBuffer("rotation_times", texs[3]);
    ////                    //scale
    ////                    glBindBuffer(GL_TEXTURE_BUFFER, vbos[2]);
    ////                    glBufferData(GL_TEXTURE_BUFFER, sizeof(_feedbackScale), _feedbackScale, GL_STATIC_DRAW);
    ////                    glBindTexture(GL_TEXTURE_BUFFER, texs[2]);
    ////                    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbos[2]);
    ////                    _feedback->setUniformSamplerBuffer("scale_keys", texs[2]);
    ////                    // contains_bones
    ////                    _feedback->setUniformIntV("contains_bones", 4, contains_bones);
    ////                    // trans_bone_keyframe_count
    ////                    _feedback->setUniformIVec3V("trans_bone_keyframe_count", 1, glm::value_ptr(trans_bone_keyframe_count[0]));
    ////                    // animation_info
    ////                    _feedback->setUniformV4f("animation_info", animation_info);
    ////                    // bone_count
    ////                    _feedback->setUniformInt("bone_count", 4);
    ////                }
    ////                _feedback->flushAsPoints(4);
    ////
    ////                GLfloat output[16];
    ////                // result_position
    ////                _feedback->getOutputDataFromVBOs("result_position", output, sizeof(output));
    ////                printf("result_position === (%f, %f, %f, %f), (%f, %f ,%f, %f), (%f, %f ,%f, %f), (%f, %f ,%f, %f)\n", output[0], output[1], output[2], output[3], output[4], output[5], output[6], output[7], output[8], output[9], output[10], output[11], output[12], output[13], output[14], output[15]);
    ////                // result_rotation
    ////                _feedback->getOutputDataFromVBOs("result_rotation", output, sizeof(output));
    ////                printf("result_rotation === (%f, %f, %f, %f), (%f, %f ,%f, %f), (%f, %f ,%f, %f), (%f, %f ,%f, %f)\n", output[0], output[1], output[2], output[3], output[4], output[5], output[6], output[7], output[8], output[9], output[10], output[11], output[12], output[13], output[14], output[15]);
    ////                // result_scale
    ////                _feedback->getOutputDataFromVBOs("result_scale", output, sizeof(output));
    ////                printf("result_scale === (%f, %f, %f, %f), (%f, %f ,%f, %f), (%f, %f ,%f, %f), (%f, %f ,%f, %f)\n", output[0], output[1], output[2], output[3], output[4], output[5], output[6], output[7], output[8], output[9], output[10], output[11], output[12], output[13], output[14], output[15]);
    //
    //            }
    //            else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    //            {
    ////                if (!_computeProgram)
    ////                {
    ////                    _computeProgram = new BaseComputeProgram();
    ////                    _computeProgram->retain();
    ////
    ////                    const std::string vert_full_path = FileUtils::getInstance()->getAbsolutePathForFilename("shaders/default/anim_compute_prog_test.comp");
    ////                    _computeProgram->initComputeProgram(vert_full_path.c_str(), 5);
    ////                    _computeProgram->addComputeBuffer("buffers", sizeof(float) * 4 * 5, GL_READ_WRITE, BufferType::TextureBuffer, GL_RGBA32F);
    ////                    _computeProgram->setupVBOs();
    ////                }
    ////                _computeProgram->executeComputeProgram();
    ////                GLfloat output[20];
    ////                _computeProgram->getOutputDataFromVBOs(0, output, sizeof(output));
    ////                printf("result === (%f, %f, %f, %f), (%f, %f ,%f, %f), (%f, %f ,%f, %f), (%f, %f ,%f, %f), (%f, %f ,%f, %f)\n", output[0], output[1], output[2], output[3], output[4], output[5], output[6], output[7], output[8], output[9]
    ////                        , output[10], output[11], output[12], output[13], output[14], output[15], output[16], output[17], output[18], output[19]);
    //
    //            }
    //        }
            

        }


}
