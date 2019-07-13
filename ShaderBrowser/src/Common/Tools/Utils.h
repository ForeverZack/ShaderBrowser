#ifndef COMMON_TOOLS_UTILS_H
#define COMMON_TOOLS_UTILS_H

#include <glad/glad.h>
#include <iostream>
#include "GL/GLDefine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using namespace std;
using namespace customGL;

namespace common
{
	// 注册读/写方法
	#define REGISTER_PROPERTY_GET_SET(varType, varName, funName)\
	public: virtual varType get##funName(void)	{return varName;}\
	public: virtual void set##funName(varType var) {varName = var;}

	// 注册只读方法
	#define REGISTER_PROPERTY_GET(varType, varName, funName)\
	public: virtual varType get##funName(void)	{return varName;}
    // 注册只读方法(返回 const &)   注意:对unordered_map这类需要两个或者更多模板参数的类型无法使用，因为宏会识别到其中的逗号
	#define REGISTER_PROPERTY_CONSTREF_GET(varType, varName, funName)\
	public: virtual const varType& get##funName(void)	{return varName;}
	// 注册返回引用方法(返回 &)   注意:对unordered_map这类需要两个或者更多模板参数的类型无法使用，因为宏会识别到其中的逗号
	#define REGISTER_PROPERTY_REF_GET(varType, varName, funName)\
	public: virtual varType& get##funName(void)	{return varName;}


	// 注册只写方法
	#define REGISTER_PROPERTY_SET(varType, varName, funName)\
	public: virtual void set##funName(varType var) {varName = var;}

	// 打印日志
	#define BROWSER_LOG(content) std::cout<<content<<endl;
    
    // 打印矩阵 (列主序，第一个维度代表列号)
    #define BROWSER_LOG_MAT4(mat4) std::cout<<mat4[0][0]<<", "<<mat4[1][0]<<", "<<mat4[2][0]<<", "<<mat4[3][0]<<endl;\
        std::cout<<mat4[0][1]<<", "<<mat4[1][1]<<", "<<mat4[2][1]<<", "<<mat4[3][1]<<endl;\
        std::cout<<mat4[0][2]<<", "<<mat4[1][2]<<", "<<mat4[2][2]<<", "<<mat4[3][2]<<endl;\
        std::cout<<mat4[0][3]<<", "<<mat4[1][3]<<", "<<mat4[2][3]<<", "<<mat4[3][3]<<endl;
    // 打印vec2
    #define BROWSER_LOG_VEC2(vec2) std::cout<<vec2[0]<<", "<<vec2[1]<<endl;
    // 打印vec3
    #define BROWSER_LOG_VEC3(vec3) std::cout<<vec3[0]<<", "<<vec3[1]<<", "<<vec3[2]<<endl;
    // 打印quaternion
    #define BROWSER_LOG_QUATERNION(quat) std::cout<<quat.x<<", "<<quat.y<<", "<<quat.z<<", "<<quat.w<<endl;
    
    // 安全删除指针
    #define BROWSER_SAFE_RELEASE_POINTER(pointerName)   \
        if (pointerName)    \
        {   \
            delete pointerName; \
            pointerName = nullptr;  \
        }
    // 安全删除指针数组
    #define BROWSER_SAFE_RELEASE_ARRAY_POINTER(pointerName)   \
        if (pointerName)    \
        {   \
            delete[] pointerName; \
            pointerName = nullptr;  \
        }

	// 断言(如果不满足条件，会中断)
	void BROWSER_ASSERT(bool condition, const char* msg);
    
    // 警告(如果不满足条件，会弹出警告)
    void BROWSER_WARNING(bool condition, const char* msg);
    
    class RenderSystem;
    
	class Utils
	{
	public:
		// 读取文件（shader）
		static GLchar* readFile(const char* filename);

        // 创建VertexAttribDeclaration
        static VertexAttribDeclaration* createVertexAttribDeclaration(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, VertexDataType dataType = VertexDataType::Float);
        static VertexAttribDeclaration* createVertexAttribDeclaration(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer, GLvoid* data, GLint dataSize, VertexDataType dataType = VertexDataType::Float);
        
        // 创建FeedbackBufferDeclaration
        static FeedbackBufferDeclaration* createFeedbackBufferDeclaration(GLuint bindIdx, GLuint size, const string& name, FeedbackBufferType type = FeedbackBufferType::ArrayBuffer, GLenum internalFormat = GL_RGBA32F);
        
        // 拆解矩阵 （从矩阵中，获取位移、旋转和缩放）
        static void parseMatrix(const glm::mat4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale);
        // 矩阵转四元素 (glm::toquat可能会得到两个不一样的结果(刚好对称，应该是三角函数处理的问题)，所以这里自己写方法代替了)
        static glm::quat convertMatrix2Quat(const glm::mat4& matrix);
	};
}

#endif
