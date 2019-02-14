#include "AABBBoundBox.h"
#include "GL/GLDefine.h"
#include "Browser/Components/Mesh/MeshFilter.h"

namespace browser
{
	AABBBoundBox::AABBBoundBox()
        : m_oInUseMeshFilter(nullptr)
	{
        // 给显示包围盒顶点数组预留空间
        m_vDisplayVertices.reserve(24);
	}

	AABBBoundBox::~AABBBoundBox()
	{
		BROWSER_LOG("~AABBBoundBox");
	}

	void AABBBoundBox::updateBoundBox(float deltaTime)
	{        
        if (m_oTransform && m_oMeshFilter)
        {
            // 当前使用的MeshFilter和本entity的MeshFilter组件不符,则重新生成AABB包围盒
            if (m_oMeshFilter && m_oMeshFilter!=m_oInUseMeshFilter)
            {
                m_oInUseMeshFilter = m_oMeshFilter;
                // 重新计算包围盒
                calculateAABBBoundBox();
            }
            
            // model矩阵
            const glm::mat4& modelMatrix = m_oTransform->getModelMatrix();
            
            glm::vec3 min(GLM_VEC3_ZERO), max(GLM_VEC3_ZERO);
            // x' = m11*x + m21*y + m31*z ; y' = m12*x + m22*y + m32*z ; z' = m13*x + m23*y + m33*z;
            // 算法优化: 例如要找Xmin,先看第一项,看m11的正负,如果m11为正,则用原Xmin来乘,否则用原Xmax。然后看
            // X
            float m11=modelMatrix[0][0], m21=modelMatrix[1][0], m31=modelMatrix[2][0];
            if(m11 < 0)
            {
                min.x += m11 * m_oMaxVertex.x;
                max.x += m11 * m_oMinVertex.x;
            }
            else
            {
                min.x += m11 * m_oMinVertex.x;
                max.x += m11 * m_oMaxVertex.x;
            }
            if(m21 < 0)
            {
                min.x += m21 * m_oMaxVertex.y;
                max.x += m21 * m_oMinVertex.y;
            }
            else
            {
                min.x += m21 * m_oMinVertex.y;
                max.x += m21 * m_oMaxVertex.y;
            }
            if(m31 < 0)
            {
                min.x += m31 * m_oMaxVertex.z;
                max.x += m31 * m_oMinVertex.z;
            }
            else
            {
                min.x += m31 * m_oMinVertex.z;
                max.x += m31 * m_oMaxVertex.z;
            }
            // y
            float m12=modelMatrix[0][1], m22=modelMatrix[1][1], m32=modelMatrix[2][1];
            if(m12 < 0)
            {
                min.y += m12 * m_oMaxVertex.x;
                max.y += m12 * m_oMinVertex.x;
            }
            else
            {
                min.y += m12 * m_oMinVertex.x;
                max.y += m12 * m_oMaxVertex.x;
            }
            if(m22 < 0)
            {
                min.y += m22 * m_oMaxVertex.y;
                max.y += m22 * m_oMinVertex.y;
            }
            else
            {
                min.y += m22 * m_oMinVertex.y;
                max.y += m22 * m_oMaxVertex.y;
            }
            if(m32 < 0)
            {
                min.y += m32 * m_oMaxVertex.z;
                max.y += m32 * m_oMinVertex.z;
            }
            else
            {
                min.y += m32 * m_oMinVertex.z;
                max.y += m32 * m_oMaxVertex.z;
            }
            // z
            float m13=modelMatrix[0][2], m23=modelMatrix[1][2], m33=modelMatrix[2][2];
            if(m13 < 0)
            {
                min.z += m13 * m_oMaxVertex.x;
                max.z += m13 * m_oMinVertex.x;
            }
            else
            {
                min.z += m13 * m_oMinVertex.x;
                max.z += m13 * m_oMaxVertex.x;
            }
            if(m23 < 0)
            {
                min.z += m23 * m_oMaxVertex.y;
                max.z += m23 * m_oMinVertex.y;
            }
            else
            {
                min.z += m23 * m_oMinVertex.y;
                max.z += m23 * m_oMaxVertex.y;
            }
            if(m33 < 0)
            {
                min.z += m33 * m_oMaxVertex.z;
                max.z += m33 * m_oMinVertex.z;
            }
            else
            {
                min.z += m33 * m_oMinVertex.z;
                max.z += m33 * m_oMaxVertex.z;
            }
            
            // 填充Vertex数组
            // 上面的3*3矩阵没有位移,所以这里把位移加上去,并且因为上面已经做了缩放旋转变换,所以这里要加的应该是世界坐标
            glm::vec3 global_position = m_oTransform->getGlobalPosition();
            min += global_position;
            max += global_position;
            
            // Front
            setVertexPosition(m_vVertices[Front_LT], min.x, max.y, max.z);
            setVertexPosition(m_vVertices[Front_LB], min.x, min.y, max.z);
            setVertexPosition(m_vVertices[Front_RB], max.x, min.y, max.z);
            setVertexPosition(m_vVertices[Front_RT], max.x, max.y, max.z);
            // Back
            setVertexPosition(m_vVertices[Back_LT], min.x, max.y, min.z);
            setVertexPosition(m_vVertices[Back_LB], min.x, min.y, min.z);
            setVertexPosition(m_vVertices[Back_RB], max.x, min.y, min.z);
            setVertexPosition(m_vVertices[Back_RT], max.x, max.y, min.z);
        }
	}
    
    bool AABBBoundBox::checkVisibility(Camera* camera, bool reCalculate /*= false*/)
    {       
        if (m_oTransform && reCalculate)
        {	
            const glm::mat4& view_matrix = camera->getViewMatrix();
            const glm::mat4& projection_matrix = camera->getProjectionMatrix();
            
            int result = 0xff;
            glm::vec4 vertex;
            // 遍历包围盒的8个顶点
            for (int i=0; i<8; ++i)
            {
                // 1. 将顶点转换到裁剪坐标系中  (注意：这里的顶点坐标本身就是世界坐标系中的坐标，所以不需要乘model矩阵)
                vertex = projection_matrix * view_matrix * m_vVertices[i];
                
                // 2. 根据裁剪坐标系的顶点坐标和w做比较,并算出一个掩码。
                // 顶点在外部,则掩码不为0。这里将min的掩码和max的掩码做和运算,得到aabb包围盒是否有顶点在视锥内
                result &= calculateVertexOutCode(vertex);
            }
           
            // 这里结果为0,则说明有顶点在视锥内,需要显示;如果不为0,则说明包围盒完全在视锥外。
            m_bRecVisibility = (result == 0);
        }
        
        return m_bRecVisibility;
    }
    
    int AABBBoundBox::calculateVertexOutCode(const glm::vec4& vertex)
    {
        int code = 0;
        
        // 一次检测包围盒的6个面,判断他们是否在视锥外
        // Left : x<-w  (0x01)
        if(vertex.x < -vertex.w)
        {
            code |= 0x01;
        }
        // Right : x>w  (0x02)
        if(vertex.x > vertex.w)
        {
            code |= 0x02;
        }
        // Bottom : y<-w    (0x04)
        if(vertex.y < -vertex.w)
        {
            code |= 0x04;
        }
        // Top : y>w    (0x08)
        if(vertex.y > vertex.w)
        {
            code |= 0x08;
        }
        // Front : z<-w (0x10)
        if(vertex.z < -vertex.w)
        {
            code |= 0x10;
        }
        // Back : z>w   (0x20)
        if(vertex.z > vertex.w)
        {
            code |= 0x20;
        }
        
        return code;
    }
    
    void AABBBoundBox::calculateDisplayVertices()
    {
        m_vDisplayVertices.clear();
        
        // 1
        m_vDisplayVertices.push_back(m_vVertices[Front_LT]);
        m_vDisplayVertices.push_back(m_vVertices[Front_RT]);
        // 2
        m_vDisplayVertices.push_back(m_vVertices[Front_RT]);
        m_vDisplayVertices.push_back(m_vVertices[Front_RB]);
        // 3
        m_vDisplayVertices.push_back(m_vVertices[Front_RB]);
        m_vDisplayVertices.push_back(m_vVertices[Front_LB]);
        // 4
        m_vDisplayVertices.push_back(m_vVertices[Front_LB]);
        m_vDisplayVertices.push_back(m_vVertices[Front_LT]);
        // 5
        m_vDisplayVertices.push_back(m_vVertices[Back_LT]);
        m_vDisplayVertices.push_back(m_vVertices[Back_RT]);
        // 6
        m_vDisplayVertices.push_back(m_vVertices[Back_RT]);
        m_vDisplayVertices.push_back(m_vVertices[Back_RB]);
        // 7
        m_vDisplayVertices.push_back(m_vVertices[Back_RB]);
        m_vDisplayVertices.push_back(m_vVertices[Back_LB]);
        // 8
        m_vDisplayVertices.push_back(m_vVertices[Back_LB]);
        m_vDisplayVertices.push_back(m_vVertices[Back_LT]);
        // 9
        m_vDisplayVertices.push_back(m_vVertices[Front_LT]);
        m_vDisplayVertices.push_back(m_vVertices[Back_LT]);
        // 10
        m_vDisplayVertices.push_back(m_vVertices[Front_RT]);
        m_vDisplayVertices.push_back(m_vVertices[Back_RT]);
        // 11
        m_vDisplayVertices.push_back(m_vVertices[Front_RB]);
        m_vDisplayVertices.push_back(m_vVertices[Back_RB]);
        // 12
        m_vDisplayVertices.push_back(m_vVertices[Front_LB]);
        m_vDisplayVertices.push_back(m_vVertices[Back_LB]);
        
    }
    
    void AABBBoundBox::setVertexPosition(glm::vec4& vertex, float x, float y, float z)
    {
        vertex.x = x;
        vertex.y = y;
        vertex.z = z;
        vertex.w = 1.0f;
    }
    
    void AABBBoundBox::calculateAABBBoundBox()
    {
        bool hasInit = false;
        
        const std::vector<Mesh*>& meshes = m_oInUseMeshFilter->getMeshes();
        for(auto itor=meshes.begin(); itor!=meshes.end(); ++itor)
        {
            const std::vector<VertexData>& vertices = (*itor)->getVertices();
            for (auto vItor=vertices.begin(); vItor!=vertices.end(); ++vItor)
            {
                const glm::vec3& position = (*vItor).position;
                
                // 初始化
                if(!hasInit)
                {
                    hasInit = true;
                    m_oMinVertex = position;
                    m_oMaxVertex = position;
                }
                
                // x
                if(position.x < m_oMinVertex.x)
                {
                    m_oMinVertex.x = position.x;
                }
                else if(position.x > m_oMaxVertex.x)
                {
                    m_oMaxVertex.x = position.x;
                }
                // y
                if(position.y < m_oMinVertex.y)
                {
                    m_oMinVertex.y = position.y;
                }
                else if(position.y > m_oMaxVertex.y)
                {
                    m_oMaxVertex.y = position.y;
                }
                // z
                if(position.z < m_oMinVertex.z)
                {
                    m_oMinVertex.z = position.z;
                }
                else if(position.z > m_oMaxVertex.z)
                {
                    m_oMaxVertex.z = position.z;
                }
            }
        }
    }

	
    
}
