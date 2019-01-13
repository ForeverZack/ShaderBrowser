#include "BaseRender.h"
#include "../../../Common/Tools/Utils.h"

namespace browser
{
	BaseRender::BaseRender()
        : m_oMaterial(nullptr)
	{
		// 组件所属系统
		m_eBelongSystem = SystemType::RenderSystem;
	}

	BaseRender::~BaseRender()
	{
	}

	void BaseRender::init(Material* material)
	{       
		m_oMaterial = material;
	}
    
}
