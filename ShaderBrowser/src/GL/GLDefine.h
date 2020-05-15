#ifndef GL_GLDEFINE_H
#define GL_GLDEFINE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace customGL
{
    // 交换缓冲数量
#define EXCHANGE_BUFFERS_COUNT 2
    
    class GLProgram;
	class Texture2D;
    class TextureBuffer;
    
    
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
    
    // 默认的GLProgram名称
    //extern const char* DEFAULT_GLPROGRAM_NAME;
    // 默认材质名称
    //extern const char* DEFAULT_MATERIAL_NAME;
    // 圆周率PI
    extern const float PI;
    // 坐标轴X
    extern const glm::vec3 GLM_AXIS_X;
    // 坐标轴Y
    extern const glm::vec3 GLM_AXIS_Y;
    // 坐标轴Z
    extern const glm::vec3 GLM_AXIS_Z;
    // 单位矩阵
    extern const glm::mat3 GLM_MAT3_UNIT;
	extern const glm::mat4 GLM_MAT4_UNIT;
	// 零矩阵
	extern const glm::mat4 GLM_MAT4_ZERO;
    // 零向量
    extern const glm::vec2 GLM_VEC2_ZERO;
    extern const glm::vec3 GLM_VEC3_ZERO;
    extern const glm::vec4 GLM_VEC4_ZERO;
    // 1向量
    extern const glm::vec3 GLM_VEC3_ONE;
	extern const glm::vec4 GLM_VEC4_ONE;
    // 颜色(非归一化)
    extern const glm::vec4 GLM_COLOR_WHITE; // 白色
    extern const glm::vec4 GLM_COLOR_RED;   // 红色
    extern const glm::vec4 GLM_COLOR_GREEN; // 绿色
    extern const glm::vec4 GLM_COLOR_BLUE;  // 蓝色
    // 单位四元数
    extern const glm::quat GLM_QUAT_UNIT;
	// 纹理默认参数
	extern const GLenum TexParameters_DefaultWrap;
	extern const GLenum TexParameters_DefaultFilter;
    // 旋转坐标空间
    enum Space
    {
        // 绕自身坐标系旋转（y-x-z）
        Self = 0,
        // 绕惯性坐标系旋转（z-x-y）
        World,
    };
    // 顶点数据类型
    enum VertexDataType
    {
        // 浮点型
        Float = 0,
        // 整形
        Int,
    };
    // Transform Feedback缓存类型枚举
    enum BufferType
    {
        // vbo
        BT_ArrayBuffer = 0,
        // tbo
        BT_TextureBuffer,
		// storage buffer	(unifor块和着色器存储缓存的最大区别在于，着色器存储缓存可以在着色器中读写)	eg: layout (std430, binding = 0) buffer BufferObject { int mode; vec4 points[]; };
		// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buf);
		BT_StorageBuffer,
    };
    
    // 单个顶点数据结构
    class VertexData
    {
    public:
        VertexData()
			: boneWeights(0.0f)
			, boneIndices(0)
        {
        }
        ~VertexData()
        {
        }
        
    public:
        // 位置
        glm::vec4 position;
        // 顶点颜色
        glm::vec4 color;
        // 主纹理uv
        glm::vec2 uv_main;
        // 法线
        glm::vec3 normal;
        // 切线
        glm::vec3 tangent;
		// 骨骼id索引	(一个顶点最多被4根骨骼所影响)
		glm::uvec4 boneIndices;
		// 骨骼权重
		glm::vec4 boneWeights;
    };
    
    // 单个顶点数据结构（含骨骼）
    class VertexDataWithBone : public VertexData
    {
    public:
        VertexDataWithBone()
        : boneWeights(0.0f)
        , boneIndices(0)
        {
        }
        ~VertexDataWithBone()
        {
        }
        
    public:
        // 骨骼id索引    (一个顶点最多被4根骨骼所影响)
        glm::uvec4 boneIndices;
        // 骨骼权重
        glm::vec4 boneWeights;
    };
    
    // 顶点属性格式(供设置vao使用)
    // void glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,const GLvoid * pointer);
    class VertexAttribDeclaration
    {
    public:
        VertexAttribDeclaration()
			: data_type(VertexDataType::Float)
			, index(-1)
			, size(3)
			, type(GL_FLOAT)
			, normalized(GL_FALSE)
			, stride(0)
			, pointer(nullptr)
			, name("")
			, data(nullptr)
			, data_size(0)
        {
        }
		VertexAttribDeclaration(const VertexAttribDeclaration&& declaration) noexcept
			: data_type(declaration.data_type)
			, index(declaration.index)
			, size(declaration.size)
			, type(declaration.type)
			, normalized(declaration.normalized)
			, stride(declaration.stride)
			, pointer(declaration.pointer)
			, name(declaration.name)
			, data(declaration.data)
			, data_size(declaration.data_size)
		{
		}
        ~VertexAttribDeclaration()
        {
        }
        
    public:
        // 必须填写的属性
        // 顶点属性类型(整形还是浮点型)
        VertexDataType data_type;
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
        // 数据
        GLvoid* data;
        // 数据大小
        GLint data_size;
    };
    
    // Transform feedback输出buffer属性
    class FeedbackBufferDeclaration
    {
    public:
        FeedbackBufferDeclaration()
        : type(BufferType::BT_ArrayBuffer)
        , varying("")
        , bindIdx(0)
        , size(0)
        , internalFormat(GL_RGBA32F)
        {
            for (int i=0; i<EXCHANGE_BUFFERS_COUNT; ++i)
            {
                vbos[i] = 0;
            }
        }
        ~FeedbackBufferDeclaration()
        {
        }
    public:
        // 必须填写的属性
        // 名称
        std::string varying;
        // GL_TRANSFORM_FEEDBACK_BUFFER的绑定点
        GLuint bindIdx;
        // 数据大小
        GLuint size;
        // 缓存类型
		BufferType type;
        // 纹理缓存格式 internalformat
        GLenum internalFormat;
        
        // 对应vbo
        GLuint vbos[EXCHANGE_BUFFERS_COUNT];
    };
    // Compute Program输入/输出buffer属性
    class ComputeBufferDeclaration
    {
    public:
        ComputeBufferDeclaration()
			: type(BufferType::BT_TextureBuffer)
			, name("")
			, size(0)
			, format(GL_RGBA32F)
			, access(GL_READ_ONLY)
		{
        }
        ~ComputeBufferDeclaration()
        {
        }
    public:
		// 缓存类型
		BufferType type;
		// 名称
		std::string name;
        // 数据大小
        GLuint size;
        // tbo存储格式
        GLenum format;
		// 读写权限 GL_READ_WRITE、GL_READ_ONLY、GL_WRITE_ONLY
        GLenum access;
        
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
		TextureData(const TextureData&& data) noexcept
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
            // matrix4x3
            UniformValueType_Mat4x3,
            // matrix3x3
            UniformValueType_Mat3,
            // matrix3x4
            UniformValueType_Mat3x4,
            // sampler2D
            UniformValueType_Sampler2D,
            // samplerBuffer
            UniformValueType_SamplerBuffer,
            
            // intv
            UniformValueType_IntV,
            // ivec2
            UniformValueType_IVec2,
            // vec2v
            UniformValueType_Vec2V,
            // ivec2v
            UniformValueType_IVec2V,
            // ivec3
            UniformValueType_IVec3,
            // vec3v
            UniformValueType_Vec3V,
            // ivec3v
            UniformValueType_IVec3V,
            // ivec4
            UniformValueType_IVec4,
            // vec4v
            UniformValueType_Vec4V,
            // ivec4v
            UniformValueType_IVec4V,
            // matrix4x4v
            UniformValueType_Mat4V,
            // matrix4x3v
            UniformValueType_Mat4x3V,
            // matrix3x3v
            UniformValueType_Mat3V,
            // matrix3x4v
            UniformValueType_Mat3x4V,

			// imageBuffer
			UniformValueType_ImageBuffer,
        };
    public:
        // 构造函数
        UniformValue();
		// 拷贝构造函数	
		// 注意：如果显式声明了移动构造函数或移动赋值运算符，则：1.不自动生成复制构造函数。2.不自动生成复制赋值运算符。
		// 注意：如果显式声明了复制构造函数或析构函数，则弃用复制赋值运算符的自动生成。
		// 注意：如果显式声明了复制赋值运算符或析构函数，则弃用复制构造函数的自动生成。
		UniformValue(const UniformValue& uniformVal);
		UniformValue& operator=(const UniformValue& uniformVal);

        // 移动构造函数
		// 注意：如果显式声明了复制构造函数、复制赋值运算符、移动构造函数、移动赋值运算符或析构函数，则：1.不自动生成移动构造函数。2.不自动生成移动赋值运算符。
        UniformValue(UniformValue&& uniformVal) noexcept;
		UniformValue& operator=(UniformValue&& uniformVal) noexcept;

        // 析构函数
        ~UniformValue();
        // 设置数值
        void setInt(int value);
        void setFloat(float value);
        void setMat3(const glm::mat3& value);
        void setMat3x4(const glm::mat3x4& value);
        void setMat4(const glm::mat4& value);
        void setMat4x3(const glm::mat4x3& value);
        void setFloatV(int count, const float* value);
        void setVec2(const glm::vec2& value);
        void setVec3(const glm::vec3& value);
        void setVec4(const glm::vec4& value);
        void setTex2D(Texture2D* texture);
        void setSamplerBuffer(TextureBuffer* textureBuffer);
        
        void setIntV(int count, const int* value);
        void setIVec2(const glm::ivec2& value);
        void setVec2V(int count, const float* value);
        void setIVec2V(int count, const int* value);
        void setIVec3(const glm::ivec3& value);
        void setVec3V(int count, const float* value);
        void setIVec3V(int count, const int* value);
        void setIVec4(const glm::ivec4& value);
        void setVec4V(int count, const float* value);
        void setIVec4V(int count, const int* value);
        void setMat4V(int count, const float* value);
        void setMat4x3V(int count, const float* value);
        void setMat3V(int count, const float* value);
        void setMat3x4V(int count, const float* value);

		void setImageBuffer(GLuint textureId, GLenum access, GLenum format);
        
        
        const glm::mat4& getMat4();
        const UniformValueType& getUniformValueType() const;
        
        
        // 更新设置GLProgram的数值
        void updateGLProgramUniformValue(GLProgram* glProgram, std::string uniformName, bool forceUpdate = false);
        
        // 重置藏标记
        void resetDirty();
		// 是否是纹理类资源(占用了纹理单元，每次切换Shader都需要设置的，所以这类资源不参与dirty)
		bool isTexture() const;

        
    public:
        // uniform数据是否改变了
        bool m_bDirty;
        // uniform数值类型
        UniformValueType m_eType;
        // uniform数据值
        union U {
            float floatValue;
            glm::vec2 v2f;
            glm::vec3 v3f;
            glm::vec4 v4f;
            int intValue;
            glm::ivec2 ivec2;
            glm::ivec3 ivec3;
            glm::ivec4 ivec4;
            glm::mat3 mat3;
            glm::mat3x4 mat3x4;
            glm::mat4 mat4;
            glm::mat4x3 mat4x3;
            struct {
//                GLuint textureId;
                Texture2D* texture;
            } tex2D;
            struct {
                TextureBuffer* textureBuffer;
            } samplerBuffer;
            struct {
                const int* pointer;
                GLsizei count;
            } intv,ivec2v,ivec3v,ivec4v;
            struct{
                const float* pointer;
                GLsizei count;
            } floatv,v2fv,v3fv,v4fv,mat4v,mat4x3v,mat3v,mat3x4v;
			struct {
				GLuint textureId;
				GLenum access;
				GLenum format;
			} imageBuffer;
            
            
//        如果在Union中的类型添加了构造函数，或者添加析构函数，就会发现程序会出现错误。由于union里面的东西共享内存，所以不能定义静态、引用类型的变量。由于在union里也不允许存放带有构造函数、析构函数和复制构造函数等的类的对象，但是可以存放对应的类对象指针。编译器无法保证类的构造函数和析构函数得到正确的调用，由此，就可能出现内存泄漏。所以，在C++中使用union时，尽量保持C语言中使用union的风格，尽量不要让union带有对象。
            // 这样可以避免报错
            U() { memset(this, 0, sizeof(*this)); }
            ~U() {}
            U& operator=(const U& other) {
                memcpy(this, &other, sizeof(*this));
                return *this;
            }
        } _value;
        
		private:
            // 是否需要数组数据
            bool checkIntVectorNeed() const;
            bool checkFloatVectorNeed() const;
			// 数组数据
			std::vector<int>* m_pIntV;
			std::vector<float>* m_pFloatV;
    };
    
}

#endif
