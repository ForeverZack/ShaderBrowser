#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/GPUResource/Shader/Material.h"
#include "GL/GPUResource/Model/Mesh.h"
#include "Browser/Components/Camera/Camera.h"
#include "Browser/Components/Transform/Transform.h"
#include "Browser/Components/Render/MeshRenderer.h"
#include "Browser/Components/Render/Commands/BaseRenderCommand.h"

using namespace customGL;

namespace browser
{
    class Transform;
    class Camera;
    

    // 网格渲染命令
    class MeshRenderCommand : public BaseRenderCommand
	{
	public:
		MeshRenderCommand();
		virtual ~MeshRenderCommand();

	public:
        // 初始化渲染命令
        void init(Material* material, Mesh* mesh, Transform* transform, Camera* camera, bool gpuInstance = false);
        
        // 绘制
        virtual void execute();
        
        
        
        // 注意：后面可能会需要用进行命令的合并，这里的方法就需要重新写了
        int getVertexCount();
        int getIndexCount();
        
        
        REGISTER_PROPERTY_GET(Material*, m_oMaterial, Material);
        REGISTER_PROPERTY_GET(Mesh*, m_oMesh, Mesh);
        
        
	protected:

        // 材质
        Material* m_oMaterial;
        // 网格模型
        Mesh* m_oMesh;
        // 是否使用gpuInstance
        bool m_bGpuInstance;
        

		// 顶点数据脏标记
		bool m_bVerticesDirty;
		// 顶点数据
		std::vector<glm::vec4> m_vVertices;
		// 顶点数量
		unsigned int m_uVertexCount;
		// 索引数量
		unsigned int m_uIndexCount;
        // Transform脏标记(为true时，model矩阵才有意义)
		bool m_bTransformDirty;
		// model矩阵
		glm::mat4 m_oModelMatrix;
		// Camera脏标记(为true时，view projection矩阵才有意义)
		bool m_bCameraDirty;
        // 相机位置
        glm::vec3 m_oCameraGlobalPosition;
		// view, projection矩阵
		glm::mat4 m_oViewMatrix;
		glm::mat4 m_oProjectionMatrix;
		// view, projection 脏标记
		unsigned short m_uViewMatrixDirtyTag;
		unsigned short m_uProjectionDirtyTag;
		// 材质属性
		std::unordered_map<std::string, UniformValue> m_mUniforms;
	};
    
}

