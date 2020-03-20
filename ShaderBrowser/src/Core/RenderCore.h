#pragma once

#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#ifdef __APPLE__
// macOS
#include <unistd.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/OpenGL.h>
#else
// win32
#include <windows.h>
#include <wingdi.h>
#pragma warning(disable:4996)
#endif

#include "Common/Tools/Utils.h"
#include "Common/Tools/BaseSingleton.h"

using namespace customGL;
using namespace common;

namespace core
{
    // 屏幕宽高
    #define SCR_WIDTH 1280
    #define SCR_HEIGHT 720
    
	// 渲染核心线程
    class RenderCore : public BaseSingleton<RenderCore>
    {
    public:
        enum RenderCoreState
        {
            // 渲染开始
            RCS_Start = 0,
            // 渲染结束
            RCS_End,
        };
        
    public:
        RenderCore();
        ~RenderCore();
        
    public:
        // 创建并初始化opengl窗口(上下文)
        void createWindow();
        // 销毁窗口
        void destoryWindow();
		// 初始化 (设置当前线程的opengl上下文、初始化glad和imgui、初始化渲染系统和GUI框架)
		void initRender();
        // 渲染循环
        void renderLoop();
        // 是否关闭窗口
        bool shouldCloseWindow();
        // 设置关闭窗口
        void setWindowShouldClose();
        // input
        int getKeyState(int keyCode);
        
    private:
        GLFWwindow* initWindow();
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void window_size_callback(GLFWwindow* window, int width, int height);
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void recTime(const std::string& log);
        
        REGISTER_PROPERTY_GET(GLFWwindow*, m_pWindow, Window)
        REGISTER_PROPERTY_GET(unsigned long, m_uFrameIndex, FrameIndex)
        
    private:
        // window
        GLFWwindow* m_pWindow;
        
        // 记录时刻
        std::chrono::steady_clock::time_point m_oLastUpdate;
        // 当前帧
        unsigned long m_uFrameIndex;
        
    };
}
