// svn测试
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "GL/GLProgram.h"
#include "Browser/Components/Transform/Transform.h"
#include "Browser/Components/Mesh/MeshFilter.h"
#include "Browser/Components/Render/BaseRender.h"
#include "Browser/Components/Render/Material.h"
#include "Browser/Components/Render/Pass.h"
#include "Browser/System/RenderSystem.h"
#include "Browser/System/TransformSystem.h"
#include "Browser/System/MeshSystem.h"
#include "Common/System/AutoReleasePool.h"
#include "GL/Texture2D.h"
#include "Common/System/ECSManager.h"
#ifdef __APPLE__
#include <unistd.h>
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

// 函数需要先声明一下，否则在定义之前调用会编译出错
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void renderScene();
void mainLoop(GLFWwindow *window);
GLFWwindow* init();
void testVal();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


//test
// vertices
glm::vec4 vertices[] = {
	// first triangle
    glm::vec4(-0.9f, -0.5f, 0.0f, 1.0f),  // left
	glm::vec4(-0.0f, -0.5f, 0.0f, 1.0f), // right
	glm::vec4(-0.45f, 0.5f, 0.0f, 1.0f),// top
	// second triangle
	glm::vec4(0.0f, -0.5f, 0.0f, 1.0f),  // left
	glm::vec4(0.9f, -0.5f, 0.0f, 1.0f),  // right
	glm::vec4(0.45f, 0.5f, 0.0f, 1.0f)   // top
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
glm::vec2 coords[] = {
    // first triangle
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(0.5f, 1.0f),
    // second triangle
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(0.5f, 1.0f)
};
// indices
GLushort indices[] = {
    // first triangle
    0, 1, 2,
    // second triangle
    3, 4, 5
};
void testVal()
{
	// 载入纹理
	Texture2D* texture1 = Texture2D::create("./res/texture/awesomeface.png");
	texture1->retain();
    Texture2D* texture2 = Texture2D::create("./res/texture/HelloWorld.png");
    texture2->retain();

    // MeshFilter
    // (GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* data)
    MeshFilter* mesh = MeshFilter::create(6);
    mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), vertices);
    mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), colors);
    mesh->addVertexAttribute(GLProgram::VERTEX_ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), coords);
    mesh->setIndicesInfo(indices, 6);
    mesh->addTexture("CGL_TEXTURE0", texture1);
    mesh->addTexture("CGL_TEXTURE1", texture2);
    mesh->setupVAO();


	// 着色器程序
	GLProgram* program = GLProgram::create("./res/shaders/triangles.vert", "./res/shaders/triangles.frag");
    // pass
    Pass* pass = Pass::createPass(program);
//    pass->setUniformV4f("CGL_COLOR", glm::vec4(1.0f, 0.0f, 0, 1.0f));
//    pass->setUniformV4f("CGL_COLOR1", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
//        pass->setUniformMat4("matTest", glm::mat4(1.0f, 0.5f, 0.0f, 1.0f,         //Shader中mat4[0][1] = 0.5f;
//                                              0.0f, 0.0f, 0.0f, 0.0f,
//                                              0.0f, 0.0f, 0.0f, 0.0f,
//                                              0.0f, 0.0f, 0.0f, 0.0f));
//    static float m[4] = {1.0f, 1.0f, 0.0f, 1.0f};
//    pass->setUniformFloatV("cusTest", 4, m);
//    pass->setUniformTex2D("CGL_TEXTURE0", texture1->getTextureId());
//    pass->setUniformTex2D("CGL_TEXTURE1", texture2->getTextureId());
	// 材质
	Material* material = Material::createMaterial();
    material->addPass(pass);
	// 组件：渲染组件
	BaseRender* renderCom = new BaseRender();
	renderCom->init(material);
//    renderCom->addVertexAttrib(vertices, 6, GLProgram::VERTEX_ATTR_POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
//    renderCom->addVertexAttrib(colors, 6, GLProgram::VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
//    renderCom->addVertexAttrib(coords, 6, GLProgram::VERTEX_ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
//    // 设置顶点索引数组
//    renderCom->setIndicesInfo(indices, 6);
	// 将组件加入渲染系统队列
	BaseEntity* entity = new BaseEntity();
	entity->addComponent(renderCom);
    
    // meshFilter
    entity->addComponent(mesh);

    // Transform组件
    browser::Transform* scene = new browser::Transform();
    scene->setName("scene");
    scene->setPosition(glm::vec3(0, 0, 0));
    scene->Rotate(glm::vec3(45, 45, 30));
    browser::TransformSystem::getInstance()->setScene(scene);   //设置场景节点
//    glm::mat4 mat = transCom->getModelMatrix();
//    BROWSER_LOG_MAT4(mat);
    browser::Transform* node = new browser::Transform();
    node->setName("node");
//    node->setPosition(11,2,3);
//    node->setScale(1, 2, 3);
//    node->setEulerAngle(90, 90, 0);
    node->Rotate(glm::vec3(45, 45, 45), customGL::Space::Self);
    node->Rotate(glm::vec3(30, 300, 45), customGL::Space::World);
//    node->Rotate(glm::vec3(45, 45, 45), customGL::Space::Self);
    scene->addChild(node);
    
//    glm::quat aa = glm::quat(-0.406684f, -0.335421f, -0.337035f, -0.78007f);
////    node->setQuaternion(aa);
////    node->setQuaternion(node->getGlobalQuaternion());
////    node->setQuaternion(-0.335421f, -0.337035f, -0.78007f, -0.406684f);
////    node->setGlobalQuaternion(-0.335421f, -0.337035f, -0.78007f, -0.406684f);
//    node->setGlobalQuaternion(aa);
//    BROWSER_LOG("============分割线===============");
//    BROWSER_LOG_QUATERNION(node->getGlobalQuaternion());
//    BROWSER_LOG_QUATERNION(node->getQuaternion());
//    BROWSER_LOG_VEC3(glm::degrees(node->getEulerAngle()));
//    node->setGlobalPosition(10, 10, 10);
//    BROWSER_LOG_VEC3(node->getPosition());
//    glm::quat qqq = node->getGlobalQuaternion();
//    BROWSER_LOG_QUATERNION(qqq);
//    BROWSER_LOG_MAT4(node->getModelMatrix());
//    node->setEulerAngle(45, 45, 45);


    Assimp::Importer importer;
    const aiScene *scene1 = importer.ReadFile("./res/models/nanosuit/nanosuit.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
}

int main()
{
	GLFWwindow* window = init();


	// test
	testVal();

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
		mainLoop(window);
	}


	// glfw: terminate, clearing all previously allocated GLFW resources.
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
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
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

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	// 注册基本系统
	ECSManager::getInstance()->registerSystem(browser::RenderSystem::getInstance());	// 渲染系统
    ECSManager::getInstance()->registerSystem(browser::TransformSystem::getInstance()); // Transform
    ECSManager::getInstance()->registerSystem(browser::MeshSystem::getInstance()); // Mesh
	// 初始化系统
	ECSManager::getInstance()->initSystem(SystemType::RenderSystem);    // 渲染系统
    ECSManager::getInstance()->initSystem(SystemType::Transform);    // Transform
    ECSManager::getInstance()->initSystem(SystemType::Mesh);    // Transform


	return window;
}

void destory()
{

}


void mainLoop(GLFWwindow *window)
{
	float deltaTime = 0;

	// 1.input
	processInput(window);

	// 2.render
    ECSManager::getInstance()->updateSystem(SystemType::Transform, deltaTime);  // 更新transform
	ECSManager::getInstance()->updateSystem(SystemType::RenderSystem, deltaTime);   // 更新渲染系统

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(window);
	glfwPollEvents();

	// auto release
	AutoReleasePool::getInstance()->update();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
