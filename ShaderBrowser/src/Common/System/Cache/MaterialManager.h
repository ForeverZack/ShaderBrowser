#pragma once

#include "BaseCache.h"
#include "BaseAsyncLoader.h"
#include "GL/GPUResource/Shader/Material.h"
#include "GL/GPUResource/Shader/MaterialParser.h"

using namespace customGL;
using namespace browser;

namespace common
{
	// MaterialManager中存储所有的材质，包括没有材质文件的材质
	class MaterialManager : public BaseMapCache<unsigned int, Material*, MaterialManager>
	{
	public:
		MaterialManager();
		~MaterialManager();
        
    public:
        // 初始化
        void init();
        // 添加（Material构造函数中调用）
        void addMaterial(Material* material, bool isShared = false, bool isDefault = false);
        // 移除
        void removeMaterial(Material* material);
        void removeMaterial(unsigned int materialId);
        // 获取
        Material* getMaterial(const unsigned int materialId);


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

