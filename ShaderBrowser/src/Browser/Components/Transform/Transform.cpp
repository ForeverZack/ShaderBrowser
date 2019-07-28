#include "Transform.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "GL/GLDefine.h"
#include <tuple>

using namespace customGL;

namespace browser
{
	Transform::Transform()
        : BaseComponent("Transform")
        , m_oParent(nullptr)
        , m_bTransDirty(true)
        , m_oObjectPos(GLM_VEC3_ZERO)
        , m_oEulerAngle(GLM_VEC3_ZERO)
        , m_bUpdateInspectorEuler(true)
        , m_oQuaternion(GLM_QUAT_UNIT)
        , m_oScale(GLM_VEC3_ONE)
        , m_oModelMatrix(GLM_MAT4_UNIT)
        , m_sName("Nonamed Entity")
        , m_oGlobalEulerAngle(GLM_VEC3_ZERO)
        , m_oGlobalQuaternion(GLM_QUAT_UNIT)
        , m_iBoneId(-1)
        , m_oBoneMatrix(GLM_MAT4_UNIT)
        , m_oBindposeMatrix(GLM_MAT4_UNIT)
        , m_oBoneRoot(nullptr)
        , m_oBoneRootSpaceMatrix(GLM_MAT4_UNIT)
        , m_bHasSrcModelInfo(false)
        , m_oSrcModelPos(GLM_VEC3_ZERO)
        , m_oSrcModelQuat(GLM_QUAT_UNIT)
        , m_oSrcModelScale(GLM_VEC3_ONE)
	{
        // 组件所属系统
        m_eBelongSystem = SystemType::Transform;
        
        // init
        m_vRotateDelayRotations.clear();
        m_vRotateDelaySpaces.clear();
	}

	Transform::~Transform()
	{
		BROWSER_LOG("~Transform");
	}

	Reference* Transform::clone()
	{
		Transform* transform = new Transform();
		transform->m_sName = m_sName;

		*transform = *this;

		return transform;
	}
    
    void Transform::onInspectorGUI(InspectorPanel* inspector)
    {
        // 显示名称
        inspector->addPropertyText("Name: " + m_sName);
        
        // 显示模型坐标位置
        inspector->addPropertyVector3("Position", &m_oObjectPos, [=](const glm::vec3& value)
            {
                setPosition(value);
            }, false);
        
        // 显示欧拉角
        // 注意万向锁：由于使用了yaw-pitch-roll，所以这里x轴旋转必须控制在-90~90度的范围内，否则会出现万向锁
        inspector->addPropertyTransformEulerAngle("Rotation", &m_oInspectorEuler, [=](const glm::vec3& value)
            {
                m_oInspectorEuler = value;
                m_bUpdateInspectorEuler = false;
                
                setEulerAngle(value);
            }, false);

        // 显示缩放
        inspector->addPropertyVector3("Scale", &m_oScale, [=](const glm::vec3& value)
            {
                setScale(value);
            }, false);
        
    }
    
    void Transform::setPosition(float x, float y, float z)
    {
        TRANS_SET_VEC3(m_oObjectPos, x, y, z);
        TRANS_DIRTY(this, true);
    }
    
    void Transform::Rotate(const glm::vec3& rotation, customGL::Space sp /*= customGL::Space::Self*/)
    {
        m_vRotateDelayRotations.push_back(rotation);
        m_vRotateDelaySpaces.push_back(sp);
        
        TRANS_DIRTY(this, true);
    }
    
    void Transform::Translate(const glm::vec3& offset, customGL::Space sp /*= customGL::Space::Self*/)
    {
        switch(sp)
        {
            case Space::Self:
                {
                    // 自身坐标系
                    setPosition(m_oObjectPos.x+offset.x, m_oObjectPos.y+offset.y, m_oObjectPos.z+offset.z);
                }
                break;
                
            case Space::World:
                {
                    // 世界坐标系
                    glm::vec3 global_position = getGlobalPosition();
                    setGlobalPosition(global_position.x+offset.x, global_position.y+offset.y, global_position.z+offset.z);
                }
                break;
        }
    }
    
    glm::vec3 Transform::getForward()
    {
        const glm::mat4& modelMatrix = getTransformModelMatrix();
        glm::vec3 forward = modelMatrix * glm::vec4(GLM_AXIS_Z, 0.0f);
        return glm::normalize(forward);
    }
    
    glm::vec3 Transform::getLeft()
    {
        const glm::mat4& modelMatrix = getTransformModelMatrix();
        glm::vec3 left = modelMatrix * glm::vec4(GLM_AXIS_X, 0.0f);
        return glm::normalize(left);
    }
    
    glm::vec3 Transform::getUp()
    {
        const glm::mat4& modelMatrix = getTransformModelMatrix();
        glm::vec3 up = modelMatrix * glm::vec4(GLM_AXIS_Y, 0.0f);
        return glm::normalize(up);
    }
    
    void Transform::RotateDelay(const glm::mat4& parentMMatrix)
    {
        glm::quat parentQuat = Utils::convertMatrix2Quat(parentMMatrix);
        
//        BROWSER_LOG(m_sName);
        
        // 如果当前从属性面板中输入了欧拉角，则取消所有变换，仅使用属性面板设置的欧拉角的值
        if(!m_bUpdateInspectorEuler)
        {
            setEulerAngle(m_oInspectorEuler);
        }
        
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
        if (m_bUpdateInspectorEuler)
        {
            m_oInspectorEuler = m_oEulerAngle;
        }
        else
        {
            m_bUpdateInspectorEuler = true;
        }
        // 计算惯性坐标空间下的四元数和欧拉角
        m_oGlobalQuaternion = parentQuat * m_oQuaternion;
        m_oGlobalEulerAngle = quaternion2EulerAngle(m_oGlobalQuaternion);
        

//        BROWSER_LOG("====local=====");
//        BROWSER_LOG_QUATERNION(m_oQuaternion);
//        BROWSER_LOG_VEC3(m_oEulerAngle);
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
            p = PI / 2 * sp;
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
        
        p = formatDegree(glm::degrees(p));
        h = formatDegree(glm::degrees(h));
        b = formatDegree(glm::degrees(b));

        
        return glm::vec3(p, h, b);
    }
    
    float Transform::formatDegree(float degrees)
    {
        while(degrees<0.0f)
        {
            degrees+=360.0f;
        }
        while(degrees>=360.0f)
        {
            degrees-=360.0f;
        }
        if (degrees == -0.0f)
        {
            degrees = 0.0f;
        }
        return degrees;
    }
    
    void Transform::setQuaternion(float x, float y, float z, float w)
    {
        TRANS_SET_VEC4(m_oQuaternion, x, y, z, w);
        TRANS_DIRTY(this, true);
        
        // 清除将要进行的旋转
        m_vRotateDelayRotations.clear();
        m_vRotateDelaySpaces.clear();

        // 父节点model矩阵
		const std::tuple<glm::mat4, bool>& parentTransInfo = getParentTransformModelMatrix();
		const glm::mat4& parentMMatrix = std::get<0>(parentTransInfo);
        // 计算模型坐标空间下的欧拉角
        m_oEulerAngle = quaternion2EulerAngle(m_oQuaternion);   // 模型->惯性 四元数转欧拉角
        // 计算惯性坐标空间下的四元数和欧拉角
        m_oGlobalQuaternion = Utils::convertMatrix2Quat(parentMMatrix) * m_oQuaternion;
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
        
        // 重置当前旋转
        m_oQuaternion = GLM_QUAT_UNIT;
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

	void Transform::removeChild(Transform* child)
	{
		bool hasFound = false;

		for (auto itor = m_vChildren.begin(); itor != m_vChildren.end(); ++itor)
		{
			if (*itor == child)
			{
				hasFound = true;
				m_vChildren.erase(itor);
				child->m_oParent = nullptr;
				TRANS_DIRTY(child, true);
				break;
			}
		}

		BROWSER_ASSERT(hasFound, "Transform did not have this child, it can't be removed in function Transform::removeChild");
	}
    
    glm::vec3 Transform::getGlobalPosition()
    {
        // 获取父节点的model矩阵
		const std::tuple<glm::mat4, bool>& parentTransInfo = getParentTransformModelMatrix();
		const glm::mat4& parentModelMatrix = std::get<0>(parentTransInfo);
        return parentModelMatrix * glm::vec4(m_oObjectPos, 1.0f);
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
		const std::tuple<glm::mat4, bool>& parentTransInfo = getParentTransformModelMatrix();
        glm::mat4 inverse_parentModelMatrix = glm::inverse(std::get<0>(parentTransInfo));
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
		const std::tuple<glm::mat4, bool>& parentTransInfo = getParentTransformModelMatrix();
        const glm::mat4& parentMMatrix = std::get<0>(parentTransInfo);
        // 计算惯性坐标空间下的四元数和欧拉角
        m_oGlobalQuaternion = globalQuaternion;
        m_oGlobalEulerAngle = quaternion2EulerAngle(m_oGlobalQuaternion);
        
        // 计算模型坐标空间下的四元数欧拉角
        m_oQuaternion = glm::inverse(Utils::convertMatrix2Quat(parentMMatrix)) * globalQuaternion;
        m_oEulerAngle = quaternion2EulerAngle(m_oQuaternion);   // 模型->惯性 四元数转欧拉角
        
        TRANS_DIRTY(this, true);
    }
    
    void Transform::setGlobalQuaternion(const glm::quat& quaternion)
    {
        setGlobalQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    }
    
    void Transform::setSrcModelTransformation(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
    {
        m_bHasSrcModelInfo = true;
        m_oSrcModelPos = pos;
        m_oSrcModelQuat = rot;
        m_oSrcModelScale = scale;
        
        setPosition(pos.x, pos.y, pos.z);
        setQuaternion(rot.x, rot.y, rot.z, rot.w);
        setScale(scale.x, scale.y, scale.z);
    }
    
	std::tuple<glm::mat4, bool> Transform::getParentTransformModelMatrix()
    {
        //只要父级节点中，有一个dirty，则他下面的子级节点全都需要重新计算model
        bool parentDirty = false;
        
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
        
        return std::make_tuple(parentMMatrix, parentDirty);
    }
    
    const glm::mat4& Transform::getTransformModelMatrix()
    {
        // 获取父节点的model矩阵
		const std::tuple<glm::mat4, bool>& parentTransInfo = getParentTransformModelMatrix();
        const glm::mat4& parentMMatrix = std::get<0>(parentTransInfo);
		bool parentDirty = std::get<1>(parentTransInfo);

        if (parentDirty || m_bTransDirty)
        {
            // 更新自身的model矩阵
            updateSelfModelMatrix(parentMMatrix);
            
            // 重置脏标记
            TRANS_DIRTY(this, false);
        }
        
        return m_oModelMatrix;
    }

	glm::mat4 Transform::traverseNoScaleModelMatrix()
	{
		Transform* node = m_oParent;
		glm::mat4 noScaleModelMat = m_oNoScaleModelMatrix;
		glm::vec3 totalScale(m_oScale);
		while (node)
		{
			totalScale.x *= node->getScaleX();
			totalScale.y *= node->getScaleY();
			totalScale.z *= node->getScaleZ();

			node = node->m_oParent;
		}

		totalScale.x = 1.0f / totalScale.x;
		totalScale.y = 1.0f / totalScale.y;
		totalScale.z = 1.0f / totalScale.z;

		return glm::scale(m_oNoScaleModelMatrix, totalScale);
	}
    
    void Transform::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
    {
        switch (event) {
            case ComponentEvent::Animator_UpdateBonesTransform:
                {
                    AnimatorUpdateBonesTransformMessage* convert_msg = static_cast<AnimatorUpdateBonesTransformMessage*>(msg);
                    
                    auto bonesIdMap = convert_msg->getBonesIdMap();
                    auto bonesPosition = convert_msg->getBonesPosition();
                    auto bonesRotation = convert_msg->getBonesRotation();
                    auto bonesScale = convert_msg->getBonesScale();

                    // 变换信息里只记录了部分发生变换的骨骼
                    if (m_iBoneId!=-1 && bonesPosition->find(m_iBoneId)!=bonesPosition->end())
                    {
                        const glm::vec3& position = (*bonesPosition)[m_iBoneId];
                        const glm::quat& quaternion = (*bonesRotation)[m_iBoneId];
                        const glm::vec3& scale = (*bonesScale)[m_iBoneId];
                        setQuaternion(quaternion);
                        setPosition(position);
                        setScale(scale);
                    }
                    else if(m_bHasSrcModelInfo && this!=m_oBoneRoot)
                    {
                        setPosition(m_oSrcModelPos);
                        setQuaternion(m_oSrcModelQuat);
                        setScale(m_oSrcModelScale);
                    }
                    
//                    auto itor = bonesIdMap->find(m_sName);
//                    if(itor != bonesIdMap->end())
//                    {
//                        unsigned int boneId = itor->second;
//
//                        if(bonesPosition->find(boneId) != bonesPosition->end())
//                        {
//                            const glm::vec3& position = (*bonesPosition)[boneId];
//                            const glm::quat& quaternion = (*bonesRotation)[boneId];
//                            const glm::vec3& scale = (*bonesScale)[boneId];
//                            setScale(scale.x, scale.y, scale.z);
//                            setQuaternion(quaternion);
//                            setPosition(position);
//                        }
//                        else
//                        {
//                            setScale(1, 1, 1);
//                            setEulerAngle(0, 0, 0);
//                            setPosition(0, 0, 0);
//                        }
//                    }
                    
                }
                break;
                
            default:
                break;
        }
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
		m_oModelMatrix = m_oModelMatrix * glm::toMat4(m_oQuaternion);
        m_oNoScaleModelMatrix = m_oModelMatrix; //这里记录下不包含缩放的model矩阵
		// 1.缩放
		m_oModelMatrix = glm::scale(m_oModelMatrix, m_oScale);

//                    BROWSER_LOG_MAT4(m_oModelMatrix);
        
//        if (m_sName == "namizhuang")
//        {
//            std::cout<<"mPos=("<<m_oObjectPos.x<<","<<m_oObjectPos.y<<","<<m_oObjectPos.z<<"), mRotate=("<<m_oQuaternion.x<<","<<m_oQuaternion.y<<","<<m_oQuaternion.z<<","<<m_oQuaternion.w<<") , mScale=("<<m_oScale.x<<","<<m_oScale.y<<","<<m_oScale.z<<")"<<endl;
//            glm::vec3 pos, scale;
//            glm::quat rot;
//            Utils::parseMatrix(m_oModelMatrix, pos, rot, scale);
//            std::cout<<"parse mPos=("<<pos.x<<","<<pos.y<<","<<pos.z<<"), mRotate=("<<rot.x<<","<<rot.y<<","<<rot.z<<","<<rot.w<<") , mScale=("<<scale.x<<","<<scale.y<<","<<scale.z<<")"<<endl;
//            
//            int aaa = 0;
//        }

	}
    
    void Transform::beforeUpdate(float deltaTime)
    {
        m_bCurFrameDirty = false;
    }
    
    void Transform::visit(const glm::mat4& parentMMatrix, bool bDirty)
    {
        // 是否需要更新
        bool dirty = bDirty || m_bTransDirty;
        m_bCurFrameDirty = dirty | m_bCurFrameDirty;
        
        // 计算自己的model矩阵
        if (dirty)
        {
			updateSelfModelMatrix(parentMMatrix);
            
        }

        // 自身就是骨骼根节点
        if (m_oBoneRoot == this)
        {
            m_oBoneRootSpaceMatrix = glm::inverse(m_oModelMatrix);
        }
        // 自身是骨骼，要计算骨骼矩阵
        if (m_iBoneId != -1)
        {
            m_oBoneMatrix = m_oBoneRoot->m_oBoneRootSpaceMatrix * m_oModelMatrix * m_oBindposeMatrix;
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
