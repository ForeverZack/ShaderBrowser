#pragma once

#include "BaseCache.h"
#include "BaseAsyncLoader.h"
#include "GL/GPUResource/Shader/Material.h"
#include "GL/GPUResource/Shader/MaterialParser.h"

using namespace customGL;
using namespace browser;

namespace common
{
	class MaterialCache : public BaseMapCache<unsigned int, Material*, MaterialCache>, public BaseAsyncLoader<MaterialParameters, std::function<void(Material*)>>
	{
        // MaterialCache异步加载最大允许线程数
        #define MAX_MATERIALCACHE_THREAD_COUNT 2
        
	public:
		MaterialCache();
		~MaterialCache();
        
    public:
        // 初始化
        void init();
        // 加载材质 (靠材质文件添加)
        void addMaterial(const std::string& filepath);
        
        
        // 添加（Material构造函数中调用）
        void addMaterial(Material* material, bool isShared = false, bool isDefault = false);
        // 移除
        void removeMaterial(Material* material);
        void removeMaterial(unsigned int materialId);
        // 获取
        Material* getMaterial(const unsigned int materialId);


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

