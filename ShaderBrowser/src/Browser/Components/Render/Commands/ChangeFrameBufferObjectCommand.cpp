#include "ChangeFrameBufferObjectCommand.h"

namespace browser
{
	ChangeFrameBufferObjectCommand * ChangeFrameBufferObjectCommand::create(Camera* camera)
	{
		ChangeFrameBufferObjectCommand * command = new ChangeFrameBufferObjectCommand();
		command->init(camera);
		return command;
	}

	ChangeFrameBufferObjectCommand::ChangeFrameBufferObjectCommand()
        : m_oCamera(nullptr)
	{
		m_oRenderType = MeshRenderer::RendererType::RendererType_RenderTexture;
	}

	ChangeFrameBufferObjectCommand ::~ChangeFrameBufferObjectCommand()
	{

	}

    void ChangeFrameBufferObjectCommand::init(Camera* camera)
    {
		m_oCamera = camera;

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
		glClear(DEFAULT_GL_CLEAR_BIT);

		// 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
		AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_oCamera);
		if (rt)
		{
			AutoReleasePool::getInstance()->addReferenceFromRenderCore(rt);
		}
    }
    
    

}
