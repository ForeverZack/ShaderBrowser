#pragma once

#include "BaseCache.h"
#include "BaseAsyncLoader.h"
#include "../../../GL/Texture2D.h"

using namespace customGL;

namespace common
{
	class TextureCache : public BaseMapCache<std::string, Texture2D, TextureCache>, public BaseAsyncLoader<std::function<Image*(const char*)>, Image, std::function<void(Texture2D*)>>
	{
	public:
		TextureCache();
		~TextureCache();
        
    public:
        // 添加纹理
        void addTexture(std::string filepath);
		// 异步添加纹理
		void addTextureAsync(std::string filepath, std::function<void(Texture2D*)> callback);
		void addTexturesAsync(std::string filepaths[], std::function<void(Texture2D*)> callback);
        // 获取
        Texture2D* getTexture(std::string name);


		// TODO: 临时
		void update(float dt);

	};
}

