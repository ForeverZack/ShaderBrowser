#pragma once

#include "BaseRenderCommand.h"
#include "Browser/Entitys/BaseEntity.h"


namespace browser
{
    class BaseEntity;

    // 渲染命令基类
    class SkinnedRenderCommand : public BaseRenderCommand
	{
	public:
		SkinnedRenderCommand();
		~SkinnedRenderCommand();

	public:
        // 初始化渲染命令
        void init(BaseEntity* entity, Material* material, Mesh* mesh, Transform* transform, Camera* camera, bool gpuInstance = false);
        
        // 绘制
        virtual void draw();
        
        
	protected:
        // 实体
        BaseEntity* m_oEntity;
        
        
	};
    
}

