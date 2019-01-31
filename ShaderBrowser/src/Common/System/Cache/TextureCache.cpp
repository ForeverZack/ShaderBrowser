#include "TextureCache.h"
#include "../../Tools/Utils.h"
#include "../../Tools/FileUtils.h"
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
		// TODO: 这里暂时先retain一下
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

				if (asyncData->callback)
				{
					asyncData->callback(texture);
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

		remove(full_path);
	}
	


}
