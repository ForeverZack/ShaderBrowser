#include "MaterialManager.h"
#include "GL/GPUResource/Shader/Material.h"

namespace customGL
{

	MaterialManager::MaterialManager()
	{
		m_lMaterials.clear();
	}

	MaterialManager::~MaterialManager()
	{

	}

	void MaterialManager::addMaterial(Material* material)
	{
		m_lMaterials.push_back(material);
	}

	void MaterialManager::removeMaterial(Material* material)
	{
		m_lMaterials.remove(material);
	}

	void MaterialManager::updateMaterialsProperties()
	{

	}

}

