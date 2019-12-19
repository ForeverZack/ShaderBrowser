#include "RenderSystem.h"
#include "CameraSystem.h"
#include "Browser/Components/Render/Commands/BaseRenderCommand.h"
#include "Browser/Components/Render/Commands/SkinnedRenderCommand.h"
#include "GL/GPUResource/Shader/Material.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Browser/Components/BoundBox/AABBBoundBox.h"
#include "Browser/Components/Render/SkinnedMeshRenderer.h"
#include "Browser/Components/Animation/Animator.h"
#include "GL/GPUResource/Shader/Pass.h"
#include "Common/System/Cache/GLProgramCache.h"
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
		, m_uFrameIndex(0)
        , m_uNoRenderVertices(0)
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
		clearRenders();

        m_vRenderCommands.clear();
       

        // 生成坐标轴模型
        m_oAxisMesh = Mesh::create(6);
        m_oAxisMesh->addVertexAttribute(GLProgram::VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), axis_vertices);
        m_oAxisMesh->addVertexAttribute(GLProgram::VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), axis_colors);
        m_oAxisMesh->setupVAO();
        m_oAxisMesh->retain();
        // 坐标轴缩放矩阵
        glm::vec3 axis_scale(SHOW_AXIS_SCALE, SHOW_AXIS_SCALE, SHOW_AXIS_SCALE);
        m_oAxisScaleMatrix = glm::scale(GLM_MAT4_UNIT, axis_scale);
	}

	void RenderSystem::clearRenders()
	{
        m_vRenderCommands.clear();
        
//        for (auto itor = m_mComponentsList.begin(); itor != m_mComponentsList.end(); ++itor)
//        {
//            itor->second.clear();
//        }
//        m_mComponentsList.clear();
	}
    
    void RenderSystem::flushRenders()
    {
        std::stable_sort(std::begin(m_vRenderCommands), std::end(m_vRenderCommands), [](BaseRenderCommand* c1, BaseRenderCommand* c2) {
            return c1->getMaterial()->getSharedId() < c2->getMaterial()->getSharedId();
        });
        
        Material* lastMaterial = nullptr;
        Material* curMaterial = nullptr;
        BaseRenderCommand* command;
        for(int i=0; i<m_vRenderCommands.size(); ++i)
        {
            command = m_vRenderCommands[i];
            
            curMaterial = command->getMaterial();
            if(command->getRenderType() == BaseRender::RendererType::Base
               && curMaterial->getSharedId() != 0
               && lastMaterial == curMaterial
               && command->getVertexCount() <= MAX_DYNAMIC_BATCH_VERTEX_COUNT)
            {
                // 可以合批的条件
//                int iii = 0;
            }
            lastMaterial = curMaterial;
            
            //
            command->draw();
            
            // 增加1次draw call
            ++m_uDrawCalls;
            // 增加顶点数量
            m_uVerticesCount += command->getVertexCount();
            // 增加三角面数量
            m_uFaceCount += command->getIndexCount() / 3;
            
            // 删除命令
            delete command;
        }
        
        
        // 重置
        m_vRenderCommands.clear();
        m_uNoRenderVertices = 0;
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
    
    void RenderSystem::setupVAO(GLuint vao, unsigned int vbos[], const std::unordered_map<GLuint, VertexAttribDeclaration*>& declarations)
    {
        // 1.绑定vao和vbo
        glBindVertexArray(vao);
        
        
        // 2.设置顶点属性
        // 在glBindBuffer后使用glVertexAttribPointer，会让该vbo(刚绑定的)绑定到对应的vertex attr上
        for (auto itor=declarations.cbegin(); itor!=declarations.cend(); ++itor)
        {
            const VertexAttribDeclaration* declaration = itor->second;
            switch(declaration->index)
            {
				case GLProgram::VERTEX_ATTR_POSITION:	// 1.顶点位置
				case GLProgram::VERTEX_ATTR_COLOR:	// 2.顶点颜色
				case GLProgram::VERTEX_ATTR_TEX_COORD:	// 3.主纹理坐标
				case GLProgram::VERTEX_ATTR_NORMAL:		// 4.法线
				case GLProgram::VERTEX_ATTR_TANGENT:	// 5.切线
				case GLProgram::VERTEX_ATTR_BONE_WEIGHTS:	// 7.骨骼权重
                    {
                        // 1.顶点位置
                        glBindBuffer(GL_ARRAY_BUFFER, vbos[declaration->index]);
                        glVertexAttribPointer(declaration->index, declaration->size, declaration->type, declaration->normalized, declaration->stride, (void*)0);
                    }
                    break;
				case GLProgram::VERTEX_ATTR_BONE_IDS:
					{
						// 6.骨骼id
						// 注意！！！这里要用glVertexAttribIPointer来传递int值，不然都是float类型的，索引数组会找不到
                        glBindBuffer(GL_ARRAY_BUFFER, vbos[declaration->index]);
						glVertexAttribIPointer(declaration->index, declaration->size, declaration->type, declaration->stride, (void*)0);
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
        // 重置draw call
        m_uDrawCalls = 0;
		// 重置顶点数量
		m_uVerticesCount = 0;
		// 重置三角面数量
		m_uFaceCount = 0;
        // 重置待渲染顶点数量
        m_uNoRenderVertices = 0;
        
		// 渲染主相机场景
		renderScene(CameraSystem::getInstance()->getMainCamera(), deltaTime);

		// 当前第几帧
		++m_uFrameIndex;
	}

	void RenderSystem::renderScene(Camera* camera, float deltaTime)
	{
		//	清理FrameBuffer
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // 开启深度测试
        GLStateCache::getInstance()->openDepthTest();

		// TODO: 以后需要实现合批
		// 思考：什么情况下才可以合批
		// 1. 排序
		// 2. 阴影投射阶段，使用系统内置的shader，在cpu中计算裁剪坐标系下的坐标位置，所以全部可以合批，需要从光源和camera两个角度获取深度贴图，得到最后的shadermap（camera方向）。具体可以参考unity的framedebug
		// 3. 延迟渲染不透明物体（G-Buffer）：相同的mesh，使用了相同的纹理和Pass(实际上只要Pass里使用的GLProgram和uniform变量一样)，就可以合批
		// 4. 正向渲染透明物体
        if (camera)
        {
            // 清空渲染队列
            clearRenders();
            
            switch(camera->getRenderPathType())
            {
                case Camera::RenderPathType::Forward:
                    {
#ifdef _SHADER_BROWSER_RENDER_SYSTEM_DEBUG
						auto timeRec = std::chrono::steady_clock::now();
						BROWSER_LOG("=======start=========");
#endif
                        
                        // 前向渲染
						forwardRenderScene(camera, deltaTime);

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
                    linesProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_VIEW_MATRIX], camera->getViewMatrix());
                    linesProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_PROJECTION_MATRIX], camera->getProjectionMatrix());
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
                    glm::vec4* vertices = m_oAxisMesh->getVertices22();
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
                    linesProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_VIEW_MATRIX], camera->getViewMatrix());
                    linesProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_PROJECTION_MATRIX], camera->getProjectionMatrix());
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
    
    void RenderSystem::forwardRenderScene(Camera* camera, float deltaTime)
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
        GLuint vao;
        size_t vertCount;
        int indexCount;
        BaseRender* render;
        SkinnedMeshRenderer* skinRenderer;
        Transform* transform;
        MeshFilter* meshFilter;
        Mesh* mesh;
        BaseEntity* entity;
		Animator* animator;
        BaseRenderCommand* command;
		bool verticesDirty;
        for (auto itor = m_mComponentsList.begin(); itor != m_mComponentsList.end(); ++itor)
        {
			const std::list<BaseComponent*>& renderList = itor->second;
            render = static_cast<BaseRender*>(*(renderList.begin()));
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
            if(render->getRendererType() == BaseRender::RendererType::Skinned)
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
                    case BaseRender::RendererType::Base:
                        {
                            command = new BaseRenderCommand();
                            command->init(material, mesh, transform, camera);
                        }
                        break;
                        
                    case BaseRender::RendererType::Skinned:
                        {
                            command = new SkinnedRenderCommand();
                            static_cast<SkinnedRenderCommand*>(command)->init(entity, material, mesh, transform, camera);
                        }
                        break;
                }
                m_vRenderCommands.push_back(command);
                m_uNoRenderVertices += vertCount;


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
        
        // 递交渲染命令
        flushRenders();


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
