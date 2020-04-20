#include "MaterialCache.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"
#include "Common/System/Cache/TextureCache.h"

namespace common
{    

    MaterialCache::MaterialCache()
        : BaseAsyncLoader<MaterialParameters, std::function<void(Material*)>>(MaterialParser::parseMaterialFile, MAX_MATERIALCACHE_THREAD_COUNT)
    {

    }
    
    MaterialCache::~MaterialCache()
    {
        
    }
    
    void MaterialCache::init()
    {
    }
    
    void MaterialCache::addMaterial(const std::string& filepath)
    {
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);
		MaterialParameters* parameters = MaterialParser::parseMaterialFile(full_path);
		
		// 1.加载纹理
		for (auto itor = parameters->textures_path.begin(); itor != parameters->textures_path.end(); ++itor)
		{
			TextureCache::getInstance()->addTexture(*itor);
		}
		// 2.解析创建材质
		Material* material = Material::create(parameters);
		BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::addMaterial(const std::string& filepath)");
		material->setFilePath(full_path);
		add(full_path, material);

		// 释放数据
		parameters->autorelease();
    }

	void MaterialCache::addMaterialAsync(const std::string& filepath, std::function<void(Material*)> callback)
	{
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);

		// 先从缓存中查找看看是否已经加载过该纹理
		Material* material= get(full_path);
		if (material)
		{
			if (callback)
			{
				callback(material);
			}
			return;
		}

		// 判断文件是否存在
		if (!FileUtils::getInstance()->isDirectoryOrFileExist(full_path))
		{
			if (callback)
			{
				callback(nullptr);
			}
			BROWSER_WARNING(false, "Material file is not exist, warning in function MaterialCache::addMaterialAsync");
			return;
		}

		loadResourceAsync(full_path, callback);
	}
    
    void MaterialCache::removeMaterial(Material* material)
    {
        remove(material->getFilePath());
    }
    
    void MaterialCache::removeMaterial(const std::string& filepath)
    {
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);

		Material* material = get(full_path);
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::removeSharedMaterial");
        remove(full_path);
    }
    
    Material* MaterialCache::getMaterial(const std::string& filepath)
    {
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);
		Material* material = get(full_path);
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::getShareMaterial");

        return material;
    }

	void MaterialCache::createMaterial(AsyncData<MaterialParameters, std::function<void(Material*)>>* asyncData)
	{
		MaterialParameters* parameters = asyncData->data;
		Material* material = Material::create(parameters);
		BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::createMaterial(AsyncData<MaterialParameters, std::function<void(Material*)>>* asyncData)");
		material->setFilePath(asyncData->fullpath);
		add(asyncData->fullpath, material);
		// 添加材质进cache
		add(asyncData->fullpath, material);

		const std::vector<std::function<void(Material*)>>& callbacks = asyncData->callbacks;
		if (!callbacks.empty())
		{
			for (int cbIndex = 0; cbIndex != callbacks.size(); ++cbIndex)
			{
				callbacks[cbIndex](material);
			}
		}

		// 释放材质数据
		parameters->autorelease();
	}

	void MaterialCache::update(float dt)
	{
        // 注意：要先加载纹理资源
		const std::vector<AsyncData<MaterialParameters, std::function<void(Material*)>>*>& responses = getResponseQueue();

		AsyncData<MaterialParameters, std::function<void(Material*)>>* asyncData = nullptr;
		MaterialParameters* parameters = nullptr;
		int textures_count, loaded_count;
		for (int i = 0; i < responses.size(); ++i)
		{
			asyncData = responses[i];
			parameters = asyncData->data;
			if (asyncData->loadSuccess)
			{
				textures_count = parameters->textures_path.size();
				if (textures_count > 0)
				{
					loaded_count = 0;
					// 加载材质所需纹理
					// 注意:Lambda表达式使用的时候要注意,按引用捕获类似于传递实参或者引用,函数内部修改这个变量会传递到外边,但需要注意这个变量的生命周期,不能传递一个在调用之前被销毁的对象。而按值捕获类似于传值,内部修改并不会影响到外部,会在lambda表达式创建的时候生成一份复制,也可以保证外部的变量如果值发生变化,表达式内部该值也不会收到影响。参考下方的i和textureCount。i是一个每次循环都会变化的变量,而lambda表达式内想取到每次循环的固定值,所以按值捕获复制了一份。textureCount是一个局部变量,在lambda表达式函数被调用的时候,如果按引用捕获在调用的时候,访问到的值会不正确。
					// 按值传递函数对象参数时，加上mutable修饰符后，可以修改按值传递进来的拷贝（注意是能修改拷贝，而不是值本身）
					TextureCache::getInstance()->addTexturesAsync(parameters->textures_path, [&, asyncData, loaded_count, textures_count](Texture2D* texture) mutable -> void
					{
						++loaded_count;
						if (loaded_count >= textures_count)
						{
							createMaterial(asyncData);
						}
					});
				}
				else
				{
					createMaterial(asyncData);
				}

			}
			else
			{
				BROWSER_WARNING(false, "material resource load failed .");
			}

			// 清除数据
			delete asyncData;
		}
	}



}
