#include "GUIFramework.h"
#include "Common/Tools/UI/InspectorPanel.h"
#include "Common/Tools/UI/HierarchyPanel.h"
#include "Common/Tools/UI/GameStatusPanel.h"
#include "Core/LogicCore.h"

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
		// 新建imgui上下文
		newFrameContext(core::LogicCore::getInstance()->getFrameIndex());

		// 创建新的GUI帧缓存
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 刷新所有GUI面板
		for (auto itor = m_vPanels.cbegin(); itor != m_vPanels.cend(); ++itor)
		{
			(*itor)->update(deltaTime);
		}

		// ImGui rendering
		ImGui::Render();
	}

	ImGuiContext* GUIFramework::popImGUIContext()
	{
		if (m_qFreeImGUIContexts.empty())
		{
			return ImGui::CreateContext();
		}
		else
		{
			return m_qFreeImGUIContexts.pop();
		}
	}

	void GUIFramework::newFrameContext(unsigned long frameIndex)
	{
		ImGuiContext* context = popImGUIContext();
		m_mImGUIContexts[frameIndex] = context;
		ImGui::SetCurrentContext(context);
	}

	void GUIFramework::pushImGUIContext(unsigned long frameIndex)
	{
		ImGuiContext* context = m_mImGUIContexts[frameIndex];
		m_mImGUIContexts.erase(frameIndex);
		m_qFreeImGUIContexts.push(context);
	}

	ImGuiContext* GUIFramework::getImGUIContext(unsigned long frameIndex)
	{
		BROWSER_ASSERT(m_mImGUIContexts.containsKey(frameIndex), "Cannot find the imgui context by frameIndex in function GUIFramework::getImGUIContext");
		return m_mImGUIContexts[frameIndex];
	}




}
