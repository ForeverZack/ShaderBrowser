#pragma once

#include <vector>
#include "../../Common/Tools/Utils.h"
#include "../../Common/System/BaseSystem.h"
#include "../../Browser/Components/Transform/Transform.h"

using namespace common;

namespace browser
{
    
	class TransformSystem : public common::BaseSystem
	{
	public:
		static TransformSystem* getInstance();
	public:
		TransformSystem();
		~TransformSystem() {};

		// init
		void init();
		// 每帧更新
		void update(float deltaTime);
        
        
        
        REGISTER_PROPERTY_GET_SET(Transform*, m_oScene, Scene)
	private:
        Transform* m_oScene;
	};
}
