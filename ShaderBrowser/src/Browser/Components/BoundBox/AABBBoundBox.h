#pragma once

#include "Browser/Components/BoundBox/BaseBoundBox.h"
#include "Browser/Components/Camera/Camera.h"

namespace browser
{
	class AABBBoundBox : public BaseBoundBox
	{
    public:
        enum AABBBoundBoxVertices
        {
            // 前面4个顶点
            // 左上
            Front_LT = 0,
            // 左下
            Front_LB,
            // 右下
            Front_RB,
            // 右上
            Front_RT,
            
            // 后面4个顶点
            // 左上
            Back_LT,
            // 左下
            Back_LB,
            // 右下
            Back_RB,
            // 右上
            Back_RT
        };
        
	public:
		AABBBoundBox();
		~AABBBoundBox();

	public:
		// 刷新
        virtual void updateBoundBox(float deltaTime);
        
        // 计算绘制顶点
        virtual void calculateDisplayVertices();
        
        // 可见性检测
        virtual bool checkVisibility(Camera* camera, bool reCalculate = false);
        
        // 重载属性面板显示方法
        virtual void onInspectorGUI(InspectorPanel* inspector);
        
        
    protected:
        // 重新生成包围盒
        void calculateAABBBoundBox();
        // 设置顶点
        void setVertexPosition(glm::vec4& vertex, float x, float y, float z);
        // 计算可见性掩码
        int calculateVertexOutCode(const glm::vec4& vertex);
        
	protected:
        // 模型坐标系下(原始数据)
        // 最小顶点
        glm::vec3 m_oMinVertex;
        // 最大顶点
        glm::vec3 m_oMaxVertex;
        
        // AABB包围盒的顶点
        glm::vec4 m_vVertices[8];
        // 生成当前包围盒的MeshFilter
        MeshFilter* m_oInUseMeshFilter;
        
	};
}

