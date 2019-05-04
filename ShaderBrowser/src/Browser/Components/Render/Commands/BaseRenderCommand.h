#pragma once

#include "Browser/Components/Render/Material.h"
#include "Browser/Components/Mesh/Mesh.h"
#include "Browser/Components/Camera/Camera.h"
#include "Browser/Components/Transform/Transform.h"
#include "Browser/Components/Render/BaseRender.h"


namespace browser
{
    class Material;
    class Mesh;
    class Transform;
    class Camera;
    

    // 渲染命令基类
    class BaseRenderCommand
	{
	public:
		BaseRenderCommand();
		~BaseRenderCommand();

	public:
        // 初始化渲染命令
        void init(Material* material, Mesh* mesh, Transform* transform, Camera* camera, bool gpuInstance = false);
        
        // 绘制
        virtual void draw();
        
        
        
        // 注意：后面可能会需要用进行命令的合并，这里的方法就需要重新写了
        int getVertexCount();
        int getIndexCount();
        
        
        REGISTER_PROPERTY_GET(Material*, m_oMaterial, Material);
        REGISTER_PROPERTY_GET(Mesh*, m_oMesh, Mesh);
        REGISTER_PROPERTY_GET(Transform*, m_oTransform, Transform);
        REGISTER_PROPERTY_GET(Camera*, m_oCamera, Camera);
        REGISTER_PROPERTY_GET(BaseRender::RendererType, m_oRenderType, RenderType);
        
        
	protected:
        // 类型
        BaseRender::RendererType m_oRenderType;
        // 材质
        Material* m_oMaterial;
        // 网格模型
        Mesh* m_oMesh;
        // Transfor组件
        Transform* m_oTransform;
        // 相机
        Camera* m_oCamera;
        // 是否使用gpuInstance
        bool m_bGpuInstance;
        
        
	};
    
}

