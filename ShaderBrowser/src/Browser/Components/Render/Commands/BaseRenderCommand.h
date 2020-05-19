#pragma once

namespace browser
{
	class Camera;

    // 渲染命令基类
    class BaseRenderCommand
	{
	public:
		BaseRenderCommand() {};
		virtual ~BaseRenderCommand() {};

	public:
        // 绘制
		virtual void execute() {};


		REGISTER_PROPERTY_GET(MeshRenderer::RendererType, m_oRenderType, RenderType);
        
	protected:
		// 类型
		MeshRenderer::RendererType m_oRenderType;
	};
    
}

