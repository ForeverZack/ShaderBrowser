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
		RenderSystem();
		~RenderSystem();

		// init
		void init();
        // 遍历渲染队列并渲染
        void flushRenders();
        // 渲染前（逻辑线程做好渲染准备）
        virtual void beforeUpdate(float deltaTime);
        // 每帧更新 (渲染线程拷贝渲染命令队列)
		void update(float deltaTime);
        // 渲染 (渲染线程执行渲染命令)
        void afterUpdate(float deltaTime);
		// 绘制场景
		void renderScene(Camera* camera, float deltaTime);
        // 前向渲染
        void forwardRenderScene(Camera* camera, float deltaTime);

        
		REGISTER_PROPERTY_GET_SET(unsigned int, m_uDrawCalls, DrawCalls)
		REGISTER_PROPERTY_GET_SET(unsigned long, m_uVerticesCount, VerticesCount)
		REGISTER_PROPERTY_GET_SET(unsigned long, m_uFaceCount, FaceCount)
        
    private:
        // 画出辅助工具
        void renderAssistTools(Camera* camera);
        // 添加渲染命令
        void addCurFrameCommand(BaseRenderCommand* command);
        // 获取渲染命令队列
        const std::vector<BaseRenderCommand*> getCommands(unsigned long frameIndex);
        // 删除渲染命令
        void eraseCommands(unsigned long frameIndex);
	
	private:
        // 渲染队列 (渲染线程)
        // 注意：这里没有和GPUOperateSystem一样使用MutexQueue，因为这里的队列只在逻辑线程和渲染线程中使用。它们的复制过程只允许发生在逻辑线程一帧结束，并且渲染线程刚刚开始时，这里逻辑线程会被锁住，只有渲染线程在跑
        std::vector<BaseRenderCommand*> m_vRenderCommands;
        // 渲染队列队列 (逻辑线程)
        MutexUnorderedMap<unsigned long, std::vector<BaseRenderCommand*>> m_mWaitRenderCommands;
        
        // 绘制所用的draw call次数
        unsigned int m_uDrawCalls;
		// 顶点数量
		unsigned long m_uVerticesCount;
		// 面数量
		unsigned long m_uFaceCount;
		
        
        // 坐标轴模型
        Mesh* m_oAxisMesh;
        // 坐标轴缩放矩阵
        glm::mat4 m_oAxisScaleMatrix;
	};
}

#endif
