#include "MeshFilter.h"
#include "GL/GLProgram.h"
#include "Browser/System/RenderSystem.h"

using namespace customGL;

namespace browser
{
    MeshFilter* MeshFilter::create()
    {
        MeshFilter* meshFilter = new MeshFilter();
        
        return meshFilter;
    }
    
	MeshFilter::MeshFilter()
        : BaseComponent("Mesh Filter")
	{
        // 组件所属系统
        m_eBelongSystem = SystemType::Mesh;
     
		// 清理
		m_vMeshes.clear();
	}

	MeshFilter::~MeshFilter()
	{
        for(auto itor=m_vMeshes.begin(); itor!=m_vMeshes.end(); ++itor)
        {
            (*itor)->release();
        }
        m_vMeshes.clear();
	}
    
    Reference* MeshFilter::clone()
    {
        MeshFilter* meshFilter = new MeshFilter();
        meshFilter->m_vMeshes = m_vMeshes;
        
        return meshFilter;
    }

	void MeshFilter::onInspectorGUI(InspectorPanel* inspector)
	{
		// 模型列表（临时）
		for (auto itor = m_vMeshes.begin(); itor != m_vMeshes.end(); ++itor)
		{
			inspector->addPropertyText("Mesh: " + (*itor)->getMeshName() + " , Mat: " + (*itor)->getMaterialName());
		}
	}
    
	void MeshFilter::addMesh(Mesh* mesh)
	{
		m_vMeshes.push_back(mesh);
        mesh->retain();
	}



}
