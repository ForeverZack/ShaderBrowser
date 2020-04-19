#include "MaterialCache.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"

namespace common
{    

    MaterialCache::MaterialCache()
        : BaseAsyncLoader<MaterialParameters, std::function<void(Material*)>>(MaterialParser::parseMaterialFile, MAX_MATERIALCACHE_THREAD_COUNT)
        , m_uIdGenerator(0)
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
        
    }
    
	void MaterialCache::addMaterial(Material* material, bool isShared/* = false*/, bool isDefault/* = false*/)
    {
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::addMaterial(Material* material)");
        BROWSER_WARNING(material->getMaterialId()==0, "Material has been added, please check your program in function MaterialCache::addMaterial(Material* material)");

        material->setMaterialId(generateId());
		if (isShared)
		{
			material->setSharedId(material->getMaterialId());
			material->setDefaultMaterialFlag(isDefault);
		}
        add(material->getMaterialId(), material);
    }
    
    void MaterialCache::removeMaterial(Material* material)
    {
        remove(material->getMaterialId());
    }
    
    void MaterialCache::removeMaterial(unsigned int materialId)
    {
        Material* material = get(materialId);
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::removeSharedMaterial");
        remove(materialId);
    }
    
    Material* MaterialCache::getMaterial(const unsigned int materialId)
    {
        Material* material = get(materialId);
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::getShareMaterial");
        return material;
    }

	void MaterialCache::update(float dt)
	{
        // 注意：要先加载纹理资源
	}



}
