#include "BaseGPUResource.h"

namespace customGL
{
	BaseGPUResource::BaseGPUResource()
		: m_eResouceState(GPUResourceState::GRS_UnLoad)
		, m_eResourceType(GPUResourceType::GRT_Undefined)
	{
		this->autorelease();
	}

	void BaseGPUResource::createGPUResource()
	{
		m_bDirty = false;
	}

	void BaseGPUResource::updateGPUResource()
	{
		m_bDirty = false;
	}

	void BaseGPUResource::deleteGPUResource()
	{

	}


}
