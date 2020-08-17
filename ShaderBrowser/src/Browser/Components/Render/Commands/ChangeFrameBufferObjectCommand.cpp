#include "ChangeFrameBufferObjectCommand.h"

namespace browser
{
	ChangeFrameBufferObjectCommand * ChangeFrameBufferObjectCommand::create(Camera* camera, GLbitfield clearBit/* = DEFAULT_GL_CLEAR_BIT*/)
	{
		ChangeFrameBufferObjectCommand * command = new ChangeFrameBufferObjectCommand();
		command->init(camera, clearBit);
		return command;
	}

	ChangeFrameBufferObjectCommand::ChangeFrameBufferObjectCommand()
        : m_oCamera(nullptr)
		, m_oClearBit(DEFAULT_GL_CLEAR_BIT)
	{
		m_oRenderType = MeshRenderer::RendererType::RendererType_RenderTexture;
	}

	ChangeFrameBufferObjectCommand ::~ChangeFrameBufferObjectCommand()
	{

	}

    void ChangeFrameBufferObjectCommand::init(Camera* camera, GLbitfield clearBit)
    {
		m_oCamera = camera;
		m_oClearBit = clearBit;

		// 逻辑线程调用，防止autorelease先执行
		m_oCamera->retain();
		RenderTexture* rt = m_oCamera->getRenderTexture();
		if (rt)
		{
			rt->retain();
		}
	}
    
    void ChangeFrameBufferObjectCommand::execute()
    {
		// 如果你想将你的屏幕渲染到一个更小或更大的纹理上，你需要（在渲染到你的帧缓冲之前）再次调用glViewport，使用纹理的新维度作为参数，否则只有一小部分的纹理或屏幕会被渲染到这个纹理上。
		glViewport(0, 0, m_oCamera->getViewportWidth(), m_oCamera->getViewportHeight());

		// 使用渲染纹理
		RenderTexture* rt = m_oCamera->getRenderTexture();
		if (rt)
		{
			rt->useRenderTexture();
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// 清理FrameBuffer
		const glm::vec4& backgroundColor = m_oCamera->getBackgroundColor();
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
		glClear(m_oClearBit);

		// 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
		AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_oCamera);
		if (rt)
		{
			AutoReleasePool::getInstance()->addReferenceFromRenderCore(rt);
		}
    }
    
    

}
