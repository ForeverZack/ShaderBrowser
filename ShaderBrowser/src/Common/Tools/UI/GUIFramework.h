#pragma once

#include <vector>
#include <memory>
#include "Common/Tools/BaseSingleton.h"
#include "Common/Tools/UI/BaseGUIPanel.h"

namespace common
{
    class InspectorPanel;
    class HierarchyPanel;
	class GameStatusPanel;
    
	class GUIFramework : public BaseSingleton<GUIFramework>
	{
	public:
		GUIFramework();
		~GUIFramework();

	public:
		// 初始化
		void init(int winWidth, int winHeight);
		// 每帧更新
		void update(float deltaTime);

        
        REGISTER_PROPERTY_GET(std::shared_ptr<InspectorPanel>, m_oInspectorPanel, InspectorPanel)
		REGISTER_PROPERTY_GET(std::shared_ptr<HierarchyPanel>, m_oHierarchyPanel, HierarchyPanel)
		REGISTER_PROPERTY_GET(std::shared_ptr<GameStatusPanel>, m_oGameStatusPanel, GameStatusPanel)
        
	protected:
		// 面板队列
		std::vector<std::shared_ptr<BaseGUIPanel>> m_vPanels;
		
		// 特殊面板（单独标记一下）
		// Inspector
		std::shared_ptr<InspectorPanel> m_oInspectorPanel;
        // Hierarchy
        std::shared_ptr<HierarchyPanel> m_oHierarchyPanel;
		// GameStatusPanel
		std::shared_ptr<GameStatusPanel> m_oGameStatusPanel;
	};

}
