#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"

using namespace common;

namespace browser
{
	class BoundBoxSystem : public common::BaseSystem, public BaseSingleton<BoundBoxSystem>
	{
	public:
		BoundBoxSystem();
		~BoundBoxSystem() {};

	public:
		// 每帧刷新
		void update(float deltaTime);


	private:

	};
}
