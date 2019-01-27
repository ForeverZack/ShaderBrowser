#include "MeshFilter.h"
#include "./../../../GL/GLProgram.h"
#include "./../../System/RenderSystem.h"

using namespace customGL;

namespace browser
{
    MeshFilter* MeshFilter::create()
    {
        MeshFilter* meshFilter = new MeshFilter();
        
        return meshFilter;
    }
    
	MeshFilter::MeshFilter()
        //: 
	{
        // 组件所属系统
        m_eBelongSystem = SystemType::Mesh;
     
		// 清理
		m_vMeshes.clear();
	}

	MeshFilter::~MeshFilter()
	{
	}
    
	void MeshFilter::addMesh(Mesh* mesh)
	{
		m_vMeshes.push_back(mesh);
	}



}
