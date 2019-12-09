#include "BaseGPUResource.h"

namespace customGL
{
	BaseGPUResource::BaseGPUResource()
		: m_eResouceState(GPUResourceState::GRS_UnLoad)
		, m_eResourceType(GPUResourceType::GRT_Undefined)
		, m_bDirty(false)
		, m_bGPUDeleted(false)
	{
		this->autorelease();
	}

	void BaseGPUResource::release()
	{
		BROWSER_ASSERT(m_iRefenceCount > 0, "BaseGPUResource::release() : reference must bigger then 0");
		--m_iRefenceCount;
		if (m_iRefenceCount == 0)
		{
			if (m_bGPUDeleted)
			{
				delete this;
			}
			else
			{
				deleteGPUResource();
			}
		}
	}

	void BaseGPUResource::createGPUResource()
	{
		m_bDirty = false;
	}

	void BaseGPUResource::updateGPUResource()
	{
		m_bDirty = false;
	}


}
