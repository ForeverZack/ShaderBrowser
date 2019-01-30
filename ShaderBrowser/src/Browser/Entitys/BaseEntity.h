#pragma once

#include <vector>
#include "../../Common/Components/Reference.h"
#include "../Components/BaseComponent.h"
#include "../Components/Transform/Transform.h"

namespace browser
{

	class BaseEntity : public common::Reference
	{
	public:
		static BaseEntity* create(std::string transformName = "");

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
        // 传递组件消息(在同一个Entity内部传递)
        // 注意:这里的BaseComponentMessage是一个Reference，每帧结束时会自动清除，并且这套消息机制是实时的，接收者会立刻处理
        void deliverComponentMessage(ComponentEvent event, BaseComponentMessage* msg);

        // 添加子实体
		void addChild(BaseEntity* entity);
		// 移除子实体
		void removeChild(BaseEntity* entity);
		// 判断组件是否可渲染
		bool isRenderable();
        
		REGISTER_PROPERTY_GET(BaseComponent*, m_oMeshFilterComponent, MeshFilter);
		REGISTER_PROPERTY_GET(Transform*, m_oTransformComponent, Transform);

	private:
		// 添加\移除组件时，标记或者去标记特殊的组件
		void markSpecialComponent(BaseComponent* component, bool bEmpty = false);

		// 记录\移除特殊的组件  (注意！！！这里第一个参数使用的是指针的引用(T* &)，用来给成员变量赋值)
		template <typename T>
		void MARK_SPECIAL_COMPONENT(T* &markVar, BaseComponent* component, bool isEmpty)
		{
			if (isEmpty)
			{
				markVar = nullptr;
			}
			else
			{
				markVar = dynamic_cast<T*>(component);
			}
		}

	private:
		// 组件队列
		// 注意：这里的组件列表是通过system添加过来的，system会检测组件是否互斥（一个entity是否已经拥有相同类型的组件）
		std::vector<BaseComponent*> m_vComponents;
		// Transform组件
		Transform* m_oTransformComponent;
		// 渲染组件(方便快速获取组件对象)
		BaseComponent* m_oRenderComponent;
		// 网格过滤器组件
		BaseComponent* m_oMeshFilterComponent;
	};


}

