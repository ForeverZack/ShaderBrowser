#include "BaseRenderCommand.h"

namespace browser
{
	BaseRenderCommand::BaseRenderCommand()
        : m_oRenderType(BaseRender::RendererType::Base)
        , m_oMaterial(nullptr)
        , m_oMesh(nullptr)
        , m_oTransform(nullptr)
        , m_oCamera(nullptr)
        , m_bGpuInstance(false)
		, m_bTransformDirty(false)
		, m_bCameraDirty(false)
	{
	}

	BaseRenderCommand::~BaseRenderCommand()
	{

	}
    
    int BaseRenderCommand::getVertexCount()
    {
        return m_oMesh->getVertexCount();
    }
    
    int BaseRenderCommand::getIndexCount()
    {
        return m_oMesh->getIndexCount();
    }

    void BaseRenderCommand::init(Material* material, Mesh* mesh, Transform* transform, Camera* camera, bool gpuInstance /*= false*/)
    {
        m_oMaterial = material;
        m_oMesh = mesh;
        m_oTransform = transform;
        m_oCamera = camera;
        m_bGpuInstance = gpuInstance;

		m_bTransformDirty = transform->getCurFrameDirty();
		if (m_bTransformDirty)
		{
			m_oModelMatrix = transform->getModelMatrix();
		}
		m_bCameraDirty = camera!=material->getCurCamera() || camera->getTransDirty();
		if (m_bCameraDirty)
		{
			material->setCurCamera(camera);
			m_oViewMatrix = camera->getViewMatrix();
			m_oProjectionMatrix = camera->getProjectionMatrix();
		}
    }
    
    void BaseRenderCommand::draw()
    {
        GLuint vao = m_oMesh->getVAO();
        int indexCount = m_oMesh->getIndexCount();
        m_oMaterial->useMaterial(m_bTransformDirty, m_oModelMatrix, m_bCameraDirty, m_oViewMatrix, m_oProjectionMatrix);

        // 5.绘制
        glBindVertexArray(vao);
        //typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)0);
        //            glDrawArrays(GL_TRIANGLES, 0, vertCount);
        glBindVertexArray(0);
    }
    
    

}
