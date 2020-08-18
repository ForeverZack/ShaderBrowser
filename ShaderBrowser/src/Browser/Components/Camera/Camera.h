#pragma once

#include "Browser/Components/BaseComponent.h"
#include "Browser/Components/Transform/Transform.h"
#include "Common/Tools/Utils.h"
#include "GL/GPUResource/Texture/RenderTexture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace common;
using namespace customGL;

namespace browser
{
	/*
		相机只有两种情况下会被启用：1.相机的渲染纹理不为空的相机；2.渲染纹理为空的相机列表中Depth最大的相机。
		第1种情况下，相机的视口大小与渲染纹理大小保持一致；第2种情况下，相机的视口大小为游戏窗口的大小。
	*/

	/*
		相机的脏标记是以unsigned short来处理的，每当相机的位置或者视口发生变化时，脏标记都会更新。这样Material可以记录下来，对比Camera跟Material的脏标记，来判断需不需要更新Material的参数。
	*/
	class Camera : public BaseComponent
	{
	public:
		static EnumComponentClass ComponentClass;

	public:
		// 脏标记数值范围
		#define VALUE_DIRTY_TAG_MAX 255
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
		static Camera* create(ProjectionType type=ProjectionType::Ortho, float nearPlane=0.3f, float farPlane=1000.0f, float FOV = 60.0f, int viewportWidth=800, int viewportHeight=600);

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

		// 计算脏标记
		void generateViewMatrixDirtyTag();
		void generateProjectionMatrixDirtyTag();

		REGISTER_PROPERTY_CONSTREF_GET(glm::vec3, m_oGlobalPosition, GlobalPosition)
		REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oViewMatrix, ViewMatrix)
		REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oProjectionMatrix, ProjectionMatrix)
		REGISTER_PROPERTY_GET_SET(RenderPathType, m_eRenderPathType, RenderPathType)
		REGISTER_PROPERTY_GET(unsigned short, m_uViewMatrixDirtyTag, ViewMatrixDirtyTag)
		REGISTER_PROPERTY_GET(unsigned short, m_uProjectionMatrixDirtyTag, ProjectionMatrixDirtyTag)
		REGISTER_PROPERTY_GET(RenderTexture*, m_pRenderTexture, RenderTexture)
		REGISTER_PROPERTY_GET(int, m_iDepth, Depth)
		REGISTER_PROPERTY_GET(int, m_iViewportWidth, ViewportWidth)
		REGISTER_PROPERTY_GET(int, m_iViewportHeight, ViewportHeight)
		void setDepth(int depth);
		REGISTER_PROPERTY_CONSTREF_GET(glm::vec4, m_oBackgroundColor, BackgroundColor)
		REGISTER_PROPERTY_GET_SET(bool, m_bIsRenderable, IsRenderable)
		void setRenderTexture(RenderTexture* rt);
		void setViewportSize(int width, int height);

	protected:
		// view matrix 相关
		// view matrix
		glm::mat4 m_oViewMatrix;
        // 相机世界位置
        glm::vec3 m_oGlobalPosition;
		// view matrix脏标记 (包含 m_oViewMatrix 和 m_oGlobalPosition 的变动)
		unsigned short m_uViewMatrixDirtyTag;
        
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
		// projection matrix脏标记 (包含 m_oProjectionMatrix 的变动)
		unsigned short m_uProjectionMatrixDirtyTag;
        
        // 渲染路径
        RenderPathType m_eRenderPathType;
		// 渲染纹理
		RenderTexture* m_pRenderTexture;
		// 相机深度
		int m_iDepth;
		// 背景颜色 (清除颜色)
		glm::vec4 m_oBackgroundColor;
		// 是否渲染 (用于特殊相机处理)
		bool m_bIsRenderable;
	};
}

