#include "RenderSystem.h"
#include "../Components/Render/Material.h"
#include "../Components/Mesh/MeshFilter.h"

namespace browser
{
	static RenderSystem* m_oInstance = nullptr;

	RenderSystem* RenderSystem::getInstance()
	{
		if (!m_oInstance)
		{
			m_oInstance = new RenderSystem();
		}
		return m_oInstance;
	}

	RenderSystem::RenderSystem()
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::RenderSystem;
		m_bComponentMutex = true;
	}

	void RenderSystem::init()
	{
		clearRenders();

		// 生成VBO
		glGenBuffers(RenderSystem_Buffer_Maxcount, m_uVBOs);
        
	}

	void RenderSystem::clearRenders()
	{
		for (auto itor = m_mComponentsList.begin(); itor != m_mComponentsList.end(); ++itor)
		{
			itor->second.clear();
		}
		m_mComponentsList.clear();
	}
    
//    void RenderSystem::setupVAO(GLuint vao)
//    {
//        glBindVertexArray(vao);
//        
//        // 1.顶点位置
//        glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[RenderSystem_ArrayBuffer_Position]);
//        glVertexAttribPointer(GLProgram::VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
//        glEnableVertexAttribArray(GLProgram::VERTEX_ATTR_POSITION);
//        
//        // 2.顶点颜色
//        glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[RenderSystem_ArrayBuffer_Color]);
//        glVertexAttribPointer(GLProgram::VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
//        glEnableVertexAttribArray(GLProgram::VERTEX_ATTR_COLOR);
//        
//        // 3.主纹理坐标
//        glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[RenderSystem_ArrayBuffer_UV1]);
//        glVertexAttribPointer(GLProgram::VERTEX_ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
//        glEnableVertexAttribArray(GLProgram::VERTEX_ATTR_TEX_COORD);
//        
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//    }
    
    void RenderSystem::setupVAO(GLuint vao, const std::unordered_map<GLuint, VertexAttribDeclaration*>& declarations)
    {
        // 1.绑定vao和vbo
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[RenderSystem_Vertices_Buffer]);
        
        // 2.设置顶点属性
        for (auto itor=declarations.cbegin(); itor!=declarations.cend(); ++itor)
        {
            const VertexAttribDeclaration* declaration = itor->second;
            switch(declaration->index)
            {
                case GLProgram::VERTEX_ATTR_POSITION:
                    {
                        // 1.顶点位置
                        glVertexAttribPointer(declaration->index, declaration->size, declaration->type, declaration->normalized, declaration->stride, (void*)offsetof(VertexData, position));
                    }
                    break;
                case GLProgram::VERTEX_ATTR_COLOR:
                    {
                        // 2.顶点颜色
                        glVertexAttribPointer(declaration->index, declaration->size, declaration->type, declaration->normalized, declaration->stride, (void*)offsetof(VertexData, color));
                    }
                    break;
                case GLProgram::VERTEX_ATTR_TEX_COORD:
                    {
                        // 3.主纹理坐标
                        glVertexAttribPointer(declaration->index, declaration->size, declaration->type, declaration->normalized, declaration->stride, (void*)offsetof(VertexData, uv_main));
                    }
                    break;
            }
            
            glEnableVertexAttribArray(declaration->index);
        }
        
        // 3.解绑vao和vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

	void RenderSystem::update(float deltaTime)
	{
		//	清理FrameBuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// TODO: 以后需要实现合批
		Material* material;
		GLuint vao;
		int vertCount;
		int indexCount;
		BaseRender* render;
        MeshFilter* meshFilter;
		for (auto itor = m_mComponentsList.begin(); itor != m_mComponentsList.end(); ++itor)
		{
			const std::list<BaseComponent*>& renderList = itor->second;
			render = static_cast<BaseRender*>(*(renderList.begin()));
            meshFilter = static_cast<MeshFilter*>(render->getBelongEntity()->getMeshFilter());
            vao = meshFilter->getVAO();
			material = render->getMaterial();
            
            
            // 顶点属性
            const std::vector<VertexData>& vertices = meshFilter->getVertices();
            vertCount = meshFilter->getVertexCount();
            // 顶点索引数组
            const std::vector<GLushort>& indices = meshFilter->getIndices();
            indexCount = meshFilter->getIndexCount();

			// 1.绑定对应的vao
			glBindVertexArray(vao);
			
            // 2.传递顶点数据
            glBindBuffer(GL_ARRAY_BUFFER, m_uVBOs[RenderSystem_Vertices_Buffer]);
			glBufferData(GL_ARRAY_BUFFER, vertCount*sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);
            
            // 3.传递索引数组
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uVBOs[RenderSystem_Indices_Buffer]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indexCount, &indices[0], GL_STATIC_DRAW);
            
            
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			// 4.绘制
			material->useMaterial(meshFilter);
			glBindVertexArray(vao);
            //typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)0);
            //            glDrawArrays(GL_TRIANGLES, 0, vertCount);
            glBindVertexArray(0);
        }
	}

	void RenderSystem::batchRenderScene()
	{

	}


}
