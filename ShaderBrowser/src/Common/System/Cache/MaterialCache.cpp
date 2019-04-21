#include "MaterialCache.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"

namespace common
{    

    MaterialCache::MaterialCache()
        : m_uIdGenerator(0)
    {

    }
    
    MaterialCache::~MaterialCache()
    {
        
    }
    
    void MaterialCache::init()
    {
    }
    
    void MaterialCache::addSharedMaterial(Material* material)
    {
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::addShareMaterial(Material* material)");
        BROWSER_WARNING(material->getMaterialId()==0, "Material has been added, please check your program in function MaterialCache::addShareMaterial(Material* material)");

        material->setMaterialId(generateId());
        material->retain();
        add(material->getMaterialId(), material);
    }
    
    Material* MaterialCache::addSharedMaterial(const std::string& materialName /*= DEFAULT_MATERIAL_NAME*/)
    {
        Material* material = Material::createMaterial(materialName);
        addSharedMaterial(material);
        
        return material;
    }
    
    void MaterialCache::removeSharedMaterial(Material* material)
    {
        material->release();
        remove(material->getMaterialId());
    }
    
    void MaterialCache::removeSharedMaterial(unsigned int materialId)
    {
        Material* material = get(materialId);
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::removeSharedMaterial");
        material->release();
        remove(materialId);
    }
    
    Material* MaterialCache::getSharedMaterial(const unsigned int materialId)
    {
        Material* material = get(materialId);
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialCache::getShareMaterial");
        return material;
    }
    
    
}
