#include "GLDefine.h"
#include "GL/GPUResource/Shader/GLProgram.h"
#include "Common/Tools/Utils.h"

namespace customGL {
    
    // 圆周率
    extern const float PI = 3.141592454f;
    // 坐标轴X
    extern const glm::vec3 GLM_AXIS_X = glm::vec3(1.0f, 0.0f, 0.0f);
    // 坐标轴Y
    extern const glm::vec3 GLM_AXIS_Y = glm::vec3(0.0f, 1.0f, 0.0f);
    // 坐标轴Z
    extern const glm::vec3 GLM_AXIS_Z = glm::vec3(0.0f, 0.0f, 1.0f);
    // 单位矩阵
    extern const glm::mat3 GLM_MAT3_UNIT = glm::mat3(1.0f);
    extern const glm::mat4 GLM_MAT4_UNIT = glm::mat4(1.0f);
	// 零矩阵
	extern const glm::mat4 GLM_MAT4_ZERO = glm::mat4(0.0f);
    // 零向量
    extern const glm::vec2 GLM_VEC2_ZERO = glm::vec2(0.0f, 0.0f);
    extern const glm::vec3 GLM_VEC3_ZERO = glm::vec3(0.0f, 0.0f, 0.0f);
    extern const glm::vec4 GLM_VEC4_ZERO = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    // 1向量
    extern const glm::vec3 GLM_VEC3_ONE = glm::vec3(1.0f, 1.0f, 1.0f);
	extern const glm::vec4 GLM_VEC4_ONE = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // 颜色 (非归一化)
    extern const glm::vec4 GLM_COLOR_WHITE = glm::vec4(255, 255, 255, 255); // 白色
    extern const glm::vec4 GLM_COLOR_RED = glm::vec4(255, 0, 0, 255);   // 红色
    extern const glm::vec4 GLM_COLOR_GREEN = glm::vec4(0, 255, 0, 255); // 绿色
    extern const glm::vec4 GLM_COLOR_BLUE = glm::vec4(0, 0, 255, 255);  // 蓝色
    // 单位四元数
    extern const glm::quat GLM_QUAT_UNIT = glm::quat(1.0f, 0, 0, 0);
	// 纹理默认参数
	extern const GLenum TexParameters_DefaultSRGB = false;
	extern const GLenum TexParameters_DefaultWrap = GL_CLAMP_TO_EDGE;
	extern const GLenum TexParameters_DefaultFilter = GL_LINEAR;
	// GL默认清除位
	extern const GLbitfield DEFAULT_GL_CLEAR_BIT = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
	// GL默认fbo清除颜色
	extern const glm::vec4 DEFAULT_GL_CLEAR_COLOR = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
    // 获取GLSL类型转换为存储大小
    extern size_t GLSLTypeSize(GLenum type)
    {
        size_t size;
        
        #define CASE(Enum, Count, Type) \
            case Enum: size = Count * sizeof(Type); break;
        
        switch(type)
        {
            CASE(GL_FLOAT, 1, GLfloat);
                CASE(GL_FLOAT_VEC2, 2, GLfloat);
                CASE(GL_FLOAT_VEC3, 3, GLfloat);
                CASE(GL_FLOAT_VEC4, 4, GLfloat);
                CASE(GL_INT, 1, GLint);
                CASE(GL_INT_VEC2, 2, GLint);
                CASE(GL_INT_VEC3, 3, GLint);
                CASE(GL_INT_VEC4, 4, GLint);
                CASE(GL_UNSIGNED_INT, 1, GLuint);
                CASE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
                CASE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
                CASE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
                CASE(GL_BOOL, 1, GLboolean);
                CASE(GL_BOOL_VEC2, 2, GLboolean);
                CASE(GL_BOOL_VEC3, 3, GLboolean);
                CASE(GL_BOOL_VEC4, 4, GLboolean);
                CASE(GL_FLOAT_MAT2, 4, GLfloat);
                CASE(GL_FLOAT_MAT2x3, 6, GLfloat);
                CASE(GL_FLOAT_MAT2x4, 8, GLfloat);
                CASE(GL_FLOAT_MAT3, 9, GLfloat);
                CASE(GL_FLOAT_MAT3x2, 6, GLfloat);
                CASE(GL_FLOAT_MAT3x4, 12, GLfloat);
                CASE(GL_FLOAT_MAT4, 16, GLfloat);
                CASE(GL_FLOAT_MAT4x2, 8, GLfloat);
                CASE(GL_FLOAT_MAT4x3, 12, GLfloat);
        }
        
        return size;
    }
	// std140布局下类型的对齐值
	extern const std::unordered_map<size_t, size_t> STD140_BaseAlignments
	{
		{ typeid(float).hash_code(), sizeof(float) },
		{ typeid(int).hash_code(), sizeof(float) },
		{ typeid(bool).hash_code(), sizeof(float) },
		{ typeid(glm::vec2).hash_code(), sizeof(float) * 2 },
		{ typeid(glm::vec3).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::vec4).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::ivec2).hash_code(), sizeof(float) * 2 },
		{ typeid(glm::ivec3).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::ivec4).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::uvec2).hash_code(), sizeof(float) * 2 },
		{ typeid(glm::uvec3).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::uvec4).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat2).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat2x3).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat2x4).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat3).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat3x2).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat3x4).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat4).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat4x2).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat4x3).hash_code(), sizeof(float) * 4 },
	};
	// std140布局下类型的大小
	extern const std::unordered_map<size_t, size_t> STD140_BaseSizes
	{
		{ typeid(float).hash_code(), sizeof(float) },
		{ typeid(int).hash_code(), sizeof(float) },
		{ typeid(bool).hash_code(), sizeof(float) },
		{ typeid(glm::vec2).hash_code(), sizeof(float) * 2 },
		{ typeid(glm::vec3).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::vec4).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::ivec2).hash_code(), sizeof(float) * 2 },
		{ typeid(glm::ivec3).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::ivec4).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::uvec2).hash_code(), sizeof(float) * 2 },
		{ typeid(glm::uvec3).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::uvec4).hash_code(), sizeof(float) * 4 },
		{ typeid(glm::mat2).hash_code(), sizeof(float) * 8 },
		{ typeid(glm::mat2x3).hash_code(), sizeof(float) * 8 },		// 2列3行
		{ typeid(glm::mat2x4).hash_code(), sizeof(float) * 8 },		// 2列4行
		{ typeid(glm::mat3).hash_code(), sizeof(float) * 12 },
		{ typeid(glm::mat3x2).hash_code(), sizeof(float) * 12 },	// 3列2行
		{ typeid(glm::mat3x4).hash_code(), sizeof(float) * 12 },	// 3列4行
		{ typeid(glm::mat4).hash_code(), sizeof(float) * 16 },
		{ typeid(glm::mat4x2).hash_code(), sizeof(float) * 16 },	// 4列2行
		{ typeid(glm::mat4x3).hash_code(), sizeof(float) * 16 },	// 4列3行
	};
    
    BufferData::BufferData()
        : m_pData(nullptr)
        , m_uElementSize(0)
        , m_uLength(0)
		, m_uRealSize(0)
    {
    }

	BufferData::BufferData(const BufferData& bufferData)
		: m_uElementSize(bufferData.m_uElementSize)
		, m_uLength(bufferData.m_uLength)
		, m_uRealSize(bufferData.m_uRealSize)
	{
		// 注意！！！：这里使用malloc的话，无法调用到数据对象的构造函数
		// 不过这里指使用一些简单的数据类型，不会又什么影响
		m_pData = malloc(m_uRealSize);
		memcpy(m_pData, bufferData.m_pData, m_uRealSize);
	}

	BufferData::BufferData(BufferData&& bufferData)
	{
		m_pData = bufferData.m_pData;
		m_uElementSize = bufferData.m_uElementSize;
		m_uLength = bufferData.m_uLength;
		m_uRealSize = bufferData.m_uRealSize;

		bufferData.m_pData = nullptr;
	}
    
    BufferData::~BufferData()
    {
		// 注意！！！：释放void类型的指针并不安全，因为不知道确切的对象类型，无法调用它的析构函数（只会释放对象本身的内存，如果对象中含有其他指针，则无法释放）
		// 不过这里只使用一些简单的数据类型，不会造成内存泄漏
        BROWSER_SAFE_RELEASE_POINTER(m_pData);
    }

	void BufferData::initData(size_t elementSize, size_t length)
	{
		m_uElementSize = elementSize;
		m_uLength = length;
		m_uRealSize = elementSize * length;
		m_pData = malloc(m_uRealSize);
	}

	void BufferData::setData(void* data, size_t index, size_t dataSize)
	{
		BROWSER_ASSERT(m_pData, "BufferData has not been inited !");

		size_t offset = index * m_uElementSize;
		BROWSER_ASSERT(offset + dataSize < m_uRealSize, "BufferData's cache is not bigger enough !");
		memcpy((common::BYTE*)m_pData + offset, data, dataSize);
	}

    
    
// ===================================UniformValue::UniformValue()================================================
    UniformValue::UniformValue()
        : m_eType(UniformValueType::UniformValueType_Undefined)
        , m_bDirty(true)
		, m_pIntV(nullptr)
		, m_pFloatV(nullptr)
    {
        
    }

	UniformValue::UniformValue(const UniformValue& uniformVal)
	{
		m_eType = uniformVal.m_eType;
		_value = uniformVal._value;
		m_bDirty = uniformVal.m_bDirty;
		// 复制vector指针数据
		if (checkIntVectorNeed() && uniformVal.m_pIntV && uniformVal.m_pIntV->size()>0)
		{
			m_pIntV = new std::vector<int>(&(*uniformVal.m_pIntV)[0], &(*uniformVal.m_pIntV)[0] + uniformVal.m_pIntV->size());
		}
		if (checkFloatVectorNeed() && uniformVal.m_pFloatV && uniformVal.m_pFloatV->size() > 0)
		{
			m_pFloatV = new std::vector<float>(&(*uniformVal.m_pFloatV)[0], &(*uniformVal.m_pFloatV)[0] + uniformVal.m_pFloatV->size());
		}
	}

	UniformValue& UniformValue::operator=(const UniformValue& uniformVal)
	{
		m_eType = uniformVal.m_eType;
		_value = uniformVal._value;
		m_bDirty = uniformVal.m_bDirty;
		// 复制vector指针数据
		if (checkIntVectorNeed() && uniformVal.m_pIntV && uniformVal.m_pIntV->size() > 0)
		{
			m_pIntV = new std::vector<int>(&(*uniformVal.m_pIntV)[0], &(*uniformVal.m_pIntV)[0] + uniformVal.m_pIntV->size());
		}
		if (checkFloatVectorNeed() && uniformVal.m_pFloatV && uniformVal.m_pFloatV->size() > 0)
		{
			m_pFloatV = new std::vector<float>(&(*uniformVal.m_pFloatV)[0], &(*uniformVal.m_pFloatV)[0] + uniformVal.m_pFloatV->size());
		}
		return *this;
	}
    
    UniformValue::UniformValue(UniformValue&& uniformVal) noexcept
    {
        m_eType = uniformVal.m_eType;
        _value = std::move(uniformVal._value);
        m_bDirty = uniformVal.m_bDirty;
		// 接管vector指针数据
		if (checkIntVectorNeed() && uniformVal.m_pIntV)
		{
			m_pIntV = uniformVal.m_pIntV;
			uniformVal.m_pIntV = nullptr;
		}
		if (checkFloatVectorNeed() && uniformVal.m_pFloatV)
		{
			m_pFloatV = uniformVal.m_pFloatV;
			uniformVal.m_pFloatV = nullptr;
		}
	}

	UniformValue& UniformValue::operator=(UniformValue&& uniformVal) noexcept
	{
		m_eType = uniformVal.m_eType;
		_value = std::move(uniformVal._value);
		m_bDirty = uniformVal.m_bDirty;
		// 接管vector指针数据
		if (checkIntVectorNeed() && uniformVal.m_pIntV)
		{
			m_pIntV = uniformVal.m_pIntV;
			uniformVal.m_pIntV = nullptr;
		}
		if (checkFloatVectorNeed() && uniformVal.m_pFloatV)
		{
			m_pFloatV = uniformVal.m_pFloatV;
			uniformVal.m_pFloatV = nullptr;
		}
		return *this;
	}
    
    UniformValue::~UniformValue()
    {
        if (checkIntVectorNeed())
        {
            BROWSER_SAFE_RELEASE_POINTER(m_pIntV);
        }
		if (checkFloatVectorNeed())
        {
            BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
        }
    }
    
    bool UniformValue::checkIntVectorNeed() const
    {
        return m_eType==UniformValue::UniformValueType::UniformValueType_IntV
            || m_eType==UniformValue::UniformValueType::UniformValueType_IVec2V
            || m_eType==UniformValue::UniformValueType::UniformValueType_IVec3V
            || m_eType==UniformValue::UniformValueType::UniformValueType_IVec4V;
    }
    
    bool UniformValue::checkFloatVectorNeed() const
    {
        return m_eType==UniformValue::UniformValueType::UniformValueType_FloatV
            || m_eType==UniformValue::UniformValueType::UniformValueType_Vec2V
            || m_eType==UniformValue::UniformValueType::UniformValueType_Vec3V
            || m_eType==UniformValue::UniformValueType::UniformValueType_Vec4V
            || m_eType==UniformValue::UniformValueType::UniformValueType_Mat4V
            || m_eType==UniformValue::UniformValueType::UniformValueType_Mat4x3V
            || m_eType==UniformValue::UniformValueType::UniformValueType_Mat3V
            || m_eType==UniformValue::UniformValueType::UniformValueType_Mat3x4V;
    }
    
    void UniformValue::setInt(int value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Int,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setInt");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Int;
        _value.intValue = value;
    }
    
    void UniformValue::setIntV(int count, const int* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_IntV,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setIntV");
        
		m_bDirty = true;
		m_eType = UniformValueType::UniformValueType_IntV;
		BROWSER_SAFE_RELEASE_POINTER(m_pIntV);
		m_pIntV = new std::vector<int>(value, value + count);
        _value.intv.count = count;
        _value.intv.pointer = &(*m_pIntV)[0];
    }
    
    void UniformValue::setFloat(float value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Float,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setFloat");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Float;
        _value.floatValue = value;
    }
    
    void UniformValue::setMat3(const glm::mat3& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Mat3,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setMat3");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Mat3;
        _value.mat3 = value;
    }
    
    void UniformValue::setMat3x4(const glm::mat3x4 &value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Mat3x4,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setMat3x4");
        
        m_bDirty = true;
        m_eType = UniformValueType_Mat3x4;
        _value.mat3x4 = value;
    }
    
    void UniformValue::setMat4(const glm::mat4& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Mat4,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setMat4");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Mat4;
        _value.mat4 = value;
    }
    
    void UniformValue::setMat4x3(const glm::mat4x3& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Mat4x3,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setMat4x3");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Mat4x3;
        _value.mat4x3 = value;
    }
    
    void UniformValue::setMat4V(int count, const float* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Mat4V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setMat4V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Mat4V;
		BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
		m_pFloatV = new std::vector<float>(value, value + count);
        _value.mat4v.count = count;
        _value.mat4v.pointer = &(*m_pFloatV)[0];
    }
    
    void UniformValue::setMat4x3V(int count, const float* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Mat4x3V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setMat4x3V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Mat4x3V;
		BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
		m_pFloatV = new std::vector<float>(value, value + count);
        _value.mat4x3v.count = count;
        _value.mat4x3v.pointer = &(*m_pFloatV)[0];
    }
    
    void UniformValue::setMat3V(int count, const float* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Mat3V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setMat3V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Mat3V;
		BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
		m_pFloatV = new std::vector<float>(value, value + count);
        _value.mat3v.count = count;
        _value.mat3v.pointer = &(*m_pFloatV)[0];
    }
    
    void UniformValue::setMat3x4V(int count, const float* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Mat3x4V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setMat3x4V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Mat3x4V;
		BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
		m_pFloatV = new std::vector<float>(value, value + count);
        _value.mat3x4v.count = count;
        _value.mat3x4v.pointer = &(*m_pFloatV)[0];
    }
    
    void UniformValue::setFloatV(int count, const float* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
               || m_eType==UniformValueType::UniformValueType_FloatV,
               "UniformValueType has already defined, you cannot change it again in function UniformValue::setFloatV");
        
		m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_FloatV;
		BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
		m_pFloatV = new std::vector<float>(value, value + count);
        _value.floatv.count = count;
        _value.floatv.pointer = &(*m_pFloatV)[0];
    }
    
    void UniformValue::setVec2(const glm::vec2& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
               || m_eType==UniformValueType::UniformValueType_Vec2,
               "UniformValueType has already defined, you cannot change it again in function UniformValue::setVec2");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Vec2;
        _value.v2f = value;
    }
    
    void UniformValue::setIVec2(const glm::ivec2& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_IVec2,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setIVec2");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_IVec2;
        _value.ivec2 = value;
    }
    
    void UniformValue::setVec2V(int count, const float* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Vec2V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setVec2V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Vec2V;
		BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
		m_pFloatV = new std::vector<float>(value, value + count);
        _value.v2fv.count = count;
        _value.v2fv.pointer = &(*m_pFloatV)[0];
    }
    
    void UniformValue::setIVec2V(int count, const int* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_IVec2V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setIVec2V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_IVec2V;
		BROWSER_SAFE_RELEASE_POINTER(m_pIntV);
		m_pIntV = new std::vector<int>(value, value + count);
        _value.ivec2v.count = count;
        _value.ivec2v.pointer = &(*m_pIntV)[0];
    }
    
    void UniformValue::setVec3(const glm::vec3& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
               || m_eType==UniformValueType::UniformValueType_Vec3,
               "UniformValueType has already defined, you cannot change it again in function UniformValue::setVec3");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Vec3;
        _value.v3f = value;
    }
    
    void UniformValue::setIVec3(const glm::ivec3& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_IVec3,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setIVec3");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_IVec3;
        _value.ivec3 = value;
    }
    
    void UniformValue::setVec3V(int count, const float* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Vec3V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setVec3V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Vec3V;
		BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
		m_pFloatV = new std::vector<float>(value, value + count);
        _value.v3fv.count = count;
        _value.v3fv.pointer = &(*m_pFloatV)[0];
    }
    
    void UniformValue::setIVec3V(int count, const int* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_IVec3V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setIVec3V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_IVec3V;
		BROWSER_SAFE_RELEASE_POINTER(m_pIntV);
		m_pIntV = new std::vector<int>(value, value + count);
        _value.ivec3v.count = count;
        _value.ivec3v.pointer = &(*m_pIntV)[0];
    }
    
    void UniformValue::setVec4(const glm::vec4& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
               || m_eType==UniformValueType::UniformValueType_Vec4,
               "UniformValueType has already defined, you cannot change it again in function UniformValue::setVec4");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Vec4;
        _value.v4f = value;
    }
    
    void UniformValue::setIVec4(const glm::ivec4& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_IVec4,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setIVec4");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_IVec4;
        _value.ivec4 = value;
    }
    
    void UniformValue::setVec4V(int count, const float* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Vec4V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setVec4V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Vec4V;
		BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
		m_pFloatV = new std::vector<float>(value, value + count);
        _value.v4fv.count = count;
        _value.v4fv.pointer = &(*m_pFloatV)[0];
    }
    
    void UniformValue::setIVec4V(int count, const int* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_IVec4V,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setIVec4V");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_IVec4V;
		BROWSER_SAFE_RELEASE_POINTER(m_pIntV);
		m_pIntV = new std::vector<int>(value, value + count);
        _value.ivec4v.count = count;
        _value.ivec4v.pointer = &(*m_pIntV)[0];
    }
    
    void UniformValue::setTex2D(Texture2D* texture)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Sampler2D,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setTex2D");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Sampler2D;
        _value.tex2D.texture = texture;
    }
    
    void UniformValue::setTex2D(RenderTexture* texture)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_RenderTexture,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setTex2D");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_RenderTexture;
        _value.tex2D.renderTexture = texture;
    }
    
    void UniformValue::setSamplerBuffer(TextureBuffer* textureBuffer)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_SamplerBuffer,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setSamplerBuffer");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_SamplerBuffer;
        _value.samplerBuffer.textureBuffer = textureBuffer;
    }

	void UniformValue::setImageBuffer(GLuint textureId, GLenum access, GLenum format)
	{
		common::BROWSER_ASSERT(m_eType == UniformValueType::UniformValueType_Undefined
			|| m_eType == UniformValueType::UniformValueType_ImageBuffer,
			"UniformValueType has already defined, you cannot change it again in function UniformValue::setImageBuffer");

		m_bDirty = true;
		m_eType = UniformValueType::UniformValueType_ImageBuffer;
		_value.imageBuffer.textureId = textureId;
		_value.imageBuffer.access = access;
		_value.imageBuffer.format = format;
	}

	void UniformValue::setUniformBuffer(UniformBuffer* buffer)
	{
		common::BROWSER_ASSERT(m_eType == UniformValueType::UniformValueType_Undefined
			|| m_eType == UniformValueType::UniformValueType_UniformBuffer,
			"UniformValueType has already defined, you cannot change it again in function UniformValue::setUniformBuffer");

		m_bDirty = true;
		m_eType = UniformValueType::UniformValueType_UniformBuffer;
		_value.uniformBuffer.buffer = buffer;
	}
    
    const glm::mat4& UniformValue::getMat4()
    {
        return _value.mat4;
    }
    
    const UniformValue::UniformValueType& UniformValue::getUniformValueType() const
    {
        return m_eType;
    }

	bool UniformValue::isTexture() const
	{
		return m_eType == UniformValueType_ImageBuffer
            || m_eType == UniformValueType_SamplerBuffer
            || m_eType == UniformValueType_Sampler2D
            || m_eType == UniformValueType_RenderTexture;
	}
    
    void UniformValue::resetDirty()
    {
        m_bDirty = false;
    }
    
    void UniformValue::updateGLProgramUniformValue(GLProgram* glProgram, std::string uniformName, bool forceUpdate/* = false*/)
    {
		/* 在渲染结束后通过MaterialManager::resetAllMaterialsUniformsDirty来重置所有UniformValue的dirty标记重置为false
		，并且现在由于RenderCommand中的UniformValue都是材质UniformValue的副本(并非引用)，所以在这里设置dirty没用。
		不过要注意Pass在第一次使用的时候要强制更新所有Uniforms，来做一次初始化 */
        if(!forceUpdate && !m_bDirty && !isTexture())
        {
            return;
        }

        //m_bDirty = false;	
        switch (m_eType)
        {
            case UniformValueType_Int:
                // int
                glProgram->setUniformWithInt(uniformName.c_str(), _value.intValue);
                break;
                
            case UniformValueType_Float:
                // float
                glProgram->setUniformWithFloat(uniformName.c_str(), _value.floatValue);
                break;
                
            case UniformValueType_Mat3:
                // mat3
                glProgram->setUniformWithMat3(uniformName.c_str(), _value.mat3);
                break;
                
            case UniformValueType_Mat3x4:
                // mat3x4
                glProgram->setUniformWithMat3x4(uniformName.c_str(), _value.mat3x4);
                break;
                
            case UniformValueType_Mat4:
                // mat4
                glProgram->setUniformWithMat4(uniformName.c_str(), _value.mat4);
                break;
                
            case UniformValueType_Mat4x3:
                // mat4x3
                glProgram->setUniformWithMat4x3(uniformName.c_str(), _value.mat4x3);
                break;
                
            case UniformValueType_FloatV:
                // floatV
                glProgram->setUniformWithFloatV(uniformName.c_str(), _value.floatv.count, _value.floatv.pointer);
                break;
                
            case UniformValueType_Vec2:
                // vec2
                glProgram->setUniformWithVec2(uniformName.c_str(), _value.v2f.x, _value.v2f.y);
                break;
                
            case UniformValueType_Vec3:
                // vec3
                glProgram->setUniformWithVec3(uniformName.c_str(), _value.v3f.x, _value.v3f.y, _value.v3f.z);
                break;
                
            case UniformValueType_Vec4:
                // vec4
                glProgram->setUniformWithVec4(uniformName.c_str(), _value.v4f.x, _value.v4f.y, _value.v4f.z, _value.v4f.w);
                break;
                
            case UniformValueType_Sampler2D:
                // texture2D
                glProgram->setUniformWithTex2D(uniformName.c_str(), _value.tex2D.texture);
                break;
                
            case UniformValueType_RenderTexture:
                // renderTexture
                glProgram->setUniformWithTex2D(uniformName.c_str(), _value.tex2D.renderTexture);
                break;
                
            case UniformValueType_SamplerBuffer:
                // texture2D
                glProgram->setUniformWithSamplerBuffer(uniformName.c_str(), _value.samplerBuffer.textureBuffer);
                break;
                
                
            case UniformValueType_IntV:
                // intv
                glProgram->setUniformWithIntV(uniformName.c_str(), _value.intv.count, _value.intv.pointer);
                break;
                
            case UniformValueType_IVec2:
                // ivec2
                glProgram->setUniformWithIVec2(uniformName.c_str(), _value.ivec2.x, _value.ivec2.y);
                break;

            case UniformValueType_Vec2V:
                // vec2v
                glProgram->setUniformWithVec2V(uniformName.c_str(), _value.v2fv.count, _value.v2fv.pointer);
                break;
                
            case UniformValueType_IVec2V:
                // ivec2v
                glProgram->setUniformWithIVec2V(uniformName.c_str(), _value.ivec2v.count, _value.ivec2v.pointer);
                break;
                
            case UniformValueType_IVec3:
                // ivec3
                glProgram->setUniformWithIVec3(uniformName.c_str(), _value.ivec3.x, _value.ivec3.y, _value.ivec3.z);
                break;
                
            case UniformValueType_Vec3V:
                // vec3v
                glProgram->setUniformWithVec3V(uniformName.c_str(), _value.v3fv.count, _value.v3fv.pointer);
                break;
                
            case UniformValueType_IVec3V:
                // ivec3v
                glProgram->setUniformWithIVec3V(uniformName.c_str(), _value.ivec3v.count, _value.ivec3v.pointer);
                break;
                
            case UniformValueType_IVec4:
                // ivec4
                glProgram->setUniformWithIVec4(uniformName.c_str(), _value.ivec4.x, _value.ivec4.y, _value.ivec4.z, _value.ivec4.w);
                break;
                
            case UniformValueType_Vec4V:
                // vec4v
                glProgram->setUniformWithVec4V(uniformName.c_str(), _value.v4fv.count, _value.v4fv.pointer);
                break;
                
            case UniformValueType_IVec4V:
                // ivec4v
                glProgram->setUniformWithIVec4V(uniformName.c_str(), _value.ivec4v.count, _value.ivec4v.pointer);
                break;
                
            case UniformValueType_Mat4V:
                // matrix4x4v
                glProgram->setUniformWithMat4v(uniformName.c_str(), _value.mat4v.count, _value.mat4v.pointer);
                break;
                
            case UniformValueType_Mat4x3V:
                // matrix4x3v
                glProgram->setUniformWithMat4x3v(uniformName.c_str(), _value.mat4x3v.count, _value.mat4x3v.pointer);
                break;
                
            case UniformValueType_Mat3V:
                // matrix3x3v
                glProgram->setUniformWithMat3V(uniformName.c_str(), _value.mat3v.count, _value.mat3v.pointer);
                break;
                
            case UniformValueType_Mat3x4V:
                // matrix3x4v
                glProgram->setUniformWithMat3x4V(uniformName.c_str(), _value.mat3x4v.count, _value.mat3x4v.pointer);
                break;

			case UniformValueType_ImageBuffer:
				// imageBuffer
				glProgram->setUniformWithImageBuffer(uniformName.c_str(), _value.imageBuffer.textureId, _value.imageBuffer.access, _value.imageBuffer.format);
				break;

			case UniformValueType_UniformBuffer:
				// uniformBuffer
				glProgram->setUniformWithUniformBuffer(uniformName.c_str(), _value.uniformBuffer.buffer);
				break;
                
            default:
                break;
        }
        
    }
    
    
}


