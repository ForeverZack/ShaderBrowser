#pragma once

#include "BaseCache.h"
#include "BaseAsyncLoader.h"
#include "GL/GPUResource/Model/Model.h"
#include <vector>


using namespace customGL;

namespace common
{
    class ModelCache : public BaseMapCache<std::string, Model*, ModelCache>, public BaseAsyncLoader<Model, std::function<void(Model*)>, std::vector<std::string>>
	{
        // ModelCache异步加载最大允许线程数
        #define MAX_MODELCACHE_THREAD_COUNT 5
            
	public:
		ModelCache();
		~ModelCache();
        
    public:
        // 添加模型
        void addModel(const std::string& filepath, const std::vector<std::string>& animFilePaths, std::function<void(Model*)> callback);
		// 异步添加单个纹理
		void addModelAsync(const std::string& filepath, std::vector<std::string> animFilePaths, std::function<void(Model*)> callback);
		// 异步添加多个纹理		eg:  addTexturesAsync({"xxx.png", "xxx2.png"}, callback)
		void addModelsAsync(const std::vector<std::string>& filepaths, std::vector<std::vector<std::string>> animFilePathsVec, std::function<void(Model*)> callback);
        // 获取
        Model* getModel(const std::string& name);

		// 移除纹理资源
		void removeModel(const std::string& name);
        void removeModel(Model* model);
        void removeFromCache(Model* model);


		// TODO: 临时
		void update(float dt);

	};
}

