#include "GameStatusPanel.h"
#include "Browser/System/RenderSystem.h"

#ifdef _WIN32
#pragma warning(disable:4996)
#endif // _WIN32



using namespace browser;

namespace common
{
	GameStatusPanel::GameStatusPanel(float width, float height, bool showClose /*= true*/, float posX /*= 0.0f*/, float posY /*= 0.0f*/, float bgAlpha /*= 0.6f*/, ImGuiWindowFlags flags /*= ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings*/)
		: BaseGUIPanel("Game Status", true, width, height, showClose, posX, posY, bgAlpha, flags)
		, m_bHasInit(false)
		, m_fDeltaTime(0)
	{
		// 按顺序生成信息表
		for (int i = 0; i < GameStatusType::MaxCount; ++i)
		{
			switch (i)
			{
			case GameStatusType::FPS:
				{
					addPropertyText("");
				}
				break;

			case GameStatusType::DrawCall:
				{
					addPropertyText("");
				}
				break;

			case GameStatusType::VerticesCount:
				{
					addPropertyText("");
				}
				break;

			case GameStatusType::FaceCount:
				{
					addPropertyText("");
				}
				break;

			case GameStatusType::OpenGLVersion:
				{
					// OpenGL版本
					char* glInfos = (char*)glGetString(GL_VERSION);
					addPropertyText("GL Version: " + std::string(glInfos));
				}
				break;

			case GameStatusType::Hardware:
				{
					// 显卡信息
					char* hardwareInfos = (char*)glGetString(GL_RENDERER);
					addPropertyText("Hardware: " + std::string(hardwareInfos));
				}
				break;
			}
		}
	}

	GameStatusPanel::~GameStatusPanel()
	{

	}
  
	void GameStatusPanel::updateContent()
    {   

		/*, browser::RenderSystem::getInstance()->getDrawCalls()
			, browser::RenderSystem::getInstance()->getVerticesCount(), browser::RenderSystem::getInstance()->getFaceCount()*/

		// FPS info
		sprintf(m_sInfo, "Framerate : %.1f FPS / %.4fs", 1.0f/m_fDeltaTime, m_fDeltaTime);
		m_vContents[GameStatusType::FPS].value.text.show_name = m_sInfo;

		// Draw call
		sprintf(m_sInfo, "Draw Calls: %d", browser::RenderSystem::getInstance()->getDrawCalls());
		m_vContents[GameStatusType::DrawCall].value.text.show_name = m_sInfo;

		// Vertices count
		sprintf(m_sInfo, "Vertices Count: %d", browser::RenderSystem::getInstance()->getVerticesCount());
		m_vContents[GameStatusType::VerticesCount].value.text.show_name = m_sInfo;

		// Face count
		sprintf(m_sInfo, "Face Count: %d", browser::RenderSystem::getInstance()->getFaceCount());
		m_vContents[GameStatusType::FaceCount].value.text.show_name = m_sInfo;

    }

}
