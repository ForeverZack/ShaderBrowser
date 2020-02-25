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

		// 记录新材质
		recordNewMaterial(material);
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
    
	void MaterialCache::operateAllMaterials(std::function<void(Material*)> callback)
	{
		Material* material = nullptr;
		for (auto itor=m_mContents.begin(); itor!=m_mContents.end(); ++itor)
		{
			material = itor->second;
			callback(material);
		}
	}

	void MaterialCache::operateAllNewMaterials(std::function<void(Material*)> callback)
	{
		Material* material = nullptr;
		for (auto itor = m_vNewMaterials.begin(); itor!=m_vNewMaterials.end(); ++itor)
		{
			material = *itor;
			callback(material);
		}
	}

	void MaterialCache::update(float dt)
	{
		// 重置本帧新增材质
		resetAllNewMaterials();
	}

	void MaterialCache::resetAllNewMaterials()
	{
		m_vNewMaterials.clear();
	}

	void MaterialCache::recordNewMaterial(Material* material)
	{
		m_vNewMaterials.push_back(material);
	}

}
