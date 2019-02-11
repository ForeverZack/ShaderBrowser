#pragma once

#include <vector>
#include "Browser/Components/BaseComponent.h"
#include "Browser/Components/Transform/Transform.h"
#include "Common/Tools/Utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace common;
using namespace customGL;

namespace browser
{
    class Camera;
    
	class BaseBoundBox : public BaseComponent
	{
    public:
        // 包围盒类型
        enum BoundBoxType
        {
            // AABB
            BoundBox_AABB = 0,
        };
        
	public:
		BaseBoundBox();
		~BaseBoundBox();

	public:
		// 刷新
        virtual void updateBoundBox(float deltaTime) {}
        
        // 计算绘制顶点
        virtual void calculateDisplayVertices() {}
        
        // 可见性检测
        virtual bool checkVisibility(Camera* camera, bool reCalculate = false)
        {
            return true;
        }

	protected:
		// 处理组件事件
		virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg);

        
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<glm::vec3>, m_vDisplayVertices, DisplayVertices)
        
	protected:
        // 包围盒绘制数组
        std::vector<glm::vec3> m_vDisplayVertices;
        // 记录可见性检测
        bool m_bRecVisibility;
	};
}

