#include "InspectorPanel.h"
#include "Browser/Entitys/BaseEntity.h"
#include "Browser/Components/BaseComponent.h"

using namespace browser;

namespace common
{
	InspectorPanel::InspectorPanel(float width, float height, bool showClose /*= true*/, float posX /*= 0.0f*/, float posY /*= 0.0f*/, float bgAlpha /*= 0.6f*/, ImGuiWindowFlags flags /*= ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings*/)
		: BaseGUIPanel("Inspector", false, width, height, showClose, posX, posY, bgAlpha, flags)
        , m_oSelEntity(nullptr)
        , m_oPreSelEntity(nullptr)
	{
	}

	InspectorPanel::~InspectorPanel()
	{

	}
    
    void InspectorPanel::selectEntity(BaseEntity* entity)
    {
        if (!entity || entity==m_oSelEntity)
        {
            return;
        }
        
        m_oSelEntity = entity;
        
        // 更新内容
        updateContent();
    }

	void InspectorPanel::updateContent()
    {
        if (!m_oSelEntity)
        {
            cleanContent();
            return;
        }
        
        if (m_oSelEntity != m_oPreSelEntity)
        {
            cleanContent();
            m_oPreSelEntity = m_oSelEntity;
        }
        
        // 组件列表
        BaseComponent* component = nullptr;
        const std::vector<BaseComponent*>& components = m_oSelEntity->getComponents();
        for(auto itor=components.cbegin(); itor!=components.cend(); ++itor)
        {
            component = (*itor);
            // 添加折叠标题
            addCollapsingHeader(component->getComponentName(), true);
            // 添加内容
            component->onInspectorGUI(this);
        }
        
    }

}
