#include "SkinnedRenderCommand.h"
#include "Browser/System/RenderSystem.h"

namespace browser
{
	SkinnedRenderCommand::SkinnedRenderCommand()
	{
        // 记得修改渲染类型
        m_oRenderType = MeshRenderer::RendererType::RendererType_Skinned;
    }

	SkinnedRenderCommand::~SkinnedRenderCommand()
	{

	}

    void SkinnedRenderCommand::init(BaseEntity* entity, Material* material, Mesh* mesh, Transform* transform, Camera* camera, bool gpuInstance /*= false*/)
    {
		MeshRenderCommand::init(material, mesh, transform, camera, gpuInstance);


		// 更新骨骼矩阵samplerBuffer
		entity->useBonesMatrix(material);

		// 更新顶点数据 (TODO: 顶点数据应该在Mesh中更新，不要在渲染命令中检测)
		m_vVertices.clear();
		glm::vec4* vertices = entity->getVertices(mesh, m_bVerticesDirty);
		if (m_bVerticesDirty)
		{
			for (unsigned int i = 0; i < m_uVertexCount; ++i)
			{
				m_vVertices.push_back(vertices[i]);
			}
		}
		
	}
    
    void SkinnedRenderCommand::execute()
    {
        GLuint vao = m_oMesh->getVAO();
        
        
        if (m_bVerticesDirty)
        {
            // 1.绑定对应的vao
            glBindVertexArray(vao);
            
            // 2.传递顶点数据
            glBindBuffer(GL_ARRAY_BUFFER, m_oMesh->getVBOs()[GLProgram::VERTEX_ATTR::VERTEX_ATTR_POSITION]);
            glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * sizeof(glm::vec4), &m_vVertices[0], GL_DYNAMIC_DRAW);
            
            // normal
            
            // tangents

            
            //// 3.传递索引数组
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uVBOs[RenderSystem_Indices_Buffer]);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indexCount, &indices[0], GL_STATIC_DRAW);
            //
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        



		// 遍历pass并绘制
		glBindVertexArray(vao);
		size_t pass_count = m_oMaterial->getPassCount();
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
