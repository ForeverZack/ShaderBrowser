#include "BaseRenderCommand.h"

namespace browser
{
	BaseRenderCommand::BaseRenderCommand()
        : m_oRenderType(BaseRender::RendererType::Base)
        , m_oMaterial(nullptr)
        , m_oMesh(nullptr)
        , m_bGpuInstance(false)
		, m_bVerticesDirty(false)
		, m_uVertexCount(0)
		, m_uIndexCount(0)
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
        m_bGpuInstance = gpuInstance;

		m_uVertexCount = mesh->getVertexCount();
		m_uIndexCount = mesh->getIndexCount();

		m_bTransformDirty = true;// transform->getCurFrameDirty();
		if (m_bTransformDirty)
		{
			m_oModelMatrix = transform->getModelMatrix();
		}
		m_bCameraDirty = camera != material->getCurCamera() || camera->getTransDirty();
		if (m_bCameraDirty)
		{
			material->setCurCamera(camera);
			m_oViewMatrix = camera->getViewMatrix();
			m_oProjectionMatrix = camera->getProjectionMatrix();
		}
        
        // 记录uniform    TODO: 这里的uniform是复制的，在所有的所有渲染命令都拷贝完material之后，应该将材质UniformValue的dirty重置为false (Tips: 可以试试独立了渲染线程之后，还需不需要这个dirty了)
		const std::unordered_map<std::string, UniformValue>& uniforms = material->getUniforms();
//		m_mUniforms = uniforms; // TODO: mac直接拷贝有问题
        m_mUniforms.clear();
        m_mUniforms.reserve(uniforms.size());
        for(auto itor=uniforms.begin(); itor!=uniforms.end(); ++itor)
        {
            m_mUniforms.emplace(itor->first, itor->second);
        }
    }
    
    void BaseRenderCommand::draw()
    {
        GLuint vao = m_oMesh->getVAO();
        m_oMaterial->useMaterial(m_bTransformDirty, m_oModelMatrix, m_bCameraDirty, m_oViewMatrix, m_oProjectionMatrix, m_mUniforms);

        // 5.绘制
        glBindVertexArray(vao);
        //typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
        glDrawElements(GL_TRIANGLES, m_uIndexCount, GL_UNSIGNED_SHORT, (void*)0);
        //            glDrawArrays(GL_TRIANGLES, 0, vertCount);
        glBindVertexArray(0);
    }
    
    

}
