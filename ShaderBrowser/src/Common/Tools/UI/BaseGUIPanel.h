#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "Common/Tools/Utils.h"

namespace common
{
    // imgui的InputFloatText组件小数精确到第几位
	#define ImGui_InputFloatText_Decimal_Precision 2
	#define ImGui_InputFloatText_Decimal_Precision_STR "%.2f"
    
    // GUI显示结构体
    class ShowGUIData
    {
    public:
        // GUI显示的枚举
		enum ShowGUIType
		{
			// 折叠标签 :
			ExpandTitle = 0,
			// 树状图节点 :
			TreeNode,
			// 属性 : 
			// float
			Property_InputFloat,
			Property_Vector2,
			Property_Vector3,
			Property_Vector4,
			// text
			Property_Text,
			// checkbox
			Property_Checkbox,
			// color
			Property_Color3,
			Property_Color4,
			// slider
			Property_SliderFloat,
			Property_SliderFloat2,
			Property_SliderFloat3,
			Property_SliderFloat4,
            
            //特殊处理Transform的欧拉角属性
            Property_TransformEulerAngle,
			// 分割线
			Separator,

        };
        
        // 回调函数类型
		typedef std::function<void(float)> ValueChangeFunc_inputFloat;
		typedef std::function<void(const glm::vec2&)> ValueChangeFunc_vec2;
        typedef std::function<void(const glm::vec3&)> ValueChangeFunc_vec3;
        typedef std::function<void(const glm::vec4&)> ValueChangeFunc_vec4;
        typedef std::function<void(bool)> ValueChangeFunc_checkbox;
        typedef std::function<void(ShowGUIData&)> ClickFunc_TreeNode;
		typedef std::function<void(const glm::vec3&)> ValueChangeFunc_color3;
		typedef std::function<void(const glm::vec4&)> ValueChangeFunc_color4;
		typedef std::function<void(float)> ValueChangeFunc_sliderFloat;
		typedef std::function<void(const glm::vec2&)> ValueChangeFunc_sliderFloat2;
		typedef std::function<void(const glm::vec3&)> ValueChangeFunc_sliderFloat3;
		typedef std::function<void(const glm::vec4&)> ValueChangeFunc_sliderFloat4;

    public:
        ShowGUIData();
        ShowGUIData(bool readOnly, bool expand = true);
        ShowGUIData(const ShowGUIData& data);
        ShowGUIData(const ShowGUIData&& data) noexcept;
        ~ShowGUIData();
        
    public:
        void setTitle(const std::string& title, bool expand = false, bool enable = true);
        void setText(const std::string& text);
		void setInputFloat(const std::string& showName, float* val, ValueChangeFunc_inputFloat callback = nullptr);
		void setVec2(const std::string& showName, glm::vec2* vec2, ValueChangeFunc_vec2 callback = nullptr);
        void setVec3(const std::string& showName, glm::vec3* vec3, ValueChangeFunc_vec3 callback = nullptr);
        void setVec4(const std::string& showName, glm::vec4* vec4, ValueChangeFunc_vec4 callback = nullptr);
        void setTransformEulerAngle(const std::string& showName, glm::vec3* vec3, ValueChangeFunc_vec3 callback = nullptr);
        void setCheckbox(const std::string& showName, bool enable, ValueChangeFunc_checkbox callback = nullptr);
        void setTreeNode(const std::string& showName, unsigned long nodeID, bool selected = false, bool expand = false, bool hasChildren = false, ClickFunc_TreeNode callback = nullptr);
		void setColor3(const std::string& showName, glm::vec3* color, ValueChangeFunc_color3 callback = nullptr);
		void setColor4(const std::string& showName, glm::vec4* color, ValueChangeFunc_color4 callback = nullptr);
		void setSliderFloat(const std::string& showName, float* val, float min, float max, ValueChangeFunc_sliderFloat callback = nullptr);
		void setSliderFloat2(const std::string& showName, glm::vec2* vec2, float min, float max, ValueChangeFunc_sliderFloat2 callback = nullptr);
		void setSliderFloat3(const std::string& showName, glm::vec3* vec3, float min, float max, ValueChangeFunc_sliderFloat3 callback = nullptr);
		void setSliderFloat4(const std::string& showName, glm::vec4* vec4, float min, float max, ValueChangeFunc_sliderFloat4 callback = nullptr);
		void setSeparator();

        bool isValueChange();

        
    public:
        // 类型
        ShowGUIType m_eType;
        // 是否被折叠进标签
        bool canExpand;
        // 是否只读
        bool isReadOnly;
        // 值
        union U
        {
            struct
            {
                std::string show_name;
                bool isExpand;
				bool isEnable;
            } title;
            struct
            {
                std::string show_name;
                std::string key_id;
                bool selected;
                bool isExpand;
                bool hasChildren;
                unsigned long node_id;
            } tree_node;
            struct
            {
                std::string show_name;
            } text;
			struct
			{
				std::string show_name;
				float* pointer;
				float show_value;
			} input_float;
            struct
            {
                std::string show_name;
                glm::vec2* pointer;
                glm::vec2 show_value;
            } vec2;
            struct
            {
                std::string show_name;
                glm::vec3* pointer;
                glm::vec3 show_value;
            } vec3, color3;
            struct
            {
                std::string show_name;
                glm::vec4* pointer;
                glm::vec4 show_value;
            } vec4, color4;
            struct
            {
                std::string show_name;
                bool isEnable;
            } checkbox;
			struct
			{
				std::string show_name;
				float* pointer;
				float show_value;
				float min;
				float max;
			} sliderFloat;
			struct
			{
				std::string show_name;
				glm::vec2* pointer;
				glm::vec2 show_value;
				float min;
				float max;
			} sliderFloat2;
			struct
			{
				std::string show_name;
				glm::vec3* pointer;
				glm::vec3 show_value;
				float min;
				float max;
			} sliderFloat3;
			struct
			{
				std::string show_name;
				glm::vec4* pointer;
				glm::vec4 show_value;
				float min;
				float max;
			} sliderFloat4;
            
            // 这一段一定要加，暂时还不理解为什么，不然不能使用构造函数和析构函数
            U() { memset(this, 0, sizeof(*this)); }
            ~U() {}
            U& operator=(const U& other) {
                memcpy(this, &other, sizeof(*this));
                return *this;
            }

                        
        } value;
        
		ValueChangeFunc_inputFloat callback_inputFloat;
		ValueChangeFunc_vec2 callback_vec2;
        ValueChangeFunc_vec3 callback_vec3;
        ValueChangeFunc_vec4 callback_vec4;
        ValueChangeFunc_vec3 callback_tsEulerAngle;
        ValueChangeFunc_checkbox callback_checkbox;
        ClickFunc_TreeNode callback_treeNode;
		ValueChangeFunc_color3 callback_color3;
		ValueChangeFunc_color4 callback_color4;
		ValueChangeFunc_sliderFloat callback_sliderFloat;
		ValueChangeFunc_sliderFloat2 callback_sliderFloat2;
		ValueChangeFunc_sliderFloat3 callback_sliderFloat3;
		ValueChangeFunc_sliderFloat4 callback_sliderFloat4;
    };
    
	class BaseGUIPanel
	{
	public:
		BaseGUIPanel(const char* title, bool needUpdatePerFrame, float width, float height, bool showClose = true, float posX = 0.0f, float posY = 0.0f, float bgAlpha = 0.8f, ImGuiWindowFlags flags 
					= ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		virtual ~BaseGUIPanel();
    
    public:
        // 添加折叠标签页
        void addCollapsingHeader(const std::string& title, bool expand = false, bool enable = true);
        // 添加树节点
        void addTreeNode(const std::string& title, unsigned long nodeID, bool selected = false, bool expand = false, bool hasChildren = false, ShowGUIData::ClickFunc_TreeNode callback = nullptr);
        void addTreeNode(const ShowGUIData& nodeData);
        // 添加属性
        void addPropertyText(const std::string& text, bool readOnly = true, bool expand = true);
		void addPropertyInputFloat(const std::string& title, float* val, ShowGUIData::ValueChangeFunc_inputFloat callback = nullptr, bool readOnly = true, bool expand = true);
        void addPropertyVector2(const std::string& title, glm::vec2* vec2, ShowGUIData::ValueChangeFunc_vec2 callback = nullptr, bool readOnly = true, bool expand = true);
        void addPropertyVector3(const std::string& title, glm::vec3* vec3, ShowGUIData::ValueChangeFunc_vec3 callback = nullptr, bool readOnly = true, bool expand = true);
        void addPropertyVector4(const std::string& title, glm::vec4* vec4, ShowGUIData::ValueChangeFunc_vec4 callback = nullptr, bool readOnly = true, bool expand = true);
        void addPropertyTransformEulerAngle(const std::string& title, glm::vec3* vec3, ShowGUIData::ValueChangeFunc_vec3 callback = nullptr, bool readOnly = true, bool expand = true);
        void addPropertyCheckbox(const std::string& title, bool enable, ShowGUIData::ValueChangeFunc_checkbox callback = nullptr, bool readOnly = true, bool expand = true);
		void addPropertyColor3(const std::string& title, glm::vec3* color, ShowGUIData::ValueChangeFunc_color3 callback = nullptr, bool readOnly = true, bool expand = true);
		void addPropertyColor4(const std::string& title, glm::vec4* color, ShowGUIData::ValueChangeFunc_color4 callback = nullptr, bool readOnly = true, bool expand = true);
		void addPropertySliderFloat(const std::string& title, float* val, float min, float max, ShowGUIData::ValueChangeFunc_inputFloat callback = nullptr, bool readOnly = true, bool expand = true);
		void addPropertySliderFloat2(const std::string& title, glm::vec2* vec2, float min, float max, ShowGUIData::ValueChangeFunc_vec2 callback = nullptr, bool readOnly = true, bool expand = true);
		void addPropertySliderFloat3(const std::string& title, glm::vec3* vec3, float min, float max, ShowGUIData::ValueChangeFunc_vec3 callback = nullptr, bool readOnly = true, bool expand = true);
		void addPropertySliderFloat4(const std::string& title, glm::vec4* vec4, float min, float max, ShowGUIData::ValueChangeFunc_vec4 callback = nullptr, bool readOnly = true, bool expand = true);
		void addSeparator();

	public:
		// 设置窗口样式颜色
		void setWindowStyleColor(ImGuiCol_ style, float red, float green, float blue, float alpha);
		// 每帧更新
		void update(float deltaTime);
        
        // 清空窗口内容
        void cleanContent();
        // 更新窗口内容
        virtual void updateContent() {}
        
        // 渲染窗口内容
        virtual void drawContent();
        
        
    protected:
        
        // 插入InputFloatText组件
        template<typename T, typename CreateFunc, typename CallbackFunc>
        void _createInputFloatText(ShowGUIData& data, CreateFunc createFunc, const std::string& title, T& showValue, CallbackFunc callback)
        {
            static ImGuiInputTextFlags readOnlyInputTextFlag = ImGuiInputTextFlags_ReadOnly;
            static ImGuiInputTextFlags writableInputTextFlag = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll;
            
            if (data.isReadOnly)
            {
                // 只读数据
                createFunc(title.c_str(), &showValue[0], ImGui_InputFloatText_Decimal_Precision, readOnlyInputTextFlag);
            }
            else
            {
                // 可写数据
                if (createFunc(title.c_str(), &showValue[0], ImGui_InputFloatText_Decimal_Precision, writableInputTextFlag))
                {
                    if (callback && data.isValueChange())
                    {
                        BROWSER_LOG("value change ====" + title)
                        callback(showValue);
                    }
                }
            }
        }
		// 插入ColorEdit组件
		template<typename T, typename CreateFunc, typename CallbackFunc>
		void _createColorEdit(ShowGUIData& data, CreateFunc createFunc, const std::string& title, T& showValue, CallbackFunc callback)
		{
			if (data.isReadOnly)
			{
				// 只读数据
				createFunc(title.c_str(), &showValue[0], 0);
			}
			else
			{
				// 可写数据
				if (createFunc(title.c_str(), &showValue[0], 0))
				{
					if (callback && data.isValueChange())
					{
						BROWSER_LOG("value change ====" + title)
						callback(showValue);
					}
				}
			}
		}
		// 插入SliderFloat组件
		template<typename T, typename CreateFunc, typename CallbackFunc>
		void _createSliderFloat(ShowGUIData& data, CreateFunc createFunc, const std::string& title, T& showValue, float min, float max, CallbackFunc callback)
		{
			if (data.isReadOnly)
			{
				// 只读数据
				createFunc(title.c_str(), &showValue[0], min, max, ImGui_InputFloatText_Decimal_Precision_STR, 1);
			}
			else
			{
				// 可写数据
				if (createFunc(title.c_str(), &showValue[0], min, max, ImGui_InputFloatText_Decimal_Precision_STR, 1))
				{
					if (callback && data.isValueChange())
					{
						callback(showValue);
					}
				}
			}
		}
        
        // 生成唯一ID
        unsigned long generateTreeNodeID()
        {
            // 这里用0作为初始值，前置++保证0不会被使用
            return ++m_uTreeNodeIDCounter;
        }
        
        


	protected:
		// 标题
		const char* m_sTitle;
		// 宽高
		ImVec2 m_oSize;
		// 位置
		ImVec2 m_oPosition;
		// 背景透明度
		float m_fBgAlpha;
		// 是否开启
		bool m_bOpen;
		// 是否显示关闭按钮
		bool m_bShowClose;
        // 窗口样式flag
        ImGuiWindowFlags m_iWindowFlag;
		// 样式默认颜色
		std::unordered_map<ImGuiCol_, ImVec4> m_mStyleColors;
        
        // 窗口要显示的内容队列
        std::vector<ShowGUIData> m_vContents;
        // 内容是否需要被每帧都刷新（如果不需要，可以自己手动刷新内容，如Inspector面板）
        bool m_bNeedUpdataContentPerFrame;

        // 节点id生成
        unsigned long m_uTreeNodeIDCounter;
	};

}
