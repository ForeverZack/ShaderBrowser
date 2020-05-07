#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common/Components/Reference.h"


using namespace common;

namespace customGL
{
    /*
         骨架类: 记录了模型中所有骨骼的信息
     */
    class Skeleton : public Reference
	{
    public:
        // 检测是否是assimp生成的辅助枢轴(preserve pivot)，暂时是通过名称来检查的，后面看看有没有更好的优化方法
        static bool isPreservePivot(aiNode* node);
        
	public:
		Skeleton();
		~Skeleton();
        
    public:
        // 添加骨骼
        unsigned int addBone(aiBone* bone);
        // assimp生成的辅助枢轴(PreservePivots)也要算作骨骼，但它不会影响并不会影响顶点的boneIndices和boneWeights。算作骨骼更多的是告诉Animator它参与骨骼动画的计算
        unsigned int addPreservePivot(aiNode* pivotNode);
        // 获取骨骼数量 (包含assimp生成的辅助枢轴(Preserve Pivots))
        unsigned int getBoneNum();
        // 
        // 获取骨骼信息
        aiBone* getBone(unsigned int boneId);
		// 检测节点是否是骨骼
		bool isBone(const char* boneName);
		bool isBone(const char* boneName, aiBone*& bone);
		bool isBone(const char* boneName, unsigned int& boneId);
		bool isBone(const char* boneName, unsigned int& boneId, aiBone*& bone);
		bool isBone(const std::string& boneName);
		bool isBone(const std::string& boneName, aiBone*& bone);
		bool isBone(const std::string& boneName, unsigned int& boneId);
        
        
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<aiBone*>, m_vBones, Bones)
        const std::unordered_map<std::string, unsigned int>& getBonesIdMap()
        {
            return m_mBonesIdMap;
        }
        std::unordered_map<std::string, unsigned int>* getBonesIdMapPointer()
        {
            return &m_mBonesIdMap;
        }
        std::vector<glm::vec4>& getBonesInitPositionRef()
        {
            return m_vBonesInitPosition;
        }
        std::vector<glm::vec4>& getBonesInitRotationRef()
        {
            return m_vBonesInitRotation;
        }
        std::vector<glm::vec4>& getBonesInitScaleRef()
        {
            return m_vBonesInitScale;
        }
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<glm::vec4>, m_vBonesInitPosition, BonesInitPosition)
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<glm::vec4>, m_vBonesInitRotation, BonesInitRotation)
        REGISTER_PROPERTY_CONSTREF_GET(std::vector<glm::vec4>, m_vBonesInitScale, BonesInitScale)
        REGISTER_PROPERTY_GET_SET(aiNode*, m_pRootBoneNode, RootBoneNode)
        
    private:
        // 骨骼列表
        std::vector<aiBone*> m_vBones;
        // 骨骼根节点
        aiNode* m_pRootBoneNode;
		// 骨骼id列表
		std::unordered_map<std::string, unsigned int> m_mBonesIdMap;
        // 骨骼节点初始变换 (bindpose)
        std::vector<glm::vec4> m_vBonesInitPosition;
        std::vector<glm::vec4> m_vBonesInitRotation;
        std::vector<glm::vec4> m_vBonesInitScale;
        // assimp枢轴(FBX_PRESERVE_PIVOTS)生成的骨骼
        std::vector<aiBone*> m_vPreservePivots;
        
        // test
        bool hasInit;
        int sizeSK;
	};
}

