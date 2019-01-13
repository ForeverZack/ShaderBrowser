#pragma once

#include <vector>
#include "../../Common/System/BaseSystem.h"

using namespace common;

namespace browser
{
    
	class MeshSystem : public common::BaseSystem
	{
	public:
		static MeshSystem* getInstance();
	public:
		MeshSystem();
		~MeshSystem() {};

	private:

	};
}
