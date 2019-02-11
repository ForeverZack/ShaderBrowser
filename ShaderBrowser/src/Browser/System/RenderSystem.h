#ifndef SYS_RENDERSYSTEM_H
#define SYS_RENDERSYSTEM_H

#include <glad/glad.h>
#include <unordered_map>
#include "Common/System/BaseSystem.h"
#include "Browser/Components/Render/BaseRender.h"
#include "GL/GLProgram.h"
#include "Common/Tools/BaseSingleton.h"

namespace browser
{
    // VBO 最大size
    static const int VBO_SIZE = 65536;

	class Camera;
    
	class RenderSystem : public common::BaseSystem, public BaseSingleton<RenderSystem>
	{
		// vbo缓存类型
		enum VertexBufferType
        {
//            // 0. 顶点位置缓冲
//            RenderSystem_ArrayBuffer_Position = 0,
//            // 1. 索引缓冲
//            RenderSystem_ElementArray_Buffer,
//            // 2. 顶点颜色缓冲
//            RenderSystem_ArrayBuffer_Color,
//            // 3. 主纹理uv缓冲
//            RenderSystem_ArrayBuffer_UV1,
            
            // 0. 索引缓冲
            RenderSystem_Indices_Buffer,
            // 1. 顶点属性缓冲
            RenderSystem_Vertices_Buffer,
            
            
            // 最大值
            RenderSystem_Buffer_Maxcount,
		};

	public:
		RenderSystem();
		~RenderSystem() {};

		// init
		void init();
		// 清空渲染队列
		void clearRenders();
		// 每帧更新
		void update(float deltaTime);
		// 绘制场景
		void renderScene(Camera* camera);
        
        // 设置vao和vbo
        void setupVAO(GLuint vao);
        void setupVAO(GLuint vao, const std::unordered_map<GLuint, VertexAttribDeclaration*>& declarations);
        
        
        REGISTER_PROPERTY_GET_SET(unsigned int, m_uDrawCalls, DrawCalls)
	
	private:
		// vbos
		unsigned int m_uVBOs[RenderSystem_Buffer_Maxcount];
        // 绘制所用的draw call次数
        unsigned int m_uDrawCalls;
        
        // 坐标轴模型
        browser::Mesh* m_oAxisMesh;
        // 坐标轴缩放矩阵
        glm::mat4 m_oAxisScaleMatrix;
	};
}

#endif
