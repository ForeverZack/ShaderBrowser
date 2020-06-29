#include "RenderSystem.h"
#include "CameraSystem.h"
#include "Browser/Components/Render/Commands/ChangeFrameBufferObjectCommand.h"
#include "Browser/Components/Render/Commands/MeshRenderCommand.h"
#include "Browser/Components/Render/Commands/SkinnedRenderCommand.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Browser/Components/BoundBox/AABBBoundBox.h"
#include "Browser/Components/Render/SkinnedMeshRenderer.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "Common/System/Cache/MaterialManager.h"
#include "Core/LogicCore.h"
#include "Core/RenderCore.h"
#include "GL/GPUResource/Shader/Material.h"
#include "GL/GPUResource/Shader/Pass.h"
#include "GL/GPUResource/Texture/RenderTexture.h"
#include "GL/GLStateCache.h"
#include <chrono>
#include <stdio.h>
#ifdef  _WIN32
	#pragma warning(disable:4996)
#endif //  _WIN32


// 开启渲染系统调试日志
//#define _SHADER_BROWSER_RENDER_SYSTEM_DEBUG 

namespace browser
{
    
    // 坐标轴
    // 显示缩放
    #define SHOW_AXIS_SCALE 0.5
    // vertices
    glm::vec3 axis_vertices[] = {
        // x_axis
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        // y_axis
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        // z_axis
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
    // colors
    glm::vec4 axis_colors[] = {
        // x_axis
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        // y_axis
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        // z_axis
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    };
    // 包围盒
    // 包围盒默认颜色
    #define SHOW_BOUND_BOX_COLOR glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
    
    
	RenderSystem::RenderSystem()
		: m_uDrawCalls(0)
		, m_uVerticesCount(0)
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::RenderSystem;
		m_bComponentMutex = true;
	}
    
    RenderSystem::~RenderSystem()
    {
    }

	void RenderSystem::init()
	{
        m_vRenderCommands.clear();
       

        // 生成坐标轴模型
        m_oAxisMesh = Mesh::create(6);
        m_oAxisMesh->setVertices(axis_vertices);
        m_oAxisMesh->setColors(axis_colors);
        m_oAxisMesh->retain();
        // 坐标轴缩放矩阵
        glm::vec3 axis_scale(SHOW_AXIS_SCALE, SHOW_AXIS_SCALE, SHOW_AXIS_SCALE);
        m_oAxisScaleMatrix = glm::scale(GLM_MAT4_UNIT, axis_scale);
	}
    
    void RenderSystem::flushRenders()
    {
		glClearColor(DEFAULT_GL_CLEAR_COLOR.r, DEFAULT_GL_CLEAR_COLOR.g, DEFAULT_GL_CLEAR_COLOR.b, DEFAULT_GL_CLEAR_COLOR.a);
		glClear(DEFAULT_GL_CLEAR_BIT);

        // 开启深度测试
        GLStateCache::getInstance()->openDepthTest();

		// 根据fbo拆分渲染命令
		std::vector<BaseRenderCommand*>::iterator s_itor, e_itor;
		s_itor = e_itor = m_vRenderCommands.begin();

		while (e_itor != m_vRenderCommands.end())
		{
			s_itor = e_itor;

			// 切换fbo
			if ((*s_itor)->getRenderType() == MeshRenderer::RendererType::RendererType_RenderTexture)
			{
				// 切换到渲染纹理的fbo
				(*s_itor)->execute();
				++s_itor;
				if (s_itor == m_vRenderCommands.end())
				{
					break;
				}
			}

			e_itor = s_itor;
			// 截取当前fbo的渲染命令  范围在[s_itor, e_itor)内，左闭右开
			for (; e_itor != m_vRenderCommands.end(); ++e_itor)
			{
				if ((*e_itor)->getRenderType() == MeshRenderer::RendererType::RendererType_RenderTexture)
				{
					break;
				}
			}
			// 合批	std::stable_sort(start_itor, end_itor):   [start_itor, end_itor) 左闭右开
			Material* mat1 = nullptr;
			Material* mat2 = nullptr;
			std::stable_sort(s_itor, e_itor, [&mat1, &mat2](BaseRenderCommand* c1, BaseRenderCommand* c2) {
				mat1 = static_cast<MeshRenderCommand*>(c1)->getMaterial();
				mat2 = static_cast<MeshRenderCommand*>(c2)->getMaterial();

				// RenderQueue > SharedId
				if (mat1->getRenderQueue() != mat2->getRenderQueue())
				{
					return mat1->getRenderQueue() < mat2->getRenderQueue();
				}
				else
				{
					return mat1->getSharedId() < mat1->getSharedId();
				}
			});
			// 渲染	[start_itor, end_itor)
			Material* lastMaterial = nullptr;
			Material* curMaterial = nullptr;
			MeshRenderCommand* command;
			for (auto itor=s_itor; itor!=e_itor; ++itor)
			{
				command = static_cast<MeshRenderCommand*>(*itor);

				curMaterial = command->getMaterial();
				if (command->getRenderType() == MeshRenderer::RendererType::RendererType_Mesh
					&& curMaterial->getSharedId() != 0
					&& lastMaterial == curMaterial
					&& command->getVertexCount() <= MAX_DYNAMIC_BATCH_VERTEX_COUNT)
				{
					// 可以合批的条件
					//                int iii = 0;
				}
				lastMaterial = curMaterial;

				//
				command->execute();

				// 删除命令
				delete command;
			}
		}
		
        
        // 重置
        m_vRenderCommands.clear();

		// 切换回默认fbo
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void RenderSystem::beforeUpdate(float deltaTime)
    {
		// 此方法在逻辑线程中！！！！!

        // 重置draw call
        m_uDrawCalls = 0;
        // 重置顶点数量
        m_uVerticesCount = 0;
        // 重置三角面数量
        m_uFaceCount = 0;
        
        // 渲染主相机场景
		auto cameras = CameraSystem::getInstance()->getActiveCameras();
		for (auto itor = cameras.begin(); itor != cameras.end(); ++itor)
		{
			renderScene((*itor));
		}

		// 重置所有材质UniformValue的dirty标记重置为false	(注意：材质的m_mUniforms应该都是在逻辑线程中修改的！！！)
		MaterialManager::getInstance()->resetAllMaterialsUniformsDirty();
    }
    
	void RenderSystem::update(float deltaTime)
	{
		// 此方法在渲染线程中！！！！!

		// 从逻辑线程拷贝渲染命令队列
		unsigned long frameIndex = core::RenderCore::getInstance()->getFrameIndex();
		m_vRenderCommands = getCommands(frameIndex);
		eraseCommands(frameIndex);

		// 递交渲染命令
		flushRenders();

		Camera* camera = CameraSystem::getInstance()->getMainCamera();
		// 渲染包围盒跟坐标轴
		renderAssistTools(camera);
	}

	void RenderSystem::renderScene(Camera* camera)
	{
		// TODO: 以后需要实现合批
		// 思考：什么情况下才可以合批
		// 1. 排序
		// 2. 阴影投射阶段，使用系统内置的shader，在cpu中计算裁剪坐标系下的坐标位置，所以全部可以合批，需要从光源和camera两个角度获取深度贴图，得到最后的shadermap（camera方向）。具体可以参考unity的framedebug
		// 3. 延迟渲染不透明物体（G-Buffer）：相同的mesh，使用了相同的纹理和Pass(实际上只要Pass里使用的GLProgram和uniform变量一样)，就可以合批
		// 4. 正向渲染透明物体
        if (camera)
        {         
			// 相机帧缓冲对象
			addCurFrameCommand(ChangeFrameBufferObjectCommand::create(camera));

			// 渲染
            switch(camera->getRenderPathType())
            {
                case Camera::RenderPathType::Forward:
                    {
#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
						auto timeRec = std::chrono::steady_clock::now();
						BROWSER_LOG("=======start=========");
#endif
                        
                        // 前向渲染
						forwardRenderScene(camera);

#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
						float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
						BROWSER_LOG("=======end========="+std::to_string(deltaTime)+"ms");
#endif
                    }
                    break;
                    
                case Camera::RenderPathType::Deffered:
                    {
                        // 延迟渲染
                    }
                    break;
            }
        }

	}
    
    void RenderSystem::renderAssistTools(Camera* camera)
    {
        GLuint vao;
        size_t vertCount;
        unsigned int* vbos = nullptr;
        BaseEntity* entity = nullptr;
        Transform* transform = nullptr;
        // 绘制调试信息
        // 1.包围盒
        // 开启深度测试
        GLStateCache::getInstance()->openDepthTest();
        {
            SkinnedMeshRenderer* skinnedRenderer = nullptr;
            BaseBoundBox* boundBox = nullptr;
            GLProgram* linesProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::DEFAULT_LINES_GLPROGRAM_NAME);
            for(auto itor = m_mComponentsList.begin(); itor!=m_mComponentsList.end(); ++itor)
            {
                skinnedRenderer = nullptr;
                entity = itor->first;
                if (entity->isRenderable() && entity->getIsVisible() && entity->getIsBoundBoxVisible() && entity->checkVisibility(camera))
                {
                    vao = m_oAxisMesh->getVAO();    // TODO: 这里使用的是模型坐标轴的vao,后面看看要不要换掉
                    vbos = m_oAxisMesh->getVBOs();
                    skinnedRenderer = entity->getComponent<SkinnedMeshRenderer>();
                    transform = entity->getComponent<Transform>();
                    boundBox = skinnedRenderer ? skinnedRenderer->getBoundBox() : entity->getComponent<AABBBoundBox>();
                    
                    BROWSER_ASSERT(boundBox, "Entity have no BoundBox compoent, block in function RenderSystem::renderScene(Camera* camera)");
                    // 显示包围盒的顶点
                    boundBox->calculateDisplayVertices();
                    
                    // 顶点属性
                    const std::vector<glm::vec3>& vertices = boundBox->getDisplayVertices();
                    vertCount = vertices.size();
                    
                    // 在CPU中处理顶点位置
                    glm::vec4* trans_vertices =  new glm::vec4[vertCount];
                    glm::vec4* trans_colors = new glm::vec4[vertCount];
                    for(int i=0; i<vertCount; ++i)
                    {
                        trans_vertices[i] = glm::vec4(vertices[i], 1.0f);
                        trans_colors[i] = SHOW_BOUND_BOX_COLOR;
                    }
                    
                    // 1.绑定对应的vao
                    glBindVertexArray(vao);
                    
                    // 2.传递顶点数据
                    glBindBuffer(GL_ARRAY_BUFFER, vbos[GLProgram::VERTEX_ATTR::VERTEX_ATTR_POSITION]);
                    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(glm::vec4), &trans_vertices[0], GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, vbos[GLProgram::VERTEX_ATTR::VERTEX_ATTR_COLOR]);
                    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(glm::vec4), &trans_colors[0], GL_STATIC_DRAW);
                    
                    
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                    
                    // 4.绘制
                    linesProgram->useProgram();
					if (camera)
					{
						linesProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_VIEW_MATRIX], camera->getViewMatrix());
						linesProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_PROJECTION_MATRIX], camera->getProjectionMatrix());
					}
                    glBindVertexArray(vao);
                    glDrawArrays(GL_LINES, 0, (int)vertCount);
                    glBindVertexArray(0);
                    
                    // 释放cpu的顶点数组
                    delete[] trans_vertices;
                    delete[] trans_colors;
                }
            }
        }
        
        // 2.坐标轴
        // 关闭深度测试
        GLStateCache::getInstance()->closeDepthTest();
        {
            GLProgram* linesProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::DEFAULT_LINES_GLPROGRAM_NAME);
            for(auto itor = m_mComponentsList.begin(); itor!=m_mComponentsList.end(); ++itor)
            {
                entity = itor->first;
                transform = entity->getComponent<Transform>();
                
//                if (entity->getIsVisible() && entity->getIsAxisVisible() && entity->checkVisibility(camera))
                if (entity->getIsVisible() && entity->getIsAxisVisible())
                {
                    vao = m_oAxisMesh->getVAO();
                    vbos = m_oAxisMesh->getVBOs();
                    
                    
                    // 顶点属性
                    glm::vec4* vertices = m_oAxisMesh->getVertices();
                    vertCount = m_oAxisMesh->getVertexCount();
                    
                    // 在CPU中处理顶点位置
                    glm::vec4* trans_vertices = new glm::vec4[vertCount];
                    glm::vec4* trans_colors = new glm::vec4[vertCount];
                    glm::mat4 noScaleModelMatrix = transform->traverseNoScaleModelMatrix();
                    for(int i=0; i<vertCount; ++i)
                    {
                        trans_vertices[i] = noScaleModelMatrix * m_oAxisScaleMatrix * vertices[i];
                        trans_colors[i] = axis_colors[i];
                    }
                    
                    // 1.绑定对应的vao
                    glBindVertexArray(vao);
                    
                    // 2.传递顶点数据
                    glBindBuffer(GL_ARRAY_BUFFER, vbos[GLProgram::VERTEX_ATTR::VERTEX_ATTR_POSITION]);
                    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(glm::vec4), &trans_vertices[0], GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, vbos[GLProgram::VERTEX_ATTR::VERTEX_ATTR_COLOR]);
                    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(glm::vec4), &trans_colors[0], GL_STATIC_DRAW);
                    
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                    
                    // 4.绘制
                    linesProgram->useProgram();
					if (camera)
					{
						linesProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_VIEW_MATRIX], camera->getViewMatrix());
						linesProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_PROJECTION_MATRIX], camera->getProjectionMatrix());
					}
                    glBindVertexArray(vao);
                    //typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
                    glDrawArrays(GL_LINES, 0, vertCount);
                    glBindVertexArray(0);
                    
                    // 释放cpu的顶点数组
                    delete[] trans_vertices;
                    delete[] trans_colors;
                }
            }
        }
    }
    
    void RenderSystem::addCurFrameCommand(BaseRenderCommand* command)
    {
        unsigned long frameIndex = core::LogicCore::getInstance()->getFrameIndex();
        m_mWaitRenderCommands[frameIndex].push_back(command);
    }
    
    const std::vector<BaseRenderCommand*> RenderSystem::getCommands(unsigned long frameIndex)
    {
        return m_mWaitRenderCommands[frameIndex];
    }
    
    void RenderSystem::eraseCommands(unsigned long frameIndex)
    {
        m_mWaitRenderCommands.erase(frameIndex);
    }
    
    void RenderSystem::forwardRenderScene(Camera* camera)
    {
#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
		auto timeRec = std::chrono::steady_clock::now();
		float deltaTime;
		float totalBufferDataTime = 0;
		float totalDrawTime = 0;
		float totalParamTime = 0;
		float totalUniformTime = 0;
#endif


#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
		timeRec = std::chrono::steady_clock::now();
#endif

        Material* material;
        size_t vertCount;
        MeshRenderer* render;
        SkinnedMeshRenderer* skinRenderer;
        Transform* transform;
        MeshFilter* meshFilter;
        Mesh* mesh;
        BaseEntity* entity;
		MeshRenderCommand* command;
        for (auto itor = m_mComponentsList.begin(); itor != m_mComponentsList.end(); ++itor)
        {
			const std::list<BaseComponent*>& renderList = itor->second;
            render = static_cast<MeshRenderer*>(*(renderList.begin()));
            entity = render->getBelongEntity();
            meshFilter = entity->getComponent<MeshFilter>();
            transform = entity->getComponent<Transform>();
            
            // 是否需要渲染
            if (!entity->getIsVisible() || !entity->checkVisibility(camera, true))
            {
                continue;
            }

#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
			deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
			totalParamTime += deltaTime;
#endif
            
            // 蒙皮渲染器
            skinRenderer = nullptr;
            if(render->getRendererType() == MeshRenderer::RendererType::RendererType_Skinned)
            {
                skinRenderer = static_cast<SkinnedMeshRenderer*>(render);
            }
            
            // 遍历mesh
            const std::vector<Mesh*>& meshes = skinRenderer ? skinRenderer->getMeshes() : meshFilter->getMeshes();
            for (int i = 0; i < meshes.size(); ++i)
            {
#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
				timeRec = std::chrono::steady_clock::now();
#endif

                mesh = meshes[i];
                material = render->getMaterialByIndex(i);
                vertCount = mesh->getVertexCount();
                
                
                // 生成渲染命令
                switch(render->getRendererType())
                {                       
                    case MeshRenderer::RendererType::RendererType_Skinned:
                        {
                            command = new SkinnedRenderCommand();
                            static_cast<SkinnedRenderCommand*>(command)->init(entity, material, mesh, transform, camera);
                        }
                        break;

					case MeshRenderer::RendererType::RendererType_Mesh:
					default:
						{
							command = new MeshRenderCommand();
							command->init(material, mesh, transform, camera);
						}
						break;
                }
                addCurFrameCommand(command);
                
                // 增加1次draw call
                ++m_uDrawCalls;
                // 增加顶点数量
                m_uVerticesCount += command->getVertexCount();
                // 增加三角面数量
                m_uFaceCount += command->getIndexCount() / 3;


#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
				deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
				timeRec = std::chrono::steady_clock::now();
				totalBufferDataTime += deltaTime;
#endif
                

#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
				deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
				timeRec = std::chrono::steady_clock::now();
				totalUniformTime += deltaTime;
#endif
   
//                // 递交渲染命令
//                if(m_uNoRenderVertices >= COMMIT_COMMAND_VERTEX_COUNT)
//                {
//                    flushRenders();
//                }

#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
				deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeRec).count() / 1000.0f;
				totalDrawTime += deltaTime;
#endif
            }
            
        }

#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG

		char *strTotalBufferDataTime = new char[100];
		sprintf(strTotalBufferDataTime, "===========totalBufferDataTime================%.3f ms", totalBufferDataTime);

		char *strTotalDrawTime = new char[100];
		sprintf(strTotalDrawTime, "===========totalDrawTime================%.3f ms", totalDrawTime);

		char *strTotalUniformTime = new char[100];
		sprintf(strTotalUniformTime, "===========totalUniformTime================%.3f ms", totalUniformTime);

		char *strTotalParamTime = new char[100];
		sprintf(strTotalParamTime, "===========totalParamTime================%.3f ms", totalParamTime);

		BROWSER_LOG(strTotalBufferDataTime);
		BROWSER_LOG(strTotalDrawTime);
		BROWSER_LOG(strTotalUniformTime);
		BROWSER_LOG(strTotalParamTime);
#endif
    }


}
