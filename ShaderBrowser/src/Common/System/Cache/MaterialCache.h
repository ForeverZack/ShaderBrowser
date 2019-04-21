#pragma once

#include "BaseCache.h"
#include "Browser/Components/Render/Material.h"

using namespace customGL;
using namespace browser;

namespace common
{
	class MaterialCache : public BaseMapCache<unsigned int, Material*, MaterialCache>
	{
	public:
		MaterialCache();
		~MaterialCache();
        
    public:
        // 初始化
        void init();
        // 添加
        void addSharedMaterial(Material* material);
        Material* addSharedMaterial(const std::string& materialName = browser::Material::DEFAULT_MATERIAL_NAME);
        // 移除
        void removeSharedMaterial(Material* material);
        void removeSharedMaterial(unsigned int materialId);
        // 获取
        Material* getSharedMaterial(const unsigned int materialId);
        
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

