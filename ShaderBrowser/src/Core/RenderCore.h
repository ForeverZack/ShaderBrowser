#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

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
using namespace customGL;
using namespace common;

namespace core
{
    // settings
    #define SCR_WIDTH 1280
    #define SCR_HEIGHT 720
    
    class RenderCore
    {
    public:
        RenderCore();
        ~RenderCore();
        
    public:
        // 创建并初始化opengl窗口(上下文)
        void createWindow();
        // 销毁窗口
        void destoryWindow();
        // 循环
        void renderLoop(float deltaTime);
        // 是否关闭窗口
        bool shouldCloseWindow();
        
    private:
        GLFWwindow* init();
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void window_size_callback(GLFWwindow* window, int width, int height);
        static void processInput(GLFWwindow *window);
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        
        REGISTER_PROPERTY_GET(GLFWwindow*, m_pWindow, Window)
        
    private:
        // window
        GLFWwindow* m_pWindow;

    };
}
