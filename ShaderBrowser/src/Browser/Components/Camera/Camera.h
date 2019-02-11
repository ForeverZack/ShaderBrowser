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
		// 投影类型
		enum ProjectionType
		{
			// 正交投影
			Ortho = 0,
			// 透视投影
			Perspective,
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

		// 刷新projection matrix
		void updateProjectionMatrix();


		REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oViewMatrix, ViewMatrix)
		REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oProjectionMatrix, ProjectionMatrix)

	protected:
		// view matrix 相关
		// view matrix
		glm::mat4 m_oViewMatrix;

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
	};
}

