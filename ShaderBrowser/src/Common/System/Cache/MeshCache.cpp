#include "MeshCache.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"

namespace common
{    

	MeshCache::MeshCache()
        : m_uIdGenerator(0)
    {

    }
    
	MeshCache::~MeshCache()
    {
        
    }
    
    void MeshCache::addSharedMesh(Mesh* mesh)
    {
        BROWSER_ASSERT(mesh, "Mesh is not invalid in function MeshCache::addSharedMesh(Mesh* mesh)");
        BROWSER_WARNING(mesh->getMeshId()==0, "Mesh has been added, please check your program in function MeshCache::addSharedMesh(Mesh* mesh)");

		mesh->setMeshId(generateId());
		mesh->retain();
        add(mesh->getMeshId(), mesh);
    }
    
    void MeshCache::removeSharedMesh(Mesh* mesh)
    {
		mesh->release();
        remove(mesh->getMeshId());
    }
    
    void MeshCache::removeSharedMesh(unsigned int meshId)
    {
        Mesh* mesh = get(meshId);
        BROWSER_ASSERT(mesh, "Mesh is not invalid in function MeshCache::removeSharedMesh");
		mesh->release();
        remove(meshId);
    }
    
    Mesh* MeshCache::getSharedMesh(const unsigned int meshId)
    {
		Mesh* mesh = get(meshId);
        BROWSER_ASSERT(mesh, "Mesh is not invalid in function MeshCache::getSharedMesh");
        return mesh;
    }
    
    
}
