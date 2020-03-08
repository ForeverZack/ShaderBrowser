#pragma once

#include "BaseCache.h"
#include "BaseAsyncLoader.h"
#include "GL/GPUResource/Texture/Texture2D.h"

using namespace customGL;

namespace common
{
	class TextureCache : public BaseMapCache<std::string, Texture2D*, TextureCache>, public BaseAsyncLoader<Image, std::function<void(Texture2D*)>>
	{
        // TextureCache异步加载最大允许线程数
        #define MAX_TEXTURECACHE_THREAD_COUNT 5

    public:
		TextureCache();
		~TextureCache();
        
    public:
        // 初始化
        void init();
        // 添加纹理
        void addTexture(std::string filepath);
		// 异步添加单个纹理
		void addTextureAsync(std::string filepath, std::function<void(Texture2D*)> callback);
		// 异步添加多个纹理		eg:  addTexturesAsync({"xxx.png", "xxx2.png"}, callback)
		void addTexturesAsync(std::vector<std::string> filepaths, std::function<void(Texture2D*)> callback);
        // 获取
        Texture2D* getTexture(std::string name);

		// 移除纹理资源
		void removeTexture(std::string name);
        void removeTexture(Texture2D* texture);
        void removeFromCache(Texture2D* texture);


		// TODO: 临时
		void update(float dt);

	};
}

