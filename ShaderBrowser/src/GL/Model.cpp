#include "Model.h"
#include "Common/Tools/FileUtils.h"


using namespace common;

namespace customGL
{

	Model* Model::create(const char* fileName, shared_ptr<std::vector<std::string>> animFileNames)
	{
		Model* model = new Model();
		if (!model->initWithFile(fileName, *animFileNames, DEFAULT_ASSIMP_FLAG))
		{
			delete model;
			return nullptr;
		}

		return model;
	}
       
    Model* Model::createAlone(std::string fileName, const std::vector<std::string>& animFiles, std::function<void(Model*)> success, unsigned int pFlags /*= DEFAULT_ASSIMP_FLAG*/)
    {
        Model* model = new Model();
        model->m_oSuccessCallback = success;
        
        if (!model->initWithFile(fileName.c_str(), animFiles, pFlags))
        {
            delete model;
            return nullptr;
        }
        model->autorelease();
        
        return model;
    }

	Model::Model()
        : m_bLoadSuccess(false)
        , m_oSuccessCallback(nullptr)
        , m_iLoadTextureNum(0)
        , m_oMeshFilter(nullptr)
		, m_oRootNode(nullptr)
	{
        // 清理
		m_vImporters.clear();
        m_vMeshes.clear();
        m_vTextures.clear();
        m_vMeshTextureData.clear();
		m_mAnimations.clear();
	}

	Model::~Model()
	{
        BROWSER_LOG("~Model");
        
        // release
        if (m_oMeshFilter)
        {
            m_oMeshFilter->release();
        }
        // 从cache中移除texture
        for(auto itor=m_vTextures.begin(); itor!=m_vTextures.end(); ++itor)
        {
            TextureCache::getInstance()->removeTexture(*itor);
        }
        // 删除mesh
        for(auto itor=m_vMeshes.begin(); itor!=m_vMeshes.end(); ++itor)
        {
            (*itor)->release();
        }
		// 释放模型原始数据
		for (auto itor = m_vImporters.begin(); itor != m_vImporters.end(); ++itor)
		{
			const shared_ptr<Assimp::Importer>& importer = (*itor);
			importer->FreeScene();
		}
	}

	bool Model::initWithFile(const char* fileName, const std::vector<std::string>& animFiles, unsigned int pFlags)
	{
        // 加载模型（主模型文件）
		{
			// 获取文件全名和路径
			std::string model_file = FileUtils::getInstance()->getAbsolutePathForFilename(fileName, m_sDirectory);

			std::shared_ptr<Assimp::Importer> importer = make_shared<Assimp::Importer>();
			const aiScene* scene = importer->ReadFile(model_file, pFlags);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				// 加载模型失败
				cout << "ERROR::ASSIMP::" << importer->GetErrorString() << endl;
				return false;
			}
			m_vImporters.push_back(importer);
			// 加载模型动画数据
			loadAnimations(scene);
			// 加载模型网格数据
			m_oRootNode = scene->mRootNode;
			m_vMeshes.resize(scene->mNumMeshes);
			traverseNode(scene->mRootNode, scene);
		}
		
		// 加载其余的动画文件（动画文件列表）
		{
			for (auto itor = animFiles.begin(); itor != animFiles.end(); ++itor)
			{
				std::shared_ptr<Assimp::Importer> importer = make_shared<Assimp::Importer>();
				const aiScene* scene = importer->ReadFile(*itor, aiProcess_GenSmoothNormals |
					aiProcess_LimitBoneWeights |
					aiProcess_FlipUVs);
				unsigned int fff = scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE;
				if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
				{
					// 加载模型失败
					cout << "ERROR::ASSIMP::" << importer->GetErrorString() << endl;
					return false;
				}
				else
				{
					BROWSER_LOG("success" + (*itor));
				}
			}
		}

        // 加载创建纹理
        if (m_oSuccessCallback)
        {
            loadTextures(m_sDirectory);
        }
        
		return true;
	}
    
    MeshFilter* Model::getOrCreateMeshFilter()
    {
        if (!m_oMeshFilter)
        {
            m_oMeshFilter = MeshFilter::create();
            int i=0;
            for(auto itor=m_vMeshes.begin(); itor!=m_vMeshes.end(); ++itor)
            {
//                if (i < 3)
                {
                    ++i;
                    m_oMeshFilter->addMesh(*itor);
                }
            }
            m_oMeshFilter->retain();
        }
        
        return dynamic_cast<MeshFilter*>(m_oMeshFilter->clone());
    }

	BaseEntity* Model::createNewEntity(const std::string& name)
	{
		BaseEntity* entity = traverseNodeAndCreateEntity(m_oRootNode, nullptr);
		entity->setName(name);
		return entity;
	}

	BaseEntity* Model::traverseNodeAndCreateEntity(aiNode* node, BaseEntity* parent)
	{
		BaseEntity* entity = BaseEntity::create(node->mName.C_Str());
		if (parent)
		{
			parent->addChild(entity);
		}

		if (node->mNumMeshes > 0)
		{
			// MeshFilter组件
			MeshFilter* meshFilter = MeshFilter::create();
			entity->addComponent(meshFilter);
			// 渲染组件
			BaseRender* renderer = BaseRender::createBaseRender();
			entity->addComponent(renderer);
			// 包围盒组件
			entity->addComponent(new AABBBoundBox());
			for (unsigned int i = 0; i < node->mNumMeshes; ++i)
			{
				browser::Mesh* mesh = m_vMeshes[node->mMeshes[i]];
				if (mesh)
				{
					renderer->addMaterial(mesh->getMaterialName(), GLProgram::DEFAULT_GLPROGRAM_NAME);
					meshFilter->addMesh(mesh);
				}
			}
		}
		
		// 继续遍历子节点
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			traverseNodeAndCreateEntity(node->mChildren[i], entity);
		}

		return entity;
	}
    
    void Model::traverseNode(aiNode* node, const aiScene*& scene)
    {
        // 处理该节点所有的网格
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			browser::Mesh* mesh = generateMesh(node->mMeshes[i], scene);
			if (mesh)
			{
				m_vMeshes[node->mMeshes[i]] = mesh;
			}
		}
            
        // 继续遍历子节点
        for (unsigned int i=0; i<node->mNumChildren; ++i)
        {
            traverseNode(node->mChildren[i], scene);
        }
    }
    
    browser::Mesh* Model::generateMesh(unsigned int meshIdx, const aiScene*& scene)
    {
        // 根据aiMesh的id获取aiMesh
        aiMesh* aiMesh = scene->mMeshes[meshIdx];
        if (aiMesh)
        {
            // 注意:如果纹理创建不成功(例如没有找到),应该有一张白色默认纹理来代替,以防程序出问题
            browser::Mesh* mesh = browser::Mesh::create(aiMesh->mNumVertices, aiMesh->mName.C_Str());
            // 顶点位置
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mVertices);
            // 顶点颜色
			if (aiMesh->mColors[0] && aiMesh->mColors[1])
			{
				mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mColors);
			}
            // 顶点uv坐标
            if (aiMesh->mTextureCoords[0])
            {
                mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mTextureCoords[0]);
            }
            // 法线
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mNormals);
            // 切线
            mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), aiMesh->mTangents);
            // 索引信息
            mesh->setIndicesInfo([=](std::vector<GLushort>& indices, unsigned int& indexCount) -> void
                 {
                     indices.clear();
                     indexCount = 0;
                     
                     for(int i=0; i<aiMesh->mNumFaces; ++i)
                     {
                         const aiFace& face = aiMesh->mFaces[i];
                         indexCount += face.mNumIndices;
                         for(int j=0; j<face.mNumIndices; ++j)
                         {
                             indices.push_back(face.mIndices[j]);
                         }
                         
                     }
                 });
            
            // 处理材质(这里的材质指的是纹理)
            if (aiMesh->mMaterialIndex >= 0)
            {
                aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];
                // 材质名称
				mesh->setMaterialName(material->GetName().C_Str());
				// 读取材质默认属性
				aiColor4D color;
				if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS)
				{
					mesh->addColorProperty(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_ALBEDO_COLOR], glm::vec4(color.r, color.g, color.b, color.a));
				}
                // 漫反射纹理: 主纹理   GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0]
                readTextureData(mesh, material, aiTextureType_DIFFUSE);
                // 镜面反射纹理
                readTextureData(mesh, material, aiTextureType_SPECULAR, GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE1]);
                
                
            }
            if (m_oSuccessCallback)
            {
                mesh->setupVAO();
            }
                
            return mesh;
        }
        
        return nullptr;
    }
    
    void Model::setupMeshesVAO()
    {
        for(auto itor=m_vMeshes.begin(); itor!=m_vMeshes.end(); ++itor)
        {
            (*itor)->setupVAO();
        }
    }
    
    void Model::readTextureData(browser::Mesh* mesh, aiMaterial* material, aiTextureType type, const char* uniformName /*= GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0] */)
    {
        int textureCount = material->GetTextureCount(type);
        
        // 当加载不到模型的纹理时,用white_default代替
        if (textureCount == 0)
        {
            MeshTextureData textureData(mesh, "texture/default/default_white.png", type, uniformName, false);
            m_vMeshTextureData.push_back(std::move(textureData));
        }
        
        for(int i=0; i<textureCount; ++i)
        {
            aiString filename;
            material->GetTexture(type, i, &filename);
            
            MeshTextureData textureData(mesh, filename.C_Str(), type, uniformName);
            m_vMeshTextureData.push_back(std::move(textureData));
        }
    }

	void Model::loadAnimations(const aiScene*& scene)
	{
		if (scene->HasAnimations())
		{
			for (int i = 0; i < scene->mNumAnimations; ++i)
			{
				aiAnimation* animation = scene->mAnimations[i];
				std::string name(animation->mName.C_Str());
				m_mAnimations.push_back(std::make_tuple(animation, name));
			}
		}
	}
    
    void Model::loadTextures(const std::string& directory)
    {
        int textureCount = m_vMeshTextureData.size();
        
        for(int i=0; i<m_vMeshTextureData.size(); ++i)
        {
            const MeshTextureData& textureData = m_vMeshTextureData[i];
            // 纹理路径
            std::string full_path;
            if (textureData.bFound)
            {
                full_path = directory + textureData.filename;
            }
            else
            {
                full_path = textureData.filename;
            }
            // 异步载入纹理
            // 注意:Lambda表达式使用的时候要注意,按引用捕获类似于传递实参或者引用,函数内部修改这个变量会传递到外边,但需要注意这个变量的生命周期,不能传递一个在调用之前被销毁的对象。而按值捕获类似于传值,内部修改并不会影响到外部,会在lambda表达式创建的时候生成一份复制,也可以保证外部的变量如果值发生变化,表达式内部该值也不会收到影响。参考下方的i和textureCount。i是一个每次循环都会变化的变量,而lambda表达式内想取到每次循环的固定值,所以按值捕获复制了一份。textureCount是一个局部变量,在lambda表达式函数被调用的时候,如果按引用捕获在调用的时候,访问到的值会不正确。
                //按值传递函数对象参数时，加上mutable修饰符后，可以修改按值传递进来的拷贝（注意是能修改拷贝，而不是值本身）
            TextureCache::getInstance()->addTextureAsync(full_path, [&, i, textureCount](Texture2D* texture) mutable -> void     // 除了itor按值捕获,其他都按引用捕获
                 {
                     // 将texture的环绕方式设为repeat
                     texture->setTexWrapParams(GL_REPEAT, GL_REPEAT);
                     // 记录texture
                     m_vTextures.push_back(texture);
                     
                     browser::Mesh* mesh = m_vMeshTextureData[i].mesh;
                     mesh->addTexture(m_vMeshTextureData[i].uniformName, texture);
                     
                     ++m_iLoadTextureNum;
                     if (m_iLoadTextureNum == textureCount)
                     {
                         // 载入模型成功
                         m_bLoadSuccess = true;
                         
                         if (m_oSuccessCallback)
                         {
                             m_oSuccessCallback(this);
                         }
                     }
                 });
        }
    }
    

}
