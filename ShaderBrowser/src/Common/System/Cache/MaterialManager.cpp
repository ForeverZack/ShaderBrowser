#include "MaterialManager.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"
#include "Common/System/Cache/TextureCache.h"

namespace common
{    

	MaterialManager::MaterialManager()
        : m_uIdGenerator(0)
    {

    }
    
	MaterialManager::~MaterialManager()
    {
        
    }
    
    void MaterialManager::init()
    {
    }
    
	void MaterialManager::addMaterial(Material* material, bool isShared/* = false*/, bool isDefault/* = false*/)
    {
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialManager::addMaterial(Material* material)");
        BROWSER_WARNING(material->getMaterialId()==0, "Material has been added, please check your program in function MaterialManager::addMaterial(Material* material)");

        material->setMaterialId(generateId());
		if (isShared)
		{
			material->setSharedId(material->getMaterialId());
			material->setDefaultMaterialFlag(isDefault);
		}
        add(material->getMaterialId(), material);
    }
    
    void MaterialManager::removeMaterial(Material* material)
    {
        remove(material->getMaterialId());
    }
    
    void MaterialManager::removeMaterial(unsigned int materialId)
    {
        Material* material = get(materialId);
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialManager::removeSharedMaterial");
        remove(materialId);
    }
    
    Material* MaterialManager::getMaterial(const unsigned int materialId)
    {
        Material* material = get(materialId);
        BROWSER_ASSERT(material, "Material is not invalid in function MaterialManager::getShareMaterial");
        return material;
    }

	void MaterialManager::resetAllMaterialsUniformsDirty()
	{
		for (auto itor = m_mContents.begin(); itor != m_mContents.end(); ++itor)
		{
			itor->second->resetUniformsDirty();
		}
	}

}
