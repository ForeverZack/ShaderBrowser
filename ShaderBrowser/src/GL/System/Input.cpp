#include "Input.h"

using namespace core;

namespace customGL
{
    bool Input::isKeyDown(int keyCode)
    {
        return RenderCore::getInstance()->getKeyState(keyCode) == GLFW_PRESS;
    }
    
    bool Input::isKeyRelease(int keyCode)
    {
        return RenderCore::getInstance()->getKeyState(keyCode) == GLFW_RELEASE;
    }
    
    int Input::getKeyState(int keyCode)
    {
        return RenderCore::getInstance()->getKeyState(keyCode);
    }
    
    
}
