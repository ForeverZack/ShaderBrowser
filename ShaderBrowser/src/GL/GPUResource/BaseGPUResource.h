#pragma once

#include "Common/Components/Reference.h"
#include "Common/Tools/Thread/BaseThread.h"

using namespace common;

namespace customGL
{
	// gpu资源状态
	enum GPUResourceState
	{
		// 同步:
		// 未加载
		GRS_UnLoad = 0,
		// 加载中
		GRS_Loading,
		// 数据加载完成(同步加载的数据必须在渲染前立即在GPU中创建完)
		GRS_DataLoaded,
		// 资源加载完成
		GRS_Loaded,
	};

	// gpu资源类型
	enum GPUResourceType
	{
		// 未声明类型
		GRT_Undefined = 0,
		// 纹理2D
		GRT_Texture2D,
		// 网格
		GRT_Mesh,
		// 模型
		GRT_Model,
		// shader
		GRT_GLProgram,
		// 材质
		GRT_Material,
	};

	// GPU资源基类
	class BaseGPUResource : public common::Reference
	{
	public:
		BaseGPUResource();
		virtual ~BaseGPUResource() {}

	protected:
		// 创建gpu资源
		virtual void createGPUResource();
		// 更新gpu资源
		virtual void updateGPUResource();
		// 删除gpu资源
		virtual void deleteGPUResource();

	protected:
		// 资源加载状态
		MutexVariable<GPUResourceState> m_eResouceState;
		// 资源类型
		GPUResourceType m_eResourceType;
		// 数据脏标记
		bool m_bDirty;

	};

}

