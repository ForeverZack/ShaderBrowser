#include "BaseEntity.h"
#include "../../Common/System/ECSManager.h"

namespace browser
{
	BaseEntity* BaseEntity::create(std::string transformName/* = ""*/)
	{
		Transform* transform = new Transform();
		transform->setName(transformName);

		BaseEntity* entity = new BaseEntity();
		entity->addComponent(transform);
		entity->autorelease();

		return entity;
	}

	BaseEntity::BaseEntity()
		: m_oTransformComponent(nullptr)
		, m_oRenderComponent(nullptr)
		, m_oMeshFilterComponent(nullptr)
	{

	}

	BaseEntity::~BaseEntity()
	{
        BROWSER_LOG("~BaseEntity");
	}

	void BaseEntity::addChild(BaseEntity* entity)
	{
		BROWSER_ASSERT(m_oTransformComponent, "Cannot find Transform component on parent entity in function BaseEntity::addChild(BaseEntity* entity)");

		Transform* transform = entity->getTransform();
		BROWSER_ASSERT(transform, "Cannot find Transform component on child entity in function BaseEntity::addChild(BaseEntity* entity)");
		m_oTransformComponent->addChild(transform);
		entity->retain();
	}

	void BaseEntity::removeChild(BaseEntity* entity)
	{
		BROWSER_ASSERT(m_oTransformComponent, "Cannot find Transform component on parent entity in function BaseEntity::removeChild(BaseEntity* entity)");

		Transform* transform = entity->getTransform();
		BROWSER_ASSERT(transform, "Cannot find Transform component on child entity in function BaseEntity::addChild(BaseEntity* entity)");
		m_oTransformComponent->removeChild(transform);
		entity->release();
	}

	bool BaseEntity::isRenderable()
	{
		// TODO: 目前MeshFilter还没有写，所以暂时只用renderCom来判断
		return m_oRenderComponent;
		//return m_oRenderComponent && m_oMeshFilterComponent;
	}

	void BaseEntity::addComponent(BaseComponent* component)
	{
		BROWSER_ASSERT(!component->getBelongEntity(), "BaseEntity::addComponent() : Component has already benn added on entity!");

		// 将组件加入对应的系统
		if (ECSManager::getInstance()->addComponentToSystem(this, component))
		{
			// 添加入自身组件队列
			m_vComponents.push_back(component);

			// 引用计数+1
			component->retain();

			// 记录特殊组件
			markSpecialComponent(component);
		}
	}

	void BaseEntity::removeComponent(BaseComponent* component)
	{
		BROWSER_ASSERT(component->getBelongEntity()==this, "BaseEntity::removeComponent() : Component do not belong to this entity, you can't remove it!");

		// 将组件从对应的系统中移除
		if (ECSManager::getInstance()->removeComponentFromSystem(this, component))
		{
			// 清除特殊组件
			markSpecialComponent(component, true);

			// 从自身组件队列中移除
			auto itor = find(m_vComponents.begin(), m_vComponents.end(), component);
			m_vComponents.erase(itor);

			// 引用计数-1
			component->release();
		}
		
	}

	void BaseEntity::removeComponents(SystemType type)
	{
		for (auto itor = m_vComponents.begin(); itor != m_vComponents.end(); ++itor)
		{
			if ((*itor)->getBelongSystem() == type)
			{
				removeComponent(*itor);
			}
		}
	}
    
    void BaseEntity::deliverComponentMessage(ComponentEvent event, BaseComponentMessage* msg)
    {
        BaseComponent* component;
        for (int i=0; i<m_vComponents.size(); ++i)
        {
            component = m_vComponents[i];
            component->handleEvent(event, msg);
        }
    }

	void BaseEntity::markSpecialComponent(BaseComponent* component, bool bEmpty/* = false*/)
	{
		// 仅记录需要的特殊组件
		switch (component->getBelongSystem())
		{
		case SystemType::Transform:
			//Transform组件
			MARK_SPECIAL_COMPONENT(m_oTransformComponent, component, bEmpty);
			deliverComponentMessage(ComponentEvent::Transform_AddComponent, new TransformAddComponentMessage(m_oTransformComponent));
			break;
                
        case SystemType::Mesh:
            //MeshFilter组件
			MARK_SPECIAL_COMPONENT(m_oMeshFilterComponent, component, bEmpty);
            deliverComponentMessage(ComponentEvent::MeshFilter_AddComponent, new MeshFilterAddComponentMessage(this));
            break;

		case SystemType::RenderSystem:
			// 渲染组件
			MARK_SPECIAL_COMPONENT(m_oRenderComponent, component, bEmpty);
            deliverComponentMessage(ComponentEvent::Render_AddComponent, new RenderAddComponentMessage(this));
			break;

		case SystemType::Camera:
			// 相机
			deliverComponentMessage(ComponentEvent::Camera_AddComponent, new CameraAddComponentMessage(m_oTransformComponent));
			break;

		}
	}





}
