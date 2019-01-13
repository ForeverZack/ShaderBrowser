#include "Material.h"
#include "../../../Common/Tools/Utils.h"

namespace browser
{
	Material* Material::createMaterial()
	{
		Material* material = new Material();
		
		material->init();

		return material;
	}

	Material::Material()
	{

	}

	Material::~Material()
	{

	}

	void Material::init()
	{
        m_vPass.clear();
	}

    void Material::addPass(Pass* pass)
    {
        m_vPass.push_back(pass);
    }

	void Material::useMaterial(int index/* = 0*/)
	{
        common::BROWSER_ASSERT(m_vPass.size()>index && m_vPass[index], "cannot found pass in function Material::useMaterial");
        m_vPass[index]->usePass();
	}
    

}
