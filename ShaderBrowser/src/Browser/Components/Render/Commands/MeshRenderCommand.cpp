#include "MeshRenderCommand.h"

namespace browser
{
	MeshRenderCommand::MeshRenderCommand()
        : m_oMaterial(nullptr)
        , m_oMesh(nullptr)
        , m_bGpuInstance(false)
		, m_bVerticesDirty(false)
		, m_uVertexCount(0)
		, m_uIndexCount(0)
		, m_bTransformDirty(false)
		, m_bCameraDirty(false)
	{
		m_oRenderType = MeshRenderer::RendererType::RendererType_Mesh;
	}

	MeshRenderCommand::~MeshRenderCommand()
	{

	}
    
    int MeshRenderCommand::getVertexCount()
    {
        return m_oMesh->getVertexCount();
    }
    
    int MeshRenderCommand::getIndexCount()
    {
        return m_oMesh->getIndexCount();
    }

    void MeshRenderCommand::init(Material* material, Mesh* mesh, Transform* transform, Camera* camera, bool gpuInstance /*= false*/)
    {
        m_oMaterial = material;
        m_oMesh = mesh;
        m_bGpuInstance = gpuInstance;

		m_uVertexCount = mesh->getVertexCount();
		m_uIndexCount = mesh->getIndexCount();

		// 物体发生形变、位移、旋转；材质参数需要更新  （这些情况下都需要更新材质中的下列属性）
		m_bTransformDirty = transform->getCurFrameDirty() || material->getDirty();
		if (m_bTransformDirty)
		{
			m_oModelMatrix = transform->getModelMatrix();
		}
		// 材质相机跟当前不同；材质参数需要更新；相机位置、视口发生改变  （这些情况下都需要更新材质中的下列属性）
		m_bCameraDirty = camera!=material->getCurCamera() || material->getDirty() || material->getViewMatrixDirtyTag()!=camera->getViewMatrixDirtyTag() || material->getProjectionMatrixDirtyTag()!=camera->getProjectionMatrixDirtyTag();
		if (m_bCameraDirty)
		{
			material->setViewMatrixDirtyTag(camera->getViewMatrixDirtyTag());
			material->setProjectionMatrixDirtyTag(camera->getProjectionMatrixDirtyTag());

			material->setCurCamera(camera);
            m_oCameraGlobalPosition = camera->getGlobalPosition();
			m_oViewMatrix = camera->getViewMatrix();
			m_oProjectionMatrix = camera->getProjectionMatrix();
		}
		material->setDirty(false);	// 材质更新完成
        
        // 记录uniform    TODO: 这里的uniform是复制的，在所有的所有渲染命令都拷贝完material之后，应该将材质UniformValue的dirty重置为false (Tips: 可以试试独立了渲染线程之后，还需不需要这个dirty了)
		const std::unordered_map<std::string, UniformValue>& uniforms = material->getUniforms();
//		m_mUniforms = uniforms; // TODO: mac直接拷贝有问题
        m_mUniforms.clear();
        m_mUniforms.reserve(uniforms.size());
        for(auto itor=uniforms.begin(); itor!=uniforms.end(); ++itor)
        {
            m_mUniforms.emplace(itor->first, itor->second);
        }

		// 逻辑线程调用，防止autorelease先执行
		m_oMaterial->retain();
		m_oMesh->retain();
	}
    
    void MeshRenderCommand::execute()
    {
        GLuint vao = m_oMesh->getVAO();
        
		// 遍历pass并绘制
		size_t pass_count = m_oMaterial->getPassCount();
		glBindVertexArray(vao);
		for (int i = 0; i < pass_count; ++i)
		{
			// 使用材质
			m_oMaterial->useMaterial(m_bTransformDirty, m_oModelMatrix, m_bCameraDirty, m_oCameraGlobalPosition, m_oViewMatrix, m_oProjectionMatrix, m_mUniforms, i);

			// draw
			//typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
			glDrawElements(GL_TRIANGLES, m_uIndexCount, GL_UNSIGNED_SHORT, (void*)0);
			//            glDrawArrays(GL_TRIANGLES, 0, vertCount);
		}
		glBindVertexArray(0);


		// 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
		AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_oMaterial);
		AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_oMesh);
    }
    
    

}
