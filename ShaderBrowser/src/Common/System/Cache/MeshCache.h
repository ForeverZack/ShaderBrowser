#pragma once

#include "BaseCache.h"
#include "GL/GPUResource/Model/Mesh.h"

using namespace customGL;
using namespace browser;

namespace common
{
	class MeshCache : public BaseMapCache<unsigned int, Mesh*, MeshCache>
	{
	public:
		MeshCache();
		~MeshCache();
        
    public:
        // 添加
        void addSharedMesh(Mesh* mesh);
        // 移除
        void removeSharedMesh(Mesh* mesh);
        void removeSharedMesh(unsigned int meshId);
        // 获取
		Mesh* getSharedMesh(const unsigned int meshId);
        
    private:
        unsigned int generateId()
        {
            return ++m_uIdGenerator;
        }

    private:
        // id生成器
        unsigned int m_uIdGenerator;
        
	};
}

