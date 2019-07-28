#include "BaseEntity.h"
#include "Common/System/ECSManager.h"
#include "Browser/Components/BoundBox/AABBBoundBox.h"
#include "Browser/Components/Render/SkinnedMeshRenderer.h"
#include <deque>
#ifdef  _WIN32
#pragma warning(disable:4996)
#endif //  _WIN32

namespace browser
{
	BaseEntity* BaseEntity::create(const std::string& transformName/* = ""*/)
	{
        BaseEntity* entity = new BaseEntity();
        entity->autorelease();
        
		// 默认拥有Transform组件
        Transform* transform = new Transform();
		transform->setName(transformName);
        entity->addComponent(transform);
        
//        // TODO: 这里先将AABBBoundBox作为默认组件
//        AABBBoundBox* boundBox = new AABBBoundBox();
//        entity->addComponent(boundBox);

		return entity;
	}

	BaseEntity* BaseEntity::clone()
	{
		BaseEntity* entity = new BaseEntity();
		entity->autorelease();

		for (auto itor = m_vComponents.begin(); itor != m_vComponents.end(); ++itor)
		{
			addComponent(static_cast<BaseComponent*>((*itor)->clone()));
		}

		return entity;
	}

	BaseEntity::BaseEntity()
		: m_oTransformComponent(nullptr)
		, m_oRenderer(nullptr)
		, m_oMeshFilterComponent(nullptr)
        , m_oBoundBox(nullptr)
        , m_bIsVisible(true)
        , m_bIsAxisVisible(false)
        , m_bIsBoundBoxVisible(false)
        , m_oAnimator(nullptr)
        , m_oModel(nullptr)
        , m_oModelRootEntity(nullptr)
	{

	}

	BaseEntity::~BaseEntity()
	{
        BROWSER_LOG("~BaseEntity");

		// 移除所有children
		if (m_oTransformComponent)
		{
			const std::vector<Transform*>& children = m_oTransformComponent->getChildren();
			for (int i=0; i<children.size(); ++i)
			{
				children[i]->getBelongEntity()->release();
			}
		}

		// clear
		for (auto itor = m_vComponents.begin(); itor != m_vComponents.end(); ++itor)
		{
			// 1.将Entity从system中移除
			ECSManager::getInstance()->removeEntityFromSystem(this, (*itor)->getBelongSystem());
			// 2.移除Component
			(*itor)->release();
		}

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

	void BaseEntity::setPosition(float x, float y, float z)
	{
		BROWSER_ASSERT(m_oTransformComponent, "Cannot find Transform component on parent entity in function BaseEntity::setPosition");
		m_oTransformComponent->setPosition(x, y, z);
	}

	void BaseEntity::setEulerAngle(float x, float y, float z)
	{
		BROWSER_ASSERT(m_oTransformComponent, "Cannot find Transform component on parent entity in function BaseEntity::setEulerAngle");
		m_oTransformComponent->setEulerAngle(x, y, z);
	}

    void BaseEntity::setQuaternion(float x, float y, float z, float w)
    {
        BROWSER_ASSERT(m_oTransformComponent, "Cannot find Transform component on parent entity in function BaseEntity::setQuaternion");
        m_oTransformComponent->setQuaternion(x, y, z, w);
    }
    
	void BaseEntity::setScale(float x, float y, float z)
	{
		BROWSER_ASSERT(m_oTransformComponent, "Cannot find Transform component on parent entity in function BaseEntity::setScale");
		m_oTransformComponent->setScale(x, y, z);
	}

	void BaseEntity::setName(const std::string& name)
	{
		BROWSER_ASSERT(m_oTransformComponent, "Cannot find Transform component on parent entity in function BaseEntity::setName");
		m_oTransformComponent->setName(name);
	}
    
    void BaseEntity::setModelRootEntity(BaseEntity* root)
    {
        m_oModelRootEntity = root;
        m_oTransformComponent->setBoneRoot(root->getTransform());
    }
    
    void BaseEntity::setBoneInfo(int boneId, const glm::mat4& bindpose)
    {
        m_oTransformComponent->setBoneId(boneId);
        m_oTransformComponent->setBindposeMatrix(bindpose);
    }
    
	bool BaseEntity::isRenderable()
	{
        // 是否需要被渲染
        if (m_oRenderer)
        {
            switch(m_oRenderer->getRendererType())
            {
                case BaseRender::RendererType::Base:
                    {
                        if(m_oMeshFilterComponent)
                        {
                            return true;
                        }
                    }
                    break;
                    
                case BaseRender::RendererType::Skinned:
                    {
                        return true;
                    }
                    break;
            }
        }
        
        return false;
	}
    
    bool BaseEntity::checkVisibility(Camera* camera, bool reCalculate/* = false*/)
    {
        return (
                // 普通网格检测
                (m_oBoundBox && m_oBoundBox->checkVisibility(camera, reCalculate))
                // 蒙皮网格检测
                || (m_oRenderer && m_oRenderer->getRendererType()==BaseRender::RendererType::Skinned ? static_cast<SkinnedMeshRenderer*>(m_oRenderer)->checkVisibility(camera, reCalculate) : false)
        );
    }
    
    glm::vec4* BaseEntity::getVertices(Mesh* mesh, bool& dirty)
    {
        // 检测模型根节点是否含有动画组件
        if(m_oModelRootEntity && m_oModelRootEntity->m_oAnimator && m_oModelRootEntity->m_oAnimator->getIsPlaying() && !m_oModelRootEntity->m_oAnimator->getUseGPU())
        {
			dirty = true;
            const std::unordered_map<Mesh*, glm::vec4*>& vertices = m_oModelRootEntity->m_oAnimator->getVertices();
            return vertices.find(mesh)->second;
        }
        else
        {
			dirty = false;
            return mesh->getVertices22();
        }
    }

	void BaseEntity::useBonesMatrix(Material* material)
	{
		if (!m_oModelRootEntity || !m_oModelRootEntity->m_oAnimator)
		{
			return;
		}
		char uniformName[50];
        const std::vector<glm::mat4>& bonesMatrix = m_oModelRootEntity->m_oAnimator->getBonesMatrix();
		for (unsigned int i=0; i<bonesMatrix.size(); ++i)
		{
			sprintf(uniformName, GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_BONES_MATRIX], i);
            const glm::mat4& boneMatrix = bonesMatrix[i];
//            BROWSER_LOG_MAT4(bonesMatrix[i]);
            
            // 方式1:
            material->setUniformMat3x4(uniformName, glm::mat3x4(boneMatrix[0][0], boneMatrix[1][0], boneMatrix[2][0], boneMatrix[3][0],
                                                            boneMatrix[0][1], boneMatrix[1][1], boneMatrix[2][1], boneMatrix[3][1],
                                                            boneMatrix[0][2], boneMatrix[1][2], boneMatrix[2][2], boneMatrix[3][2]));
            // 方式2:
//            pass->setUniformMat3x4(uniformName, static_cast<glm::mat3x4>(glm::transpose(boneMatrix)));
		}
	}
    
    void BaseEntity::playAnimation(const std::string& animName, bool repeat/* = false*/, float speed/* = 1.0f*/, bool interpolate/* = true*/)
    {
        // 只有模型的根节点可以播放动画，因为只有根节点可以拿到完整的模型数据
        BROWSER_ASSERT(m_oModelRootEntity, "Entity must set the model root entity, after that it can play animation!");
        BROWSER_ASSERT(this==m_oModelRootEntity, "You cannot play animation on a child entity, please select the root model entity! Block in function BaseEntity::playAnimation");
        
        // 播放动画
        m_oAnimator->play(animName, repeat, speed, interpolate);
    }
    
    void BaseEntity::playAnimation(int animIdx, bool repeat /*= false*/, float speed /*= 1.0f*/, bool interpolate /*= true*/)
    {
        // 只有模型的根节点可以播放动画，因为只有根节点可以拿到完整的模型数据
        BROWSER_ASSERT(m_oModelRootEntity, "Entity must set the model root entity, after that it can play animation!");
        BROWSER_ASSERT(this==m_oModelRootEntity, "You cannot play animation on a child entity, please select the root model entity! Block in function BaseEntity::playAnimation");
        
        // 播放动画
        m_oAnimator->play(animIdx, repeat, speed, interpolate);
    }

	void BaseEntity::setAnimatorUseGPU(bool useGPU)
	{
		BROWSER_ASSERT(m_oModelRootEntity, "Entity must set the model root entity, after that it can play animation!");
		BROWSER_ASSERT(this == m_oModelRootEntity, "You cannot play animation on a child entity, please select the root model entity! Block in function BaseEntity::playAnimation");

		m_oAnimator->setUseGPU(useGPU);
	}

	void BaseEntity::changeAllMeshesMaterial(const std::string& programName)
	{
		traverseEntity(this, [=](BaseEntity* entity) {
			BaseRender* renderer = entity->getRenderer();
			MeshFilter* meshFilter = entity->getMeshFilter();
			if (renderer && meshFilter)
			{
				const std::vector<Mesh*>& meshes = meshFilter->getMeshes();
				for (int i=0; i<meshes.size(); ++i)
				{
					renderer->changeMaterial(i, meshes[i]->getMaterialName(), programName);
				}
			}
		});
	}

	void BaseEntity::traverseEntity(BaseEntity* entity, std::function<void(BaseEntity* entity)> callback)
	{
		callback(entity);

		if (entity->m_oTransformComponent)
		{
			const std::vector<Transform*>& children = entity->m_oTransformComponent->getChildren();
			for (auto itor = children.begin(); itor != children.end(); ++itor)
			{
				traverseEntity((*itor)->getBelongEntity(), callback);
			}
		}
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
        for (int i=0; i<m_vComponents.size(); ++i)
        {
            m_vComponents[i]->handleEvent(event, msg);
        }
    }
    
    void BaseEntity::deliverComponentMessageToChildren(ComponentEvent event, BaseComponentMessage* msg)
    {
        std::deque<Transform*> traverseVec;
        if (m_oTransformComponent)
        {
            traverseVec.push_back(m_oTransformComponent);
        }
        
        Transform* node = nullptr;
        BaseEntity* entity = nullptr;
        while(!traverseVec.empty())
        {
            node = traverseVec.front();
            traverseVec.pop_front();
            
            // 派发事件
            {
                entity = node->getBelongEntity();
                const std::vector<BaseComponent*>& components = entity->getComponents();
                for(auto itor=components.begin(); itor!=components.end(); ++itor)
                {
                    (*itor)->handleEvent(event, msg);
                }
            }
            
            // 遍历子节点
            const std::vector<Transform*>& children = node->getChildren();
            for(int i=0; i<children.size(); ++i)
            {
                traverseVec.push_back(children[i]);
            }
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
                
        case SystemType::MeshFilter:
            //MeshFilter组件
			MARK_SPECIAL_COMPONENT(m_oMeshFilterComponent, component, bEmpty);
            deliverComponentMessage(ComponentEvent::MeshFilter_AddComponent, new MeshFilterAddComponentMessage(m_oMeshFilterComponent));
            break;

		case SystemType::RenderSystem:
			// 渲染组件
			MARK_SPECIAL_COMPONENT(m_oRenderer, component, bEmpty);
            deliverComponentMessage(ComponentEvent::Render_AddComponent, new RenderAddComponentMessage(this));
			break;

		case SystemType::Camera:
			// 相机
			deliverComponentMessage(ComponentEvent::Camera_AddComponent, new CameraAddComponentMessage(m_oTransformComponent));
			break;
                
        case SystemType::BoundBox:
            // 包围盒
            MARK_SPECIAL_COMPONENT(m_oBoundBox, component, bEmpty);
            deliverComponentMessage(ComponentEvent::BoundBox_AddComponent, new BoundBoxAddComponentMessage(m_oTransformComponent, m_oMeshFilterComponent));
            break;
                
        case SystemType::Animation:
            // 包围盒
            MARK_SPECIAL_COMPONENT(m_oAnimator, component, bEmpty);
            deliverComponentMessage(ComponentEvent::Animator_AddComponent, new AnimatorAddComponentMessage(m_oModel));
            break;

		}
	}





}
