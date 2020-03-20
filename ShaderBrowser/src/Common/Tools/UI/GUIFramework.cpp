#include "GUIFramework.h"
#include "Common/Tools/UI/InspectorPanel.h"
#include "Common/Tools/UI/HierarchyPanel.h"
#include "Common/Tools/UI/GameStatusPanel.h"

namespace common
{
	GUIFramework::GUIFramework()
		: m_oInspectorPanel(nullptr)
        , m_oHierarchyPanel(nullptr)
		, m_oGameStatusPanel(nullptr)
	{
		// 清理
		m_vPanels.clear();
	}

	GUIFramework::~GUIFramework()
	{

	}

	void GUIFramework::init(int winWidth, int winHeight)
	{
		//ImGui::StyleColorsDark();

		// Inspector面板
		m_oInspectorPanel = std::make_shared<InspectorPanel>(250, winHeight, false, winWidth-250, 0);
		m_oInspectorPanel->setWindowStyleColor(ImGuiCol_TitleBgActive, 0.04f, 0.04f, 0.04f, 1.00f);
		m_vPanels.push_back(m_oInspectorPanel);
        
        // Hierarchy面板
        m_oHierarchyPanel = std::make_shared<HierarchyPanel>(250, winHeight-150, false, 0, 0);
		m_oHierarchyPanel->setWindowStyleColor(ImGuiCol_TitleBgActive, 0.04f, 0.04f, 0.04f, 1.00f);
        m_vPanels.push_back(m_oHierarchyPanel);

		// GameStatus面板
		m_oGameStatusPanel = std::make_shared<GameStatusPanel>(250, 150, false, 0, winHeight-150);
		m_oGameStatusPanel->setWindowStyleColor(ImGuiCol_TitleBgActive, 0.04f, 0.04f, 0.04f, 1.00f);
		m_vPanels.push_back(m_oGameStatusPanel);
	}

	void GUIFramework::update(float deltaTime)
	{
		// 刷新所有GUI面板
		for (auto itor = m_vPanels.cbegin(); itor != m_vPanels.cend(); ++itor)
		{
			(*itor)->update(deltaTime);
		}
	}

}
