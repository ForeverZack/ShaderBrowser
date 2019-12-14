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
		// 纹理缓存
		GRT_TextureBuffer,
		// 网格
		GRT_Mesh,
		// 模型
		GRT_Model,
		// shader
		GRT_GLProgram,
	};

	// GPU资源基类
	class BaseGPUResource : public common::Reference
	{
	public:
		BaseGPUResource();
		virtual ~BaseGPUResource() {}

	public:
		// 引用计数-1，并在引用计数=0的时候判断是否立即释放
		virtual void release();

	protected:
		// 创建gpu资源
		virtual void createGPUResource();
		// 更新gpu资源
		virtual void updateGPUResource();
		// 删除gpu资源 (因为要在release()调用的时候，判断是否释放GPU资源，所以这个函数是纯虚函数，子类一定要实现！！！)
		virtual void deleteGPUResource() = 0;


		REGISTER_PROPERTY_GET_SET(bool, m_bGPUDeleted, GPUDeleted)

	protected:
		// 资源加载状态
		MutexVariable<GPUResourceState> m_eResouceState;
		// 资源类型
		GPUResourceType m_eResourceType;
		// 数据脏标记
		bool m_bDirty;
		// GPU上是否被删除 (release的时候要先释放GPU资源，才能释放CPU资源)
		bool m_bGPUDeleted;
	};

}

