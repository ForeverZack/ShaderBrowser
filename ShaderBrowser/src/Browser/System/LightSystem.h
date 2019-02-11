#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"

using namespace common;

namespace browser
{
    
	class LightSystem : public common::BaseSystem, public BaseSingleton<LightSystem>
	{

	public:
		LightSystem();
		~LightSystem() {};

	private:

	};
}
