#include "Utils.h"
#include "Common/Tools/FileUtils.h"

namespace common
{
	void BROWSER_ASSERT(bool condition, const char* msg)
	{
        if (!condition)
        {
            std::cout << "ERROR: " << msg << endl;
            assert(condition);
        }
    }
    
    void BROWSER_WARNING(bool condition, const char* msg)
    {
        if (!condition)
        {
            std::cout << "WARNING: " << msg << endl;
        }
    }

	std::string Utils::readAbsolutePathFile(const char* absolutePath)
	{
#ifdef _WIN32
        // WIN32
		FILE* infile;
		fopen_s(&infile, absolutePath, "rb");
#else
        // macOS
		FILE* infile = fopen(absolutePath, "rb");
#endif

		if (!infile) {
#ifdef _DEBUG
			std::cerr << "Unable to open file '" << absolutePath << "'" << std::endl;
#endif /* DEBUG */
			return NULL;
		}

		fseek(infile, 0, SEEK_END);
		int len = ftell(infile);
		fseek(infile, 0, SEEK_SET);

		std::string source(len, '\0');
		fread(&source[0], 1, len, infile);
		fclose(infile);

		return source;
	}
    
    std::string Utils::readFile(const char* filepath)
    {
        const std::string inc_full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath);
        return readAbsolutePathFile(inc_full_path.c_str());
    }
    
    VertexAttribDeclaration* Utils::createVertexAttribDeclaration(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, VertexDataType dataType /*= VertexDataType::Float*/)
    {
        VertexAttribDeclaration* declaration = new VertexAttribDeclaration();
        declaration->index = location;
        declaration->size = size;
        declaration->type = type;
        declaration->normalized = normalized;
        declaration->stride = stride;
        declaration->data_type = dataType;
        
        return declaration;
    }
    
    VertexAttribDeclaration* Utils::createVertexAttribDeclaration(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer, GLvoid* data, GLint dataSize, VertexDataType dataType /*=VertexDataType::Float*/)
    {
        VertexAttribDeclaration* declaration = new VertexAttribDeclaration();
        declaration->index = location;
        declaration->size = size;
        declaration->type = type;
        declaration->normalized = normalized;
        declaration->stride = stride;
        declaration->pointer = pointer;
        declaration->data = data;
        declaration->data_size = dataSize;
        declaration->data_type = dataType;
        
        return declaration;
    }

    FeedbackBufferDeclaration* Utils::createFeedbackBufferDeclaration(GLuint bindIdx, GLuint size, const string& name, BufferType type/* = BufferType::ArrayBuffer*/, GLenum internalFormat /*= GL_RGBA32F*/)
    {
        FeedbackBufferDeclaration* declaration = new FeedbackBufferDeclaration();
        declaration->varying = name;
        declaration->bindIdx = bindIdx;
        declaration->size = size;
        declaration->type = type;
        declaration->internalFormat = internalFormat;
        
        return declaration;
    }

	ComputeBufferDeclaration* Utils::createComputeBufferDeclaration(const string& name, GLuint size, GLenum access/* = GL_READ_ONLY*/, BufferType type/* = BufferType::TextureBuffer*/, GLenum format/* = GL_RGBA32F*/)
	{
		ComputeBufferDeclaration* declaration = new ComputeBufferDeclaration();
		declaration->name = name;
		declaration->size = size;
		declaration->access = access;
		declaration->type = type;
		declaration->format = format;

		return declaration;
	}
    
    void Utils::setUniformInt(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setInt(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setInt(value);
        }
    }
    void Utils::setUniformFloat(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, float value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setFloat(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setFloat(value);
        }
    }
    void Utils::setUniformMat3(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::mat3& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3(value);
        }
    }
    void Utils::setUniformMat3x4(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::mat3x4& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3x4(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3x4(value);
        }
    }
    void Utils::setUniformMat4(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::mat4& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4(value);
        }
    }
    
    void Utils::setUniformMat4x3(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::mat4x3& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4x3(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4x3(value);
        }
    }
    
    void Utils::setUniformFloatV(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int size, const float* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setFloatV(size, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setFloatV(size, value);
        }
    }
    
    void Utils::setUniformV2f(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::vec2& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec2(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec2(value);
        }
    }
    
    void Utils::setUniformV3f(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::vec3& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec3(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec3(value);
        }
    }
    
    void Utils::setUniformV4f(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::vec4& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec4(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec4(value);
        }
    }
    
    void Utils::setUniformTex2D(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, Texture2D* texture)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setTex2D(texture);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setTex2D(texture);
        }
    }
    
    void Utils::setUniformSamplerBuffer(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, TextureBuffer* textureBuffer)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setSamplerBuffer(textureBuffer);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setSamplerBuffer(textureBuffer);
        }
    }

    void Utils::setUniformIntV(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const int* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIntV(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIntV(count, value);
        }
    }
    
    void Utils::setUniformIVec2(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::ivec2& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec2(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec2(value);
        }
    }
    
    void Utils::setUniformVec2V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec2V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec2V(count, value);
        }
    }
    
    void Utils::setUniformIVec2V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const int* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec2V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec2V(count, value);
        }
    }
    
    void Utils::setUniformIVec3(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::ivec3& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec3(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec3(value);
        }
    }
    
    void Utils::setUniformVec3V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec3V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec3V(count, value);
        }
    }
    
    void Utils::setUniformIVec3V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const int* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec3V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec3V(count, value);
        }
    }
    
    void Utils::setUniformIVec4(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, const glm::ivec4& value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec4(value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec4(value);
        }
    }
    
    void Utils::setUniformVec4V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec4V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec4V(count, value);
        }
    }
    
    void Utils::setUniformIVec4V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const int* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec4V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec4V(count, value);
        }
    }
    
    void Utils::setUniformMat4V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4V(count, value);
        }
    }
    
    void Utils::setUniformMat4x3V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4x3V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4x3V(count, value);
        }
    }
    
    void Utils::setUniformMat3V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3V(count, value);
        }
    }
    
    void Utils::setUniformMat3x4V(std::unordered_map<std::string, UniformValue>& uniforms, const std::string& uniformName, int count, const float* value)
    {
        auto itor = uniforms.find(uniformName);
        if (itor == uniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3x4V(count, value);
            uniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3x4V(count, value);
        }
    }
    
    void Utils::parseMatrix(const glm::mat4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale)
    {
        /* 假设有列主序矩阵M（其变换顺序为M=Translate*Rotate*Scale=Translate*Scale*Rotate）
                      m00, m10, m20, m30
                M = { m01, m11, m21, m31 }
                      m02, m13, m22, m32
                      m03, m13, m23, m33
            另L0,L1,L2分别为矩阵M第0列，第1列，第2列所构成向量的长度,则拆分后的矩阵为:
                   1, 0, 0, m30           L0, 0, 0, 0           m00/L0, m10/L1, m20/L2, 0
             T = { 0, 1, 0, m31 } , S = { 0, L1, 0, 0 } , R = { m01/L0, m11/L1, m21/L2, 0 }
                   0, 0, 0, m32           0, 0, L2, 0           m02/L0, m12/L1, m22/L2, 0
                   0, 0, 0, 1             0, 0,  0, 1             0,      0,      0,    1
         */
        position = glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
        float L0 = glm::length(glm::vec4(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]));
        float L1 = glm::length(glm::vec4(matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]));
        float L2 = glm::length(glm::vec4(matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]));
        scale = glm::vec3(L0, L1, L2);
        glm::mat4 rotateMat(matrix[0][0]/L0, matrix[1][0]/L1, matrix[2][0]/L2, 0,
                            matrix[0][1]/L0, matrix[1][1]/L1, matrix[2][1]/L2, 0,
                            matrix[0][2]/L0, matrix[1][2]/L1, matrix[2][2]/L2, 0,
                            0, 0, 0, 1);
        rotation = convertMatrix2Quat(rotateMat);
    }
    
    void Utils::parseMatrix(const aiMatrix4x4& matrix, glm::vec4& position, glm::vec4& rotation, glm::vec4& scale)
    {
        // 注意：aiMatrix4x4是行主序的矩阵！！！！
        position = glm::vec4(matrix[0][3], matrix[1][3], matrix[2][3], 1);
        float L0 = glm::length(glm::vec4(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0]));
        float L1 = glm::length(glm::vec4(matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1]));
        float L2 = glm::length(glm::vec4(matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2]));
        scale = glm::vec4(L0, L1, L2, 0);
        glm::mat4 rotateMat(matrix[0][0]/L0, matrix[0][1]/L1, matrix[0][2]/L2, 0,
                            matrix[1][0]/L0, matrix[1][1]/L1, matrix[1][2]/L2, 0,
                            matrix[2][0]/L0, matrix[2][1]/L1, matrix[2][2]/L2, 0,
                            0, 0, 0, 1);
        rotation = convertMatrix2Vec4(rotateMat);
    }
    
    void Utils::parseMatrix(const aiMatrix4x4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale)
    {
        // 注意：aiMatrix4x4是行主序的矩阵！！！！
        position = glm::vec3(matrix[0][3], matrix[1][3], matrix[2][3]);
        float L0 = glm::length(glm::vec4(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0]));
        float L1 = glm::length(glm::vec4(matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1]));
        float L2 = glm::length(glm::vec4(matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2]));
        scale = glm::vec3(L0, L1, L2);
        glm::mat4 rotateMat(matrix[0][0]/L0, matrix[0][1]/L1, matrix[0][2]/L2, 0,
                            matrix[1][0]/L0, matrix[1][1]/L1, matrix[1][2]/L2, 0,
                            matrix[2][0]/L0, matrix[2][1]/L1, matrix[2][2]/L2, 0,
                            0, 0, 0, 1);
        rotation = convertMatrix2Quat(rotateMat);
    }
    
    glm::quat Utils::convertMatrix2Quat(const glm::mat4& matrix)
    {
        float x, y, z, w;
        
        float fourWSquaredMinus1 = matrix[0][0] + matrix[1][1] + matrix[2][2];
        float fourXSquaredMinus1 = matrix[0][0] - matrix[1][1] - matrix[2][2];
        float fourYSquaredMinus1 = matrix[1][1] - matrix[0][0] - matrix[2][2];
        float fourZSquaredMinus1 = matrix[2][2] - matrix[0][0] - matrix[1][1];
        
        int biggestIndex = 0;
        float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
        if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourXSquaredMinus1;
            biggestIndex = 1;
        }
        if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourYSquaredMinus1;
            biggestIndex = 2;
        }
        if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourZSquaredMinus1;
            biggestIndex = 3;
        }
        
        float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
        float mult = 0.25f / biggestVal;
        switch (biggestIndex)
        {
            case 0:
                {
                    w = biggestVal;
                    x = (matrix[2][1] - matrix[1][2]) * mult;
                    y = (matrix[0][2] - matrix[2][0]) * mult;
                    z = (matrix[1][0] - matrix[0][1]) * mult;
                }
                break;
                
            case 1:
                {
                    x = biggestVal;
                    w = (matrix[2][1] - matrix[1][2]) * mult;
                    y = (matrix[1][0] + matrix[0][1]) * mult;
                    z = (matrix[0][2] + matrix[2][0]) * mult;
                }
                break;
                
            case 2:
                {
                    y = biggestVal;
                    w = (matrix[0][2] - matrix[2][0]) * mult;
                    x = (matrix[1][0] + matrix[0][1]) * mult;
                    z = (matrix[2][1] + matrix[1][2]) * mult;
                }
                break;
                
            case 3:
                {
                    z = biggestVal;
                    w = (matrix[1][0] - matrix[0][1]) * mult;
                    x = (matrix[0][2] + matrix[2][0]) * mult;
                    y = (matrix[2][1] + matrix[1][2]) * mult;
                }
                break;
        }
        
        
        return glm::quat(w, x, y, z);
    }
    
    glm::vec4 Utils::convertMatrix2Vec4(const glm::mat4& matrix)
    {
        float x, y, z, w;
        
        float fourWSquaredMinus1 = matrix[0][0] + matrix[1][1] + matrix[2][2];
        float fourXSquaredMinus1 = matrix[0][0] - matrix[1][1] - matrix[2][2];
        float fourYSquaredMinus1 = matrix[1][1] - matrix[0][0] - matrix[2][2];
        float fourZSquaredMinus1 = matrix[2][2] - matrix[0][0] - matrix[1][1];
        
        int biggestIndex = 0;
        float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
        if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourXSquaredMinus1;
            biggestIndex = 1;
        }
        if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourYSquaredMinus1;
            biggestIndex = 2;
        }
        if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourZSquaredMinus1;
            biggestIndex = 3;
        }
        
        float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
        float mult = 0.25f / biggestVal;
        switch (biggestIndex)
        {
            case 0:
            {
                w = biggestVal;
                x = (matrix[2][1] - matrix[1][2]) * mult;
                y = (matrix[0][2] - matrix[2][0]) * mult;
                z = (matrix[1][0] - matrix[0][1]) * mult;
            }
                break;
                
            case 1:
            {
                x = biggestVal;
                w = (matrix[2][1] - matrix[1][2]) * mult;
                y = (matrix[1][0] + matrix[0][1]) * mult;
                z = (matrix[0][2] + matrix[2][0]) * mult;
            }
                break;
                
            case 2:
            {
                y = biggestVal;
                w = (matrix[0][2] - matrix[2][0]) * mult;
                x = (matrix[1][0] + matrix[0][1]) * mult;
                z = (matrix[2][1] + matrix[1][2]) * mult;
            }
                break;
                
            case 3:
            {
                z = biggestVal;
                w = (matrix[1][0] - matrix[0][1]) * mult;
                x = (matrix[0][2] + matrix[2][0]) * mult;
                y = (matrix[2][1] + matrix[1][2]) * mult;
            }
                break;
        }
        
        
        return glm::vec4(x, y, z, w);
    }
    

}
