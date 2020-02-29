#pragma once

#include "BaseCache.h"
#include "GL/GPUResource/Shader/Material.h"

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
        void addMaterial(Material* material, bool isShared = false, bool isDefault = false);
        // 移除
        void removeMaterial(Material* material);
        void removeMaterial(unsigned int materialId);
        // 获取
        Material* getMaterial(const unsigned int materialId);
		// 遍历操作所有Material
		void operateAllMaterials(std::function<void(Material*)> callback);
		// 获取所有Matieral
		std::unordered_map<unsigned int, Material*>& getAllMaterials()
		{
			return m_mContents;
		}

		void update(float dt);

    private:
		// 生成materialId
        unsigned int generateId()
        {
            return ++m_uIdGenerator;
        }

    private:
        // id生成器
        unsigned int m_uIdGenerator;

        
	};
}

