#include "BaseEntity.h"
#include "Common/System/ECSManager.h"
#include "Browser/Components/BoundBox/AABBBoundBox.h"
#include "Browser/Components/Render/SkinnedMeshRenderer.h"
#include "GL/GPUResource/Texture/TextureBuffer.h"
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
		: m_oTransform(nullptr)
		, m_oRenderer(nullptr)
		, m_oMeshFilter(nullptr)
        , m_oBoundBox(nullptr)
        , m_bIsVisible(true)
        , m_bIsAxisVisible(false)
        , m_bIsBoundBoxVisible(false)
        , m_oAnimator(nullptr)
        , m_oCamera(nullptr)
        , m_oModel(nullptr)
        , m_oModelRootEntity(nullptr)
		, m_bIsActive(true)
	{

	}

	BaseEntity::~BaseEntity()
	{
        BROWSER_LOG("~BaseEntity");

		// 移除所有children
		if (m_oTransform)
		{
			const std::vector<Transform*>& children = getComponent<Transform>()->getChildren();
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
		BROWSER_ASSERT(m_oTransform, "Cannot find Transform component on parent entity in function BaseEntity::addChild(BaseEntity* entity)");

		Transform* transform = entity->getComponent<Transform>();
		BROWSER_ASSERT(transform, "Cannot find Transform component on child entity in function BaseEntity::addChild(BaseEntity* entity)");
		getComponent<Transform>()->addChild(transform);
		entity->retain();
	}

	void BaseEntity::removeChild(BaseEntity* entity)
	{
		BROWSER_ASSERT(m_oTransform, "Cannot find Transform component on parent entity in function BaseEntity::removeChild(BaseEntity* entity)");

		Transform* transform = entity->getComponent<Transform>();
		BROWSER_ASSERT(transform, "Cannot find Transform component on child entity in function BaseEntity::addChild(BaseEntity* entity)");
		getComponent<Transform>()->removeChild(transform);
		entity->release();
	}

	void BaseEntity::setPosition(float x, float y, float z)
	{
		BROWSER_ASSERT(m_oTransform, "Cannot find Transform component on parent entity in function BaseEntity::setPosition");
		getComponent<Transform>()->setPosition(x, y, z);
	}

	void BaseEntity::setEulerAngle(float x, float y, float z)
	{
		BROWSER_ASSERT(m_oTransform, "Cannot find Transform component on parent entity in function BaseEntity::setEulerAngle");
		getComponent<Transform>()->setEulerAngle(x, y, z);
	}

    void BaseEntity::setQuaternion(float x, float y, float z, float w)
    {
        BROWSER_ASSERT(m_oTransform, "Cannot find Transform component on parent entity in function BaseEntity::setQuaternion");
		getComponent<Transform>()->setQuaternion(x, y, z, w);
    }
    
	void BaseEntity::setScale(float x, float y, float z)
	{
		BROWSER_ASSERT(m_oTransform, "Cannot find Transform component on parent entity in function BaseEntity::setScale");
		getComponent<Transform>()->setScale(x, y, z);
	}

	void BaseEntity::setName(const std::string& name)
	{
		BROWSER_ASSERT(m_oTransform, "Cannot find Transform component on parent entity in function BaseEntity::setName");
		getComponent<Transform>()->setName(name);
	}
    
    void BaseEntity::setModelRootEntity(BaseEntity* root)
    {
        m_oModelRootEntity = root;
		getComponent<Transform>()->setBoneRoot(root->getComponent<Transform>());
    }
    
    void BaseEntity::setBoneInfo(int boneId, const glm::mat4& bindpose)
    {
		getComponent<Transform>()->setBoneId(boneId);
		getComponent<Transform>()->setBindposeMatrix(bindpose);
    }
    
	bool BaseEntity::isRenderable()
	{
        // 是否需要被渲染
        if (m_oRenderer)
        {
            switch(static_cast<BaseRender*>(m_oRenderer)->getRendererType())
            {
                case BaseRender::RendererType::Base:
                    {
                        if(m_oMeshFilter)
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
		SkinnedMeshRenderer* skinnedMeshRenderer = getComponent<SkinnedMeshRenderer>();
        return (
                // 普通网格检测
                (m_oBoundBox && getComponent<AABBBoundBox>()->checkVisibility(camera, reCalculate))
                // 蒙皮网格检测
                || (skinnedMeshRenderer && skinnedMeshRenderer->checkVisibility(camera, reCalculate))
        );
    }
    
    glm::vec4* BaseEntity::getVertices(Mesh* mesh, bool& dirty)
    {
        // 检测模型根节点是否含有动画组件
        if(m_oModelRootEntity && m_oModelRootEntity->m_oAnimator && m_oModelRootEntity->getComponent<Animator>()->getIsPlaying() && !m_oModelRootEntity->getComponent<Animator>()->getUseGPU())
        {
			dirty = true;
            const std::unordered_map<Mesh*, glm::vec4*>& vertices = m_oModelRootEntity->getComponent<Animator>()->getVertices();
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
        
        // 将骨骼矩阵存入tbo，并传给材质
        TextureBuffer* textureBuffer = m_oModelRootEntity->getComponent<Animator>()->useBonesMatrix();
        material->setUniformSamplerBuffer(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_BONES_MATRIX], textureBuffer);
    }
    
    void BaseEntity::playAnimation(const std::string& animName, bool repeat/* = false*/, float speed/* = 1.0f*/, bool interpolate/* = true*/)
    {
        // 只有模型的根节点可以播放动画，因为只有根节点可以拿到完整的模型数据
        BROWSER_ASSERT(m_oModelRootEntity, "Entity must set the model root entity, after that it can play animation!");
        BROWSER_ASSERT(this==m_oModelRootEntity, "You cannot play animation on a child entity, please select the root model entity! Block in function BaseEntity::playAnimation");
        
        // 播放动画
		getComponent<Animator>()->play(animName, repeat, speed, interpolate);
    }
    
    void BaseEntity::playAnimation(int animIdx, bool repeat /*= false*/, float speed /*= 1.0f*/, bool interpolate /*= false*/)
    {
        // 只有模型的根节点可以播放动画，因为只有根节点可以拿到完整的模型数据
        BROWSER_ASSERT(m_oModelRootEntity, "Entity must set the model root entity, after that it can play animation!");
        BROWSER_ASSERT(this==m_oModelRootEntity, "You cannot play animation on a child entity, please select the root model entity! Block in function BaseEntity::playAnimation");
        
        // 播放动画
		getComponent<Animator>()->play(animIdx, repeat, speed, interpolate);
    }

	void BaseEntity::setAnimatorUseGPU(bool useGPU)
	{
		BROWSER_ASSERT(m_oModelRootEntity, "Entity must set the model root entity, after that it can play animation!");
		BROWSER_ASSERT(this == m_oModelRootEntity, "You cannot play animation on a child entity, please select the root model entity! Block in function BaseEntity::playAnimation");

		getComponent<Animator>()->setUseGPU(useGPU);
	}

	void BaseEntity::changeAllMeshesMaterial(const std::string& programName)
	{
		traverseEntity(this, [=](BaseEntity* entity) {
			BaseRender* renderer = entity->getComponent<BaseRender>();
			MeshFilter* meshFilter = entity->getComponent<MeshFilter>();
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

		if (entity->m_oTransform)
		{
			const std::vector<Transform*>& children = entity->getComponent<Transform>()->getChildren();
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
        if (m_oTransform)
        {
            traverseVec.push_back(getComponent<Transform>());
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
			MARK_SPECIAL_COMPONENT(m_oTransform, component, bEmpty);
			deliverComponentMessage(ComponentEvent::Transform_AddComponent, new TransformAddComponentMessage(getComponent<Transform>()));
			break;
                
        case SystemType::MeshFilter:
            //MeshFilter组件
			MARK_SPECIAL_COMPONENT(m_oMeshFilter, component, bEmpty);
            deliverComponentMessage(ComponentEvent::MeshFilter_AddComponent, new MeshFilterAddComponentMessage(getComponent<MeshFilter>()));
            break;

		case SystemType::RenderSystem:
			// 渲染组件
			MARK_SPECIAL_COMPONENT(m_oRenderer, component, bEmpty);
            deliverComponentMessage(ComponentEvent::Render_AddComponent, new RenderAddComponentMessage(static_cast<BaseRender*>(m_oRenderer), this));
			break;

		case SystemType::Camera:
			// 相机
			MARK_SPECIAL_COMPONENT(m_oCamera, component, bEmpty);
            deliverComponentMessage(ComponentEvent::Camera_AddComponent, new CameraAddComponentMessage(getComponent<Camera>(), getComponent<Transform>()));
			break;
                
        case SystemType::BoundBox:
            {
                // 包围盒
                MARK_SPECIAL_COMPONENT(m_oBoundBox, component, bEmpty);
                Animator* rootAnimator = nullptr;
                if (m_oModelRootEntity && m_oModelRootEntity->getComponent<Animator>())
                {
                    rootAnimator = m_oModelRootEntity->getComponent<Animator>();
                }
				deliverComponentMessage(ComponentEvent::BoundBox_AddComponent, new BoundBoxAddComponentMessage(getComponent<AABBBoundBox>(), getComponent<Transform>(), getComponent<MeshFilter>(), rootAnimator));
			}
			break;

        case SystemType::Animation:
			// 动画
			MARK_SPECIAL_COMPONENT(m_oAnimator, component, bEmpty);
			deliverComponentMessage(ComponentEvent::Animator_AddComponent, new AnimatorAddComponentMessage(getComponent<Animator>(), m_oModel));
			deliverComponentMessageToChildren(ComponentEvent::Animator_ParentAddComponent, new AnimatorAddComponentMessage(getComponent<Animator>(), m_oModel));
            break;

		}

	}





}
