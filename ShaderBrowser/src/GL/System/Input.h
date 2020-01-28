#pragma once

#include "Core/RenderCore.h"
#include <GLFW/glfw3.h>

namespace customGL
{
	class Input
	{
	public:
        // 键盘是否按下
        static bool isKeyDown(int keyCode);
        // 键盘是否释放
        static bool isKeyRelease(int keyCode);
        // 键盘状态
        static int getKeyState(int keyCode);
        
	};
}
