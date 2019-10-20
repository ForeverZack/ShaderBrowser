#pragma once

#include <glad/glad.h>
#include <unordered_map>
#include "GL/GLDefine.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Browser/Components/Camera/Camera.h"
#include "BaseRender.h"

using namespace std;
using namespace customGL;
using namespace common;

namespace browser
{
    class BaseBoundBox;
    
    class SkinnedMeshRenderer : public BaseRender
	{
	public:
        static SkinnedMeshRenderer* createSkinnedMeshRenderer(const std::string& materialName = Material::DEFAULT_MATERIAL_NAME, const std::string& programeName = GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME);
            static SkinnedMeshRenderer* createSkinnedMeshRenderer(Material* material);
	public:
		SkinnedMeshRenderer();
		~SkinnedMeshRenderer();

	public:
		// 重载属性面板显示方法
		virtual void onInspectorGUI(InspectorPanel* inspector);
        
        // 添加模型
        void addMesh(Mesh* mesh);
        
        // 检测可见性
        bool checkVisibility(Camera* camera, bool reCalculate = false);
        
        
        const std::vector<Mesh*>& getMeshes()
        {
            return m_oMeshFilter->getMeshes();
        }
        
        REGISTER_PROPERTY_GET(BaseBoundBox*, m_oBoundBox, BoundBox);
        REGISTER_PROPERTY_GET(MeshFilter*, m_oMeshFilter, MeshFilter);
        
    protected:
        // 处理组件事件
        virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        
	protected:
        // 网格筛选器
        MeshFilter* m_oMeshFilter;
        // 包围盒
        BaseBoundBox* m_oBoundBox;
        
	};
}

