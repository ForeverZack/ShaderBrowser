#pragma once

#include "BaseGUIPanel.h"


namespace browser {
    class BaseEntity;
}

namespace common
{
    class GameStatusPanel : public BaseGUIPanel
	{
	public:
		enum GameStatusType
		{
			FPS = 0,
			DrawCall,
			VerticesCount,
			FaceCount,
			OpenGLVersion,
			Hardware,


			MaxCount,
		};

	public:
		GameStatusPanel(float width, float height, bool showClose = true, float posX = 0.0f, float posY = 0.0f, float bgAlpha = 0.6f, ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
		~GameStatusPanel();

	public:
        
        // 更新窗口内容
        virtual void updateContent();
        

		REGISTER_PROPERTY_GET_SET(float, m_fDeltaTime, DeltaTime)

	protected:
		// 是否已经初始化
		bool m_bHasInit;
		// 每帧间隔时间
		float m_fDeltaTime;

		// 临时字符串用来生成信息
		char m_sInfo[200];
	};

}
