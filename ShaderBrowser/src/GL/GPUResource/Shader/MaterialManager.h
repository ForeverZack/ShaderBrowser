#pragma once

#include <list>
#include "Common/Tools/BaseSingleton.h"

namespace customGL
{
	class Material;

    class MaterialManager : public common::BaseSingleton<MaterialManager>
	{
	public:
		MaterialManager();
		~MaterialManager();

	public:
		// 添加材质
		void addMaterial(Material* material);
		// 删除材质
		void removeMaterial(Material* material);
		// 更新材质在GPU中的属性
		void updateMaterialsProperties();

	private:
		std::list<Material*> m_lMaterials;

	};
}

