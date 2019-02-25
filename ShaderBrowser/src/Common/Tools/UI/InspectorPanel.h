#pragma once

#include "BaseGUIPanel.h"


namespace browser {
    class BaseEntity;
}

namespace common
{
    class InspectorPanel : public BaseGUIPanel
	{
	public:
		InspectorPanel(float width, float height, bool showClose = true, float posX = 0.0f, float posY = 0.0f, float bgAlpha = 0.6f, ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
		~InspectorPanel();

	public:
        
        // 选择新的entity
        void selectEntity(browser::BaseEntity* entity);
        
        // 更新窗口内容
        virtual void updateContent();
        
        
        REGISTER_PROPERTY_GET(browser::BaseEntity*, m_oSelEntity, SelEntity)

	protected:
        // 选中的实体组件
        browser::BaseEntity* m_oSelEntity;
        // 上次选中的实体组件
        browser::BaseEntity* m_oPreSelEntity;
        
	};

}
