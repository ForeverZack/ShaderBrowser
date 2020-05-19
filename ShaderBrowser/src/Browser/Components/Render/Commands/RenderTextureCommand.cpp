#include "RenderTextureCommand.h"

namespace browser
{
	RenderTextureCommand* RenderTextureCommand::create(RenderTexture* rt)
	{
		RenderTextureCommand* command = new RenderTextureCommand();
		command->init(rt);
		return command;
	}

	RenderTextureCommand::RenderTextureCommand()
        : m_oRenderTexture(nullptr)
	{
		m_oRenderType = MeshRenderer::RendererType::RendererType_RenderTexture;
	}

	RenderTextureCommand::~RenderTextureCommand()
	{

	}

    void RenderTextureCommand::init(RenderTexture* rt)
    {
		m_oRenderTexture = rt;

		// 逻辑线程调用，防止autorelease先执行
		m_oRenderTexture->retain();
	}
    
    void RenderTextureCommand::execute()
    {
		// 使用渲染纹理
		m_oRenderTexture->useRenderTexture();

		// 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
		AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_oRenderTexture);
    }
    
    

}
