#include "SkinnedRenderCommand.h"
#include "Browser/System/RenderSystem.h"

namespace browser
{
	SkinnedRenderCommand::SkinnedRenderCommand()
        : m_oEntity(nullptr)
	{
        // 记得修改渲染类型
        m_oRenderType = BaseRender::RendererType::Skinned;
    }

	SkinnedRenderCommand::~SkinnedRenderCommand()
	{

	}

    void SkinnedRenderCommand::init(BaseEntity* entity, Material* material, Mesh* mesh, Transform* transform, Camera* camera, bool gpuInstance /*= false*/)
    {
		BaseRenderCommand::init(material, mesh, transform, camera, gpuInstance);

		m_oEntity = entity;        
    }
    
    void SkinnedRenderCommand::draw()
    {
        GLuint vao = m_oMesh->getVAO();
        
        bool verticesDirty = false;
        // 顶点属性
        glm::vec4* vertices = m_oEntity->getVertices(m_oMesh, verticesDirty);
        int vertCount = m_oMesh->getVertexCount();
        // 顶点索引数组
        int indexCount = m_oMesh->getIndexCount();
        
        if (verticesDirty)
        {
            // 1.绑定对应的vao
            glBindVertexArray(vao);
            
            // 2.传递顶点数据
            glBindBuffer(GL_ARRAY_BUFFER, m_oMesh->getVBOs()[RenderSystem::VertexBufferType::RenderSystem_ArrayBuffer_Position]);
            glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(glm::vec4), &vertices[0], GL_STATIC_DRAW);
            
            // normal
            
            // tangents

            
            //// 3.传递索引数组
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uVBOs[RenderSystem_Indices_Buffer]);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indexCount, &indices[0], GL_STATIC_DRAW);
            //
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        
        // 4.使用材质
        m_oEntity->useBonesMatrix(m_oMaterial);
        m_oMaterial->useMaterial(m_bTransformDirty, m_oModelMatrix, m_bCameraDirty, m_oViewMatrix, m_oProjectionMatrix);
        
        // 5.绘制
        glBindVertexArray(vao);
        //typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)0);
        //            glDrawArrays(GL_TRIANGLES, 0, vertCount);
        glBindVertexArray(0);
    }

}
