#include "BaseGUIPanel.h"

namespace common
{
    /*
        ShowGUIData
     */
    ShowGUIData::ShowGUIData()
        : isReadOnly(true)
        , canExpand(true)
		, helpMarker("")
		, callback_inputFloat(nullptr)
        , callback_vec2(nullptr)
        , callback_vec3(nullptr)
        , callback_vec4(nullptr)
        , callback_tsEulerAngle(nullptr)
        , callback_checkbox(nullptr)
        , callback_treeNode(nullptr)
		, callback_color3(nullptr)
		, callback_color4(nullptr)
		, callback_sliderFloat(nullptr)
		, callback_sliderFloat2(nullptr)
		, callback_sliderFloat3(nullptr)
		, callback_sliderFloat4(nullptr)
		, callback_inputText(nullptr)
    {
    }
    
    ShowGUIData::ShowGUIData(bool readOnly, bool expand /*= true*/)
        : isReadOnly(readOnly)
        , canExpand(expand)
		, helpMarker("")
        , callback_vec2(nullptr)
        , callback_vec3(nullptr)
        , callback_vec4(nullptr)
        , callback_tsEulerAngle(nullptr)
        , callback_checkbox(nullptr)
        , callback_treeNode(nullptr)
		, callback_color3(nullptr)
		, callback_color4(nullptr)
		, callback_sliderFloat(nullptr)
		, callback_sliderFloat2(nullptr)
		, callback_sliderFloat3(nullptr)
		, callback_sliderFloat4(nullptr)
		, callback_inputText(nullptr)
    {
        
    }
    
    ShowGUIData::ShowGUIData(const ShowGUIData& data)
        : m_eType(data.m_eType)
        , canExpand(data.canExpand)
        , isReadOnly(data.isReadOnly)
		, helpMarker("")
		, callback_inputFloat(data.callback_inputFloat)
        , callback_vec2(data.callback_vec2)
        , callback_vec3(data.callback_vec3)
        , callback_vec4(data.callback_vec4)
        , callback_tsEulerAngle(data.callback_tsEulerAngle)
        , callback_checkbox(data.callback_checkbox)
        , callback_treeNode(data.callback_treeNode)
		, callback_color3(data.callback_color3)
		, callback_color4(data.callback_color4)
		, callback_sliderFloat(data.callback_sliderFloat)
		, callback_sliderFloat2(data.callback_sliderFloat2)
		, callback_sliderFloat3(data.callback_sliderFloat3)
		, callback_sliderFloat4(data.callback_sliderFloat4)
		, callback_inputText(data.callback_inputText)
    {
        value = data.value;
    }
    
    ShowGUIData::ShowGUIData(const ShowGUIData&& data) noexcept
    {
        m_eType = data.m_eType;
        value = data.value;
        canExpand = data.canExpand;
        isReadOnly = data.isReadOnly;
		helpMarker = data.helpMarker;
		callback_inputFloat = data.callback_inputFloat;
        callback_vec2 = data.callback_vec2;
        callback_vec3 = data.callback_vec3;
        callback_vec4 = data.callback_vec4;
        callback_tsEulerAngle = data.callback_tsEulerAngle;
        callback_checkbox = data.callback_checkbox;
        callback_treeNode = data.callback_treeNode;
		callback_color3 = data.callback_color3;
		callback_color4 = data.callback_color4;
		callback_sliderFloat = data.callback_sliderFloat;
		callback_sliderFloat2 = data.callback_sliderFloat2;
		callback_sliderFloat3 = data.callback_sliderFloat3;
		callback_sliderFloat4 = data.callback_sliderFloat4;
		callback_inputText = data.callback_inputText;
    }
    
    ShowGUIData::~ShowGUIData()
    {
    }
    
    bool ShowGUIData::isValueChange()
    {
        switch (m_eType) {
			case ShowGUIType::Property_InputFloat:
				{
					return(*value.input_float.pointer) != value.input_float.show_value;
				}
				break;

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

			case ShowGUIType::Property_Color3:
				{
					return (*value.color3.pointer) != value.color3.show_value;
				}
				break;

			case ShowGUIType::Property_Color4:
				{
					return (*value.color4.pointer) != value.color4.show_value;
				}
				break;

			case ShowGUIType::Property_SliderFloat:
				{
					return(*value.sliderFloat.pointer) != value.sliderFloat.show_value;
				}
				break;

			case ShowGUIType::Property_SliderFloat2:
				{
					return (*value.sliderFloat2.pointer) != value.sliderFloat2.show_value;
				}
				break;

			case ShowGUIType::Property_SliderFloat3:
				{
					return (*value.sliderFloat3.pointer) != value.sliderFloat3.show_value;
				}
				break;

			case ShowGUIType::Property_SliderFloat4:
				{
					return (*value.sliderFloat4.pointer) != value.sliderFloat4.show_value;
				}
				break;

			case ShowGUIType::Property_InputText:
			case ShowGUIType::Property_InputTextMultiline:
				{
					return (*value.inputText.pointer) != value.inputText.show_value;
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

	void ShowGUIData::setInputFloat(const std::string& showName, float* val, ValueChangeFunc_inputFloat callback/* = nullptr*/)
	{
		m_eType = ShowGUIType::Property_InputFloat;
		value.input_float.show_name = showName;
		value.input_float.pointer = val;
		callback_inputFloat = callback;
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

	void ShowGUIData::setColor3(const std::string& showName, glm::vec3* color, ValueChangeFunc_color3 callback /*= nullptr*/)
	{
		m_eType = ShowGUIType::Property_Color3;
		value.color3.show_name = showName;
		value.color3.pointer = color;
		callback_color3 = callback;
	}

	void ShowGUIData::setColor4(const std::string& showName, glm::vec4* color, ValueChangeFunc_color4 callback /*= nullptr*/)
	{
		m_eType = ShowGUIType::Property_Color4;
		value.color4.show_name = showName;
		value.color4.pointer = color;
		callback_color4 = callback;
	}
    
	void ShowGUIData::setSliderFloat(const std::string& showName, float* val, float min, float max, ValueChangeFunc_sliderFloat callback/* = nullptr*/)
	{
		m_eType = ShowGUIType::Property_SliderFloat;
		value.sliderFloat.show_name = showName;
		value.sliderFloat.pointer = val;
		value.sliderFloat.min = min;
		value.sliderFloat.max = max;
		callback_sliderFloat = callback;
	}

	void ShowGUIData::setSliderFloat2(const std::string& showName, glm::vec2* vec2, float min, float max, ValueChangeFunc_sliderFloat2 callback/* = nullptr*/)
	{
		m_eType = ShowGUIType::Property_SliderFloat2;
		value.sliderFloat2.show_name = showName;
		value.sliderFloat2.pointer = vec2;
		value.sliderFloat2.min = min;
		value.sliderFloat2.max = max;
		callback_sliderFloat2 = callback;
	}

	void ShowGUIData::setSliderFloat3(const std::string& showName, glm::vec3* vec3, float min, float max, ValueChangeFunc_sliderFloat3 callback/* = nullptr*/)
	{
		m_eType = ShowGUIType::Property_SliderFloat3;
		value.sliderFloat3.show_name = showName;
		value.sliderFloat3.pointer = vec3;
		value.sliderFloat3.min = min;
		value.sliderFloat3.max = max;
		callback_sliderFloat3 = callback;
	}

	void ShowGUIData::setSliderFloat4(const std::string& showName, glm::vec4* vec4, float min, float max, ValueChangeFunc_sliderFloat4 callback/* = nullptr*/)
	{
		m_eType = ShowGUIType::Property_SliderFloat4;
		value.sliderFloat4.show_name = showName;
		value.sliderFloat4.pointer = vec4;
		value.sliderFloat4.min = min;
		value.sliderFloat4.max = max;
		callback_sliderFloat4 = callback;
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
    
	void ShowGUIData::setInputText(const std::string& showName, std::string* val, ValueChangeFunc_inputText callback/* = nullptr*/)
	{
		m_eType = ShowGUIType::Property_InputText;
		value.inputText.show_name = showName;
		value.inputText.pointer = val;
		callback_inputText = callback;
	}

	void ShowGUIData::setInputTextMultiline(const std::string& showName, std::string* val, float width/* = 0*/, float height/* = 0*/, ValueChangeFunc_inputText callback/* = nullptr*/)
	{
		m_eType = ShowGUIType::Property_InputTextMultiline;
		value.inputText.show_name = showName;
		value.inputText.pointer = val;
		callback_inputText = callback;
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
    
	ShowGUIData& BaseGUIPanel::addCollapsingHeader(const std::string& title, bool expand /*= false*/, bool enable /*= true*/)
    {
        ShowGUIData data;
        data.setTitle(title, expand, enable);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }
    
	ShowGUIData& BaseGUIPanel::addTreeNode(const std::string& title, unsigned long nodeID, bool selected /*= false*/, bool expand /*= false*/, bool hasChildren /*= false*/, ShowGUIData::ClickFunc_TreeNode callback /*= nullptr*/)
    {
        ShowGUIData data(true);
        data.setTreeNode(title, nodeID, selected, expand, hasChildren, callback);        
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }
    
	ShowGUIData& BaseGUIPanel::addTreeNode(ShowGUIData& nodeData)
    {
        m_vContents.push_back(nodeData);
		return m_vContents.back();
    }
    
	ShowGUIData& BaseGUIPanel::addPropertyText(const std::string& text, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setText(text);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }

	ShowGUIData& BaseGUIPanel::addSeparator()
	{
		ShowGUIData data(true);
		data.setSeparator();
		m_vContents.push_back(std::move(data));
		return m_vContents.back();
	}
    
	ShowGUIData& BaseGUIPanel::addPropertyInputFloat(const std::string& title, float* val, ShowGUIData::ValueChangeFunc_inputFloat callback/* = nullptr*/, bool readOnly/* = true*/, bool expand/* = true*/)
	{
		ShowGUIData data(readOnly, expand);
		data.setInputFloat(title, val, callback);
		m_vContents.push_back(std::move(data));
		return m_vContents.back();
	}

	ShowGUIData& BaseGUIPanel::addPropertyVector2(const std::string& title, glm::vec2* vec2, ShowGUIData::ValueChangeFunc_vec2 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setVec2(title, vec2, callback);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }
    
	ShowGUIData& BaseGUIPanel::addPropertyVector3(const std::string& title, glm::vec3* vec3, ShowGUIData::ValueChangeFunc_vec3 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setVec3(title, vec3, callback);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
	}
    
	ShowGUIData& BaseGUIPanel::addPropertyVector4(const std::string& title, glm::vec4* vec4, ShowGUIData::ValueChangeFunc_vec4 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setVec4(title, vec4, callback);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }

	ShowGUIData& BaseGUIPanel::addPropertyColor3(const std::string& title, glm::vec3* color, ShowGUIData::ValueChangeFunc_color3 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
	{
		ShowGUIData data(readOnly, expand);
		data.setColor3(title, color, callback);
		m_vContents.push_back(std::move(data));
		return m_vContents.back();
	}

	ShowGUIData& BaseGUIPanel::addPropertyColor4(const std::string& title, glm::vec4* color, ShowGUIData::ValueChangeFunc_color4 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
	{
		ShowGUIData data(readOnly, expand);
		data.setColor4(title, color, callback);
		m_vContents.push_back(std::move(data));
		return m_vContents.back();
	}

	ShowGUIData& BaseGUIPanel::addPropertyTransformEulerAngle(const std::string& title, glm::vec3* vec3, ShowGUIData::ValueChangeFunc_vec3 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setTransformEulerAngle(title, vec3, callback);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }

	ShowGUIData& BaseGUIPanel::addPropertySliderFloat(const std::string& title, float* val, float min, float max, ShowGUIData::ValueChangeFunc_inputFloat callback/* = nullptr*/, bool readOnly/* = true*/, bool expand/* = true*/)
	{
		ShowGUIData data(readOnly, expand);
		data.setSliderFloat(title, val, min, max, callback);
		m_vContents.push_back(std::move(data));
		return m_vContents.back();
	}

	ShowGUIData& BaseGUIPanel::addPropertySliderFloat2(const std::string& title, glm::vec2* vec2, float min, float max, ShowGUIData::ValueChangeFunc_vec2 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setSliderFloat2(title, vec2, min, max, callback);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }
    
	ShowGUIData& BaseGUIPanel::addPropertySliderFloat3(const std::string& title, glm::vec3* vec3, float min, float max, ShowGUIData::ValueChangeFunc_vec3 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setSliderFloat3(title, vec3, min, max, callback);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }
    
	ShowGUIData& BaseGUIPanel::addPropertySliderFloat4(const std::string& title, glm::vec4* vec4, float min, float max, ShowGUIData::ValueChangeFunc_vec4 callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setSliderFloat4(title, vec4, min, max, callback);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }
    
	ShowGUIData& BaseGUIPanel::addPropertyCheckbox(const std::string& title, bool enable, ShowGUIData::ValueChangeFunc_checkbox callback /*= nullptr*/, bool readOnly /*= true*/, bool expand /*= true*/)
    {
        ShowGUIData data(readOnly, expand);
        data.setCheckbox(title, enable, callback);
        m_vContents.push_back(std::move(data));
		return m_vContents.back();
    }

	ShowGUIData& BaseGUIPanel::addPropertyInputText(const std::string& title, std::string* val, ShowGUIData::ValueChangeFunc_inputText callback/* = nullptr*/, bool readOnly/* = true*/, bool expand/* = true*/)
	{
		ShowGUIData data(readOnly, expand);
		data.setInputText(title, val, callback);
		m_vContents.push_back(std::move(data));
		return m_vContents.back();
	}

	ShowGUIData& BaseGUIPanel::addPropertyInputTextMultiline(const std::string& title, std::string* val, float width/* = 0*/, float height/* = 0*/, ShowGUIData::ValueChangeFunc_inputText callback/* = nullptr*/, bool readOnly/* = true*/, bool expand/* = true*/)
	{
		ShowGUIData data(readOnly, expand);
		data.setInputTextMultiline(title, val, width, height, callback);
		m_vContents.push_back(std::move(data));
		return m_vContents.back();
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
		ImGui::SetNextWindowSize(m_oSize, ImGuiCond_Appearing);
		ImGui::SetNextWindowBgAlpha(m_fBgAlpha);
		if (m_bShowClose)
		{
			ImGui::Begin(m_sTitle, &m_bOpen, m_iWindowFlag);
		}
		else
		{
			ImGui::Begin(m_sTitle, nullptr, m_iWindowFlag);
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

	void BaseGUIPanel::setViewportSize(int width, int height)
	{

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

				data.value.title.isExpand = ImGui::CollapsingHeader(data.value.title.show_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
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
					case ShowGUIData::ShowGUIType::Property_InputFloat:
						{
							data.value.input_float.show_value = (*data.value.input_float.pointer);
							if (data.isReadOnly)
							{
								// 只读数据
								ImGui::InputFloat(data.value.input_float.show_name.c_str(), &data.value.input_float.show_value, 0, 0, ImGui_InputFloatText_Decimal_Precision, readOnlyInputTextFlag);
							}
							else
							{
								// 可写数据
								if (ImGui::InputFloat(data.value.input_float.show_name.c_str(), &data.value.input_float.show_value, 0, 0, ImGui_InputFloatText_Decimal_Precision, writableInputTextFlag))
								{
									if (data.callback_inputFloat && data.isValueChange())
									{
										data.callback_inputFloat(data.value.input_float.show_value);
									}
								}
							}
						}
						break;

                    case ShowGUIData::ShowGUIType::Property_Vector2:
                        {
                            data.value.vec2.show_value = (*data.value.vec2.pointer);
							// 注意！！！：重载的函数指针编译器无法判断使用哪个，所以要自己手动拿到函数指针
							bool (*createFunc)(const char*, float[2], int, ImGuiInputTextFlags) = ImGui::InputFloat2;
							//_createInputFloatText(data, ImGui::InputFloat2, data.value.vec2.show_name, data.value.vec2.show_value, data.callback_vec2);	// 错误
							_createInputFloatText(data, createFunc, data.value.vec2.show_name, data.value.vec2.show_value, data.callback_vec2);	// 正确
                        }
                        break;
                        
                    case ShowGUIData::ShowGUIType::Property_Vector3:
                        {
                            data.value.vec3.show_value = (*data.value.vec3.pointer);
							// 注意！！！：重载的函数指针编译器无法判断使用哪个，所以要自己手动拿到函数指针
							bool(*createFunc)(const char*, float[3], int, ImGuiInputTextFlags) = ImGui::InputFloat3;
							//_createInputFloatText(data, ImGui::InputFloat3, data.value.vec3.show_name, data.value.vec3.show_value, data.callback_vec3);	// 错误
							_createInputFloatText(data, createFunc, data.value.vec3.show_name, data.value.vec3.show_value, data.callback_vec3);	// 正确
                        }
                        break;
                        
                    case ShowGUIData::ShowGUIType::Property_Vector4:
                        {
                            data.value.vec4.show_value = (*data.value.vec4.pointer);
							// 注意！！！：重载的函数指针编译器无法判断使用哪个，所以要自己手动拿到函数指针
							bool(*createFunc)(const char*, float[4], int, ImGuiInputTextFlags) = ImGui::InputFloat4;
							//_createInputFloatText(data, ImGui::InputFloat4, data.value.vec4.show_name, data.value.vec4.show_value, data.callback_vec4);	// 错误
							_createInputFloatText(data, createFunc, data.value.vec4.show_name, data.value.vec4.show_value, data.callback_vec4);	// 正确
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

					case ShowGUIData::ShowGUIType::Property_Color3:
						{
							data.value.color3.show_value = (*data.value.color3.pointer);
							// 注意！！！：重载的函数指针编译器无法判断使用哪个，所以要自己手动拿到函数指针
							bool(*createFunc)(const char*, float[3], ImGuiColorEditFlags) = ImGui::ColorEdit3;
							_createColorEdit(data, createFunc, data.value.color3.show_name, data.value.color3.show_value, data.callback_color3);	// 正确
						}
						break;

					case ShowGUIData::ShowGUIType::Property_Color4:
						{
							data.value.color4.show_value = (*data.value.color4.pointer);
							// 注意！！！：重载的函数指针编译器无法判断使用哪个，所以要自己手动拿到函数指针
							bool(*createFunc)(const char*, float[4], ImGuiColorEditFlags) = ImGui::ColorEdit4;
							_createColorEdit(data, createFunc, data.value.color4.show_name, data.value.color4.show_value, data.callback_color4);	// 正确
						}
						break;

					case ShowGUIData::ShowGUIType::Property_SliderFloat:
					{
							data.value.sliderFloat.show_value = (*data.value.sliderFloat.pointer);

							if (data.isReadOnly)
							{
								// 只读数据
								ImGui::SliderFloat(data.value.sliderFloat.show_name.c_str(), &data.value.sliderFloat.show_value, data.value.sliderFloat.min, data.value.sliderFloat.max, ImGui_InputFloatText_Decimal_Precision_STR, 1);
							}
							else
							{
								// 可写数据
								if (ImGui::SliderFloat(data.value.sliderFloat.show_name.c_str(), &data.value.sliderFloat.show_value, data.value.sliderFloat.min, data.value.sliderFloat.max, ImGui_InputFloatText_Decimal_Precision_STR, 1))
								{
									if (data.callback_sliderFloat && data.isValueChange())
									{
										data.callback_sliderFloat(data.value.sliderFloat.show_value);
									}
								}
							}

						}
						break;

					case ShowGUIData::ShowGUIType::Property_SliderFloat2:
						{
							data.value.sliderFloat2.show_value = (*data.value.sliderFloat2.pointer);
							// 注意！！！：重载的函数指针编译器无法判断使用哪个，所以要自己手动拿到函数指针
							bool(*createFunc)(const char*, float[2], float, float, const char*, float) = ImGui::SliderFloat2;
							_createSliderFloat(data, createFunc, data.value.sliderFloat2.show_name, data.value.sliderFloat2.show_value, data.value.sliderFloat2.min, data.value.sliderFloat2.max, data.callback_sliderFloat2);	// 正确
						}
						break;

					case ShowGUIData::ShowGUIType::Property_SliderFloat3:
						{
							data.value.sliderFloat3.show_value = (*data.value.sliderFloat3.pointer);
							// 注意！！！：重载的函数指针编译器无法判断使用哪个，所以要自己手动拿到函数指针
							bool(*createFunc)(const char*, float[3], float, float, const char*, float) = ImGui::SliderFloat3;
							_createSliderFloat(data, createFunc, data.value.sliderFloat3.show_name, data.value.sliderFloat3.show_value, data.value.sliderFloat3.min, data.value.sliderFloat3.max, data.callback_sliderFloat3);	// 正确
						}
						break;

					case ShowGUIData::ShowGUIType::Property_SliderFloat4:
						{
							data.value.sliderFloat4.show_value = (*data.value.sliderFloat4.pointer);
							// 注意！！！：重载的函数指针编译器无法判断使用哪个，所以要自己手动拿到函数指针
							bool(*createFunc)(const char*, float[3], float, float, const char*, float) = ImGui::SliderFloat4;
							_createSliderFloat(data, createFunc, data.value.sliderFloat4.show_name, data.value.sliderFloat4.show_value, data.value.sliderFloat4.min, data.value.sliderFloat4.max, data.callback_sliderFloat4);	// 正确
						}
						break;

					case ShowGUIData::ShowGUIType::Property_InputText:
						{
							data.value.inputText.show_value = *data.value.inputText.pointer;
							if (_createCustomInputText(data.value.inputText.show_name.c_str(), &data.value.inputText.show_value))
							{
								if (data.callback_inputText && data.isValueChange())
								{
									data.callback_inputText(data.value.inputText.show_value);
								}
							}
						}
						break;

					case ShowGUIData::ShowGUIType::Property_InputTextMultiline:
						{
							data.value.inputText.show_value = *data.value.inputText.pointer;
							if (_createCustomInputTextMultiline(data.value.inputText.show_name.c_str(), &data.value.inputText.show_value, ImVec2(data.value.inputText.width, data.value.inputText.height)))
							{
								if (data.callback_inputText && data.isValueChange())
								{
									data.callback_inputText(data.value.inputText.show_value);
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

			if (data.helpMarker != "")
			{
				_createHelpMarker(data.helpMarker.c_str());
			}
            
            ++itor;
        }
    }
    
	void BaseGUIPanel::_createHelpMarker(const char* desc)
	{
		ImGui::SameLine();
		ImGui::TextDisabled("[?]");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	int BaseGUIPanel::customInputTextResizeCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			std::string* my_str = (std::string*)data->UserData;
			IM_ASSERT(&((*my_str)[0]) == data->Buf);
			my_str->resize(data->BufSize);  // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
			data->Buf = &((*my_str)[0]);
		}
		return 0;
	}

	bool BaseGUIPanel::_createCustomInputTextMultiline(const char* label, std::string* my_str, const ImVec2& size/* = ImVec2(0, 0)*/, ImGuiInputTextFlags flags/* = 0*/)
	{
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		return ImGui::InputTextMultiline(label, &(*my_str)[0], (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, BaseGUIPanel::customInputTextResizeCallback, (void*)my_str);
	}

	bool BaseGUIPanel::_createCustomInputText(const char* label, std::string* my_str, ImGuiInputTextFlags flags/* = 0*/)
	{
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		return ImGui::InputText(label, &(*my_str)[0], (size_t)my_str->size(), flags | ImGuiInputTextFlags_CallbackResize, BaseGUIPanel::customInputTextResizeCallback, (void*)my_str);
	}
    

}
