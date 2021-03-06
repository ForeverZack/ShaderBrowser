#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H

namespace common
{
	// 系统类型
	enum SystemType
	{
		// 特殊的系统类型
		// 渲染系统
		RenderSystem = 0,
        // 形变位移系统
        Transform,
        // 网格系统
        MeshFilter,
		// 相机
		Camera,
        // 包围盒
        BoundBox,
        // 光照
        Light,
        // 动画
        Animation,


		// 自定义的系统
		CustomSystem,
	};
    
    // 渲染队列
	// 注意：添加修改RenderQueue后，MaterialParser::m_mString2RenderQueue也需要修改
    enum RenderQueue
    {
        // 背景
        Background = 0,
        // 天空盒
        SkyBox = 5,
        
        // 不透明
        Opaque = 1000,
        
        // 透明
        Transparent = 3000,
    };
    
}

#endif
