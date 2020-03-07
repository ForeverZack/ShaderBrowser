#pragma once

#include <vector>
#include "Browser/Components/Light/BaseLight.h"
using namespace common;

namespace browser
{
	class PointLight : public BaseLight
	{
	public:
		PointLight();
		~PointLight();

	public:
		// 刷新
		virtual void updateLight();
		// 更新材质的Light数据
		virtual void updateMaterialsLight(const std::unordered_map<unsigned int, Material*>& materials, unsigned int index = 0, bool forceUpdate = false);
		// 数据是否发生改变
		virtual bool isLightDirty();
		// 光源系统是否需要更新
		virtual bool isLightSystemDirty();
        
	protected:
       // 阴影属性等
        
	};
}

