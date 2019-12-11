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
    class MeshFilter;
    class Animator;
    
	class BaseBoundBox : public BaseComponent
	{
	public:
		static EnumComponentClass ComponentClass;

    public:
        // 包围盒类型
        enum BoundBoxType
        {
            // AABB
            BoundBox_AABB = 0,
        };
        
	public:
		BaseBoundBox();
		virtual ~BaseBoundBox();

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
        // 获取MeshFilter组件
        template <typename MsgType>
        void getMeshFilterFromMsg(BaseComponentMessage* msg)
        {
            MsgType* convertMsg = static_cast<MsgType*>(msg);
            MeshFilter* meshFilter = convertMsg->getMeshFilter();
            if (!m_oMeshFilter && meshFilter)
            {
                m_oMeshFilter = meshFilter;
            }
        }
        // 获取Animator组件
        template <typename MsgType>
        void getAnimatorFromMsg(BaseComponentMessage* msg)
        {
            MsgType* convertMsg = static_cast<MsgType*>(msg);
            Animator* animator = convertMsg->getAnimator();
            if (!m_oAnimator && animator)   // 如果Animator已经被设置，则不会更新（用来防治存在多个animator嵌套的情况）
            {
                m_oAnimator = animator;
            }
        }
        
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<glm::vec3>, m_vDisplayVertices, DisplayVertices)
        REGISTER_PROPERTY_GET_SET(MeshFilter*, m_oMeshFilter, MeshFilter)
        
	protected:
        // MeshFilter组件
        MeshFilter* m_oMeshFilter;
        // Animator组件（来自entityRoot）
        Animator* m_oAnimator;
        // 骨骼矩阵
        std::vector<glm::mat4> m_vBonesMatrix;
        // 是否根据骨骼动画的变换重新生成包围盒
        bool m_bDynamic;
        
        // 包围盒绘制数组
        std::vector<glm::vec3> m_vDisplayVertices;
        // 记录可见性检测
        bool m_bRecVisibility;
	};
}

