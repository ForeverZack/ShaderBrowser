#include "GLDefine.h"
#include "GLProgram.h"
#include "Common/Tools/Utils.h"

namespace customGL {
    
    // 默认GLProgram名称
    //extern const char* DEFAULT_GLPROGRAM_NAME = "ShaderBrowser_DefaultGLProgram";
    
    // 默认材质名称
    //extern const char* DEFAULT_MATERIAL_NAME = "ShaderBrowser_DefaultMaterial";
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
    
    UniformValue::UniformValue()
        : m_eType(UniformValueType::UniformValueType_Undefined)
        , m_bDirty(true)
    {
        
    }
    
    UniformValue::UniformValue(const UniformValue&& uniformVal)
    {
        m_eType = uniformVal.m_eType;
        _value = uniformVal._value;
        m_bDirty = uniformVal.m_bDirty;
    }
    
    UniformValue::~UniformValue()
    {
        
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
    
    void UniformValue::setMat4(const glm::mat4x4& value)
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
    
    void UniformValue::setFloatV(int size, const float* value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
               || m_eType==UniformValueType::UniformValueType_FloatV,
               "UniformValueType has already defined, you cannot change it again in function UniformValue::setFloatV");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_FloatV;
        _value.floatv.size = size;
        _value.floatv.pointer = value;
    }
    
    void UniformValue::setVec2(const glm::vec2& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
               || m_eType==UniformValueType::UniformValueType_Vec2,
               "UniformValueType has already defined, you cannot change it again in function UniformValue::setVec2");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Vec2;
        _value.v2f.pointer = value;
    }
    
    void UniformValue::setVec3(const glm::vec3& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
               || m_eType==UniformValueType::UniformValueType_Vec3,
               "UniformValueType has already defined, you cannot change it again in function UniformValue::setVec3");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Vec3;
        _value.v3f.pointer = value;
    }
    
    void UniformValue::setVec4(const glm::vec4& value)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
               || m_eType==UniformValueType::UniformValueType_Vec4,
               "UniformValueType has already defined, you cannot change it again in function UniformValue::setVec4");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Vec4;
        _value.v4f.pointer = value;
    }
    
    void UniformValue::setTex2D(GLuint textureId)
    {
        common::BROWSER_ASSERT(m_eType==UniformValueType::UniformValueType_Undefined
                               || m_eType==UniformValueType::UniformValueType_Sampler2D,
                               "UniformValueType has already defined, you cannot change it again in function UniformValue::setTex2D");
        
        m_bDirty = true;
        m_eType = UniformValueType::UniformValueType_Sampler2D;
        _value.tex2D.textureId = textureId;
    }
    
    const glm::mat4& UniformValue::getMat4()
    {
        return _value.mat4;
    }
    
    void UniformValue::updateGLProgramUniformValue(GLProgram* glProgram, std::string uniformName, bool forceUpdate/* = false*/)
    {
        if(!m_bDirty)
        {
            return;
        }
        
        m_bDirty = false;
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
                glProgram->setUniformWithFloatV(uniformName.c_str(), _value.floatv.size, _value.floatv.pointer);
                break;
                
            case UniformValueType_Vec2:
                // vec2
                glProgram->setUniformWithVec2(uniformName.c_str(), _value.v4f.pointer.x, _value.v4f.pointer.y);
                break;
                
            case UniformValueType_Vec3:
                // vec3
                glProgram->setUniformWithVec3(uniformName.c_str(), _value.v4f.pointer.x, _value.v4f.pointer.y, _value.v4f.pointer.z);
                break;
                
            case UniformValueType_Vec4:
                // vec4
                glProgram->setUniformWithVec4(uniformName.c_str(), _value.v4f.pointer.x, _value.v4f.pointer.y, _value.v4f.pointer.z, _value.v4f.pointer.w);
                break;
                
            case UniformValueType_Sampler2D:
                // texture2D
                glProgram->setUniformWithTex2D(uniformName.c_str(), _value.tex2D.textureId);
                break;
                
            default:
                break;
        }
        
    }
    
    
}


