// svn测试
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <iostream>
#include "Core/Application.h"
#include "Core/RenderCore.h"
#include "GL/GPUResource/Shader/GLProgram.h"
#include "Browser/Components/Transform/Transform.h"
#include "Browser/Components/Camera/Camera.h"
#include "Browser/Components/Light/DirectionalLight.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "GL/GPUResource/Model/Mesh.h"
#include "Browser/Components/Render/BaseRender.h"
#include "Browser/Components/Render/SkinnedMeshRenderer.h"
#include "GL/GPUResource/Shader/Material.h"
#include "GL/GPUResource/Shader/Pass.h"
#include "Browser/Components/BoundBox/AABBBoundBox.h"
#include "Browser/System/TransformSystem.h"
#include "Common/Tools/FileUtils.h"
#include "Common/Tools/BinaryFileUtils.h"
#include "Browser/System/CameraSystem.h"
#include "GL/GPUResource/Texture/Texture2D.h"
#include "Common/System/ECSManager.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "Common/System/Cache/TextureCache.h"
#include "Common/System/Cache/ModelCache.h"
#include "Common/Tools/UI/GUIFramework.h"
#include "Common/Tools/UI/InspectorPanel.h"
#include "Common/Tools/UI/GameStatusPanel.h"


using namespace customGL;
using namespace browser;
using namespace common;
using namespace core;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>

#include "GL/GPUResource/Model/Model.h"

// 函数需要先声明一下，否则在定义之前调用会编译出错
void mainLoop();
void testVal();

// 加载进度
int _curLoadedNum = 0;
int _totalLoadNum = 8;




//test
// vertices
glm::vec3 vertices[] = {
	// first triangle
    glm::vec3(-0.9f, -0.5f, 0.0f),  // left
	glm::vec3(-0.0f, -0.5f, 0.0f), // right
	glm::vec3(-0.45f, 0.5f, 0.0f),// top
	// second triangle
	glm::vec3(0.0f, -0.5f, 0.0f),  // left
	glm::vec3(0.9f, -0.5f, 0.0f),  // right
	glm::vec3(0.45f, 0.5f, 0.0f)   // top
};
// colors
glm::vec4 colors[] = {
    // first triangle
    glm::vec4(0.9f, 0.0f, 0.0f, 1.0f),  // left
    glm::vec4(0.0f, 0.5f, 0.0f, 1.0f), // right
    glm::vec4(0.45f, 0.5f, 0.0f, 1.0f),// top
    // second triangle
    glm::vec4(0.0f, 0.5f, 0.0f, 1.0f),  // left
    glm::vec4(0.9f, 0.5f, 0.0f, 1.0f),  // right
    glm::vec4(0.45f, 0.5f, 0.0f, 1.0f)   // top
};
// coords
glm::vec3 coords[] = {
    // first triangle
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.5f, 1.0f, 0.0f),
    // second triangle
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.5f, 1.0f, 0.0f)
};
// indices
GLushort indices[] = {
    // first triangle
    0, 1, 2,
    // second triangle
    3, 4, 5
};

Model* m_oModel = nullptr;	//纳米装
Model* m_oModel2 = nullptr;	//Fighter
Model* m_oModelLamp = nullptr;	//拿灯
Model* m_oModelYBot = nullptr;
Model* m_oModelUnity = nullptr;

std::unordered_map<std::string, Model*> m_mModels;

BaseEntity* m_YBotEntity = nullptr;

void testVal()
{
//    std::string path = FileUtils::getInstance()->convertToAbsolutePath("./res/texture/awesomeface1.png");
    //std::string path = FileUtils::getInstance()->convertToAbsolutePath("res");
	//FileUtils::getInstance()->addSearchPath(".");
	//FileUtils::getInstance()->addSearchPath("res");
	//std::string directoryPath;
	//std::string filepath = FileUtils::getInstance()->getAbsolutePathForFilename("texture/awesomeface.png", directoryPath);

	//// 创建纹理
	//TextureCache::getInstance()->addTexture("./res/texture/awesomeface.png");
	//TextureCache::getInstance()->addTexture("texture/HelloWorld.png");


	// 创建场景根节点
	BaseEntity* scene = BaseEntity::create("scene");
    scene->setIsAxisVisible(true);
    scene->addComponent(BaseRender::createBaseRender());
	scene->retain();
	browser::TransformSystem::getInstance()->setScene(scene);   //设置场景节点

	//=============================创建相机===================================
	// 相机Entity
	BaseEntity* mainCameraEntity = BaseEntity::create("MainCamera");
	scene->addChild(mainCameraEntity);
	// 相机Camera组件
	browser::Camera* mainCamera = Camera::create(Camera::ProjectionType::Perspective, 0.3f, 1000.0f, SCR_WIDTH, SCR_HEIGHT, 60.0f);
	mainCameraEntity->addComponent(mainCamera);
	// 设置相机位置
	mainCameraEntity->setPosition(0, 1.5, 30);
    mainCameraEntity->setEulerAngle(0, 180, 0);
	// 设置主相机
	CameraSystem::getInstance()->setMainCamera(mainCamera);
	//=============================创建相机==================================
    
    //=============================创建平行光==================================
    BaseEntity* directionalLightEntity = BaseEntity::create("Directional Light");
    scene->addChild(directionalLightEntity);
    // 平行光组件
    DirectionalLight* directionalLight = new DirectionalLight();
    directionalLightEntity->addComponent(directionalLight);
    // 设置平行光位置（没啥用）
    directionalLightEntity->setPosition(100, 100, 100);
    // 平行光方向
    directionalLight->setDirection(-1, -1, 1);
    // 平行光强度
    directionalLight->setIntensity(0.5f);
    
    //=============================创建平行光==================================


	// 载入纹理
	Texture2D* texture1 = TextureCache::getInstance()->getTexture("res/texture/awesomeface.png");
    Texture2D* texture2 = TextureCache::getInstance()->getTexture("texture/HelloWorld.png");

    // MeshFilter
    // (GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* data)
    Mesh* mesh = Mesh::create(6);
    mesh->setVertices(vertices);
    mesh->setColors(colors);
    mesh->setUVs(coords);
    mesh->setIndices(indices, 6);
    //mesh->addTexture("CGL_TEXTURE0", texture1);
    //mesh->addTexture("CGL_TEXTURE1", texture2);
	mesh->retain();

	// 着色器程序
	//GLProgram* program = GLProgram::create("./res/shaders/triangles.vert", "./res/shaders/triangles.frag");
    //GLProgramCache::getInstance()->addGLProgram(GLProgram::DEFAULT_GLPROGRAM_NAME, program);
    GLProgramCache::getInstance()->addGLProgram("Triangles", "shaders/triangles.vert", "shaders/triangles.frag");


    // 将组件加入渲染系统队列
    BaseEntity* entity = BaseEntity::create("Triangle");
    scene->addChild(entity);
    entity->setEulerAngle(0, 45, 0);
    entity->setScale(2, 1, 1);
    entity->setPosition(0, 1, 0);
    // 组件：渲染组件
    BaseRender* renderCom = BaseRender::createBaseRender();
    Material* mat = renderCom->changeMaterial(0, mesh->getMaterialName(), "Triangles");   // 修改mesh的材质对应的shader
	mat->setUniformTex2D("CGL_TEXTURE0", texture1);
	mat->setUniformTex2D("CGL_TEXTURE1", texture2);
    entity->addComponent(renderCom);
    // 包围盒
    entity->addComponent(new AABBBoundBox());
    // meshFilter
    browser::MeshFilter* meshFilter = MeshFilter::create();
    meshFilter->addMesh(mesh);
    entity->addComponent(meshFilter);

    
    // "models/Fighter/fighterChar.FBX", "models/nanosuit/nanosuit.obj", "models/man/model.dae", "models/YBot/ybot.fbx" }
    m_oModel2 = m_mModels[FileUtils::getInstance()->getAbsolutePathForFilename("models/Fighter/fighterChar.FBX")];
    m_oModel = m_mModels[FileUtils::getInstance()->getAbsolutePathForFilename("models/nanosuit/nanosuit.obj")];
    m_oModelLamp = m_mModels[FileUtils::getInstance()->getAbsolutePathForFilename("models/man/model.dae")];
    m_oModelYBot = m_mModels[FileUtils::getInstance()->getAbsolutePathForFilename("models/YBot/ybot.fbx")];
    m_oModelUnity = m_mModels[FileUtils::getInstance()->getAbsolutePathForFilename("models/RawMocap/DefaultAvatar.fbx")];

    // 渲染模型0
    BaseEntity* modelEntity = m_oModel->createNewEntity("namizhuang");
    modelEntity->setScale(0.2f, 0.2f, 0.2f);
    modelEntity->setEulerAngle(0, 180, 0);
    modelEntity->setEulerAngle(0, 0, 0);
//    modelEntity->getTransform()->setQuaternion(0.144154,0.155388,0.259979,0.942064);
    modelEntity->setPosition(0, 1, 0);
    scene->addChild(modelEntity);

    // 渲染模型2    fighter => aabb Min:-20, 0, -13  Max:20, 80, 15
    modelEntity = m_oModel2->createNewEntity("fighter");
    modelEntity->setScale(0.2f, 0.2f, 0.2f);
    modelEntity->setEulerAngle(90, 90, 0);
//    modelEntity->getTransform()->setQuaternion(0, -0.707106709, 0, 0.707106829);
    modelEntity->setPosition(1, 0, -2);
    scene->addChild(modelEntity);
    modelEntity->playAnimation("Take 001", true);
//    modelEntity->changeAllMeshesMaterial(GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME);
	SkinnedMeshRenderer* fighterSkinedMeshRenderer = modelEntity->getComponent<browser::Transform>()->getChildren()[0]->getBelongEntity()->getComponent<SkinnedMeshRenderer>();
    browser::MeshFilter* fighterMeshFilter = fighterSkinedMeshRenderer->getMeshFilter();
//    MeshFilter* fighterMeshFilter = static_cast<SkinnedMeshRenderer*>(modelEntity->getTransform()->getChildren()[0]->getBelongEntity()->getRenderer())->getMeshFilter();
    //fighterMeshFilter->getMeshes()[0]->setTexture(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0], TextureCache::getInstance()->getTexture("models/Fighter/Fighter.png"));
	fighterSkinedMeshRenderer->getMaterialByIndex()->setUniformTex2D(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0], TextureCache::getInstance()->getTexture("models/Fighter/Fighter.png"));

    // MeshFilter组件
//    MeshFilter* fighterMeshFilter = modelEntity->getTransform()->getChildren()[0]->getBelongEntity()->getMeshFilter();
//    MeshFilter* fighterMeshFilter = modelEntity->getTransform()->getChildren()[0]->getChildren()[0]->getBelongEntity()->getMeshFilter();
//    fighterMeshFilter->getMeshes()[0]->setTexture(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0], TextureCache::getInstance()->getTexture("models/Fighter/Fighter.png"));

    // 渲染模型3
    modelEntity = m_oModelLamp->createNewEntity("LampBob");
    modelEntity->setEulerAngle(-90, 90, 0);
    modelEntity->setPosition(10, 0, 5);
    scene->addChild(modelEntity);
    modelEntity->playAnimation(Animator::DEFAULT_ANIMATION_NAME+"0", true);
    //modelEntity->getAnimator()->setUseGPU(false);
//    modelEntity->changeAllMeshesMaterial(GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME);

    // 模型4 RawMocap
    modelEntity = m_oModelUnity->createNewEntity("RawMocap");
    modelEntity->setScale(0.1f, 0.1f, 0.1f);
    modelEntity->setPosition(-10, -10, -50);
    scene->addChild(modelEntity);
//    modelEntity->playAnimation("_1_Edit1_WalkFWD", true);
    modelEntity->playAnimation("_3_a_U1_M_P_WalkAvoid_ToLeft_Both_Fb_p0_No_0_1", true);
//    modelEntity->changeAllMeshesMaterial(GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME);
//    modelEntity->getAnimator()->setUseGPU(false);

    // 模型5 yBot
    modelEntity = m_oModelYBot->createNewEntity("yBot");
    modelEntity->setScale(0.05f, 0.05f, 0.05f);
    modelEntity->setPosition(-10, 0, 5);
    scene->addChild(modelEntity);
    modelEntity->playAnimation(1, true);
//    modelEntity->changeAllMeshesMaterial(GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME);
    m_YBotEntity = modelEntity;

    
    
	// 性能测试
	//for (int i = 0; i < 20; ++i)
	//{
 //       modelEntity = BaseEntity::create("namizhuang");
	//	modelEntity->setScale(0.2f, 0.2f, 0.2f);
	//	modelEntity->setEulerAngle(0, 180, 0);
	//	modelEntity->setPosition(i, 1, 0);
	//	scene->addChild(modelEntity);
	//	// 渲染组件
	//	modelEntity->addComponent(BaseRender::createBaseRender());
	//	// 包围盒
	//	modelEntity->addComponent(new AABBBoundBox());
	//	// MeshFilter组件
	//	modelEntity->addComponent(m_oModel->getOrCreateMeshFilter());
	//}


////    glm::mat4 mat = transCom->getModelMatrix();
////    BROWSER_LOG_MAT4(mat);
//    browser::Transform* node = new browser::Transform();
//    node->setName("node");
////    node->setPosition(11,2,3);
////    node->setScale(1, 2, 3);
////    node->setEulerAngle(90, 90, 0);
//    node->Rotate(glm::vec3(45, 45, 45), customGL::Space::Self);
//    node->Rotate(glm::vec3(30, 300, 45), customGL::Space::World);
////    node->Rotate(glm::vec3(45, 45, 45), customGL::Space::Self);
//    scene->addChild(node);
    
//    glm::quat aa = glm::quat(-0.406684f, -0.335421f, -0.337035f, -0.78007f);
////    node->setQuaternion(aa);
////    node->setQuaternion(node->getGlobalQuaternion());
////    node->setQuaternion(-0.335421f, -0.337035f, -0.78007f, -0.406684f);
////    node->setGlobalQuaternion(-0.335421f, -0.337035f, -0.78007f, -0.406684f);
//    node->setGlobalQuaternion(aa);
//    BROWSER_LOG("=============================");
//    BROWSER_LOG_QUATERNION(node->getGlobalQuaternion());
//    BROWSER_LOG_QUATERNION(node->getQuaternion());
//    BROWSER_LOG_VEC3(glm::degrees(node->getEulerAngle()));
//    node->setGlobalPosition(10, 10, 10);
//    BROWSER_LOG_VEC3(node->getPosition());
//    glm::quat qqq = node->getGlobalQuaternion();
//    BROWSER_LOG_QUATERNION(qqq);
//    BROWSER_LOG_MAT4(node->getModelMatrix());
//    node->setEulerAngle(45, 45, 45);


 //   Assimp::Importer importer;
	//const aiScene *scene1 = importer.ReadFile("./res/models/nanosuit/nanosuit.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
	//Assimp::Importer importer2;
	//const aiScene *scene2 = importer2.ReadFile("./res/models/Fighter/fighter char.FBX", aiProcess_Triangulate | aiProcess_FlipUVs);
    
    
    
}

int main()
{
	Application* app = new Application();

    TextureCache::getInstance()->addTexturesAsync({ "texture/awesomeface.png", "texture/HelloWorld.png", "models/Fighter/Fighter.png" }, [&](Texture2D* texture) {
            ++_curLoadedNum;
            common::GUIFramework::getInstance()->getGameStatusPanel()->setLoadPercent(_curLoadedNum/(float)_totalLoadNum);
            if (_curLoadedNum == _totalLoadNum)
            {
                testVal();
            }
        });
        
        int modelIdx = 0;
    //    m_oModel = Model::createAlone("models/nanosuit/nanosuit.obj", [&](Model* mo)     //"models/Fighter/fighterChar.FBX"
    //    ModelCache::getInstance()->addModel("models/nanosuit/nanosuit.obj", [&](Model* mo)
        ModelCache::getInstance()->addModelsAsync(
            { "models/Fighter/fighterChar.FBX", "models/nanosuit/nanosuit.obj", "models/man/model.dae", "models/RawMocap/DefaultAvatar.fbx", "models/YBot/ybot.fbx" },
              { {}, {}, {}, {"models/RawMocap/Animations/Walking/WalkFWD.fbx", "models/RawMocap/Animations/Idle/Idle_Ready.fbx", "models/RawMocap/Animations/Walking/WalkAvoid_ToLeft_Both.fbx"}, {"models/YBot/Localmotion/walking.fbx", "models/YBot/Localmotion/running.fbx"} }, [&](Model* mo)
                 {
                        m_mModels.insert(make_pair(mo->getFullPath(), mo));
                        ++modelIdx;
                        ++_curLoadedNum;
                        common::GUIFramework::getInstance()->getGameStatusPanel()->setLoadPercent(_curLoadedNum / (float)_totalLoadNum);
                        if (_curLoadedNum == _totalLoadNum)
                        {
                            testVal();
                        }
                 });

	// run
	app->run();

    
    return 0;
}



