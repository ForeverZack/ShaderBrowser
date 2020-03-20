#include "BaseGUIPanel.h"

namespace common
{
    /*
        ShowGUIData
     */
    ShowGUIData::ShowGUIData()
        : isReadOnly(true)
        , canExpand(true)
        , callback_vec2(nullptr)
        , callback_vec3(nullptr)
        , callback_vec4(nullptr)
        , callback_tsEulerAngle(nullptr)
        , callback_checkbox(nullptr)
        , callback_treeNode(nullptr)
    {
    }
    
    ShowGUIData::ShowGUIData(bool readOnly, bool expand /*= true*/)
        : isReadOnly(readOnly)
        , canExpand(expand)
        , callback_vec2(nullptr)
        , callback_vec3(nullptr)
        , callback_vec4(nullptr)
        , callback_tsEulerAngle(nullptr)
        , callback_checkbox(nullptr)
        , callback_treeNode(nullptr)
    {
        
    }
    
    ShowGUIData::ShowGUIData(const ShowGUIData& data)
        : m_eType(data.m_eType)
        , canExpand(data.canExpand)
        , isReadOnly(data.isReadOnly)
        , callback_vec2(data.callback_vec2)
        , callback_vec3(data.callback_vec3)
        , callback_vec4(data.callback_vec4)
        , callback_tsEulerAngle(data.callback_tsEulerAngle)
        , callback_checkbox(data.callback_checkbox)
        , callback_treeNode(data.callback_treeNode)
    
    {
        value = data.value;
    }
    
    ShowGUIData::ShowGUIData(const ShowGUIData&& data) noexcept
    {
        m_eType = data.m_eType;
        value = data.value;
        canExpand = data.canExpand;
        isReadOnly = data.isReadOnly;
        callback_vec2 = data.callback_vec2;
        callback_vec3 = data.callback_vec3;
        callback_vec4 = data.callback_vec4;
        callback_tsEulerAngle = data.callback_tsEulerAngle;
        callback_checkbox = data.callback_checkbox;
        callback_treeNode = data.callback_treeNode;
    }
    
    ShowGUIData::~ShowGUIData()
    {
    }
    
    bool ShowGUIData::isValueChange()
    {
        switch (m_eType) {
            case ShowGUIType::Property_Vector2:
                {
                    return (*value.vec2.pointer) != value.vec2.show_value;
                }
                break;
                
            case ShowGUIType::Property_Vector3:
                {
                    return (*value.vec3.pointer) != value.vec3.show_value;
                }
                break;
                
            case ShowGUIType::Property_Vector4:
                {
                    return (*value.vec4.pointer) != value.vec4.show_value;
                }
                break;
                
            case ShowGUIType::Property_TransformEulerAngle:
                {
                    return (*value.vec3.pointer) != value.vec3.show_value;
                }
                break;
                
            default:
                break;
        }
        
        return false;
    }

    void ShowGUIData::setTitle(const std::string& title, bool expand /*= false*/, bool enable /*= true*/)
    {
        m_eType = ShowGUIType::ExpandTitle;
        value.title.show_name = title;
        value.title.isExpand = expand;
		value.title.isEnable = enable;
    }
    
    void ShowGUIData::setTreeNode(const std::string& showName, unsigned long nodeID, bool selected /*= false*/, bool expand /*= false*/, bool hasChildren /*= false*/, ClickFunc_TreeNode callback /*= nullptr*/)
    {
        m_eType = ShowGUIType::TreeNode;
        value.tree_node.show_name = showName;
        value.tree_node.node_id = nodeID;
        value.tree_node.selected = selected;
        value.tree_node.isExpand = expand;
        value.tree_node.hasChildren = hasChildren;
        value.tree_node.key_id = std::to_string(nodeID);
    }
    
    void ShowGUIData::setText(const std::string& text)
    {
        m_eType = ShowGUIType::Property_Text;
        value.text.show_name = text;
    }

	void ShowGUIData::setSeparator()
	{
		m_eType = ShowGUIType::Separator;
	}
    
    void ShowGUIData::setVec2(const std::string& showName, glm::vec2* vec2, ValueChangeFunc_vec2 callback /*= nullptr*/)
    {
        m_eType = ShowGUIType::Property_Vector2;
        value.vec2.show_name = showName;
        value.vec2.pointer = vec2;
        callback_vec2 = callback;
    }

    void ShowGUIData::setVec3(const std::string& showName, glm::vec3* vec3, ValueChangeFunc_vec3 callback /*= nullptr*/)
    {
        m_eType = ShowGUIType::Property_Vector3;
        value.vec3.show_name = showName;
        value.vec3.pointer = vec3;
        callback_vec3 = callback;
    }
    
    void ShowGUIData::setVec4(const std::string& showName, glm::vec4* vec4, ValueChangeFunc_vec4 callback /*= nullptr*/)
    {
        m_eType = ShowGUIType::Property_Vector4;
        value.vec4.show_name = showName;
        value.vec4.pointer = vec4;
        callback_vec4 = callback;
    }
    
    void ShowGUIData::setTransformEulerAngle(const std::string& showName, glm::vec3* vec3, ValueChangeFunc_vec3 callback /*= nullptr*/)
    {
        m_eType = ShowGUIType::Property_TransformEulerAngle;
        value.vec3.show_name = showName;
        value.vec3.pointer = vec3;
        callback_tsEulerAngle = callback;
    }
    
    void ShowGUIData::setCheckbox(const std::string& showName, bool enable, ValueChangeFunc_checkbox callback /*= nullptr*/)
    {
        m_eType = ShowGUIType::Property_Checkbox;
        value.checkbox.show_name = showName;
        value.checkbox.isEnable = enable;
        callback_checkbox = callback;
    }
    
    
    
    
    /*
     BaseGUIPanel
     */
	BaseGUIPanel::BaseGUIPanel(const char* title, bool needUpdatePerFrame, float width, float height, bool showClose /*= true*/, float posX /*= 0.0f*/, float posY /*= 0.0f*/, float bgAlpha /*= 0.6f*/, ImGuiWindowFlags flags /*= ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings*/)
		: m_sTitle(title)
        , m_bNeedUpdataContentPerFrame(needUpdatePerFrame)
		, m_oSize(width, height)
		, m_oPosition(posX, posY)
		, m_fBgAlpha(bgAlpha)
		, m_bOpen(true)
		, m_bShowClose(showClose)
        , m_iWindowFlag(flags)
        , m_uTreeNodeIDCounter(0)
	{
        // 清理
        m_vContents.clear();
		m_mStyleColors.clear();
	}

	BaseGUIPanel::~BaseGUIPanel()
	{

	}
    
    void BaseGUIPanel::addCollapsingHeader(const std::string& title, bool expand /*= false*/, bool enable /*= true*/)
    {
        ShowGUIData data;
        data.setTitle(title, expand, enable);
        
        m_vContents.push_back(std::move(data));
    }
    
    void BaseGUIPanel::addTreeNode(const std::string& title, unsigned long nodeID, bool selected /*= false*/, bool expand /*= false*/, bool hasChildren /*= false*/, ShowGUIData::ClickFunc_TreeNode callback /*= nullptr*/)
    {
        ShowGUIData data(true);
        data.setTreeNode(title, nodeID, selected, expand, hasChildren, callback);
        
        m_vContents.push_back(std::move(data));
    }
    
    void BaseGUIPanel::addTreeNode(const ShowGUIData& nodeData)
    {
        m_vContents.push_back(nodeData);
    }
    
    void BaseGUIPanel::addPropertyText(const std::string& text, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setText(text);
        
        m_vContents.push_back(std::move(data));
    }

	void BaseGUIPanel::addSeparator()
	{
		ShowGUIData data(true);
		data.setSeparator();

		m_vContents.push_back(std::move(data));
	}
    
    void BaseGUIPanel::addPropertyVector2(const std::string& title, glm::vec2* vec2, ShowGUIData::ValueChangeFunc_vec2 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setVec2(title, vec2, callback);
        
        m_vContents.push_back(std::move(data));
    }
    
    void BaseGUIPanel::addPropertyVector3(const std::string& title, glm::vec3* vec3, ShowGUIData::ValueChangeFunc_vec3 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setVec3(title, vec3, callback);
        
        m_vContents.push_back(std::move(data));
    }
    
    void BaseGUIPanel::addPropertyVector4(const std::string& title, glm::vec4* vec4, ShowGUIData::ValueChangeFunc_vec4 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setVec4(title, vec4, callback);
        
        m_vContents.push_back(std::move(data));
    }

    void BaseGUIPanel::addPropertyTransformEulerAngle(const std::string& title, glm::vec3* vec3, ShowGUIData::ValueChangeFunc_vec3 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setTransformEulerAngle(title, vec3, callback);
        
        m_vContents.push_back(std::move(data));
    }
    
    void BaseGUIPanel::addPropertyCheckbox(const std::string& title, bool enable, ShowGUIData::ValueChangeFunc_checkbox callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setCheckbox(title, enable, callback);
        
        m_vContents.push_back(std::move(data));
    }
    
	void BaseGUIPanel::update(float deltaTime)
	{
		if (!m_bOpen)
		{
			return;
		}
		
		// 压入style color
		if (!m_mStyleColors.empty())
		{
			for (auto itor = m_mStyleColors.cbegin(); itor != m_mStyleColors.cend(); ++itor)
			{
				ImGui::PushStyleColor(itor->first, itor->second);
			}
		}
		// 第二个参数将显示一个关闭按钮，如果点击了关闭按钮，则第二个参数将被置为false
		if (m_bShowClose)
		{
			ImGui::Begin(m_sTitle, &m_bOpen, m_oSize, m_fBgAlpha, m_iWindowFlag);
		}
		else
		{
			ImGui::Begin(m_sTitle, nullptr, m_oSize, m_fBgAlpha, m_iWindowFlag);
		}
		ImGui::SetWindowPos(m_oPosition);


        // 更新内容
        if (m_bNeedUpdataContentPerFrame)
        {
            updateContent();
        }
        // 渲染窗体
        drawContent();

		// end
		ImGui::End();
		// 弹出style color
		ImGui::PopStyleColor(m_mStyleColors.size());
	}

	void BaseGUIPanel::setWindowStyleColor(ImGuiCol_ style, float red, float green, float blue, float alpha)
	{
		auto itor = m_mStyleColors.find(style);
		if (itor == m_mStyleColors.end())
		{
			m_mStyleColors.emplace(style, ImVec4(red, green, blue, alpha));
		}
		else
		{
			itor->second = ImVec4(red, green, blue, alpha);
		}
	}
    
    void BaseGUIPanel::cleanContent()
    {
        m_vContents.clear();
    }
    
    void BaseGUIPanel::drawContent()
    {
        // InputText组件默认flag
        static ImGuiInputTextFlags readOnlyInputTextFlag = ImGuiInputTextFlags_ReadOnly;
        static ImGuiInputTextFlags writableInputTextFlag = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll;
        // TreeNode组件默认flag
        // 默认（关闭，没有选中）
        static ImGuiTreeNodeFlags defaultTreeNodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        
        // 记录上一个可用来折叠的标签
        ShowGUIData* expandTitle = nullptr;
        
        auto itor=m_vContents.begin();
        while(itor != m_vContents.end())
        {
            ShowGUIData& data = (*itor);
            
            if(data.m_eType == ShowGUIData::ShowGUIType::ExpandTitle)
            {
				//if (ImGui::Checkbox(data.value.title.show_name.c_str(), "", &data.value.title.isEnable))
				//{
				//	// TODO: 回调函数
				//}
				//ImGui::SameLine();

				data.value.title.isExpand = ImGui::CollapsingHeader(data.value.title.show_name.c_str(), 0, true, data.value.title.isExpand);
				expandTitle = &data;
            }
            else
            {
                if (data.canExpand && expandTitle && !expandTitle->value.title.isExpand)
                {
                    // 如果可折叠，但是没有展开，则不绘制
                    ++itor;
                    continue;
                }
                
                switch(data.m_eType)
                {
                    case ShowGUIData::ShowGUIType::Property_Vector2:
                        {
                            data.value.vec2.show_value = (*data.value.vec2.pointer);
                            _createInputFloatText(data, ImGui::InputFloat2, data.value.vec2.show_name, data.value.vec2.show_value, data.callback_vec2);
                        }
                        break;
                        
                    case ShowGUIData::ShowGUIType::Property_Vector3:
                        {
                            data.value.vec3.show_value = (*data.value.vec3.pointer);
                            _createInputFloatText(data, ImGui::InputFloat3, data.value.vec3.show_name, data.value.vec3.show_value, data.callback_vec3);
                        }
                        break;
                        
                    case ShowGUIData::ShowGUIType::Property_Vector4:
                        {
                            data.value.vec4.show_value = (*data.value.vec4.pointer);
                            _createInputFloatText(data, ImGui::InputFloat4, data.value.vec4.show_name, data.value.vec4.show_value, data.callback_vec4);
                        }
                        break;
                        
                    case ShowGUIData::ShowGUIType::Property_Text:
                        {
                            ImGui::Text(data.value.text.show_name.c_str());
                        }
                        break;
                        
                    case ShowGUIData::ShowGUIType::Property_TransformEulerAngle:
                        {
                            data.value.vec3.show_value = (*data.value.vec3.pointer);
                            if (data.isReadOnly)
                            {
                                // 只读数据
                                ImGui::InputFloat3(data.value.vec3.show_name.c_str(), &data.value.vec3.show_value[0], ImGui_InputFloatText_Decimal_Precision, readOnlyInputTextFlag);
                            }
                            else
                            {
                                // 可写数据
                                if (ImGui::InputFloat3(data.value.vec3.show_name.c_str(), &data.value.vec3.show_value[0], ImGui_InputFloatText_Decimal_Precision, writableInputTextFlag))
                                {
                                    // 由于万向锁，这里将x轴固定在[-90,90]
                                    if (data.value.vec3.show_value.x < -90.0f)
                                    {
                                        data.value.vec3.show_value.x = -90.0f;
                                    }
                                    else if(data.value.vec3.show_value.x > 90.0f)
                                    {
                                        data.value.vec3.show_value.x = 90.0f;
                                    }
                                    
                                    if (data.callback_tsEulerAngle && data.isValueChange())
                                    {
                                        data.callback_tsEulerAngle(data.value.vec3.show_value);
                                    }
                                }
                            }
                        }
                        break;
                        
                    case ShowGUIData::ShowGUIType::Property_Checkbox:
                        {
                            if (ImGui::Checkbox(data.value.checkbox.show_name.c_str(), &data.value.checkbox.isEnable))
                            {
                                if (data.callback_checkbox)
                                {
                                    data.callback_checkbox(data.value.checkbox.isEnable);
                                }
                            }

                        }
                        break;

					case ShowGUIData::ShowGUIType::Separator:
						{
							ImGui::Separator();
						}
						break;
                        
//                         //TreeNode需要在展开的节点处调用一下ImGui::TreePop,而且其子节点要在treepop之前生成完，牵扯到一个递归问题，所以暂时没法写进m_vContent这种顺序结构里，后面看看有没有好的优化方案
//                    case ShowGUIData::ShowGUIType::TreeNode:
//                        {
//                            ImGuiTreeNodeFlags flag = defaultTreeNodeFlag
//                                | (data.value.tree_node.isExpand ? ImGuiTreeNodeFlags_DefaultOpen : 0)    // 是否展开
//                                | (data.value.tree_node.selected ? ImGuiTreeNodeFlags_Selected : 0)   // 是否选中
//                                | (data.value.tree_node.hasChildren ? (ImGuiTreeNodeFlags_Leaf) : 0); // 是否有子节点
//
//                            bool isExpand = ImGui::TreeNodeEx(data.value.tree_node.key_id.c_str(), flag, data.value.tree_node.show_name.c_str());
//                            if (ImGui::IsItemClicked())
//                            {
//                                if(data.callback_treeNode)
//                                {
//                                    data.callback_treeNode(data);
//                                }
//                            }
//                            if (isExpand)
//                            {
//                                data.value.tree_node.isExpand = true;
//                                ImGui::TreePop();
//                            }
//                        }
//                        break;
                }
            }
            
            ++itor;
        }
    }
    
    

}
