#pragma once

#include <vector>
#include "Common/Tools/Utils.h"
#include "GL/GLDefine.h"
#include "Browser/Components/BaseComponent.h"
#include "GL/GPUResource/Model/Mesh.h"

using namespace common;
using namespace customGL;

namespace browser
{
	class MeshFilter : public BaseComponent
	{
	public:
		static EnumComponentClass ComponentClass;

    public:
        static MeshFilter* create();
        
	public:
		MeshFilter();
		~MeshFilter();
        
        virtual Reference* clone();

	public:
		// 添加网格模型
		// 注意：在添加网格模型的同时，会为每个网格模型在Render组件上添加一个Material与之对应，并且可以通过mesh的名称去找到这个material来供渲染系统使用
		void addMesh(Mesh* mesh);

		// 重载属性面板显示方法
		virtual void onInspectorGUI(InspectorPanel* inspector);
        

		REGISTER_PROPERTY_CONSTREF_GET(std::vector<Mesh*>, m_vMeshes, Meshes)

    private:
		// 
        
        
	protected:
		// 网格模型列表
        // tips: 这里使用列表是因为同一个节点下，可能会存在多个Mesh和多个材质的问题(Unity中通过Mesh的subMesh来实现，这里干脆直接用个Mesh队列，如果后面结构上存在问题，可以考虑按Unity的Mesh机制来弄)
		std::vector<Mesh*> m_vMeshes;
	};
}

