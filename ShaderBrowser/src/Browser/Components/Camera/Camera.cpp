#include "Camera.h"
#include "GL/GLDefine.h"
#include "Browser/Entitys/BaseEntity.h"
#include "Browser/System/CameraSystem.h"

namespace browser
{
	Camera* Camera::create(ProjectionType type /*= ProjectionType::Ortho*/, float nearPlane /*= 0.3f*/, float farPlane /*= 1000.0f*/, float FOV /*= 60.0f*/, int viewportWidth /*= 800*/
							, int viewportHeight /*= 600*/)
	{
		Camera* camera = new Camera(type, nearPlane, farPlane, viewportWidth, viewportHeight, FOV);

		// 更新投影矩阵
		camera->updateProjectionMatrix();

		return camera;
	}

	Camera::Camera(ProjectionType type, float nearPlane, float farPlane, int viewportWidth, int viewportHeight, float FOV)
		: BaseComponent("Camera")
        , m_oViewMatrix(GLM_MAT4_UNIT)
        , m_oGlobalPosition(GLM_VEC3_ZERO)
		, m_uViewMatrixDirtyTag(0)
		, m_eProjectionType(type)
		, m_fFieldOfView(FOV)
		, m_fNearPlane(nearPlane)
		, m_fFarPlane(farPlane)
		, m_iViewportWidth(viewportWidth)
		, m_iViewportHeight(viewportHeight)
		, m_oProjectionMatrix(GLM_MAT4_UNIT)
		, m_uProjectionMatrixDirtyTag(0)
        , m_eRenderPathType(RenderPathType::Forward)
		, m_pRenderTexture(nullptr)
		, m_iDepth(0)
		, m_oBackgroundColor(DEFAULT_GL_CLEAR_COLOR)
		, m_bIsRenderable(true)
	{
		// 组件所属系统
		m_eBelongSystem = SystemType::Camera;
		
		// 给显示视锥顶点数组预留空间
		m_vDisplayVertices.reserve(24);
	}

	Camera::Camera()
		: BaseComponent("Camera")
		, m_oViewMatrix(GLM_MAT4_UNIT)
		, m_oGlobalPosition(GLM_VEC3_ZERO)
		, m_uViewMatrixDirtyTag(0)
		, m_eProjectionType(ProjectionType::Perspective)
		, m_fFieldOfView(60.0f)
		, m_fNearPlane(0.3f)
		, m_fFarPlane(1000.0f)
		, m_iViewportWidth(800)
		, m_iViewportHeight(600)
		, m_oProjectionMatrix(GLM_MAT4_UNIT)
		, m_uProjectionMatrixDirtyTag(0)
		, m_eRenderPathType(RenderPathType::Forward)
		, m_pRenderTexture(nullptr)
		, m_iDepth(0)
		, m_oBackgroundColor(DEFAULT_GL_CLEAR_COLOR)
		, m_bIsRenderable(true)
	{
		// 组件所属系统
		m_eBelongSystem = SystemType::Camera;
		
		// 给显示视锥顶点数组预留空间
		m_vDisplayVertices.reserve(24);
	}

	Camera::~Camera()
	{
		BROWSER_LOG("~Camera");
	}

	void Camera::updateCamera(float deltaTime)
	{
		Transform* transform = m_oBelongEntity ? m_oBelongEntity->getComponent<Transform>() : nullptr;
		if (transform && transform->getCurFrameDirty())
		{
			// 获取相机的model矩阵
			const glm::mat4& matrix_model = transform->getModelMatrix();
			// 计算相机上向量
			glm::vec4 vector_up(GLM_AXIS_Y, 0.0f);
			vector_up = matrix_model * vector_up;
			// 计算相机前向量
			glm::vec4 vector_forward(GLM_AXIS_Z, 0.0f);
			vector_forward = matrix_model * vector_forward;

			// 相机上向量
			glm::vec3 camera_up(vector_up.x, vector_up.y, vector_up.z);
			// 相机位置
			const glm::vec3& camera_position = transform->getPosition();
			// 目标位置
			glm::vec3 camera_front(vector_forward.x, vector_forward.y, vector_forward.z);
			glm::vec3 camera_target = camera_position + camera_front;
		
			// 计算视矩阵	( glm::LookAt函数需要一个位置、目标和上向量 )
			// glm::lookAtRH(eye, center, up)
			m_oViewMatrix = glm::lookAt(camera_position, camera_target, camera_up);
            m_oGlobalPosition = transform->getGlobalPosition();

			// 更新视锥顶点
			updateFrustumVertices();

			// 更新脏标记
			generateViewMatrixDirtyTag();
		}
	}
    
    void Camera::onInspectorGUI(InspectorPanel* inspector)
    {
		// Background Color
		inspector->addPropertyColor4("Background Color", &m_oBackgroundColor, [=](const glm::vec4& color)
		{
			m_oBackgroundColor = color;
		}, false);

		// Viewport
		inspector->addPropertyText("Viewport Width:" + std::to_string(m_iViewportWidth));
		inspector->addPropertyText("Viewport Height:" + std::to_string(m_iViewportHeight));
		// Near Plane
		inspector->addPropertyText("Near Plane:" + std::to_string(m_fNearPlane));
		// Far Plane
		inspector->addPropertyText("Far Plane:" + std::to_string(m_fFarPlane));
        // FOV
        inspector->addPropertyText("Field of view:"+std::to_string(m_fFieldOfView));
		// depth
		inspector->addPropertyText("Depth:" + std::to_string(m_iDepth));
    }

	void Camera::updateProjectionMatrix()
	{
		switch (m_eProjectionType)
		{
		case ProjectionType::Perspective:
			{
				// 透视投影		glm::persperctive(FieldOfView, ViewportWidth/ViewportHeight, NearPlane, FarPlane);
				// glm::perspectiveRH_NO(fovy, aspect, zNear, zFar)
				m_oProjectionMatrix = glm::perspective(glm::radians(m_fFieldOfView), (float)m_iViewportWidth/(float)m_iViewportHeight, m_fNearPlane, m_fFarPlane);
			}
			break;

		case ProjectionType::Ortho:
			{
				// 正交投影
				// glm::orthoRH_NO(left, right, bottom, top, zNear, zFar);
				m_oProjectionMatrix = glm::ortho(0.0f, (float)m_iViewportWidth, 0.0f, (float)m_iViewportHeight, m_fNearPlane, m_fFarPlane);
			}
			break;
		}

		// 更新视锥顶点
		updateFrustumVertices();

		// 更新脏标记
		generateProjectionMatrixDirtyTag();
	}

	void Camera::updateFrustumVertices()
	{
		// 近平面
		// 左上 NDC position : (-1, 1, -1)
		m_vVertices[Near_LT] = glm::vec4(-1, 1, -1, 1);
		// 左下 NDC position : (-1, -1, -1)
		m_vVertices[Near_LB] = glm::vec4(-1, -1, -1, 1);
		// 右下 NDC position : (1, -1, -1)
		m_vVertices[Near_RB] = glm::vec4(1, -1, -1, 1);
		// 右上 NDC position : (1, 1, -1)
		m_vVertices[Near_RT] = glm::vec4(1, 1, -1, 1);

		// 远平面
		// 左上 NDC position : (-1, 1, 1)
		m_vVertices[Far_LT] = glm::vec4(-1, 1, 1, 1);
		// 左下 NDC position : (-1, -1, 1)
		m_vVertices[Far_LB] = glm::vec4(-1, -1, 1, 1);
		// 右下 NDC position : (1, -1, 1)
		m_vVertices[Far_RB] = glm::vec4(1, -1, 1, 1);
		// 右上 NDC position : (1, 1, 1)
		m_vVertices[Far_RT] = glm::vec4(1, 1, 1, 1);

		// 计算视锥顶点的世界坐标 (NDC[-1, 1] -> world space)
		/*
			world_pos = inverse(M_projection * M_view) * ndc_pos;
			world_pos.xyzw /= world_pos.w;
		*/
		glm::mat4 vp_inverse = glm::inverse(m_oProjectionMatrix * m_oViewMatrix);
		for (int i = 0; i < 8; ++i)
		{
			glm::vec4& vertex = m_vVertices[i];
			vertex = vp_inverse * vertex;
			vertex.x /= vertex.w;
			vertex.y /= vertex.w;
			vertex.z /= vertex.w;
			vertex.w = 1;
		}

	}

	void Camera::calculateDisplayVertices()
	{
		m_vDisplayVertices.clear();

		// 1
		m_vDisplayVertices.push_back(m_vVertices[Near_LT]);
		m_vDisplayVertices.push_back(m_vVertices[Near_RT]);
		// 2
		m_vDisplayVertices.push_back(m_vVertices[Near_RT]);
		m_vDisplayVertices.push_back(m_vVertices[Near_RB]);
		// 3
		m_vDisplayVertices.push_back(m_vVertices[Near_RB]);
		m_vDisplayVertices.push_back(m_vVertices[Near_LB]);
		// 4
		m_vDisplayVertices.push_back(m_vVertices[Near_LB]);
		m_vDisplayVertices.push_back(m_vVertices[Near_LT]);
		// 5
		m_vDisplayVertices.push_back(m_vVertices[Far_LT]);
		m_vDisplayVertices.push_back(m_vVertices[Far_RT]);
		// 6
		m_vDisplayVertices.push_back(m_vVertices[Far_RT]);
		m_vDisplayVertices.push_back(m_vVertices[Far_RB]);
		// 7
		m_vDisplayVertices.push_back(m_vVertices[Far_RB]);
		m_vDisplayVertices.push_back(m_vVertices[Far_LB]);
		// 8
		m_vDisplayVertices.push_back(m_vVertices[Far_LB]);
		m_vDisplayVertices.push_back(m_vVertices[Far_LT]);
		// 9
		m_vDisplayVertices.push_back(m_vVertices[Near_LT]);
		m_vDisplayVertices.push_back(m_vVertices[Far_LT]);
		// 10
		m_vDisplayVertices.push_back(m_vVertices[Near_RT]);
		m_vDisplayVertices.push_back(m_vVertices[Far_RT]);
		// 11
		m_vDisplayVertices.push_back(m_vVertices[Near_RB]);
		m_vDisplayVertices.push_back(m_vVertices[Far_RB]);
		// 12
		m_vDisplayVertices.push_back(m_vVertices[Near_LB]);
		m_vDisplayVertices.push_back(m_vVertices[Far_LB]);
	}

	void Camera::generateViewMatrixDirtyTag()
	{
		m_uViewMatrixDirtyTag = (m_uViewMatrixDirtyTag + 1) % VALUE_DIRTY_TAG_MAX;
	}

	void Camera::generateProjectionMatrixDirtyTag()
	{
		m_uProjectionMatrixDirtyTag = (m_uProjectionMatrixDirtyTag + 1) % VALUE_DIRTY_TAG_MAX;
	}

	void Camera::setDepth(int depth)
	{
		CameraSystem::getInstance()->setDirty(true);
		m_iDepth = depth;
	}

	void Camera::setRenderTexture(RenderTexture* rt)
	{
		m_pRenderTexture = rt;

		setViewportSize(rt->getWidth(), rt->getHeight());
	}

	void Camera::setViewportSize(int width, int height)
	{
		m_iViewportWidth = width;
		m_iViewportHeight = height;
		updateProjectionMatrix();
	}

	void Camera::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
	{
		switch (event)
		{
		case ComponentEvent::Camera_AddComponent:
			{
//                BROWSER_LOG("Camera_AddComponent Message received");
			}
			break;

		case ComponentEvent::Transform_AddComponent:
			{
//                BROWSER_LOG("Transform_AddComponent Message received");
			}
			break;
		}
	}

	
    
}
