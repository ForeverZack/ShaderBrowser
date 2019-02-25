#pragma once

#include "BaseGUIPanel.h"
#include <unordered_map>
#include <vector>

namespace browser {
    class BaseEntity;
    class Transform;
}

namespace common
{
    class HierarchyPanel : public BaseGUIPanel
	{
	public:
		HierarchyPanel(float width, float height, bool showClose = true, float posX = 0.0f, float posY = 0.0f, float bgAlpha = 0.6f, ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
		~HierarchyPanel();

	public:
        
        // 更新窗口内容
        virtual void updateContent();
        
        void drawContent();
        
        
    private:
        // 遍历场景节点
        void traverseNode(browser::Transform* node, bool isParentExpand);
        

	protected:
        
        // 当前场景节点
        browser::BaseEntity* m_oScene;

        // 选中节点的Transform
        browser::Transform* m_oSelectTreeNodeTrans;
        
        // UI数据快速查询表
        std::unordered_map<browser::Transform*, ShowGUIData> m_mQuickDatas;
        
	};

}
