#ifndef COMMON_TOOLS_UTILS_H
#define COMMON_TOOLS_UTILS_H

#include <glad/glad.h>
#include <iostream>
#include <unordered_map>
#include "GL/GLDefine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

using namespace std;
using namespace customGL;

namespace common
{
    typedef unsigned char BYTE;
    
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
    // 注册只写方法(const &)
    #define REGISTER_PROPERTY_CONSTREF_SET(varType, varName, funName)\
    public: virtual void set##funName(const varType& var) {varName = var;}
    // 注册读写方法(const &)
    #define REGISTER_PROPERTY_CONSTREF_GET_SET(varType, varName, funName)\
    public: virtual const varType& get##funName(void)    {return varName;}\
    public: virtual void set##funName(const varType& var) {varName = var;}
    // 注册返回引用方法(返回 &)   注意:对unordered_map这类需要两个或者更多模板参数的类型无法使用，因为宏会识别到其中的逗号
	#define REGISTER_PROPERTY_REF_GET(varType, varName, funName)\
	public: virtual varType& get##funName(void)	{return varName;}


	// 注册只写方法
	#define REGISTER_PROPERTY_SET(varType, varName, funName)\
	public: virtual void set##funName(varType var) {varName = var;}

	// 打印日志
	#define BROWSER_LOG(content) std::cout<<content<<endl;
    
    // 打印矩阵 (列主序，第一个维度代表列号) (OpenGL和glm中矩阵的第一个维度都代表列号，即glm::mat2x4和glsl的mat2x4都表示2列4行)
    #define BROWSER_LOG_MAT4(mat4) std::cout<<mat4[0][0]<<", "<<mat4[1][0]<<", "<<mat4[2][0]<<", "<<mat4[3][0]<<endl;\
        std::cout<<mat4[0][1]<<", "<<mat4[1][1]<<", "<<mat4[2][1]<<", "<<mat4[3][1]<<endl;\
        std::cout<<mat4[0][2]<<", "<<mat4[1][2]<<", "<<mat4[2][2]<<", "<<mat4[3][2]<<endl;\
        std::cout<<mat4[0][3]<<", "<<mat4[1][3]<<", "<<mat4[2][3]<<", "<<mat4[3][3]<<endl;
    // 打印vec2
    #define BROWSER_LOG_VEC2(vec2) std::cout<<vec2[0]<<", "<<vec2[1]<<endl;
    // 打印vec3
    #define BROWSER_LOG_VEC3(vec3) std::cout<<vec3[0]<<", "<<vec3[1]<<", "<<vec3[2]<<endl;
    // 打印vec4
    #define BROWSER_LOG_VEC4(vec3) std::cout<<vec3[0]<<", "<<vec3[1]<<", "<<vec3[2]<<", "<<vec3[3]<<endl;
    // 打印quaternion
    #define BROWSER_LOG_QUATERNION(quat) std::cout<<quat.x<<", "<<quat.y<<", "<<quat.z<<", "<<quat.w<<endl;
    
    // 安全释放指针
    #define BROWSER_SAFE_FREE_POINTER(pointerName)   \
        if (pointerName)    \
        {   \
            free(pointerName); \
            pointerName = nullptr;  \
        }
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
	// 安全Release Reference
	#define BROWSER_SAFE_RELEASE_REFERENCE(ref) \
		if (ref)	\
		{	\
			ref->release();	\
			ref = nullptr;	\
		}
    
    // 置位：将某一位设为1
    #define BROWSER_SET_BIT(var, n) var|=1<<n;
    // 复位：将某一位设为0
    #define BROWSER_CLEAR_BIT(var, n) var&=~(1<<n);
    // 取位：取出某一位
    #define BROWSER_GET_BIT(var, n) var&(1<<n)

	// 断言(如果不满足条件，会中断)
	void BROWSER_ASSERT(bool condition, const char* msg);
    
    // 警告(如果不满足条件，会弹出警告)
    void BROWSER_WARNING(bool condition, const char* msg);
    
    class RenderSystem;
    
	class Utils
	{
	public:
		// 读取文件（shader）
		static std::string readAbsolutePathFile(const char* absolutePath);
        static std::string readFile(const char* filepath);

        // 创建VertexAttribDeclaration
        static VertexAttribDeclaration* createVertexAttribDeclaration(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, VertexDataType dataType = VertexDataType::Float);
        static VertexAttribDeclaration* createVertexAttribDeclaration(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer, GLvoid* data, GLint dataSize, VertexDataType dataType = VertexDataType::Float);
        
        // 创建FeedbackBufferDeclaration
        static FeedbackBufferDeclaration* createFeedbackBufferDeclaration(GLuint bindIdx, GLuint size, const string& name, BufferType type = BufferType::BT_ArrayBuffer, GLenum internalFormat = GL_RGBA32F);
        
		// 创建ComputeBufferDeclaration
		static ComputeBufferDeclaration* createComputeBufferDeclaration(const string& name, GLuint size, GLenum access = GL_READ_ONLY, BufferType type = BufferType::BT_TextureBuffer, GLenum format = GL_RGBA32F);
        
        // 添加\设置uniform
        static void setUniformInt(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int value);
        static void setUniformFloat(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, float value);
        static void setUniformMat3(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::mat3& value);
        static void setUniformMat3x4(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::mat3x4& value);
        static void setUniformMat4(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::mat4& value);
        static void setUniformMat4x3(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::mat4x3& value);
        static void setUniformFloatV(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int size, const float* value);
        static void setUniformV2f(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::vec2& value);
        static void setUniformV3f(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::vec3& value);
        static void setUniformV4f(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::vec4& value);
        static void setUniformTex2D(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, Texture2D* texture);
        static void setUniformTex2D(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, RenderTexture* texture);
		static void setUniformSamplerBuffer(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, TextureBuffer* textureBuffer);
		static void setUniformUniformBuffer(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, UniformBuffer* uniformBuffer);
        static void setUniformIntV(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const int* value);
        static void setUniformIVec2(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::ivec2& value);
        static void setUniformVec2V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value);
        static void setUniformIVec2V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const int* value);
        static void setUniformIVec3(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::ivec3& value);
        static void setUniformVec3V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value);
        static void setUniformIVec3V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const int* value);
        static void setUniformIVec4(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::ivec4& value);
        static void setUniformVec4V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value);
        static void setUniformIVec4V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const int* value);
        static void setUniformMat4V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value);
        static void setUniformMat4x3V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value);
        static void setUniformMat3V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value);
        static void setUniformMat3x4V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value);

        // 拆解矩阵 （从矩阵中，获取位移、旋转和缩放）
        static void parseMatrix(const glm::mat4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale);
        static void parseMatrix(const aiMatrix4x4& matrix, glm::vec4& position, glm::vec4& rotation, glm::vec4& scale);
        static void parseMatrix(const aiMatrix4x4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale);
        // 矩阵转四元素 (glm::toquat可能会得到两个不一样的结果(刚好对称，应该是三角函数处理的问题)，所以这里自己写方法代替了)
        static glm::quat convertMatrix2Quat(const glm::mat4& matrix);
        static glm::vec4 convertMatrix2Vec4(const glm::mat4& matrix);
	};
}

#endif
