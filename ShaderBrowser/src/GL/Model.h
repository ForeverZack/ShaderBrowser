#pragma once

#include <glad/glad.h>
#include <string>
#include "Common/Tools/Utils.h"
#include "Browser/Components/Mesh/Mesh.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Texture2D.h"
#include "Common/System/Cache/TextureCache.h"
#include "GL/GLProgram.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace browser;

namespace customGL
{
    // assimp默认参数
    #define DEFAULT_ASSIMP_FLAG  aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace
    
    // 模型纹理数据结构
    struct MeshTextureData
    {
        // 纹理所属mesh
        browser::Mesh* mesh;
        // 纹理名称
        std::string filename;
        // 类型
        aiTextureType type;
        // uniform名称
        std::string uniformName;
        // 是否找到该纹理
        bool bFound;
        
        MeshTextureData(browser::Mesh* mesh, std::string name, aiTextureType type, std::string uniform, bool found=true)
            : mesh(mesh)
            , filename(name)
            , type(type)
            , uniformName(uniform)
            , bFound(found)
        {
        }

		MeshTextureData(const MeshTextureData&& data)
			: mesh(data.mesh)
			, filename(data.filename)
			, type(data.type)
			, uniformName(data.uniformName)
			, bFound(data.bFound)
		{

		}
    };
    
    class Model : public Reference
	{
	public:
        // aiProcess_Triangulate: 如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形
        // aiProcess_FlipUVs: 将在处理的时候翻转y轴的纹理坐标
        // aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线
        // aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用
        // aiProcess_OptimizeMeshes：和上个选项相反，它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化
        static Model* create(const char* fileName);
        static Model* createAlone(std::string fileName, std::function<void(Model*)> success);

        
	public:
		Model();
		~Model();
        
    public:
        // 生成MeshFilter模型
        MeshFilter* getOrCreateMeshFilter();
        // 设置vao
        void setupMeshesVAO();
        // 加载纹理
        void loadTextures(const std::string& directory);

	private:
        // 初始化
		bool initWithFile(const char* fileName, unsigned int pFlags);
        // 递归遍历模型节点
        void traverseNode(aiNode* node, const aiScene*& scene);
        // 生成游戏内部使用的网格(aiMesh->Mesh)
        browser::Mesh* generateMesh(unsigned int meshIdx, const aiScene*& scene);
        // 读取纹理数据
        void readTextureData(browser::Mesh* mesh, aiMaterial* material, aiTextureType type, const char* uniformName = GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0]);

        
        
        REGISTER_PROPERTY_CONSTREF_GET(std::string, m_sDirectory, Directory);
        REGISTER_PROPERTY_GET_SET(std::function<void(Model*)>, m_oSuccessCallback, SuccessCallback)
        
    private:
        // 生成的MeshFilter组件
        MeshFilter* m_oMeshFilter;
        
        // 加载路径
        std::string m_sDirectory;
        // 加载完成标记
        bool m_bLoadSuccess;
        // 加载的纹理数量
        int m_iLoadTextureNum;
        // 模型载入成功回调
        std::function<void(Model*)> m_oSuccessCallback;
        
        // 网格模型队列
        std::vector<browser::Mesh*> m_vMeshes;
        // 纹理队列
        std::vector<Texture2D*> m_vTextures;
        // 纹理数据(中转)
        std::vector<MeshTextureData> m_vMeshTextureData;
        
	};
}

