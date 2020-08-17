#include "Camera.h"
#include "GL/GLDefine.h"
#include "Browser/Entitys/BaseEntity.h"
#include "Browser/System/CameraSystem.h"

namespace browser
{
	EnumComponentClass Camera::ComponentClass = EnumComponentClass::ECC_Camera;

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
			m_oViewMatrix = glm::lookAt(camera_position, camera_target, camera_up);
            m_oGlobalPosition = transform->getGlobalPosition();

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
				m_oProjectionMatrix = glm::perspective(glm::radians(m_fFieldOfView), (float)m_iViewportWidth/(float)m_iViewportHeight, m_fNearPlane, m_fFarPlane);
			}
			break;

		case ProjectionType::Ortho:
			{
				// 正交投影
				m_oProjectionMatrix = glm::ortho(0.0f, (float)m_iViewportWidth, 0.0f, (float)m_iViewportHeight, m_fNearPlane, m_fFarPlane);
			}
			break;
		}

		// 更新脏标记
		generateProjectionMatrixDirtyTag();
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
