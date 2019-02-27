#pragma once

#include <vector>
#include "Common/Tools/Utils.h"
#include "GL/GLDefine.h"
#include "Browser/Components/BaseComponent.h"
#include "Mesh.h"

using namespace common;
using namespace customGL;

namespace browser
{
	class MeshFilter : public BaseComponent
	{
	       
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
		std::vector<Mesh*> m_vMeshes;
	};
}

