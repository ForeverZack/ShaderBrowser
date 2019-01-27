#include "ComponentEvent.h"

namespace browser
{
    // BaseComponentMessage
    BaseComponentMessage::BaseComponentMessage()
    {
        this->autorelease();
    }
    
    // MeshFilterAddMeshMessage
    MeshFilterAddMeshMessage::MeshFilterAddMeshMessage(Mesh* mesh)
    {
        m_oMesh = mesh;
    }
    MeshFilterAddMeshMessage::~MeshFilterAddMeshMessage()
    {
        // 这里提供给事件的只是一个只读的mesh指针，mesh实际仍是由MeshFilter来管理的，所以不需要释放该指针
    }

}
