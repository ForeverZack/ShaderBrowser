#pragma once

#include "BaseCache.h"
#include "BaseAsyncLoader.h"
#include "GL/GPUResource/Texture/Texture2D.h"

using namespace customGL;

namespace common
{
	class TextureCache : public BaseMapCache<std::string, Texture2D*, TextureCache>, public BaseAsyncLoaderExt<Image, std::function<void(Texture2D*)>, bool>
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
        void addTexture(const std::string& filepath, bool sRGB = false);
		// 异步添加单个纹理
		void addTextureAsync(const std::string& filepath, bool sRGB = false, std::function<void(Texture2D*)> callback = nullptr);
		// 异步添加多个纹理	(非sRGB纹理！！！)	eg:  addTexturesAsync({"xxx.png", "xxx2.png"}, callback)
		void addTexturesAsync(const std::vector<std::string>& filepaths, std::function<void(Texture2D*)> callback);
		// 异步添加多个sRGB纹理	 eg:  addTexturesAsync({"xxx.png", "xxx2.png"}, callback)
		void addSRGBTexturesAsync(const std::vector<std::string>& filepaths, std::function<void(Texture2D*)> callback);
        // 获取
        Texture2D* getTexture(const std::string& name);

		// 移除纹理资源
		void removeTexture(const std::string& name);
        void removeTexture(Texture2D* texture);
        void removeFromCache(Texture2D* texture);


		void update(float dt);

	};
}

