#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"
#include "Browser/Components/Feedback/BaseFeedback.h"

using namespace common;

namespace browser
{
    
	class FeedbackSystem : public common::BaseSystem, public BaseSingleton<FeedbackSystem>
	{
	public:
		FeedbackSystem();
		~FeedbackSystem() {};
        
    public:
        // 初始化
        virtual void init() {}
        // 每帧刷新
        virtual void update(float deltaTime);

	private:

	};
}
