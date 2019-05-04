#pragma once

#include <vector>
#include <unordered_map>
#include "Common/Components/Reference.h"
#include "Browser/Components/BaseComponent.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Browser/Components/Transform/Transform.h"
#include "Browser/Components/BoundBox/BaseBoundBox.h"
#include "Browser/Components/Animation/Animator.h"
#include "Browser/Components/Render/BaseRender.h"

namespace customGL
{
	class Model;
};

namespace browser
{

	class BaseEntity : public common::Reference
	{
	public:
		static BaseEntity* create(const std::string& transformName = "");

	public:
		BaseEntity();
		virtual ~BaseEntity();

		BaseEntity* clone();

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
        void deliverComponentMessageToChildren(ComponentEvent event, BaseComponentMessage* msg);

		// Transform组件相关操作
        // 添加子实体
		void addChild(BaseEntity* entity);
		// 移除子实体
		void removeChild(BaseEntity* entity);
		// 设置位置
		void setPosition(float x, float y, float z);
		// 设置欧拉角
		void setEulerAngle(float x, float y, float z);
		// 设置缩放
		void setScale(float x, float y, float z);
		// 设置名称
		void setName(const std::string& name);

		// 判断组件是否可渲染
		bool isRenderable();
        // 可见性检测
        bool checkVisibility(Camera* camera, bool reCalculate = false);
        
		// 修改所有网格模型shader
		void changeAllMeshesMaterial(const std::string& programName);
        // 播放动画
        void playAnimation(const std::string& animName, bool repeat = false, float speed = 1.0f, bool interpolate = true);
        void playAnimation(int animIdx, bool repeat = false, float speed = 1.0f, bool interpolate = true);  
		void setAnimatorUseGPU(bool useGPU);
        // 获取顶点数组
        glm::vec4* getVertices(Mesh* mesh, bool& dirty);
		// 获取骨骼数组
		void useBonesMatrix(Material* material);
        
		REGISTER_PROPERTY_GET(MeshFilter*, m_oMeshFilterComponent, MeshFilter);
		REGISTER_PROPERTY_GET(Transform*, m_oTransformComponent, Transform);
        REGISTER_PROPERTY_GET(BaseBoundBox*, m_oBoundBox, BoundBox);
        REGISTER_PROPERTY_GET(Animator*, m_oAnimator, Animator);
        REGISTER_PROPERTY_GET_SET(bool, m_bIsVisible, IsVisible);
        REGISTER_PROPERTY_GET_SET(bool, m_bIsAxisVisible, IsAxisVisible);
        REGISTER_PROPERTY_GET_SET(bool, m_bIsBoundBoxVisible, IsBoundBoxVisible)
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<BaseComponent*>, m_vComponents, Components)
		REGISTER_PROPERTY_GET_SET(Model*, m_oModel, Model)
		REGISTER_PROPERTY_GET_SET(BaseEntity*, m_oModelRootEntity, ModelRootEntity)
		REGISTER_PROPERTY_GET_SET(BaseRender*, m_oRenderer, Renderer)

	private:
		// 添加\移除组件时，标记或者去标记特殊的组件
		void markSpecialComponent(BaseComponent* component, bool bEmpty = false);
		// 遍历节点
		void traverseEntity(BaseEntity* entity, std::function<void(BaseEntity* entity)> callback);

		// 记录\移除特殊的组件  (注意！！！这里第一个参数使用的是指针的引用(T* &)，用来给成员变量赋值)
		// 如果参数不是引用的话会调用参数对象的拷贝构造函数，所以如果有需求想改变指针所指的对象（换句话说，就是要改变指针里面存的地址），就要使用指针引用
		// 如果这里使用T*，则这里的参数实际上只是原对象指针的一个拷贝，给他赋值并不能改变原对象的值
		template <typename T>
		void MARK_SPECIAL_COMPONENT(T* &markVar, BaseComponent* component, bool isEmpty)
		{
			if (isEmpty)
			{
				markVar = nullptr;
			}
			else
			{
				markVar = static_cast<T*>(component);
			}
		}

	private:
		// 组件队列
		// 注意：这里的组件列表是通过system添加过来的，system会检测组件是否互斥（一个entity是否已经拥有相同类型的组件）
		std::vector<BaseComponent*> m_vComponents;
		// Transform组件
		Transform* m_oTransformComponent;
		// 渲染组件(方便快速获取组件对象)
		BaseRender* m_oRenderer;
		// 网格过滤器组件
		MeshFilter* m_oMeshFilterComponent;
        // 包围盒
        BaseBoundBox* m_oBoundBox;
        // 动画播放器
        Animator* m_oAnimator;
		
		// 原始模型文件
		Model* m_oModel;
        // 模型根节点处的实体
        BaseEntity* m_oModelRootEntity;
        
        // 是否可见
        bool m_bIsVisible;
        // 是否显示坐标轴
        bool m_bIsAxisVisible;
        // 是否显示包围盒
        bool m_bIsBoundBoxVisible;
        
	};


}

