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
        
        // 设置场景节点
        void setScene(Transform* scene);
        
	private:
        Transform* m_oScene;
	};
}
