#include "TextureCache.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"
#include <vector>

namespace common
{    

	TextureCache::TextureCache()
		: BaseAsyncLoader<std::function<Image*(const char*)>, Image, std::function<void(Texture2D*)>>(Image::create)
    {
    }
    
	TextureCache::~TextureCache()
    {
    }
    
	void TextureCache::addTexture(std::string filepath)
	{
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);
	
		Texture2D* texture = Texture2D::create(full_path);
        texture->retain();

		add(full_path, texture);
	}

	Texture2D* TextureCache::getTexture(std::string filepath)
	{
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);
		return get(full_path);
	}

	void TextureCache::addTextureAsync(std::string filepath, std::function<void(Texture2D*)> callback)
	{
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);

		// 先从缓存中查找看看是否已经加载过该纹理
		Texture2D* texture = get(full_path);
		if (texture)
		{
			if (callback)
			{
				callback(texture);
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
			BROWSER_WARNING(false, "Texture file is not exist, warning in function TextureCache::addTextureAsync");
			return;
		}

		loadResourceAsync(full_path, callback);
	}

	void TextureCache::addTexturesAsync(std::vector<std::string> filepaths, std::function<void(Texture2D*)> callback)
	{
		for (int i = 0; i < filepaths.size(); ++i)
		{
			addTextureAsync(filepaths[i], callback);
		}
	}

	void TextureCache::update(float dt)
	{
		const std::vector<AsyncData<Image, std::function<void(Texture2D*)>>*>& responses = getResponseQueue();

		AsyncData<Image, std::function<void(Texture2D*)>>* asyncData = nullptr;
		for (int i = 0; i < responses.size(); ++i)
		{
			asyncData = responses[i];
			if (asyncData->loadSuccess)
			{
				Texture2D* texture = Texture2D::create(responses[i]->data);
                texture->retain();
				add(asyncData->fullpath, texture);

				const std::vector<std::function<void(Texture2D*)>>& callbacks = asyncData->callbacks;
				if (!callbacks.empty())
				{
					for (int cbIndex=0; cbIndex!=callbacks.size(); ++cbIndex)
					{
						callbacks[cbIndex](texture);
					}
				}
			}
			else
			{
				BROWSER_WARNING(false, "texture resource load failed .");
			}

			// 清除数据
			delete asyncData;
		}

	}

	void TextureCache::removeTexture(std::string filepath)
	{
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);
        
        Texture2D* texture = get(full_path);
        texture->release();
        
		remove(full_path);
    }
	
    void TextureCache::removeTexture(Texture2D* texture)
    {
        texture->release();
            
        removeByValue(texture);
    }
    
    void TextureCache::removeFromCache(Texture2D* texture)
    {
        removeByValue(texture);
    }

}