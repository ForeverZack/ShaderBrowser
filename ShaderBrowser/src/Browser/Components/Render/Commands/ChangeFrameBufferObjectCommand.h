#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/GPUResource/Texture/RenderTexture.h"
#include "Browser/Components/Render/MeshRenderer.h"
#include "Browser/Components/Render/Commands/BaseRenderCommand.h"
#include "Browser/Components/Camera/Camera.h"

using namespace customGL;

namespace browser
{
    // 渲染对象切换命令
    class ChangeFrameBufferObjectCommand : public BaseRenderCommand
	{
	public:
		static ChangeFrameBufferObjectCommand * create(Camera* camera, GLbitfield clearBit = DEFAULT_GL_CLEAR_BIT);

	public:
		ChangeFrameBufferObjectCommand();
		virtual ~ChangeFrameBufferObjectCommand();

	public:
        // 初始化渲染命令
        void init(Camera* camera, GLbitfield clearBit);
        
        // 执行
        virtual void execute();
     
        
        
	protected:
		// 相机
		Camera* m_oCamera;
		// 默认的清除标记
		GLbitfield m_oClearBit;
	};
    
}

