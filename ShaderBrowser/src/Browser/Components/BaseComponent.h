#ifndef COMS_BASECOMPONENT_H
#define COMS_BASECOMPONENT_H

#include <string>
#include "Common/Components/Reference.h"
#include "Common/CommonDefine.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/UI/InspectorPanel.h"
#include "Browser/Components/Event/ComponentEvent.h"

using namespace common;

namespace browser
{    
	class BaseEntity;
    class Transform;

	// 组件类枚举
	enum EnumComponentClass
	{
		ECC_BaseComponent = 0,
		ECC_Transform,
		ECC_BaseRenderer,
		ECC_SkinnedMeshRenderer,
		ECC_MeshFilter,
		ECC_BaseBoundBox,
		ECC_AABBBoundBox,
		ECC_Animator,
		ECC_Camera,
	};

    // 注意：BaseCompoent如果被new出来没有及时使用，应该在每帧结束被release掉
	class BaseComponent : public common::Reference
	{
	public:
		static EnumComponentClass ComponentClass;

	public:
        BaseComponent();
        BaseComponent(const std::string& name);
		virtual ~BaseComponent();

	public:
        
        // GUI属性面板
        virtual void onInspectorGUI(InspectorPanel* inspector) {}

        // 每帧刷新所有系统之前
        virtual void beforeUpdate(float deltaTime) {}
		// 每帧刷新
		virtual void update(float deltaTime) {}

		// 创建的时候调用
		virtual void onAwake() {}

		// 第一次update之前调用
		virtual void onStart() {}

		// 启用对象/设置为可见 时调用
		virtual void onEnabled() {}

		// 禁用对象/设为不可见 时调用
		virtual void onDisabled() {}

		// 销毁时调用
		virtual void onDestroy() {}
        
        // 发送事件
        virtual void dispatchEvent(ComponentEvent event, BaseComponentMessage* msg);
        virtual void dispatchEventToChildren(ComponentEvent event, BaseComponentMessage* msg);
        
        // 接受事件
        virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg) {}

		REGISTER_PROPERTY_GET(SystemType, m_eBelongSystem, BelongSystem)
		REGISTER_PROPERTY_GET_SET(BaseEntity*, m_oBelongEntity, BelongEntity)
        REGISTER_PROPERTY_CONSTREF_GET(std::string, m_sComponentName, ComponentName)
        REGISTER_PROPERTY_GET_SET(Transform*, m_oTransform, Transform)
        
    public:
        // 获取Transform组件
        template <typename MsgType>
        void getTransformFromMsg(BaseComponentMessage* msg)
        {
            MsgType* convertMsg = static_cast<MsgType*>(msg);
            Transform* transform = convertMsg->getTransform();
            if (!m_oTransform && transform)
            {
                m_oTransform = transform;
            }
        }

    protected:
        // Transform组件
        Transform* m_oTransform;

        
	protected:
		// 组件属于哪个系统
		SystemType m_eBelongSystem;
		// 组件属于哪个Entity
		BaseEntity* m_oBelongEntity;
        
        // 组件名称
        std::string m_sComponentName;
	};
}

#endif
