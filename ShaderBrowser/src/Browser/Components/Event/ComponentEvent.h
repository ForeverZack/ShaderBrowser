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
        virtual ~BaseComponentMessage() {}
    };
    
    class Mesh;
    
    // 组件之间通讯的事件
    enum ComponentEvent
    {
        // 无
        None = 0,
        
        // Render组件的事件:
        // 添加Render组件
        Render_AddComponent,
        
        // MeshFilter组件的事件:
        // 添加MeshFilter组件
        MeshFilter_AddComponent,
        // 添加Mesh
        MeshFilter_AddMesh,
    };


    
    // MeshFilter添加模型事件
    class MeshFilterAddMeshMessage : public BaseComponentMessage
    {
    public:
        MeshFilterAddMeshMessage(Mesh* mesh);
        ~MeshFilterAddMeshMessage();
    
    public:
        Mesh* m_oMesh;
    };
    
    
}

