#pragma once

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
	class Camera : public BaseComponent
	{
	public:
		static EnumComponentClass ComponentClass;

	public:
		// 投影类型
		enum ProjectionType
		{
			// 正交投影
			Ortho = 0,
			// 透视投影
			Perspective,
		};
        // 渲染路径类型
        enum RenderPathType
        {
            // 前向渲染
            Forward = 0,
            // 延迟渲染
            Deffered,
        };

	public:
		static Camera* create(ProjectionType type=ProjectionType::Ortho, float nearPlane=0.3f, float farPlane=1000.0f, int viewportWidth=800, int viewportHeight=600, float FOV=60.0f);

	public:
		Camera();
		Camera(ProjectionType type, float nearPlane, float farPlane, int viewportWidth, int viewportHeight, float FOV);
		~Camera();

	public:
		// 刷新
		void updateCamera(float deltaTime);
        
	protected:
		// 处理组件事件
		void handleEvent(ComponentEvent event, BaseComponentMessage* msg);

        // 重载属性面板显示方法
        virtual void onInspectorGUI(InspectorPanel* inspector);
        
		// 刷新projection matrix
		void updateProjectionMatrix();


        REGISTER_PROPERTY_CONSTREF_GET(glm::vec3, m_oGlobalPosition, GlobalPosition)
		REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oViewMatrix, ViewMatrix)
		REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oProjectionMatrix, ProjectionMatrix)
        REGISTER_PROPERTY_GET_SET(RenderPathType, m_eRenderPathType, RenderPathType)
        REGISTER_PROPERTY_GET(bool, m_bTransDirty, TransDirty)

	protected:
		// view matrix 相关
		// view matrix
		glm::mat4 m_oViewMatrix;
        // 相机世界位置
        glm::vec3 m_oGlobalPosition;
        // 本帧是否发生了移动
        bool m_bTransDirty;
        
		// projection matrix 相关
		// 投影类型
		ProjectionType m_eProjectionType;
		// FOV 视锥体竖直方向的张开角度 (0~180)
		float m_fFieldOfView;
		// 近平面
		float m_fNearPlane;
		// 远平面
		float m_fFarPlane;
		// 视口宽
		int m_iViewportWidth;
		// 视口高
		int m_iViewportHeight;
		// projection matrix
		glm::mat4 m_oProjectionMatrix;
        
        // 渲染路径
        RenderPathType m_eRenderPathType;
	};
}

