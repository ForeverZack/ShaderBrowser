#include "RenderCore.h"
#include "Common/System/ECSManager.h"
#include "Common/Tools/UI/GUIFramework.h"
#include "Common/Tools/UI/InspectorPanel.h"
#include "Common/Tools/UI/GameStatusPanel.h"
#include "Browser/System/RenderSystem.h"
#include "GL/GLStateCache.h"
#include "GL/System/GPUOperateSystem.h"
#include "Core/LogicCore.h"
#include "Core/Application.h"

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
        , m_eRenderState(RenderCoreState::RCS_End)
    {
        
    }
    
    RenderCore::~RenderCore()
    {
        
    }
    
    void RenderCore::createWindow()
    {
        m_pWindow = initWindow();
    }
    
    bool RenderCore::shouldCloseWindow()
    {
        return glfwWindowShouldClose(m_pWindow);
    }
    
    void RenderCore::setWindowShouldClose()
    {
        glfwSetWindowShouldClose(m_pWindow, true);
    }
    
    void RenderCore::destoryWindow()
    {
        // glfw: terminate, clearing all previously allocated GLFW resources.
        ImGui_ImplGlfwGL3_Shutdown();
    }

	void RenderCore::initRender()
	{
        glfwMakeContextCurrent(m_pWindow);
        
		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}
		
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
        
        ImGui_ImplGlfwGL3_Init(m_pWindow, true);
        ImGui::StyleColorsDark();
        
		// 注册渲染系统
		ECSManager::getInstance()->registerSystem(browser::RenderSystem::getInstance());
        // 初始化渲染系统
		ECSManager::getInstance()->initSystem(SystemType::RenderSystem);

        // 初始化调试GUI框架
		common::GUIFramework::getInstance()->init(SCR_WIDTH, SCR_HEIGHT);
	}
    
    void RenderCore::recTime(const std::string& log)
    {
//        return;

        auto now = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - m_oLastUpdate).count() / 1000.0f;
        m_oLastUpdate = now;
        BROWSER_LOG(log + std::to_string(deltaTime) + "ms");
    }
    
    void RenderCore::renderLoop()
    {
        m_oLastUpdate = std::chrono::steady_clock::now();
        
        // wait
        while(LogicCore::getInstance()->getLogicState() != LogicCore::LogicCoreState::LCS_Finish);
        m_eRenderState = RenderCoreState::RCS_Start;
        recTime("========render wait=======");
        
        float deltaTime = Application::CurrentApplication->getDeltaTime();
        
        // 从逻辑线程拷贝渲染命令队列
        ECSManager::getInstance()->updateSystem(SystemType::RenderSystem, deltaTime);
        // 处理gpu操作指令  注意：这里会把LogicCore的状态置为LCS_Prepare
        GPUOperateSystem::getInstance()->update();

		// 重置GL状态（这里主要是为了防止插件如imgui绑定纹理，造成缓存失效）
		GLStateCache::getInstance()->update(deltaTime);
        
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
//		glfwPollEvents();
        
        m_eRenderState = RenderCoreState::RCS_End;
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
        
        glfwSetFramebufferSizeCallback(window, RenderCore::framebuffer_size_callback);
        glfwSetWindowSizeCallback(window, RenderCore::window_size_callback);
        glfwSetCursorPosCallback(window, RenderCore::mouse_callback);
        glfwSetMouseButtonCallback(window, RenderCore::mouse_button_callback);

        return window;
    }
    
    int RenderCore::getKeyState(int keyCode)
    {
        return glfwGetKey(m_pWindow, keyCode);
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
