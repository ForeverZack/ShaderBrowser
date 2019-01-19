#ifndef GL_GLDEFINE_H
#define GL_GLDEFINE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

namespace customGL
{
    class GLProgram;
	class Texture2D;
    
//    // 顶点数据结构体
//    struct V3F_C4F_T2F
//    {
//        // 顶点位置
//        glm::vec3 vertex;
//        // 顶点颜色
//        glm::vec4 color;
//        // 纹理坐标
//        glm::vec2 texCoord;
//    };
//
//    // 三角形结构数据
//    struct Triangles
//    {
//        // 顶点数组
//        V3F_C4F_T2F* vertices;
//        // 顶点索引数组
//        unsigned short* indices;
//        // 顶点数量
//        int vertCount;
//        // 索引数量
//        int indexCount;
//    };
    
    // 坐标轴X
    extern const glm::vec3 GLM_AXIS_X;
    // 坐标轴Y
    extern const glm::vec3 GLM_AXIS_Y;
    // 坐标轴Z
    extern const glm::vec3 GLM_AXIS_Z;
    // 单位矩阵
    extern const glm::mat3 GLM_MAT3_UNIT;
    extern const glm::mat4 GLM_MAT4_UNIT;
    // 零向量
    extern const glm::vec2 GLM_VEC2_ZERO;
    extern const glm::vec3 GLM_VEC3_ZERO;
    extern const glm::vec4 GLM_VEC4_ZERO;
    // 1向量
    extern const glm::vec3 GLM_VEC3_ONE;
    // 单位四元数
    extern const glm::quat GLM_QUAT_UNIT;
    // 旋转坐标空间
    enum Space
    {
        // 绕自身坐标系旋转（y-x-z）
        Self = 0,
        // 绕惯性坐标系旋转（z-x-y）
        World,
    };
    
    // 单个顶点数据结构
    class VertexData
    {
    public:
        VertexData()
        {
        }
        ~VertexData()
        {
        }
        
    public:
        // position
        glm::vec4 position;
        // color
        glm::vec4 color;
        // uv1
        glm::vec2 uv_main;
        // normal
        // tangent
    };
    
    // 顶点属性格式(供设置vao使用)
    // void glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,const GLvoid * pointer);
    class VertexAttribDeclaration
    {
    public:
        VertexAttribDeclaration()
        : index(-1)
        , size(3)
        , type(GL_FLOAT)
        , normalized(GL_FALSE)
        , stride(0)
        , pointer(nullptr)
        , name("")
        {}
        ~VertexAttribDeclaration()
        {
        }
        
    public:
        // 必须填写的属性
        // index: 指定要修改的顶点属性的索引值
        GLuint index;
        // size: 指定每个顶点属性的组件数量。必须为1、2、3或者4。初始值为4。（如position是由3个（x,y,z）组成，而颜色是4个（r,g,b,a））
        GLint size;
        // type: 指定数组中每个组件的数据类型。可用的符号常量有GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,GL_UNSIGNED_SHORT, GL_FIXED, 和 GL_FLOAT，初始值为GL_FLOAT。
        GLenum type;
        // normalized: 指定当被访问时，固定点数据值是否应该被归一化（GL_TRUE）或者直接转换为固定点值（GL_FALSE）。
        GLboolean normalized;
        // stride: 指定连续顶点属性之间的偏移量。如果为0，那么顶点属性会被理解为：它们是紧密排列在一起的。初始值为0。
        GLsizei stride;
        // pointer: 指定第一个组件在数组的第一个顶点属性中的偏移量。该数组与GL_ARRAY_BUFFER绑定，储存于缓冲区中。初始值为0；
        GLvoid* pointer;
        
        // 可选属性(根据需要填写数据)
        // 属性名称，可以通过glGetAttribLocation(GLuint program, )获取attribute的location
        std::string name;
    };

	// 纹理数据
	class TextureData
	{
	public:
		TextureData() 
		{
		}
        TextureData(const TextureData& data)
        {
            texture = data.texture;
            uniformName = data.uniformName;
        }
		TextureData(const TextureData&& data)
		{
			texture = data.texture;
			uniformName = data.uniformName;
		}
		~TextureData()
		{
			// 这里的Texture只是一个指针，实际上并不由该类控制，所以不在这里析构
		}
	public:
		// 2D纹理
		Texture2D* texture;
		// 对应uniform名称
        std::string uniformName;
	};
	

    // Uniform数值
    class UniformValue
    {
    public:
        enum UniformValueType
        {
            // 未定义
            UniformValueType_Undefined = 0,
            // floatv
            UniformValueType_FloatV,
            // vec2
            UniformValueType_Vec2,
            // vec3
            UniformValueType_Vec3,
            // vec4
            UniformValueType_Vec4,
            // float
            UniformValueType_Float,
            // int
            UniformValueType_Int,
            // matrix4x4
            UniformValueType_Mat4,
            // matrix3x3
            UniformValueType_Mat3,
            // sampler2D
            UniformValueType_Sampler2D,
        };
    public:
        // 构造函数
        UniformValue();
        // 移动构造函数
        UniformValue(const UniformValue&& uniformVal);
        // 析构函数
        ~UniformValue();
        // 设置数值
        void setInt(int value);
        void setFloat(float value);
        void setMat3(const glm::mat3 value);
        void setMat4(const glm::mat4 value);
        void setFloatV(int size, const float* value);
        void setVec2(glm::vec2 value);
        void setVec3(glm::vec3 value);
        void setVec4(glm::vec4 value);
        void setTex2D(GLuint textureId);
        
        
        // 更新设置GLProgram的数值
        void updateGLProgramUniformValue(GLProgram* glProgram, std::string uniformName);
        
    protected:
        // uniform数值类型
        UniformValueType m_eType;
        // uniform数据值
        union U {
            float floatValue;
            int intValue;
            glm::mat3 mat3;
            glm::mat4 mat4;
            struct {
                GLuint textureId;
                //GLuint textureUnit;
            } tex2D;
            struct {
                const float* pointer;
                GLsizei size;
            } floatv;
            struct {
                glm::vec2 pointer;
            } v2f;
            struct {
                glm::vec3 pointer;
            } v3f;
            struct {
                glm::vec4 pointer;
            } v4f;
            
            U() { memset(this, 0, sizeof(*this)); }
            ~U() {}
            U& operator=(const U& other) {
                memcpy(this, &other, sizeof(*this));
                return *this;
            }
        } _value;
        
        
    };
    
}

#endif
