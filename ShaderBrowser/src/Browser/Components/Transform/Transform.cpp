#include "Transform.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "../../../GL/GLDefine.h"

using namespace customGL;

namespace browser
{
	Transform::Transform()
        : m_oParent(nullptr)
        , m_bTransDirty(true)
        , m_oObjectPos(GLM_VEC3_ZERO)
        , m_oEulerAngle(GLM_VEC3_ZERO)
        , m_oQuaternion(GLM_QUAT_UNIT)
        , m_oScale(GLM_VEC3_ONE)
        , m_oModelMatrix(GLM_MAT4_UNIT)
        , m_sName("")
        , m_oGlobalEulerAngle(GLM_VEC3_ZERO)
        , m_oGlobalQuaternion(GLM_QUAT_UNIT)
	{
        // 组件所属系统
        m_eBelongSystem = SystemType::Transform;
        
        // init
        m_vRotateDelayRotations.clear();
        m_vRotateDelaySpaces.clear();
	}

	Transform::~Transform()
	{
        
	}
    
    void Transform::setPosition(float x, float y, float z)
    {
        TRANS_SET_VEC3(m_oObjectPos, x, y, z);
        TRANS_DIRTY(this, true);
    }
    
    void Transform::Rotate(glm::vec3 rotation, customGL::Space sp /*= customGL::Space::Self*/)
    {
        m_vRotateDelayRotations.push_back(rotation);
        m_vRotateDelaySpaces.push_back(sp);
        
        TRANS_DIRTY(this, true);
    }
    
    void Transform::RotateDelay(const glm::mat4& parentMMatrix)
    {
        glm::quat parentQuat = glm::toQuat(parentMMatrix);
        
//        BROWSER_LOG(m_sName);
        
        for (int i=0; i<m_vRotateDelayRotations.size(); ++i)
        {
            const glm::vec3& rotation = m_vRotateDelayRotations[i];
            const customGL::Space& sp = m_vRotateDelaySpaces[i];
            
            // 按照书上说的，y-x-z计算出来的应该是 惯性->模型 的旋转矩阵，但glm是列主序的矩阵，需要左乘，所以实际相乘的时序是 My*Mx*Mz*M，也就是说顺序是反的。
			// 顺序反了，求出来的是一个 模型->惯性 的    
//            BROWSER_LOG_VEC3(rotation);
            switch(sp)
            {
                case customGL::Space::Self:
                    {
                        // 绕自身坐标系旋转（旋转顺序：y-x-z）
                        m_oQuaternion = glm::rotate(m_oQuaternion, glm::radians(rotation.y), GLM_AXIS_Y);
                        m_oQuaternion = glm::rotate(m_oQuaternion, glm::radians(rotation.x), GLM_AXIS_X);
                        m_oQuaternion = glm::rotate(m_oQuaternion, glm::radians(rotation.z), GLM_AXIS_Z);
                    }
                    break;
                    
                case customGL::Space::World:
                    {
                        // 绕惯性坐标系旋转（旋转顺序：z-x-y）
                        m_oQuaternion = glm::rotate(m_oQuaternion, glm::radians(rotation.z), glm::inverse(parentQuat*m_oQuaternion)*GLM_AXIS_Z);
                        m_oQuaternion = glm::rotate(m_oQuaternion, glm::radians(rotation.x), glm::inverse(parentQuat*m_oQuaternion)*GLM_AXIS_X);
                        m_oQuaternion = glm::rotate(m_oQuaternion, glm::radians(rotation.y), glm::inverse(parentQuat*m_oQuaternion)*GLM_AXIS_Y);
                    }
                    break;
            }
            
//            BROWSER_LOG_QUATERNION(m_oQuaternion);
//            BROWSER_LOG_VEC3(glm::degrees(m_oEulerAngle));
        }
        
        // 计算模型坐标空间下的欧拉角
        m_oEulerAngle = quaternion2EulerAngle(m_oQuaternion);   // 模型->惯性 四元数转欧拉角
        // 计算惯性坐标空间下的四元数和欧拉角
        m_oGlobalQuaternion = parentQuat * m_oQuaternion;
        m_oGlobalEulerAngle = quaternion2EulerAngle(m_oGlobalQuaternion);
        

//        BROWSER_LOG("====local=====");
//        BROWSER_LOG_QUATERNION(m_oQuaternion);
//        BROWSER_LOG_VEC3(glm::degrees(m_oEulerAngle));
//        BROWSER_LOG("====global=====");
//        BROWSER_LOG_QUATERNION(m_oGlobalQuaternion);
//        BROWSER_LOG_VEC3(glm::degrees(m_oGlobalEulerAngle));

        
        // clear
        m_vRotateDelayRotations.clear();
        m_vRotateDelaySpaces.clear();
        
    }
    
    // 物体->惯性坐标系
    glm::vec3 Transform::quaternion2EulerAngle(glm::quat quat)
    {
        float h, p, b;
        
        // 计算sin(pitch)
        float sp = -2.0f * (quat.y*quat.z - quat.w*quat.x);
        // 检查万向锁，允许有一定误差
        if(fabs(sp) > 0.9999f)
        {
            // 向正上或者正下
            p = 1.570796f * sp;
            // 计算heading, bank
            h = atan2(-quat.x*quat.z + quat.w*quat.y, 0.5f - quat.y*quat.y - quat.z*quat.z);
            b = 0.0f;
        }
        else
        {
            // 计算角度
            p = asin(sp);
            h = atan2(quat.x*quat.z + quat.w*quat.y, 0.5f - quat.x*quat.x - quat.y*quat.y);
            b = atan2(quat.x*quat.y + quat.w*quat.z, 0.5f - quat.x*quat.x - quat.z*quat.z);
        }
        
        return glm::vec3(p, h, b);
    }
    
    void Transform::setQuaternion(float x, float y, float z, float w)
    {
        TRANS_SET_VEC4(m_oQuaternion, x, y, z, w);
        TRANS_DIRTY(this, true);
        
        // 清除将要进行的旋转
        m_vRotateDelayRotations.clear();
        m_vRotateDelaySpaces.clear();

        // 父节点model矩阵
        const glm::mat4& parentMMatrix = getParentTransformModelMatrix();
        // 计算模型坐标空间下的欧拉角
        m_oEulerAngle = quaternion2EulerAngle(m_oQuaternion);   // 模型->惯性 四元数转欧拉角
        // 计算惯性坐标空间下的四元数和欧拉角
        m_oGlobalQuaternion = glm::toQuat(parentMMatrix) * m_oQuaternion;
        m_oGlobalEulerAngle = quaternion2EulerAngle(m_oGlobalQuaternion);
    }
    
    void Transform::setQuaternion(const glm::quat& quaternion)
    {
        setQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    }
    
    void Transform::setEulerAngle(float x, float y, float z)
    {
        TRANS_SET_VEC3(m_oEulerAngle, x, y, z);
        TRANS_DIRTY(this, true);
        
        // 清除将要进行的旋转
        m_vRotateDelayRotations.clear();
        m_vRotateDelaySpaces.clear();
        
        // 计算当前旋转值
        Rotate(m_oEulerAngle);
    }
    
    void Transform::setEulerAngle(const glm::vec3& eulerAngle)
    {
        setEulerAngle(eulerAngle.x, eulerAngle.y, eulerAngle.z);
    }
    
    void Transform::setScale(float x, float y, float z)
    {
        TRANS_SET_VEC3(m_oScale, x, y, z);
        TRANS_DIRTY(this, true);
    }
    
    void Transform::addChild(Transform* child)
    {
        BROWSER_ASSERT(!child->m_oParent, "child has already been added to an Transform parent, it can't be added again in function Transform::addChild");
        
        // 加入子节点列表
        m_vChildren.push_back(child);
        
        // 设置父节点
        child->m_oParent = this;
        TRANS_DIRTY(child, true);
    }
    
    glm::vec3 Transform::getGlobalPosition()
    {
        return getTransformModelMatrix() * glm::vec4(m_oObjectPos, 1.0f);
    }
    
    glm::vec3 Transform::getGlobalEulerAngle()
    {
        getTransformModelMatrix();
        return m_oGlobalEulerAngle;
    }

    glm::quat Transform::getGlobalQuaternion()
    {
        getTransformModelMatrix();
        return m_oGlobalQuaternion;
    }
    
    void Transform::setGlobalPosition(float x, float y, float z)
    {
        glm::vec4 globalPosition(x, y, z, 1.0f);
        
        // 设置自身的世界坐标系的位置，需要传入的世界坐标位置，设置本地坐标。该坐标可以通过 "父节点model矩阵的逆*世界坐标位置" 来得到。
        // model: local->world     model_inverse: world->local
        glm::mat4 inverse_parentModelMatrix = glm::inverse(getParentTransformModelMatrix());
        m_oObjectPos = inverse_parentModelMatrix * globalPosition;
        
        TRANS_DIRTY(this, true);
    }
    
    void Transform::setGlobalPosition(const glm::vec3& position)
    {
        setGlobalPosition(position.x, position.y, position.z);
    }
    
    void Transform::setGlobalQuaternion(float x, float y, float z, float w)
    {
        glm::quat globalQuaternion(w, x, y, z); //注意quat初始化顺序
        
        // 清除将要进行的旋转
        m_vRotateDelayRotations.clear();
        m_vRotateDelaySpaces.clear();
        
        // 父节点model矩阵
        const glm::mat4& parentMMatrix = getParentTransformModelMatrix();
        // 计算惯性坐标空间下的四元数和欧拉角
        m_oGlobalQuaternion = globalQuaternion;
        m_oGlobalEulerAngle = quaternion2EulerAngle(m_oGlobalQuaternion);
        
        // 计算模型坐标空间下的四元数欧拉角
        m_oQuaternion = glm::inverse(glm::toQuat(parentMMatrix)) * globalQuaternion;
        m_oEulerAngle = quaternion2EulerAngle(m_oQuaternion);   // 模型->惯性 四元数转欧拉角
        
        TRANS_DIRTY(this, true);
    }
    
    void Transform::setGlobalQuaternion(const glm::quat& quaternion)
    {
        setGlobalQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    }
    
    glm::mat4 Transform::getParentTransformModelMatrix(bool& parentDirty)
    {
        //只要父级节点中，有一个dirty，则他下面的子级节点全都需要重新计算model
        parentDirty = false;
        
        // 获取层级列表
        std::vector<Transform*> parents;
        for (Transform* transform = getParent(); transform != nullptr; transform = transform->getParent())
        {
            parents.push_back(transform);
        }
        
        // 如果一个节点的父节点为空，认为他的父级model矩阵为单位矩阵
        glm::mat4 parentMMatrix = GLM_MAT4_UNIT;
        
        // 遍历父节点列表，获取父节点的model矩阵
        Transform* child;
        for (int i = parents.size() - 1; i >= 0; --i)
        {
            child = parents[i];
            // 判断该节点是否需要重新计算model矩阵
            if (child->getTransDirty() || parentDirty)
            {
                parentDirty = true;
                
                // 更新该Transform的model矩阵
                child->updateSelfModelMatrix(parentMMatrix);
                
                // 重置脏标记
                TRANS_DIRTY(child, false);
            }
            
            // 记录该Transform的model矩阵，传给下一个时使用
            parentMMatrix = child->getModelMatrix();
        }
        
        return parentMMatrix;
    }
    
    glm::mat4 Transform::getParentTransformModelMatrix()
    {
        bool parentDirty;
        return getParentTransformModelMatrix(parentDirty);
    }
    
    glm::mat4 Transform::getTransformModelMatrix()
    {
        bool parentDirty = false;
        // 获取父节点的model矩阵
        const glm::mat4& parentMMatrix = getParentTransformModelMatrix(parentDirty);
        if (parentDirty)
        {
            // 更新自身的model矩阵
            updateSelfModelMatrix(parentMMatrix);
            
            // 重置脏标记
            TRANS_DIRTY(this, false);
        }
        
        return m_oModelMatrix;
    }

	void Transform::updateSelfModelMatrix(const glm::mat4& parentMMatrix)
	{
		// 计算自身旋转
		RotateDelay(parentMMatrix);

		// 这里阅读顺序是反的，因为实际乘法当中使用的是左乘
		// 4.父节点的model
		m_oModelMatrix = parentMMatrix;
		// 3.位移
		m_oModelMatrix = glm::translate(m_oModelMatrix, m_oObjectPos);
		// 2.旋转 y-x-z
		m_oModelMatrix = glm::toMat4(m_oQuaternion) * m_oModelMatrix;
		// 1.缩放
		m_oModelMatrix = glm::scale(m_oModelMatrix, m_oScale);

//                    BROWSER_LOG_MAT4(m_oModelMatrix);
	}
    
    void Transform::visit(const glm::mat4& parentMMatrix, bool bDirty)
    {
        // 是否需要更新
        bool dirty = bDirty || m_bTransDirty;
        
        // 计算自己的model矩阵
        if (dirty)
        {
			updateSelfModelMatrix(parentMMatrix);
        }
        
        // 计算子节点的model
        Transform* child = nullptr;
        for (auto i=0; i<m_vChildren.size(); ++i)
        {
            child = m_vChildren[i];
            child->visit(m_oModelMatrix, dirty);
        }
        
        // 自身脏标记重置
        TRANS_DIRTY(this, false);
    }


}
