#pragma once

#include <unordered_map>
#include "Common/Components/Reference.h"
#include "Common/CommonDefine.h"
#include "Common/Tools/Utils.h"

using namespace common;

namespace customGL
{
    class Model;
}

namespace browser
{
    class BaseComponentMessage : public Reference
    {
    public:
        BaseComponentMessage();
		virtual ~BaseComponentMessage();
    };
    
	class BaseEntity;
    class Mesh;
	class BaseComponent;
	class Transform;
    class MeshFilter;
    
    // 组件之间通讯的事件
    enum ComponentEvent
    {
        // 无
        None = 0,
        
        // Render组件的事件:
        // 添加Render组件
        Render_AddComponent,
		// 移除Render组件
		Render_RemoveComponent,
        
        // MeshFilter组件的事件:
        // 添加MeshFilter组件
        MeshFilter_AddComponent,
		// 移除MeshFilter组件
		MeshFilter_RemoveComponent,
        // 添加Mesh
        MeshFilter_AddMesh,

		// Camera组件的事件:
		// 添加Render组件
		Camera_AddComponent,

		// Transform组件的事件:
		// 添加Transform组件的事件
		Transform_AddComponent,
		// 移除Transform组件的事件
		Transform_RemoveComponent,

        // BoundBox组件的事件:
        // 添加BoundBox
        BoundBox_AddComponent,

        // Animator组件的时间:
        // 添加Animator
        Animator_AddComponent,
        // Animator更新骨骼节点数据
        Animator_UpdateBonesTransform,
    };


    
    // MeshFilter添加模型事件
    class MeshFilterAddMeshMessage : public BaseComponentMessage
    {
    public:
		MeshFilterAddMeshMessage(MeshFilter* meshFilter) : m_oMeshFilter(meshFilter) {}
		~MeshFilterAddMeshMessage() {}
		REGISTER_PROPERTY_GET(MeshFilter*, m_oMeshFilter, MeshFilter)
	protected:
        MeshFilter* m_oMeshFilter;
    };

	// Entity添加Render组件事件
	class RenderAddComponentMessage : public BaseComponentMessage
	{
	public:
		RenderAddComponentMessage(BaseEntity* entity) : m_oEntity(entity) {}
		~RenderAddComponentMessage() {}
		REGISTER_PROPERTY_GET(BaseEntity*, m_oEntity, Entity)
	protected:
		BaseEntity* m_oEntity;
	};

	// Entity添加MeshFilter组件事件
	class MeshFilterAddComponentMessage : public BaseComponentMessage
	{
	public:
		MeshFilterAddComponentMessage(MeshFilter* meshFilter) : m_oMeshFilter(meshFilter) {}
		~MeshFilterAddComponentMessage() {}
		REGISTER_PROPERTY_GET(MeshFilter*, m_oMeshFilter, MeshFilter)
	protected:
		MeshFilter* m_oMeshFilter;
	};

	// Entity添加Transform组件事件
	class TransformAddComponentMessage : public BaseComponentMessage
	{
	public:
		TransformAddComponentMessage(Transform* transform) : m_oTransform(transform) {}
		~TransformAddComponentMessage() {}
		REGISTER_PROPERTY_GET(Transform*, m_oTransform, Transform)
	protected:
		Transform* m_oTransform;
	};
    
	// Entity添加Camera组件事件
	class CameraAddComponentMessage : public BaseComponentMessage
	{
	public:
		CameraAddComponentMessage(Transform* transform) : m_oTransform(transform) {}
		~CameraAddComponentMessage() {}
		REGISTER_PROPERTY_GET(Transform*, m_oTransform, Transform)
	protected:
		Transform* m_oTransform;
	};
    
    // Entity添加BoundBox
    class BoundBoxAddComponentMessage : public BaseComponentMessage
    {
    public:
        BoundBoxAddComponentMessage(Transform* transform, MeshFilter* meshFilter) : m_oTransform(transform), m_oMeshFilter(meshFilter) {}
        ~BoundBoxAddComponentMessage() {}
        REGISTER_PROPERTY_GET(Transform*, m_oTransform, Transform)
        REGISTER_PROPERTY_GET(MeshFilter*, m_oMeshFilter, MeshFilter)
    protected:
        Transform* m_oTransform;
        MeshFilter* m_oMeshFilter;
    };
    
    // Entity添加Animator
    class AnimatorAddComponentMessage : public BaseComponentMessage
    {
    public:
        AnimatorAddComponentMessage(customGL::Model* model) : m_oModel(model) {}
        ~AnimatorAddComponentMessage() {}
        REGISTER_PROPERTY_GET(customGL::Model*, m_oModel, Model)
    protected:
        customGL::Model* m_oModel;
    };
    
    // Animator更新骨骼节点数据
    class AnimatorUpdateBonesTransformMessage : public BaseComponentMessage
    {
    public:
        AnimatorUpdateBonesTransformMessage(std::unordered_map<std::string, unsigned int>* bonesIdMap, std::unordered_map<unsigned int, glm::vec3>* bonesPosition, std::unordered_map<unsigned int, glm::quat>* bonesRotation, std::unordered_map<unsigned int, glm::vec3>* bonesScale)
            : m_mBonesIdMap(bonesIdMap)
            , m_mBonesPosition(bonesPosition)
            , m_mBonesRotation(bonesRotation)
            , m_mBonesScale(bonesScale)
        {
        }
        ~AnimatorUpdateBonesTransformMessage() {}
        std::unordered_map<std::string, unsigned int>* getBonesIdMap() { return m_mBonesIdMap; }
        std::unordered_map<unsigned int, glm::vec3>* getBonesPosition() { return m_mBonesPosition; }
        std::unordered_map<unsigned int, glm::quat>* getBonesRotation() { return m_mBonesRotation; }
        std::unordered_map<unsigned int, glm::vec3>* getBonesScale() { return m_mBonesScale; }
    protected:
        std::unordered_map<std::string, unsigned int>* m_mBonesIdMap;
        std::unordered_map<unsigned int, glm::vec3>* m_mBonesPosition;
        std::unordered_map<unsigned int, glm::quat>* m_mBonesRotation;
        std::unordered_map<unsigned int, glm::vec3>* m_mBonesScale;
    };
    
}

