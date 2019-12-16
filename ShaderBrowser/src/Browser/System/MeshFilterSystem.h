#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"

using namespace common;

namespace browser
{
    
	class MeshFilterSystem : public common::BaseSystem, public BaseSingleton<MeshFilterSystem>
	{

	public:
		MeshFilterSystem();
		~MeshFilterSystem() {};

	private:

	};
}
