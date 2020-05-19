#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/GPUResource/Texture/RenderTexture.h"
#include "Browser/Components/Render/MeshRenderer.h"
#include "Browser/Components/Render/Commands/BaseRenderCommand.h"

using namespace customGL;

namespace browser
{
    // 渲染纹理命令
    class RenderTextureCommand : public BaseRenderCommand
	{
	public:
		static RenderTextureCommand* create(RenderTexture* rt);

	public:
		RenderTextureCommand();
		virtual ~RenderTextureCommand();

	public:
        // 初始化渲染命令
        void init(RenderTexture* rt);
        
        // 执行
        virtual void execute();
     
        
        REGISTER_PROPERTY_GET(RenderTexture*, m_oRenderTexture, RenderTexture);
        
        
	protected:
		// 渲染纹理
		RenderTexture* m_oRenderTexture;
	};
    
}

