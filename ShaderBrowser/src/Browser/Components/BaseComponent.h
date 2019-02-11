#ifndef COMS_BASECOMPONENT_H
#define COMS_BASECOMPONENT_H

#include "Common/Components/Reference.h"
#include "Common/CommonDefine.h"
#include "Common/Tools/Utils.h"
#include "Browser/Components/Event/ComponentEvent.h"

using namespace common;

namespace browser
{    
	class BaseEntity;
    class Transform;
    class MeshFilter;

    // 注意：BaseCompoent如果被new出来没有及时使用，应该在每帧结束被release掉
	class BaseComponent : public common::Reference
	{
	public:

	public:
		BaseComponent();
		virtual ~BaseComponent();

	public:

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
        
        // 接受事件
        virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg) {}

		REGISTER_PROPERTY_GET(SystemType, m_eBelongSystem, BelongSystem)
		REGISTER_PROPERTY_GET_SET(BaseEntity*, m_oBelongEntity, BelongEntity)
        
    protected:
        // 获取Transform组件
        template <typename MsgType>
        void getTransformFromMsg(BaseComponentMessage* msg)
        {
            MsgType* convertMsg = dynamic_cast<MsgType*>(msg);
            Transform* transform = convertMsg->getTransform();
            if (!m_oTransform && transform)
            {
                m_oTransform = transform;
            }
        }
        // Transform组件
        Transform* m_oTransform;
        
        // 获取MeshFilter组件
        template <typename MsgType>
        void getMeshFilterFromMsg(BaseComponentMessage* msg)
        {
            MsgType* convertMsg = dynamic_cast<MsgType*>(msg);
            MeshFilter* meshFilter = convertMsg->getMeshFilter();
            if (!m_oMeshFilter && meshFilter)
            {
                m_oMeshFilter = meshFilter;
            }
        }
        // MeshFilter组件
        MeshFilter* m_oMeshFilter;
        
	protected:
		// 组件属于哪个系统
		SystemType m_eBelongSystem;
		// 组件属于哪个Entity
		BaseEntity* m_oBelongEntity;
	};
}

#endif
