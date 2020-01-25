#include "RenderCore.h"
#include "Common/System/ECSManager.h"
#include "Common/Tools/UI/GUIFramework.h"
#include "Common/Tools/UI/InspectorPanel.h"
#include "Common/Tools/UI/GameStatusPanel.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "Browser/System/RenderSystem.h"
#include "GL/GLStateCache.h"
#include "GL/System/GPUOperateSystem.h"
#include "Core/LogicCore.h"

namespace core
{
    #ifdef _WIN32
        typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
        typedef int(*PFNWGLEXTGETSWAPINTERVALPROC) (void);
        PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
        PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;
        // 初始化函数指针接口
        bool InitVSync()
        {
            BROWSER_LOG(glGetString(GL_RENDERER));
            BROWSER_LOG(glGetString(GL_VERSION));
            BROWSER_LOG(glGetString(GL_VENDOR));

            // 注意：获取扩展（Extensions）只有在GLFW_OPENGL_COMPAT_PROFILE模式下才能获取到，以核心模式运行时获取不到，具体原因不清楚
            const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
            if (extensions && strstr(extensions, "WGL_EXT_swap_control"))
            {
                wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)wglGetProcAddress("wglSwapIntervalEXT");
                wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
                return true;
            }

            return false;
        }

        // 判断当前状态是否为垂直同步
        bool IsVSyncEnabled()
        {
            return (wglGetSwapIntervalEXT() > 0);
        }

        // 开启和关闭垂直同步
        void SetVSyncState(bool enable)
        {
            if (!wglSwapIntervalEXT)
            {
                return;
            }
            if (enable)
            {
                wglSwapIntervalEXT(1);
            }
            else
            {
                wglSwapIntervalEXT(0);
            }
        }

    #endif // WIN32
    
	RenderCore::RenderCore()
        : m_pWindow(nullptr)
    {
        
    }
    
    RenderCore::~RenderCore()
    {
        
    }
    
    void RenderCore::createWindow()
    {
        m_pWindow = initWindow();

        // 关闭垂直同步
    #ifdef _WIN32
        // win32
        bool isOk = InitVSync();
        if (isOk)
        {
            SetVSyncState(false);
        }
    #endif // WIN32
        
    #ifdef __APPLE__
        //    // mac获取extension的方法：3.0+的profile对glGetString传GL_EXTENSIONS已经被deprecated，属于invalid enumeration，正确的方法是用glGetStringi代替
        //    {
        //        GLint n, i;
        //        glGetIntegerv(GL_NUM_EXTENSIONS, &n);
        //        for (i = 0; i < n; i++)
        //        {
        //            printf("%s\n", glGetStringi(GL_EXTENSIONS, i));
        //        }
        //    }
        // 禁用mac垂直同步
        GLint sync = 0;
        CGLContextObj ctx = CGLGetCurrentContext();
        CGLSetParameter(ctx, kCGLCPSwapInterval, &sync);
    #endif

        // uncomment this call to draw in wireframe polygons.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    }
    
    bool RenderCore::shouldCloseWindow()
    {
        return glfwWindowShouldClose(m_pWindow);
    }
    
    void RenderCore::destoryWindow()
    {
        // glfw: terminate, clearing all previously allocated GLFW resources.
        ImGui_ImplGlfwGL3_Shutdown();
        glfwTerminate();
    }

	void RenderCore::initRender()
	{
		// 注册渲染系统
		ECSManager::getInstance()->registerSystem(browser::RenderSystem::getInstance());	// 渲染系统
																							// 初始化渲染系统
		ECSManager::getInstance()->initSystem(SystemType::RenderSystem);
		// 加载缓存
		GLProgramCache::getInstance()->init();  // 着色器缓存

												// 初始化调试GUI框架
		common::GUIFramework::getInstance()->init(SCR_WIDTH, SCR_HEIGHT);
	}
    
    void RenderCore::renderLoop(float deltaTime)
    {
        // wait
        while(LogicCore::getInstance()->getLogicState() != LogicCore::LogicCoreState::LCS_Finish);
        
        // 处理gpu操作指令
        GPUOperateSystem::getInstance()->update();

		// 重置GL状态（这里主要是为了防止插件如imgui绑定纹理，造成缓存失效）
		GLStateCache::getInstance()->update(deltaTime);

        // 1.input
        processInput(m_pWindow);
        
        // 渲染场景结束后
        ECSManager::getInstance()->afterUpdateSystem(SystemType::RenderSystem, deltaTime);
        
        // 更新调试信息
        common::GUIFramework::getInstance()->getGameStatusPanel()->setDeltaTime(deltaTime);
        // 更新调试GUI框架
        common::GUIFramework::getInstance()->update(deltaTime);
        // ImGui rendering
        ImGui::Render();



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(m_pWindow);
		glfwPollEvents();

    }
    
    GLFWwindow* RenderCore::initWindow()
    {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        #ifdef __APPLE__
            // macOS
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        #else
            // win32
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // 如果将opengl设置为330，则windows上无法使用Transform Feedback，所以这里将其设置为4
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        #endif


    #ifdef __APPLE__
        // macOS
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    // mac下只能用GLFW_OPENGL_CORE_PROFILE核心模式，不然程序会报错
    #else
         //win32
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);    // 这里如果用GLFW_OPENGL_CORE_PROFILE核心模式，则获取不到extensions
    #endif


        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShaderBrowser", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return nullptr;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, RenderCore::framebuffer_size_callback);
        glfwSetWindowSizeCallback(window, RenderCore::window_size_callback);
        glfwSetCursorPosCallback(window, RenderCore::mouse_callback);
        glfwSetMouseButtonCallback(window, RenderCore::mouse_button_callback);

        ImGui_ImplGlfwGL3_Init(window, true);

        ImGui::StyleColorsDark();

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return nullptr;
        }

        return window;
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
    void RenderCore::processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

//
//        browser::Camera* camera = CameraSystem::getInstance()->getMainCamera();
//        if (camera)
//        {
//            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//            {
////                browser::Transform* transform = camera->getBelongEntity()->getTransform();
////                transform->Translate(transform->getForward() * 0.1f);
//            }
//            else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//            {
////                browser::Transform* transform = camera->getBelongEntity()->getTransform();
////                transform->Translate(-transform->getForward() * 0.1f);
//            }
//            else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//            {
////                browser::Transform* transform = camera->getBelongEntity()->getTransform();
////                transform->Translate(transform->getLeft() * 0.1f);
//            }
//            else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//            {
////                browser::Transform* transform = camera->getBelongEntity()->getTransform();
////                transform->Translate(-transform->getLeft() * 0.1f);
//            }
//            else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//            {
////                browser::Transform* transform = camera->getBelongEntity()->getTransform();
////                transform->Rotate(glm::vec3(0, -1.0f, 0), Space::Self);
//            }
//            else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//            {
////                browser::Transform* transform = camera->getBelongEntity()->getTransform();
////                transform->Rotate(glm::vec3(0, 1.0f, 0), Space::Self);
//            }
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

    void RenderCore::window_size_callback(GLFWwindow* window, int width, int height)
    {

    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void RenderCore::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }
    
    void RenderCore::mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {

    }

    void RenderCore::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        //if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        //{
        //    //popup_menu();
        //}
    }
    
}
