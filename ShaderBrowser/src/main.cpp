// svn测试
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>


#include <iostream>
#include "GL/GLProgram.h"
#include "Browser/Components/Transform/Transform.h"
#include "Browser/Components/Camera/Camera.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Browser/Components/Mesh/Mesh.h"
#include "Browser/Components/Render/BaseRender.h"
#include "Browser/Components/Render/SkinnedMeshRenderer.h"
#include "Browser/Components/Render/Material.h"
#include "Browser/Components/Render/Pass.h"
#include "Browser/Components/BoundBox/AABBBoundBox.h"
#include "Browser/System/RenderSystem.h"
#include "Browser/System/TransformSystem.h"
#include "Browser/System/MeshSystem.h"
#include "Browser/System/LightSystem.h"
#include "Browser/System/AnimationSystem.h"
#include "Common/Tools/FileUtils.h"
#include "Browser/System/CameraSystem.h"
#include "Browser/System/BoundBoxSystem.h"
#include "Common/System/AutoReleasePool.h"
#include "GL/Texture2D.h"
#include "Common/System/ECSManager.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "Common/System/Cache/TextureCache.h"
#include "Common/System/Cache/ModelCache.h"
#include "Common/Tools/UI/GUIFramework.h"
#include "Common/Tools/UI/InspectorPanel.h"
#include "Common/Tools/UI/GameStatusPanel.h"
#include "GL/GLStateCache.h"
#ifdef __APPLE__
// macOS
#include <unistd.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/OpenGL.h>
#else
// win32
#include <windows.h>
#include <wingdi.h>
#pragma warning(disable:4996)
#endif


using namespace customGL;
using namespace browser;
using namespace common;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <chrono>
#include <unordered_map>

#include "GL/Model.h"

// 函数需要先声明一下，否则在定义之前调用会编译出错
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void renderScene();
void mainLoop(GLFWwindow *window);
GLFWwindow* init();
void testVal();


// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// 标题
char strTitle[150];

// 上一次更新的时间戳
std::chrono::steady_clock::time_point _lastUpdate;

#ifdef _WIN32
	typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
	typedef int(*PFNWGLEXTGETSWAPINTERVALPROC) (void);
	PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
	PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;
	// 初始化函数指针接口
	bool InitVSync()
	{
		BROWSER_LOG(glGetString(GL_RENDERER));
		BROWSER_LOG(glGetString(GL_VERSION));
		BROWSER_LOG(glGetString(GL_VENDOR));

		// 注意：获取扩展（Extensions）只有在GLFW_OPENGL_COMPAT_PROFILE模式下才能获取到，以核心模式运行时获取不到，具体原因不清楚
		const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
		if (extensions && strstr(extensions, "WGL_EXT_swap_control"))
		{
			wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)wglGetProcAddress("wglSwapIntervalEXT");
			wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
			return true;
		}

		return false;
	}

	// 判断当前状态是否为垂直同步
	bool IsVSyncEnabled()
	{
		return (wglGetSwapIntervalEXT() > 0);
	}

	// 开启和关闭垂直同步
	void SetVSyncState(bool enable)
	{
		if (!wglSwapIntervalEXT)
		{
			return;
		}
		if (enable)
		{
			wglSwapIntervalEXT(1);
		}
		else
		{
			wglSwapIntervalEXT(0);
		}
	}

#endif // WIN32



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



	// 载入纹理
	Texture2D* texture1 = TextureCache::getInstance()->getTexture("res/texture/awesomeface.png");
    Texture2D* texture2 = TextureCache::getInstance()->getTexture("texture/HelloWorld.png");

    // MeshFilter
    // (GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* data)
    browser::Mesh* mesh = Mesh::create(6);
    mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), vertices);
    mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), colors);
    mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), coords);
    mesh->setIndicesInfo(indices, 6);
    mesh->addTexture("CGL_TEXTURE0", texture1);
    mesh->addTexture("CGL_TEXTURE1", texture2);
    mesh->setupVAO();
	

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
    renderCom->changeMaterial(0, mesh->getMaterialName(), "Triangles");   // 修改mesh的材质对应的shader
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
    modelEntity->setPosition(0, 1, 0);
    scene->addChild(modelEntity);

    // 渲染模型2    fighter => aabb Min:-20, 0, -13  Max:20, 80, 15
    modelEntity = m_oModel2->createNewEntity("fighter");
    modelEntity->setScale(0.2f, 0.2f, 0.2f);
    modelEntity->setEulerAngle(0, 90, 0);
    modelEntity->setPosition(1, 0, -2);
    scene->addChild(modelEntity);
    modelEntity->playAnimation("Take 001", true);
//    modelEntity->changeAllMeshesMaterial(GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME);
    browser::MeshFilter* fighterMeshFilter = static_cast<SkinnedMeshRenderer*>(modelEntity->getTransform()->getChildren()[0]->getChildren()[0]->getBelongEntity()->getRenderer())->getMeshFilter();
//    MeshFilter* fighterMeshFilter = static_cast<SkinnedMeshRenderer*>(modelEntity->getTransform()->getChildren()[0]->getBelongEntity()->getRenderer())->getMeshFilter();
    fighterMeshFilter->getMeshes()[0]->setTexture(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0], TextureCache::getInstance()->getTexture("models/Fighter/Fighter.png"));

    // MeshFilter组件
//    MeshFilter* fighterMeshFilter = modelEntity->getTransform()->getChildren()[0]->getBelongEntity()->getMeshFilter();
//    MeshFilter* fighterMeshFilter = modelEntity->getTransform()->getChildren()[0]->getChildren()[0]->getBelongEntity()->getMeshFilter();
//    fighterMeshFilter->getMeshes()[0]->setTexture(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0], TextureCache::getInstance()->getTexture("models/Fighter/Fighter.png"));

    // 渲染模型3
    modelEntity = m_oModelLamp->createNewEntity("LampBob");
    modelEntity->setEulerAngle(0, 90, 0);
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
	GLFWwindow* window = init();

	// 关闭垂直同步
#ifdef _WIN32
	// win32
	bool isOk = InitVSync();
	if (isOk)
	{
		SetVSyncState(false);
	}
#endif // WIN32
    
#ifdef __APPLE__
    //    // mac获取extension的方法：3.0+的profile对glGetString传GL_EXTENSIONS已经被deprecated，属于invalid enumeration，正确的方法是用glGetStringi代替
    //    {
    //        GLint n, i;
    //        glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    //        for (i = 0; i < n; i++)
    //        {
    //            printf("%s\n", glGetStringi(GL_EXTENSIONS, i));
    //        }
    //    }
    // 禁用mac垂直同步
    GLint sync = 0;
    CGLContextObj ctx = CGLGetCurrentContext();
    CGLSetParameter(ctx, kCGLCPSwapInterval, &sync);
#endif

	int total = 0;
	TextureCache::getInstance()->addTexturesAsync({ "texture/awesomeface.png", "texture/HelloWorld.png", "models/Fighter/Fighter.png" }, [&](Texture2D* texture) {
		++total;
		if (total == 8)
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
                    ++total;
                    if (total == 8)
                    {
                        testVal();
                    }
             });




	//// test
	//testVal();

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 渲染循环
    _lastUpdate = std::chrono::steady_clock::now();
	while (!glfwWindowShouldClose(window))
	{
		mainLoop(window);
	}


	// glfw: terminate, clearing all previously allocated GLFW resources.
	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
	return 0;
}

GLFWwindow* init()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
	// macOS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    // mac下只能用GLFW_OPENGL_CORE_PROFILE核心模式，不然程序会报错
#else
    // win32
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);    // 这里如果用GLFW_OPENGL_CORE_PROFILE核心模式，则获取不到extensions
#endif


    // glfw window creation
    // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShaderBrowser", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	ImGui_ImplGlfwGL3_Init(window, true);

	ImGui::StyleColorsDark();

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}


	// 初始化调试GUI框架
    common::GUIFramework::getInstance()->init(SCR_WIDTH, SCR_HEIGHT);

	// 添加搜索路径
	FileUtils::getInstance()->addSearchPath(".");
	FileUtils::getInstance()->addSearchPath("res");

	// 注册基本系统
	ECSManager::getInstance()->registerSystem(browser::RenderSystem::getInstance());	// 渲染系统
    ECSManager::getInstance()->registerSystem(browser::TransformSystem::getInstance()); // Transform
    ECSManager::getInstance()->registerSystem(browser::MeshSystem::getInstance()); // Mesh
	ECSManager::getInstance()->registerSystem(browser::CameraSystem::getInstance());	// Camera
    ECSManager::getInstance()->registerSystem(browser::BoundBoxSystem::getInstance()); // 包围盒
    ECSManager::getInstance()->registerSystem(browser::LightSystem::getInstance()); // 灯光系统
    ECSManager::getInstance()->registerSystem(browser::AnimationSystem::getInstance()); // 动画系统
	// 初始化系统
	ECSManager::getInstance()->initSystem(SystemType::RenderSystem);    // 渲染系统
    ECSManager::getInstance()->initSystem(SystemType::Transform);    // Transform
	ECSManager::getInstance()->initSystem(SystemType::MeshFilter);    // Mesh
	ECSManager::getInstance()->initSystem(SystemType::Camera);    // Camera
    ECSManager::getInstance()->initSystem(SystemType::BoundBox);    // 包围盒
    ECSManager::getInstance()->initSystem(SystemType::Light);   // Light
    ECSManager::getInstance()->initSystem(SystemType::Animation);   // Animation
    // 加载缓存
    GLProgramCache::getInstance()->init();  // 着色器缓存


	return window;
}

void destory()
{

}

void mainLoop(GLFWwindow *window)
{
    // 计算deltaTime
    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastUpdate).count() / 1000000.0f;
    deltaTime = deltaTime<0 ? 0 : deltaTime;
    _lastUpdate = now;

	// 1.input
	glfwPollEvents();
	processInput(window);
    
    
	// temp
	TextureCache::getInstance()->update(deltaTime);
    ModelCache::getInstance()->update(deltaTime);

	// 重置GL状态（这里主要是为了防止插件如imgui绑定纹理，造成缓存失效）
	GLStateCache::getInstance()->update(deltaTime);

    
    // beforeUpdate
    ECSManager::getInstance()->beforeUpdateSystem(SystemType::Transform, deltaTime); // 在所有系统刷新前刷新transform
    
	// 2.render
	ECSManager::getInstance()->updateSystem(SystemType::Transform, deltaTime);  // 更新transform
	ECSManager::getInstance()->updateSystem(SystemType::Camera, deltaTime);  // 更新camera
    ECSManager::getInstance()->updateSystem(SystemType::BoundBox, deltaTime);   // 更新BoundBox
    ECSManager::getInstance()->updateSystem(SystemType::Animation, deltaTime);   // 更新动画系统
	ECSManager::getInstance()->updateSystem(SystemType::RenderSystem, deltaTime);   // 更新渲染系统
    //BROWSER_LOG(deltaTime);


	// 更新调试信息
	common::GUIFramework::getInstance()->getGameStatusPanel()->setDeltaTime(deltaTime);
	// 更新调试GUI框架
	common::GUIFramework::getInstance()->update(deltaTime);
	// ImGui rendering
	ImGui::Render();


	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(window);

	// auto release
	AutoReleasePool::getInstance()->update();
    
	// 锁帧
//    float fps = ;
//    if (deltaTime < 1.0f/60.0f)
//    {
//        sleep((1.0f/60.0f - deltaTime));
////        deltaTime = 1.0f/60.0f;
//        fps = 60;
//    }

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	//{
	//	//popup_menu();
	//}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	browser::Camera* camera = CameraSystem::getInstance()->getMainCamera();
	if (camera)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			browser::Transform* transform = camera->getBelongEntity()->getTransform();
            transform->Translate(transform->getForward() * 0.1f);
        }
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			browser::Transform* transform = camera->getBelongEntity()->getTransform();
            transform->Translate(-transform->getForward() * 0.1f);
        }
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			browser::Transform* transform = camera->getBelongEntity()->getTransform();
			transform->Translate(transform->getLeft() * 0.1f);
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			browser::Transform* transform = camera->getBelongEntity()->getTransform();
            transform->Translate(-transform->getLeft() * 0.1f);
		}
        else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            browser::Transform* transform = camera->getBelongEntity()->getTransform();
            transform->Rotate(glm::vec3(0, -1.0f, 0), Space::Self);
        }
        else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            browser::Transform* transform = camera->getBelongEntity()->getTransform();
            transform->Rotate(glm::vec3(0, 1.0f, 0), Space::Self);
        }
        else if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            static int playAniIndx = 2;
            m_YBotEntity->getAnimator()->blendTo(playAniIndx, true, 1, true);
        }
	}
	

}

void window_size_callback(GLFWwindow* window, int width, int height)
{

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
