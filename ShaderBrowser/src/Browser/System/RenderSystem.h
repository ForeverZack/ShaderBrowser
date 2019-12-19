#ifndef SYS_RENDERSYSTEM_H
#define SYS_RENDERSYSTEM_H

#include <glad/glad.h>
#include <unordered_map>
#include <vector>
#include "Common/System/BaseSystem.h"
#include "Browser/Components/Render/BaseRender.h"

#include "GL/GPUResource/Shader/GLProgram.h"
#include "Common/Tools/BaseSingleton.h"

namespace browser
{
    class BaseRenderCommand;
    
    // VBO 最大size
    static const int VBO_SIZE = 65536;
    // 渲染命令提交顶点数量节点
    static const unsigned int COMMIT_COMMAND_VERTEX_COUNT = 65536;
    // 动态合批支持的单个顶点模型最大数量
    static const unsigned int MAX_DYNAMIC_BATCH_VERTEX_COUNT = 1000;
    
	class Camera;
    
	class RenderSystem : public common::BaseSystem, public BaseSingleton<RenderSystem>
	{
	public:
        
		// vbo缓存类型
		enum VertexBufferType
        {
            // 0. 顶点位置缓冲
            RenderSystem_ArrayBuffer_Position = 0,
            // 1. 索引缓冲
            RenderSystem_Indices_Buffer,
            // 2. 顶点颜色缓冲
            RenderSystem_ArrayBuffer_Color,
            // 3. 主纹理uv缓冲
            RenderSystem_ArrayBuffer_UV1,
            // 4. 法线
            RenderSystem_ArrayBuffer_Normal,
            // 5. 切线
            RenderSystem_ArrayBuffer_Tangent,
            // 6. 骨骼id索引
            RenderSystem_ArrayBuffer_BoneIndices,
            // 7. 骨骼权重
            RenderSystem_ArrayBuffer_BoneWeights,
            
            
            // 最大值
            RenderSystem_Buffer_Maxcount,
		};

	public:
		RenderSystem();
		~RenderSystem();

		// init
		void init();
		// 清空渲染队列
		void clearRenders();
        // 遍历渲染队列并渲染
        void flushRenders();
		// 每帧更新
		void update(float deltaTime);
		// 绘制场景
		void renderScene(Camera* camera, float deltaTime);
        // 前向渲染
        void forwardRenderScene(Camera* camera, float deltaTime);
        
        // 设置vao和vbo
        void setupVAO(GLuint vao, unsigned int vbos[], const std::unordered_map<GLuint, VertexAttribDeclaration*>& declarations);
        
        
		REGISTER_PROPERTY_GET_SET(unsigned int, m_uDrawCalls, DrawCalls)
		REGISTER_PROPERTY_GET_SET(unsigned long, m_uVerticesCount, VerticesCount)
		REGISTER_PROPERTY_GET_SET(unsigned long, m_uFrameIndex, FrameIndex)
		REGISTER_PROPERTY_GET_SET(unsigned long, m_uFaceCount, FaceCount)
	
	private:
        // 渲染队列
        std::vector<BaseRenderCommand*> m_vRenderCommands;
        
        // 绘制所用的draw call次数
        unsigned int m_uDrawCalls;
		// 顶点数量
		unsigned long m_uVerticesCount;
		// 面数量
		unsigned long m_uFaceCount;
		// 当前帧
		unsigned long m_uFrameIndex;
        // 当前待渲染的顶点数量
        unsigned int m_uNoRenderVertices;
        
        // 坐标轴模型
        Mesh* m_oAxisMesh;
        // 坐标轴缩放矩阵
        glm::mat4 m_oAxisScaleMatrix;
	};
}

#endif
