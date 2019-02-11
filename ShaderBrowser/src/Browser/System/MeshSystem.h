#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"

using namespace common;

namespace browser
{
    
	class MeshSystem : public common::BaseSystem, public BaseSingleton<MeshSystem>
	{

	public:
		MeshSystem();
		~MeshSystem() {};

	private:

	};
}
