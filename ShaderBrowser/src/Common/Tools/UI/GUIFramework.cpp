#include "GUIFramework.h"
#include "Common/Tools/UI/InspectorPanel.h"
#include "Common/Tools/UI/HierarchyPanel.h"
#include "Common/Tools/UI/GameStatusPanel.h"
#include "Core/LogicCore.h"
#include <imgui_internal.h>

namespace common
{
	GUIFramework::GUIFramework()
		: m_oInspectorPanel(nullptr)
        , m_oHierarchyPanel(nullptr)
		, m_oGameStatusPanel(nullptr)
        , m_eImGuiUseState(IGUS_RenderFinish)
	{
		// 清理
		m_vPanels.clear();
	}

	GUIFramework::~GUIFramework()
	{

	}

	void GUIFramework::init(int viewportWidth, int viewportHeight)
	{
        ImGui::StyleColorsDark();

		// Inspector面板
		m_oInspectorPanel = std::make_shared<InspectorPanel>(250, viewportHeight, false, viewportWidth-250, 0);
		m_oInspectorPanel->setWindowStyleColor(ImGuiCol_TitleBgActive, 0.04f, 0.04f, 0.04f, 1.00f);
		m_vPanels.push_back(m_oInspectorPanel);
        
        // Hierarchy面板
        m_oHierarchyPanel = std::make_shared<HierarchyPanel>(250, viewportHeight-150, false, 0, 0);
		m_oHierarchyPanel->setWindowStyleColor(ImGuiCol_TitleBgActive, 0.04f, 0.04f, 0.04f, 1.00f);
        m_vPanels.push_back(m_oHierarchyPanel);

		// GameStatus面板
		m_oGameStatusPanel = std::make_shared<GameStatusPanel>(250, 150, false, 0, viewportHeight-150);
		m_oGameStatusPanel->setWindowStyleColor(ImGuiCol_TitleBgActive, 0.04f, 0.04f, 0.04f, 1.00f);
		m_vPanels.push_back(m_oGameStatusPanel);
	}

	void GUIFramework::setViewportSize(int width, int height)
	{
		// Inspector面板
		m_oInspectorPanel->setPosition(width-250, 0);
		m_oInspectorPanel->setSize(250, height);
		// Hierarchy面板
		m_oHierarchyPanel->setPosition(0, 0);
		m_oHierarchyPanel->setSize(250, height-150);
		// GameStatus面板
		m_oGameStatusPanel->setPosition(0, height-150);
	}

	void GUIFramework::update(float deltaTime)
	{
        // ImGui rendering
        if (m_eImGuiUseState == IGUS_RenderFinish)
        {
            // 创建新的GUI帧缓存
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // 刷新所有GUI面板
            for (auto itor = m_vPanels.cbegin(); itor != m_vPanels.cend(); ++itor)
            {
                (*itor)->update(deltaTime);
            }

            ImGui::Render();
            ImDrawData* drawdata = ImGui::GetDrawData();
            m_eImGuiUseState = IGUS_LogicFinish;
        }
	}
    
    GUIFramework::ImGuiUseState GUIFramework::getImGuiUseState()
    {
        return m_eImGuiUseState.getValue();
    }
    
    void GUIFramework::setImGuiUseState(ImGuiUseState state)
    {
        m_eImGuiUseState = state;
    }



}
