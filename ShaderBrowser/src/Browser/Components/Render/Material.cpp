#include "Material.h"
#include "Common/Tools/Utils.h"

namespace browser
{
	// Ä¬ÈÏ²ÄÖÊÃû³Æ
	const char* Material::DEFAULT_MATERIAL_NAME = "ShaderBrowser_DefaultMaterial";

	Material* Material::createMaterial(const std::string& materialName /*= DEFAULT_MATERIAL_NAME*/)
	{
		Material* material = new Material(materialName);
        
		material->init();
        material->autorelease();
        
		return material;
	}

	Material::Material(const std::string& materialName)
		: m_sMaterialName(materialName)
	{

	}

	Material::~Material()
	{

	}

	void Material::init()
	{
        m_mUniforms.clear();
        m_vPass.clear();
	}

    void Material::addPass(Pass* pass)
    {
        pass->setUniformsFromMaterial(&m_mUniforms);
        m_vPass.push_back(pass);
    }

	void Material::useMaterial(Mesh* mesh, Transform* transform, Camera* camera, int index/* = 0*/)
	{
        common::BROWSER_ASSERT(m_vPass.size()>index && m_vPass[index], "cannot found pass in function Material::useMaterial");
        m_vPass[index]->usePass(mesh, transform, camera);
	}

	Pass* Material::getUsePass(int index/* = 0*/)
	{
		return m_vPass[index];
	}

    

}
