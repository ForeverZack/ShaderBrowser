#pragma once

#include "BaseCache.h"
#include "BaseAsyncLoader.h"
#include "GL/GPUResource/Shader/Material.h"
#include "GL/GPUResource/Shader/MaterialParser.h"

using namespace customGL;
using namespace browser;

namespace common
{
	// MaterialCache只存储含有材质文件的材质
	class MaterialCache : public BaseMapCache<std::string, Material*, MaterialCache>, public BaseAsyncLoader<MaterialParameters, std::function<void(Material*)>>
	{
        // MaterialCache异步加载最大允许线程数
        #define MAX_MATERIALCACHE_THREAD_COUNT 2
        
	public:
		MaterialCache();
		~MaterialCache();
        
    public:
        // 初始化
        void init();
        // 加载材质 (读取材质文件)
        void addMaterial(const std::string& filepath);
        // 加载材质异步 (读取材质文件)
		void addMaterialAsync(const std::string& filepath, std::function<void(Material*)> callback);
        // 移除
        void removeMaterial(Material* material);
        void removeMaterial(const std::string& filepath);
        // 获取
        Material* getMaterial(const std::string& filepath);

		// 刷新
		void update(float dt);

	private:
		// 异步创建材质
		void createMaterial(AsyncData<MaterialParameters, std::function<void(Material*)>>* asyncData);

	};
}

