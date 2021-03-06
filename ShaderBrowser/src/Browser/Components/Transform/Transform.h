#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Browser/Components/BaseComponent.h"
#include "Common/Tools/Utils.h"
#include "GL/GLDefine.h"

using namespace common;
using namespace customGL;

namespace browser
{
    /*
         * 本类对于model矩阵的计算一般情况下都会在每帧逻辑阶段结束后计算一次(脏标记为true时)，特殊情况下(如使用了一些get的方法)会在执行代码逻辑时是遍历更新父级节点的model并更新自身的属性，对于这种
           操作尽量少用，已经针对这种情况做了适当的优化(后续会继续看看有没有可以优化的地方)。一般情况下，不会影响性能。
     
     
         * Matrix_model = Matrix_translate * Matrix_rotate * Matrix_scale * Vertex.pos;
          其中Matrix_rotate表示 "模型坐标空间->惯性坐标空间" 的旋转矩阵，
     
         * 记住，当我们说旋转的顺序是heading-pitch-bank时，是指从惯性坐标系到物体坐标系。如果从物体坐标系变换到惯性坐标系，旋转的顺序就是相反的。"heading-pitch-bank"也叫作"roll-pitch-yaw"，
           roll类似于bank，yaw类似于heading（事实上，yaw并不严格等于heading）。注意，在roll-pitch-yaw系统中，角度的命名顺序与从物体坐标系到惯性坐标系的旋转顺序一致的。
     
           注意：资料上是这样描述旋转顺序的，因为我们这里使用了glm的矩阵实际上是列主序的需要左乘，所以当我们根据欧拉角的heading-pitch-bank时(即本项目的Space.Self，每次旋转的坐标系都是模型自身的坐标系)，
                实际上左乘的顺序会变成和代码相反的顺序（即上面说的旋转的顺序是相反的），就会获得 "物体坐标系->惯性坐标系" 的旋转矩阵(四元数)，所以这里使用的四元数转欧拉角的方法，是物体->惯性的方法。
                执行roll-pitch-yaw原理跟他也是一样的，只不过他的旋转轴是惯性坐标系下的坐标轴，而这个坐标轴的表示方法本类是模型坐标系下的，所以得到的应该任然是 "物体坐标系->惯性坐标系"的四元数。
		
		 * 另外，对于欧拉角来说，他的初始坐标系位置如果和惯性坐标系重合，那么y-x-z和z-y-x这两种旋转方式得到的结果是相同的；但如果初始坐标系位置和其不重合，例如父级节点已经发生的旋转，这时候两种方式得到
		   的结果是不同的，需要按照y-x-z的顺序去旋转。

         * 如果需要执行惯性坐标空间的旋转(Space.World)，则需要将旋转的坐标轴转为惯性空间坐标轴在模型坐标空间的表示（乘上四元数的逆来获得惯性坐标空间的坐标轴在物体坐标空间的表示。
           特别说明一下，这里四元数的逆 应该是指从根节点传过来的四元数和本节点的四元数的乘积。）。
     
     
         * 关于glm::quat需要特别注意，他的构造函数参数顺序为(w, x, y, z)，一定要把w放在第一位！！！！！
     
         * 注意，临时变量的引用不可以作为函数返回值！！！(const x&, x& 都不可以，否则会先析构，再执行拷贝构造)！！！！！ 这里只有成员变量的引用可以作为函数返回值。
     
     */
	class Transform : public BaseComponent
	{
	public:
		//static Transform* create();
        
        // 定义脏标记赋值方法
        #define TRANS_DIRTY(trans, dirty) trans->m_bTransDirty = dirty; trans->m_bCurFrameDirty = dirty | m_bCurFrameDirty;
        // 定义向量vec3赋值方法
        #define TRANS_SET_VEC3(vec3, x, y, z) vec3.x = x; vec3.y = y; vec3.z = z;
        // 定义向量vec4赋值方法
        #define TRANS_SET_VEC4(vec4, x, y, z, w) vec4.x = x; vec4.y = y; vec4.z = z; vec4.w = w;
		// 定义本地属性修改方法
		#define REGISTER_TRANS_PROPERTY_SET(varType, varName, funName)\
            public: virtual void set##funName(varType var) {varName = var; this->m_bTransDirty=true;}
        // 定义本地属性修改获取方法
        #define REGISTER_TRANS_PROPERTY_GET_SET(varType, varName, funName)\
            public: virtual varType get##funName(void)    {return varName;}\
            public: virtual void set##funName(varType var) {varName = var; this->m_bTransDirty=true;}
        

	public:
		Transform();
		~Transform();

		virtual Reference* clone();

	public:
        // 添加子节点
        void addChild(Transform* child);
		// 移除子节点
		void removeChild(Transform* child);
        
        // 遍历节点
        void visit(const glm::mat4& parentMMatrix, bool bDirty);
        void visitSync(const glm::mat4& parentMMatrix, bool bDirty);
        
        // 旋转
        void Rotate(const glm::vec3& rotation, customGL::Space sp = customGL::Space::Self);
        
        // 平移
        void Translate(const glm::vec3& offset, customGL::Space sp = customGL::Space::Self);
        
        
        void beforeUpdate(float deltaTime);
        
        // 获取方向向量
        // Forward (axis: z)
        glm::vec3 getForward();
        // Left (axis: x)
        glm::vec3 getLeft();
        // Up (axis: y)
        glm::vec3 getUp();
        
        // 获取\设置本地坐标系下的属性
        // 设置位置(相对于父节点)
        void setPosition(float x, float y, float z);
		REGISTER_TRANS_PROPERTY_SET(glm::vec3, m_oObjectPos, Position)
		REGISTER_PROPERTY_CONSTREF_GET(glm::vec3, m_oObjectPos, Position)
        REGISTER_TRANS_PROPERTY_GET_SET(float, m_oObjectPos.x, PositionX)
        REGISTER_TRANS_PROPERTY_GET_SET(float, m_oObjectPos.y, PositionY)
        REGISTER_TRANS_PROPERTY_GET_SET(float, m_oObjectPos.z, PositionZ)
        // 设置欧拉角
        void setEulerAngle(float x, float y, float z);
        void setEulerAngle(const glm::vec3& eulerAngle);
        REGISTER_PROPERTY_GET(glm::vec3, m_oEulerAngle, EulerAngle)
        // 设置缩放
        void setScale(float x, float y, float z);
		REGISTER_TRANS_PROPERTY_SET(glm::vec3, m_oScale, Scale)
		REGISTER_PROPERTY_CONSTREF_GET(glm::vec3, m_oScale, Scale)
        REGISTER_TRANS_PROPERTY_GET_SET(float, m_oScale.x, ScaleX)
        REGISTER_TRANS_PROPERTY_GET_SET(float, m_oScale.y, ScaleY)
        REGISTER_TRANS_PROPERTY_GET_SET(float, m_oScale.z, ScaleZ)
        // 四元数
        void setQuaternion(float x, float y, float z, float w);
        void setQuaternion(const glm::quat& quaternion);
        const glm::quat& getQuaternion();
//        REGISTER_PROPERTY_CONSTREF_GET(glm::quat, m_oQuaternion, Quaternion)
		// 获取model矩阵
		REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oModelMatrix, ModelMatrix)
		REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oNoScaleModelMatrix, NoScaleModelMatrix)
		glm::mat4 traverseNoScaleModelMatrix();
        // name
        REGISTER_PROPERTY_GET_SET(std::string, m_sName, Name)
		// parent
		REGISTER_PROPERTY_GET(Transform*, m_oParent, Parent)
		// children
		REGISTER_PROPERTY_CONSTREF_GET(std::vector<Transform*>, m_vChildren, Children)
        // dirty标记
        REGISTER_PROPERTY_GET(bool, m_bTransDirty, TransDirty)
        REGISTER_PROPERTY_GET(bool, m_bCurFrameDirty, CurFrameDirty)
        REGISTER_PROPERTY_GET_SET(int, m_iBoneId, BoneId)
        REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oBoneMatrix, BoneMatrix)
        REGISTER_PROPERTY_CONSTREF_GET_SET(glm::mat4, m_oBindposeMatrix, BindposeMatrix)
        REGISTER_PROPERTY_GET_SET(Transform*, m_oBoneRoot, BoneRoot)
        REGISTER_PROPERTY_CONSTREF_GET(glm::mat4, m_oBoneRootSpaceMatrix, BoneRootSpaceMatrix)
        // 设置骨骼动画节点的原始变换
        void setSrcModelTransformation(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale);
        // 重置骨骼动画节点的原始变换
        void resetSrcModelTransform();
        
        
        // 获取\设置全局坐标系下的属性(并不建议过多使用，因为每次使用都会遍历父级节点，更新他们的model矩阵)
        // 获取世界坐标系下的位置
        glm::vec3 getGlobalPosition();
        // 获取世界坐标系下的欧拉角
        glm::vec3 getGlobalEulerAngle();
        // 获取世界坐标系下的四元数
        glm::quat getGlobalQuaternion();
        // 设置世界坐标系下的位置
        void setGlobalPosition(float x, float y, float z);
        void setGlobalPosition(const glm::vec3& position);
        // 设置世界坐标系下的四元数
        void setGlobalQuaternion(float x, float y, float z, float w);
        void setGlobalQuaternion(const glm::quat& quaternion);
        

		

    
    private:
        // 四元数转为欧拉角(物体->惯性坐标系)
        glm::vec3 quaternion2EulerAngle(glm::quat quat);
        // 角度标准化
        float formatDegree(float degrees);
        
        // 延迟旋转
        void RotateDelay(const glm::mat4& parentMMatrix);
        
        // 获取父节点的model矩阵(遍历父节点)	这里用tuple模板来实现返回多个参数（用法类似于pair，但参数个数不限于2个）
        std::tuple<glm::mat4, bool> getParentTransformModelMatrix();  // bool表示父级节点的model矩阵是否改变
        // 获取自身的model矩阵
		const glm::mat4& getTransformModelMatrix();

		// 更新自身的model矩阵
		void updateSelfModelMatrix(const glm::mat4& parentMMatrix);
        
        // 重载属性面板显示方法
        virtual void onInspectorGUI(InspectorPanel* inspector);
        
        // 处理组件事件
        virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        
	protected:
        // 自身属性(相对于父节点)
        // 模型空间位置
        glm::vec3 m_oObjectPos;
        // 欧拉角（模型坐标系下）
        glm::vec3 m_oEulerAngle;
        // 四元数（模型坐标系下）
        glm::quat m_oQuaternion;
        // 缩放
        glm::vec3 m_oScale;
        // model矩阵
        glm::mat4 m_oModelMatrix;
        // 欧拉角（惯性坐标系下）
        glm::vec3 m_oGlobalEulerAngle;
        // 四元数（惯性坐标系下）
        glm::quat m_oGlobalQuaternion;
        // 不含缩放的model矩阵(显示axis用)
        glm::mat4 m_oNoScaleModelMatrix;
        
        // 属性面板的欧拉角
        glm::vec3 m_oInspectorEuler;
        // 是否需要更新属性面板欧拉角的值
        bool m_bUpdateInspectorEuler;
        
        // 变换脏标记（供Transform内部使用，用来处理变换）
        bool m_bTransDirty;
        // 当前帧是否变换（供外部使用，用来判断是否发生变换）
        bool m_bCurFrameDirty;
        // 延迟旋转角度队列
        std::vector<glm::vec3> m_vRotateDelayRotations;
        // 延迟旋转坐标空间队列
        std::vector<customGL::Space> m_vRotateDelaySpaces;
        // 多线程计算自身Transform时，需要上锁
        std::shared_ptr<std::mutex> m_pVisitMutex;
        
        // 父节点
        Transform* m_oParent;
        // 子节点列表
        std::vector<Transform*> m_vChildren;
        // 该节点是否是骨骼
        int m_iBoneId;
        // 骨骼矩阵
        glm::mat4 m_oBoneMatrix;
        // bindpose矩阵
		/*
			之前从来没有注意过mOffsetMatrix，其实mOffsetMatrix很重要。模型的顶点绑定到骨骼节点上时，父节点由模型的原点变为骨骼节点，所以需要mOffsetMatrix(即unity中的bindpose)来将顶点从模型空间->当前骨骼的骨骼空间。
			再乘上骨骼节点在播放动画时发生的变换（是相对于骨骼的父节点的，类似于模型空间转换到世界空间，这里是将骨骼自身的变换和它父节点的变换相乘，得到该骨骼从骨骼空间变换到模型空间的矩阵），从而将原始的
			模型顶点变换到动画中它应该在的位置
		*/
        glm::mat4 m_oBindposeMatrix;
        // 骨骼根节点（这里指的是模型的跟节点，所有骨骼的跟节点，这个节点不一定是骨骼）
        Transform* m_oBoneRoot;
        // 骨骼跟节点空间矩阵 (将子节点转换到骨骼跟节点空间，其实就是骨骼跟节点model矩阵的逆。这样boneMatrix就等于这个矩阵乘上子节点的model矩阵)
        glm::mat4 m_oBoneRootSpaceMatrix;
        // 模型原始节点姿态
        bool m_bHasSrcModelInfo;
        glm::vec3 m_oSrcModelPos;
        glm::quat m_oSrcModelQuat;
        glm::vec3 m_oSrcModelScale;
        
        // 名字
        std::string m_sName;
	};
}

