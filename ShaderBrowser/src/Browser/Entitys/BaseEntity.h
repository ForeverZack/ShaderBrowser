#ifndef ENTITYS_BASEENTITY_H
#define ENTITYS_BASEENTITY_H

#include <vector>
#include "../../Common/Components/Reference.h"
#include "../Components/BaseComponent.h"

namespace browser
{
	class BaseEntity : public common::Reference
	{
	public:       
		// 记录\移除特殊的组件
		#define MARK_SPECIAL_COMPONENT(markVar, component, isEmpty)\
			if (isEmpty) { markVar = nullptr; }\
			else { markVar = component; }
		

	public:
		//static BaseComponent* create();

	public:
		BaseEntity();
		virtual ~BaseEntity();

	public:
		// 添加组件
		void addComponent(BaseComponent* component);
		// 移除组件
		void removeComponent(BaseComponent* component);
		// 根据类型移除组件(注意，以为是按系统类型移除的，所以可能会同时移除多个组件)
		void removeComponents(SystemType type);
        // 传递组件消息
        // 注意:这里的BaseComponentMessage是一个Reference，每帧结束时会自动清除，并且这套消息机制是实时的，接收者会立刻处理
        void deliverComponentMessage(ComponentEvent event, BaseComponentMessage* msg);

        
		//判断组件是否可渲染
		bool isRenderable();
        
        REGISTER_PROPERTY_GET(BaseComponent*, m_oMeshFilterComponent, MeshFilter);

	private:
		// 添加\移除组件时，标记或者去标记特殊的组件
		void markSpecialComponent(BaseComponent* component, bool bEmpty = false);


	private:
		// 组件队列
		// 注意：这里的组件列表是通过system添加过来的，system会检测组件是否互斥（一个entity是否已经拥有相同类型的组件）
		std::vector<BaseComponent*> m_vComponents;
		// Transform组件
		BaseComponent* m_oTransformComponent;
		// 渲染组件(方便快速获取组件对象)
		BaseComponent* m_oRenderComponent;
		// 网格过滤器组件
		BaseComponent* m_oMeshFilterComponent;
	};
}

#endif
