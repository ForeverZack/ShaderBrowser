#include "ModelCache.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"
#include <vector>

namespace common
{    

	ModelCache::ModelCache()
    : BaseAsyncLoader<std::function<Model*(const char*)>, Model, std::function<void(Model*)>>(Model::create)
    {
    }
    
	ModelCache::~ModelCache()
    {
    }
    
	void ModelCache::addModel(std::string filepath, std::function<void(Model*)> callback)
	{
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);
	
		Model* model = Model::createAlone(full_path.c_str(), callback);
        model->retain();
        
		add(full_path, model);
	}

	Model* ModelCache::getModel(std::string filepath)
	{
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);
		return get(full_path);
	}

	void ModelCache::addModelAsync(std::string filepath, std::function<void(Model*)> callback)
	{
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);

		// 先从缓存中查找看看是否已经加载过该纹理
		Model* model = get(full_path);
		if (model)
		{
			if (callback)
			{
				callback(model);
			}
			return;
		}

		// 判断文件是否存在
		if ( !FileUtils::getInstance()->isDirectoryOrFileExist(full_path) )
		{
			if (callback)
			{
				callback(nullptr);
			}
			BROWSER_WARNING(false, "Model file is not exist, warning in function ModelCache::addModelAsync");
			return;
		}

		loadResourceAsync(full_path, callback);
	}

	void ModelCache::addModelsAsync(std::vector<std::string> filepaths, std::function<void(Model*)> callback)
	{
		for (int i = 0; i < filepaths.size(); ++i)
		{
			addModelAsync(filepaths[i], callback);
		}
	}

	void ModelCache::update(float dt)
	{
		const std::vector<AsyncData<Model, std::function<void(Model*)>>*>& responses = getResponseQueue();

        Model* model = nullptr;
		AsyncData<Model, std::function<void(Model*)>>* asyncData = nullptr;
		for (int i = 0; i < responses.size(); ++i)
		{
			asyncData = responses[i];
            model = responses[i]->data;
			if (asyncData->loadSuccess)
			{
                // retain一下
                model->autorelease();
                model->retain();
                
                // 为model的所有mesh设置VAO
                model->setupMeshesVAO();
                
                // 添加纹理加载回调,表示整个模型+纹理都加载完毕
                model->setSuccessCallback([=](Model* responseModel)
                    {
                        // 将加载好的模型加入缓存
                        add(asyncData->fullpath, responseModel);
                        // 执行model加载完成的回调函数
						const std::vector<std::function<void(Model*)>>& callbacks = asyncData->callbacks;
                        if (!callbacks.empty())
                        {
							for (int cbIndex = 0; cbIndex < callbacks.size(); ++cbIndex)
							{
								callbacks[cbIndex](responseModel);
							}
                        }
                        
                        // 清除model的返回数据
                        delete asyncData;
                    });
                
                // 开始异步加载模型的所有纹理,如果加载完成,回调用
                model->loadTextures(model->getDirectory());
				
			}
			else
			{
				BROWSER_WARNING(false, "Model resource load failed .");
			}

		}

	}

	void ModelCache::removeModel(std::string filepath)
	{
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);
        
        Model* model = get(full_path);
        model->release();
        
		remove(full_path);
    }
	
    void ModelCache::removeModel(Model* model)
    {
        model->release();
            
        removeByValue(model);
    }
    
    void ModelCache::removeFromCache(Model* model)
    {
        removeByValue(model);
    }

}
