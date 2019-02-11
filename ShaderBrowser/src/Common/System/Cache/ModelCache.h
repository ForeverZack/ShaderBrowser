#pragma once

#include "BaseCache.h"
#include "BaseAsyncLoader.h"
#include "GL/Model.h"

using namespace customGL;

namespace common
{
    class ModelCache : public BaseMapCache<std::string, Model, ModelCache>, public BaseAsyncLoader<std::function<Model*(const char*)>, Model, std::function<void(Model*)>>
	{
	public:
		ModelCache();
		~ModelCache();
        
    public:
        // 添加纹理
        void addModel(std::string filepath, std::function<void(Model*)> callback);
		// 异步添加单个纹理
		void addModelAsync(std::string filepath, std::function<void(Model*)> callback);
		// 异步添加多个纹理		eg:  addTexturesAsync({"xxx.png", "xxx2.png"}, callback)
		void addModelsAsync(std::vector<std::string> filepaths, std::function<void(Model*)> callback);
        // 获取
        Model* getModel(std::string name);

		// 移除纹理资源
		void removeModel(std::string name);
        void removeModel(Model* model);
        void removeFromCache(Model* model);


		// TODO: 临时
		void update(float dt);

	};
}

