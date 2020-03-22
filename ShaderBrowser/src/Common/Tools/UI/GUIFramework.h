#pragma once

#include <vector>
#include <memory>
#include "Common/Tools/BaseSingleton.h"
#include "Common/Tools/UI/BaseGUIPanel.h"
#include "Common/Tools/Thread/BaseThread.h"

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
		// 每帧更新 (逻辑线程)
		void update(float deltaTime);

		// 回收imgui上下文 (渲染线程)
		void pushImGuiContext(unsigned long frameIndex);
		void pushImGuiContext(ImGuiContext* context);
		// 获取某帧对应的imgui上下文 (渲染线程)
		ImGuiContext* getImGuiContext(unsigned long frameIndex);


        REGISTER_PROPERTY_GET(std::shared_ptr<InspectorPanel>, m_oInspectorPanel, InspectorPanel)
		REGISTER_PROPERTY_GET(std::shared_ptr<HierarchyPanel>, m_oHierarchyPanel, HierarchyPanel)
		REGISTER_PROPERTY_GET(std::shared_ptr<GameStatusPanel>, m_oGameStatusPanel, GameStatusPanel)

	protected:
		// 获取空闲的imgui上下文
		ImGuiContext* popImGuiContext();
		// 设置当前帧imgui上下文
		void newFrameContext(unsigned long frameIndex);

	protected:
		// 每帧对应的imgui上下文 (逻辑线程写入，渲染线程读取)
		MutexUnorderedMap<unsigned long, ImGuiContext*> m_mImGuiContexts;
		// 空闲的imgui上下文队列 (逻辑线程pop，渲染线程push)
		MutexQueue<ImGuiContext*> m_qFreeImGuiContexts;
		// 上一帧使用的imgui上下文(用来复制之前imgui的一些状态，例如输入状态)
		ImGuiContext* m_pLastImGuiContext;

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
