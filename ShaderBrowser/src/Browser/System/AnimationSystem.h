#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"

using namespace common;

namespace browser
{
    
	class AnimationSystem : public common::BaseSystem, public BaseSingleton<AnimationSystem>
	{

	public:
		AnimationSystem();
		~AnimationSystem() {};

    public:
        // 每帧刷新
        void update(float deltaTime);
        
	private:

	};
}
