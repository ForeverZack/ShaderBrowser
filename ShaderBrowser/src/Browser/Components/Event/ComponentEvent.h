#pragma once

#include "../../../Common/Components/Reference.h"
#include "../../../Common/CommonDefine.h"
#include "../../../Common/Tools/Utils.h"

using namespace common;

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
    };


    
    // MeshFilter添加模型事件
    class MeshFilterAddMeshMessage : public BaseComponentMessage
    {
    public:
        MeshFilterAddMeshMessage(Mesh* mesh);
        ~MeshFilterAddMeshMessage();
		REGISTER_PROPERTY_GET(Mesh*, m_oMesh, Mesh)
	protected:
        Mesh* m_oMesh;
    };

	// Entity添加Render组件事件
	class RenderAddComponentMessage : public BaseComponentMessage
	{
	public:
		RenderAddComponentMessage(BaseEntity* entity);
		~RenderAddComponentMessage() {};
		REGISTER_PROPERTY_GET(BaseEntity*, m_oEntity, Entity)
	protected:
		BaseEntity* m_oEntity;
	};

	// Entity添加MeshFilter组件事件
	class MeshFilterAddComponentMessage : public BaseComponentMessage
	{
	public:
		MeshFilterAddComponentMessage(BaseEntity* entity);
		~MeshFilterAddComponentMessage() {};
		REGISTER_PROPERTY_GET(BaseEntity*, m_oEntity, Entity)
	protected:
		BaseEntity* m_oEntity;
	};


    
    
}

