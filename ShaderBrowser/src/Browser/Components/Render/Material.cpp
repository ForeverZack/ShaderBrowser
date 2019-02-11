#include "Material.h"
#include "Common/Tools/Utils.h"

namespace browser
{
	// Ä¬ÈÏ²ÄÖÊÃû³Æ
	const char* Material::DEFAULT_MATERIAL_NAME = "ShaderBrowser_DefaultMaterial";

	Material* Material::createMaterial()
	{
		Material* material = new Material();
        
		material->init();
        material->autorelease();
        
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

	void Material::useMaterial(Mesh* mesh, Transform* transform, Camera* camera, int index/* = 0*/)
	{
        common::BROWSER_ASSERT(m_vPass.size()>index && m_vPass[index], "cannot found pass in function Material::useMaterial");
        m_vPass[index]->usePass(mesh, transform, camera);
	}
    

}
