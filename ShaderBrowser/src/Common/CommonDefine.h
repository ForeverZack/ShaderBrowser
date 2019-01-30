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
        Mesh,
		// 相机
		Camera,

		// 自定义的系统
		CustomSystem,
	};
}

#endif
