#include "Skeleton.h"

using namespace common;

namespace customGL
{
    bool Skeleton::isPreservePivot(aiNode* node)
    {
        std::string nodeName(node->mName.C_Str());
        return nodeName.find("$AssimpFbx$")!=-1;
    }
    
    // 骨架类
    Skeleton::Skeleton()
        : m_pRootBoneNode(nullptr)
        // test
        , hasInit(false)
        , sizeSK(0)
   {
        this->autorelease();
       
       hasInit = true;
       sizeSK = m_mBonesIdMap.size();
    }
    
    Skeleton::~Skeleton()
    {
        // 干掉手动靠assimp枢轴生成的骨骼
        for (auto itor=m_vPreservePivots.begin(); itor!=m_vPreservePivots.end(); ++itor)
        {
            BROWSER_SAFE_RELEASE_POINTER(*itor);
        }
        m_vPreservePivots.clear();
    }
    
    unsigned int Skeleton::addBone(aiBone* bone)
    {
        int boneIdx;
        auto boneIdItor = m_mBonesIdMap.find(bone->mName.C_Str());
        if (boneIdItor != m_mBonesIdMap.end())
        {
            // 骨骼已存在
            boneIdx = boneIdItor->second;
        }
        else
        {
            // 骨骼id要自己生成
            boneIdx = m_vBones.size();
            m_vBones.push_back(bone);
            m_mBonesIdMap[bone->mName.C_Str()] = boneIdx;
        }
        return boneIdx;
    }
    
    unsigned int Skeleton::addPreservePivot(aiNode* pivotNode)
    {
        int boneIdx;
        auto boneIdItor = m_mBonesIdMap.find(pivotNode->mName.C_Str());
        if (boneIdItor != m_mBonesIdMap.end())
        {
            // 骨骼已存在
            boneIdx = boneIdItor->second;
        }
        else
        {
            // 骨骼id要自己生成
            boneIdx = m_vBones.size();
            aiBone* bone = new aiBone();
            bone->mName = pivotNode->mName;
            m_vBones.push_back(bone);
            m_vPreservePivots.push_back(bone);
            m_mBonesIdMap[bone->mName.C_Str()] = boneIdx;
        }
        return boneIdx;
    }
    
    unsigned int Skeleton::getBoneNum()
    {
        return m_vBones.size();
    }

    aiBone* Skeleton::getBone(unsigned int boneId)
    {
        BROWSER_ASSERT(boneId<m_vBones.size(), "BoneId is large than vector of bones size, Please check the params in function Skeleton::getBone.");
        return m_vBones[boneId];
    }

	bool Skeleton::isBone(const char* boneName)
	{
		return m_mBonesIdMap.find(boneName) != m_mBonesIdMap.end();
	}

	bool Skeleton::isBone(const char* boneName, aiBone*& bone)
	{
		bone = nullptr;
		auto itor = m_mBonesIdMap.find(boneName);
		if (itor != m_mBonesIdMap.end())
		{
			bone = m_vBones[itor->second];
			return true;
		}
		return false;
	}

	bool Skeleton::isBone(const char* boneName, unsigned int& boneId)
	{
		auto size = m_mBonesIdMap.size();

		auto itor = m_mBonesIdMap.find(boneName);
		if (itor != m_mBonesIdMap.end())
		{
			boneId = itor->second;
			return true;
		}
		return false;
	}

	bool Skeleton::isBone(const char* boneName, unsigned int& boneId, aiBone*& bone)
	{
		bone = nullptr;
		auto itor = m_mBonesIdMap.find(boneName);
		if (itor != m_mBonesIdMap.end())
		{
			boneId = itor->second;
			bone = m_vBones[boneId];
			return true;
		}
		return false;
	}

	bool Skeleton::isBone(const std::string& boneName)
	{
		return m_mBonesIdMap.find(boneName) != m_mBonesIdMap.end();
	}

	bool Skeleton::isBone(const std::string& boneName, aiBone*& bone)
	{
		bone = nullptr;
		auto itor = m_mBonesIdMap.find(boneName);
		if (itor != m_mBonesIdMap.end())
		{
			bone = m_vBones[itor->second];
			return true;
		}
		return false;
	}

	bool Skeleton::isBone(const std::string& boneName, unsigned int& boneId)
	{
		auto itor = m_mBonesIdMap.find(boneName);
		if (itor != m_mBonesIdMap.end())
		{
			boneId = itor->second;
			return true;
		}
		return false;
	}


    
}
