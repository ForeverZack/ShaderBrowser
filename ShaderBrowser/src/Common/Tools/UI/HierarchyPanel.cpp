#include "HierarchyPanel.h"
#include "Browser/Entitys/BaseEntity.h"
#include "Browser/Components/BaseComponent.h"
#include "Browser/Components/Transform/Transform.h"
#include "Browser/System/TransformSystem.h"
#include "GUIFramework.h"

using namespace browser;

namespace common
{
	HierarchyPanel::HierarchyPanel(float width, float height, bool showClose /*= true*/, float posX /*= 0.0f*/, float posY /*= 0.0f*/, float bgAlpha /*= 0.6f*/, ImGuiWindowFlags flags /*= ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings*/)
		: BaseGUIPanel("Hierarchy", true, width, height, showClose, posX, posY, bgAlpha, flags)
        , m_oSelectTreeNodeTrans(nullptr)
        , m_oScene(nullptr)
	{
        // 清理
        m_mQuickDatas.clear();
	}

	HierarchyPanel::~HierarchyPanel()
	{

	}
    
	void HierarchyPanel::updateContent()
    {
        // HierarchyPanel生成树状图牵扯到递归调用，暂时没法用m_vContent顺序结构的vector去处理，所以直接在面板内画了
    }
    
    void HierarchyPanel::traverseNode(browser::Transform* node, bool isParentExpand)
    {
        // 默认（关闭，没有选中）
        static ImGuiTreeNodeFlags defaultFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        
        if (!isParentExpand)
        {
            return;
        }
        
        // 子节点列表
        const std::vector<browser::Transform*>& children = node->getChildren();
        
        if (m_mQuickDatas.find(node) == m_mQuickDatas.end())
        {
            ShowGUIData data(true);
            data.setTreeNode(node->getName(), generateTreeNodeID(), false, false, children.empty());
            m_mQuickDatas.emplace(node, std::move(data));
            
            // 不使用m_vContent，所以也没必要赋值
//            addTreeNode(data);
        }
        
        
        ShowGUIData& data = m_mQuickDatas[node];
        ImGuiTreeNodeFlags flag = defaultFlag
                          | (data.value.tree_node.isExpand ? ImGuiTreeNodeFlags_DefaultOpen : 0)    // 是否展开
                          | (data.value.tree_node.selected ? ImGuiTreeNodeFlags_Selected : 0)   // 是否选中
                          | (children.empty() ? (ImGuiTreeNodeFlags_Leaf) : 0); // 是否有子节点
        
        bool isExpand = ImGui::TreeNodeEx(data.value.tree_node.key_id.c_str(), flag, data.value.tree_node.show_name.c_str());
        if (ImGui::IsItemClicked())
        {
            // 选中节点xxx
            BROWSER_LOG("select node "+node->getName());
            
            // 取消上一个选中节点
            if (m_oSelectTreeNodeTrans)
            {
                // 设置上一个选中节点的数据
				ShowGUIData& lastSelData = m_mQuickDatas[m_oSelectTreeNodeTrans];
                lastSelData.value.tree_node.selected = false;
				// 取消物体选择
				BROWSER_ASSERT(m_oSelectTreeNodeTrans, "The Entity that you last select has already been destoryed, please check in function HierarchyPanel::traverseNode !");
				BaseEntity* lastSelectEntity = m_oSelectTreeNodeTrans->getBelongEntity();
				if (lastSelectEntity != m_oScene)		// scene的坐标轴仍然显示
				{
					lastSelectEntity->setIsAxisVisible(false);
					lastSelectEntity->setIsBoundBoxVisible(false);
				}
            }
            // 选中当前节点
            m_oSelectTreeNodeTrans = node;
            ShowGUIData& selData = m_mQuickDatas[node];
            selData.value.tree_node.selected = true;
            selData.value.tree_node.isExpand = isExpand;

			BaseEntity* selectEntity = node->getBelongEntity();
            // Inspector面板显示选中的entity属性
            GUIFramework::getInstance()->getInspectorPanel()->selectEntity(selectEntity);
			// 展示选中物体的包围盒和坐标轴
			selectEntity->setIsAxisVisible(true);
			selectEntity->setIsBoundBoxVisible(true);
			
        }
        if (isExpand)
        {
            for(auto itor=children.begin(); itor!=children.end(); ++itor)
            {
                traverseNode((*itor), isParentExpand && isExpand);
            }
            
            ImGui::TreePop();
        }
        
    }
    
    void HierarchyPanel::drawContent()
    {
        if(!m_oScene)
        {
            m_oScene = TransformSystem::getInstance()->getScene();
        }
        
        if(!m_oScene || !m_oScene->getComponent<browser::Transform>())
        {
            return;
        }
        
        //        // TODO: 如果结构没有发生改变
        //        if ()
        //        {
        //            return;
        //        }
        
        // 如果有结构发生改变
        cleanContent();
        // 基类BaseGUIPanel的m_vContent只是用来做显示用的，但这里需要根据节点来查找数据，所以加了个unordered_map
        traverseNode(m_oScene->getComponent<browser::Transform>(), true);
    }

}
