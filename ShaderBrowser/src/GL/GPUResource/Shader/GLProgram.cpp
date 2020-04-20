#include "GLProgram.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"
#include "GL/GLStateCache.h"
#include "GL/GPUResource/Texture/Texture2D.h"
#include "GL/GPUResource/Texture/TextureBuffer.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateGLProgramCommand.h"
#include "GL/System/GPUOperateSystem.h"
#include <glm/gtc/type_ptr.hpp>
#ifdef  _WIN32
	#pragma warning(disable:4996)
#endif //  _WIN32

namespace customGL
{
	// 默认GLProgram名称
	const char* GLProgram::DEFAULT_GLPROGRAM_NAME = "ShaderBrowser_DefaultGLProgram";
	// 默认带骨骼动画的网格模型着色器
	const char* GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME = "ShaderBrowser_DefaultSkeletonGLProgram";
    // 默认的line着色器
    const char* GLProgram::DEFAULT_LINES_GLPROGRAM_NAME = "ShaderBrowser_DefaultLinesGLProgram";

	// 预定义的顶点着色器属性名称
	const char* GLProgram::ATTRIBUTE_NAME_POSITION = "a_position";
	const char* GLProgram::ATTRIBUTE_NAME_COLOR = "a_color";
	const char* GLProgram::ATTRIBUTE_NAME_COORD = "a_coord";
    const char* GLProgram::ATTRIBUTE_NAME_NORMAL = "a_normal";
	const char* GLProgram::ATTRIBUTE_NAME_TANGENT = "a_tangent";
	const char* GLProgram::ATTRIBUTE_NAME_BONE_IDS = "a_boneIds";
	const char* GLProgram::ATTRIBUTE_NAME_BONE_WEIGHTS = "a_boneWeights";

//	//  预定义的uniform变量   (已移入#include "standard.inc")
//	// 创建shader时写入shader文件头的uniform字符串
//	const char* SHADER_UNIFORMS =
//		// 定义一些基本的数据结构
//		"struct DirectionalLight { vec3 direction; float intensity; vec4 color; };\n"
//		"const int MAX_BONES = 100;\n"
//        "const int MAX_DYNAMIC_BATCH_COUNT = 1000;\n"
//
//		// shader中可以使用的内置的uniform变量名字(不一定会有值,看具体怎么使用)
//		"uniform sampler2D CGL_TEXTURE0;\n"
//		"uniform sampler2D CGL_TEXTURE1;\n"
//		"uniform mat4 CGL_MODEL_MATRIX;\n"
//		"uniform mat4 CGL_VIEW_MATRIX;\n"
//		"uniform mat4 CGL_PROJECTION_MATRIX;\n"
//		"uniform vec4 CGL_ALBEDO_COLOR = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
//		"uniform DirectionalLight CGL_DIRECTIONAL_LIGHT;\n"
//		"uniform samplerBuffer CGL_BONES_MATRIX;\n"
//        "uniform mat4 CGL_DYNAMIC_BATCH_MODEL_MATRIX[MAX_DYNAMIC_BATCH_COUNT];\n";
    
    // uniform变量名称
	const char* GLProgram::SHADER_UNIFORMS_ARRAY[] =
	{
        "CGL_MAINCAMERA_WORLDPOS",
		"CGL_TEXTURE0",
		"CGL_TEXTURE1",
		"CGL_MODEL_MATRIX",
		"CGL_VIEW_MATRIX",
		"CGL_PROJECTION_MATRIX",
		"CGL_ALBEDO_COLOR",
        "CGL_BONES_MATRIX",
        "CGL_DYNAMIC_BATCH_MODEL_MATRIX[%d]",
        "CGL_LIGHT_TEXTURE0",
		"CGL_LIGHT_TEXTUREB0",
        
        "CGL_AMBIENT_COLOR",
        "CGL_DIRECTIONAL_LIGHT_NUM",
        "CGL_DIRECTIONAL_LIGHTS[%d]",
        "CGL_POINT_LIGHT_NUM",
        "CGL_POINT_LIGHTS[%d]",
        "CGL_SPOT_LIGHT_NUM",
        "CGL_SPOT_LIGHTS[%d]",
    };
    // 预定义uniform变量名字的最大长度
    const int GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH = 100;

    GLProgram* GLProgram::create(const char* vertPath, const char* fragPath)
    {
        GLProgram* program = new GLProgram();
        program->initProgram(vertPath, fragPath);
        return program;
    }
    
    GLProgram* GLProgram::createBySource(const std::string& vertSource, const std::string& fragSource)
    {
        GLProgram* program = new GLProgram();
        program->initProgramBySource(vertSource.c_str(), fragSource.c_str());
        return program;
    }

	std::string& GLProgram::convertSourceCodeInclude(std::string& source)
	{
		std::set<std::string> includeSet;
		return traverseConvertSourceCodeInc(source, includeSet);
	}

	std::string& GLProgram::traverseConvertSourceCodeInc(std::string& source, std::set<std::string>& includeFilesSet)
	{
		int startIdx, endIdx;

		startIdx = source.find("#include \"");    // int find(str);    -1:没找到
		while (startIdx != -1)
		{
			endIdx = source.find("\"", startIdx + 10);    // int find(str, startPos);

			BROWSER_ASSERT(endIdx != -1, "The #include \"XXX\" in your shader program miss the symbol \"");

			// include文件名
			std::string inc_filename = source.substr(startIdx + 10, endIdx - startIdx - 10);    // std::string substr(pos, length)
																								// 获取文件绝对路径
			inc_filename = common::FileUtils::getInstance()->getAbsolutePathForFilename(inc_filename);
			// 检测文件是否重复包含
			if (includeFilesSet.find(inc_filename) == includeFilesSet.end())
			{
				// 未重复包含
				includeFilesSet.insert(inc_filename);
				// 读取include文件源码
				std::string inc_source = common::Utils::readAbsolutePathFile(inc_filename.c_str());
				BROWSER_ASSERT(inc_source != "", "The inc file in your shader is empty");
				// 递归转换include文件中的include
				traverseConvertSourceCodeInc(inc_source, includeFilesSet);
				source.replace(startIdx, endIdx - startIdx + 1, inc_source);    // std::string& replace(pos, length, str);
			}
			else
			{
				// 重复包含
				source.replace(startIdx, endIdx - startIdx + 1, "");
			}

			startIdx = source.find("#include \"");
		}

		return source;
	}
    
	GLProgram::GLProgram()
		: m_uProgram(0)
		, m_uVertShader(0)
		, m_uFragShader(0)
        , m_uCompShader(0)
		, m_uTextureUnitIndex(0)
        , m_sAddtionVertCode("")
        , m_sAddtionFragCode("")
        , m_sAddtionCompCode("")
        , m_sCompLocalGroupDefCode("")
		, m_sVertexSource("")
		, m_sFragSource("")
		, m_sCompSource("")
	{
        m_eResourceType = GPUResourceType::GRT_GLProgram;
        
        for (int i=0; i<MAX_ACTIVE_TEXTURE; ++i)
        {
            /*
             当你打印的时候，printf（"%d",number）则 number是当成一个又符号的数来处理。输出-1
             当你这样打印的时候，printf（"%u",number）则number当成一个无符号的数来处理。输出65535
             小结：-1 这个数在内存中存着，究竟是有符号的还是无符号的，取决于程序眼对他的操作。另外，对于一个unsigned的数和一个int型的混合计算，则int型的会自动转换成unsigned型的。
            */
            m_vTexIds[i] = (GLuint)(-1);
        }
	}

	GLProgram::~GLProgram()
	{
	}
    
    GLProgram* GLProgram::clone()
    {
        BROWSER_ASSERT(m_sVertexSource!="" || m_sFragSource!="" || m_sCompSource!="" || m_sFragFilePath!="" || m_sVertFilePath!="", "No shader source code in GLProgram obeject, you cannot use GLProgram::clone function.");
        
        GLProgram* program = new GLProgram();
		if (this->getResouceState() != GRS_Loaded)
		{
			program->initProgram(m_sVertFilePath.c_str(), m_sFragFilePath.c_str());
		}
		else
		{
			program->cloneProgram(this);
		}
        return program;
    }

	void GLProgram::bindPredefinedVertexAttribs()
	{
        // 顶点属性location
		static const struct
		{
			const char* name;
			int location;
		} attribute_locations[] = 
		{
			{GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTR_POSITION},
			{GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTR_COLOR},
			{GLProgram::ATTRIBUTE_NAME_COORD, GLProgram::VERTEX_ATTR_TEX_COORD},
            {GLProgram::ATTRIBUTE_NAME_NORMAL, GLProgram::VERTEX_ATTR_NORMAL},
            {GLProgram::ATTRIBUTE_NAME_TANGENT, GLProgram::VERTEX_ATTR_TANGENT}
		};

		const int size = sizeof(attribute_locations) / sizeof(attribute_locations[0]);
		for (int i = 0; i < size; ++i)
		{
			glBindAttribLocation(m_uProgram, attribute_locations[i].location, attribute_locations[i].name);
		}
	}

	void GLProgram::initProgram(const char* vertPath, const char* fragPath, bool saveSource /*= true*/)
	{
		if (!vertPath || strlen(vertPath) == 0 || !fragPath || strlen(fragPath) == 0)
		{
			return;
		}
        
        // 创建GPU资源
        m_eResouceState = GRS_DataLoaded;
		m_sVertFilePath = vertPath;
		m_sFragFilePath = fragPath;
        createGPUResource(vertPath, fragPath);
	}
    
    void GLProgram::initProgramBySource(const std::string& vertSource, const std::string& fragSource, bool saveSource /*= true*/)
    {
        if (vertSource.size()==0 || fragSource.size()==0)
        {
            return;
        }
        
        // 创建GPU资源
        m_eResouceState = GRS_DataLoaded;
        m_sVertexSource = vertSource;
        m_sFragSource = fragSource;
        createGPUResourceBySource(vertSource, fragSource);
    }
    
    bool GLProgram::cloneProgram(GLProgram* srcGLProgram)
    {
		// 创建GPU资源
		m_eResouceState = GRS_DataLoaded;
		createGPUResourceBySource(srcGLProgram->m_sVertexSource, srcGLProgram->m_sFragSource);
        return true;
    }

	bool GLProgram::createShader(GLenum type, GLuint& shader, const char* shaderPath)
	{
		// 读取着色器内容
		std::string source = std::move(common::Utils::readAbsolutePathFile(shaderPath));
		if (source == "")
		{
			std::cerr << "shader src is empty: " << shaderPath << std::endl;
			return false;
		}
		// 转换shader中的include文件内容
		convertSourceCodeInclude(source);

        bool successFlag = createShaderBySource(type, shader, source.c_str());

        return successFlag;
	}
    
    bool GLProgram::createShaderBySource(GLenum type, GLuint& shader, const char* shaderSource)
    {
        if (shaderSource == NULL)
        {
            std::cerr << "shader source code pointer is empty: " << std::endl;
            return false;
        }
        
        // 1.创建shader
        shader = glCreateShader(type);
        
        // 2.绑定shader源码
		std::string additionCode = "";
        switch(type)
        {
            case GL_VERTEX_SHADER:
                {
					// 顶点着色器
                    additionCode = m_sAddtionVertCode;
					const GLchar *sources[] =
					{
						"#version 330 core\n",    // 头
						additionCode.c_str(), //附加代码
						shaderSource // 源码
					};
					glShaderSource(shader, sizeof(sources) / sizeof(*sources), sources, NULL);
                }
                break;
                
            case GL_FRAGMENT_SHADER:
                {
					// 片段着色器
                    additionCode = m_sAddtionFragCode;
					const GLchar *sources[] =
					{
						"#version 330 core\n",    // 头
						additionCode.c_str(), //附加代码
						shaderSource // 源码
					};
					glShaderSource(shader, sizeof(sources) / sizeof(*sources), sources, NULL);
                }
                break;
                
            case GL_COMPUTE_SHADER:
                {
					// 计算着色器
                    additionCode = m_sCompLocalGroupDefCode + m_sAddtionCompCode;
					const GLchar *sources[] =
					{
						"#version 430 core\n",    // 头
						additionCode.c_str(), //附加代码
						shaderSource // 源码
					};
					glShaderSource(shader, sizeof(sources) / sizeof(*sources), sources, NULL);
                }
                break;
        }
        
        // 3.编译shader
        glCompileShader(shader);

		// 保存shader源码
		string& save_pointer = getSourceSavePointer(type);
		save_pointer = shaderSource;
        
        // 检查编译情况
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            //#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
            
            GLchar* log = new GLchar[len + 1];
            glGetShaderInfoLog(shader, len, &len, log);
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete[] log;
            //#endif /* DEBUG */
            
            switch(type)
            {
                case GL_VERTEX_SHADER: BROWSER_LOG(m_sVertexSource); break;
                case GL_FRAGMENT_SHADER: BROWSER_LOG(m_sFragSource); break;
            }
            common::BROWSER_ASSERT(compiled, "shader program compiled error in function GLProgram::createShader");
            
            return false;
        }
        
        return true;
    }
    
	string& GLProgram::getSourceSavePointer(GLenum type)
    {
        switch (type) {
            case GL_VERTEX_SHADER:
            {
                return m_sVertexSource;
            }

            case GL_FRAGMENT_SHADER:
            {
                return m_sFragSource;
            }
                
            case GL_COMPUTE_SHADER:
            {
                return m_sCompSource;
            }
        }
        
        return m_sVertexSource;
    }
    
    void GLProgram::createGPUResource(const char* vertPath, const char* fragPath)
    {
        BROWSER_ASSERT(m_eResouceState==GRS_DataLoaded, "GLProgram state must be GRS_DataLoaded, then it can be created on gpu");
        
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateGLProgramCommand>(GPUOperateCommandType::GOCT_GLProgram);
        cmd->setGLProgram(this);
        cmd->setVertShaderPath(vertPath);
        cmd->setFragShaderPath(fragPath);
        cmd->ready(GPUOperateType::GOT_Create);
        GPUOperateSystem::getInstance()->addCommand(cmd);
    }

	void GLProgram::createGPUResourceBySource(const string& vertSource, const string& fragSource)
	{
		BROWSER_ASSERT(m_eResouceState == GRS_DataLoaded, "GLProgram state must be GRS_DataLoaded, then it can be created on gpu");

		auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateGLProgramCommand>(GPUOperateCommandType::GOCT_GLProgram);
		cmd->setGLProgram(this);
		cmd->setVertShaderSource(vertSource);
		cmd->setFragShaderSource(fragSource);
		cmd->ready(GPUOperateType::GOT_Create);
		GPUOperateSystem::getInstance()->addCommand(cmd);
	}

    void GLProgram::updateGPUResource(const std::unordered_map<std::string, UniformValue>& uniforms)
    {
		// 绘制命令会处理这部分，所以这里没有必要更新了

		//auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateGLProgramCommand>(GPUOperateCommandType::GOCT_GLProgram);
		//cmd->setGLProgram(this);
		//cmd->setUniforms(uniforms);
		//cmd->ready(GPUOperateType::GOT_Update);
		//GPUOperateSystem::getInstance()->addCommand(cmd);
    }
    
    void GLProgram::deleteGPUResource()
    {
		BROWSER_ASSERT(m_eResouceState == GRS_Loaded, "GLProgram state must be GRS_Loaded, then it can be deleted on gpu");

		auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateGLProgramCommand>(GPUOperateCommandType::GOCT_GLProgram);
		cmd->setGLProgram(this);
		cmd->ready(GPUOperateType::GOT_Delete);
		GPUOperateSystem::getInstance()->addCommand(cmd);
    }

	void GLProgram::useProgram()
	{
		glUseProgram(m_uProgram);
	}

	void GLProgram::updatePreDefinedUniformsLocation()
	{
		GLint location = -1;
		for (int i = 0; i < UNIFORM_MAX_COUNT; ++i)
		{
            std::string uniformName(SHADER_UNIFORMS_ARRAY[i]);
			{
				location = glGetUniformLocation(m_uProgram, uniformName.c_str());
				m_mUniformLocations.emplace(uniformName, location);
			}
			
		}
	}
    
	static const std::string LOCATION_ERROR_STR = "Cannot find uniform location in function GLProgram::getUniformLocation named: ";
    GLint GLProgram::getUniformLocation(const std::string& uniformName)
    {
        std::string str_uniformName(uniformName);
        if (m_mUniformLocations.find(str_uniformName)==m_mUniformLocations.end() || m_mUniformLocations[uniformName]==-1)
        {
            GLint location = glGetUniformLocation(m_uProgram, str_uniformName.c_str());
            
            m_mUniformLocations[str_uniformName] = location;
            
            // shader中声明但未使用的变量，opengl会自动优化移除它，所以这些uniform的location也为-1，使用glUniformXXX设置位置为-1的变量并不会报错
            //common::BROWSER_WARNING(location>=0, (LOCATION_ERROR_STR + uniformName).c_str());
        }
        
        GLint location = m_mUniformLocations[str_uniformName];
        return location;
    }
    
    void GLProgram::setUniformWithInt(const std::string& uniformName, int value)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform1i(location, value);
    }
    void GLProgram::setUniformWithFloat(const std::string& uniformName, float value)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform1f(location, value);
    }
    void GLProgram::setUniformWithMat3(const std::string& uniformName, const glm::mat3& value)
    {
        GLint location = getUniformLocation(uniformName);
//        typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    
    void GLProgram::setUniformWithMat3V(const std::string& uniformName, int count, const float* fv)
    {
        GLint location = getUniformLocation(uniformName);
        //        typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
        glUniformMatrix3fv(location, count, GL_FALSE, fv);
    }
    
    void GLProgram::setUniformWithMat3x4(const std::string& uniformName, const glm::mat3x4& value)
    {
        GLint location = getUniformLocation(uniformName);
        //        typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    
    void GLProgram::setUniformWithMat3x4V(const std::string& uniformName, int count, const float* fv)
    {
        GLint location = getUniformLocation(uniformName);
        //        typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        glUniformMatrix3x4fv(location, count, GL_FALSE, fv);
    }
    
    void GLProgram::setUniformWithMat4(const std::string& uniformName, const glm::mat4& value)
    {
        GLint location = getUniformLocation(uniformName);
//        typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    
    void GLProgram::setUniformWithMat4v(const std::string& uniformName, int count, const float* fv)
    {
        GLint location = getUniformLocation(uniformName);
        //        typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
        glUniformMatrix4fv(location, count, GL_FALSE, fv);
    }
    
    void GLProgram::setUniformWithMat4x3(const std::string& uniformName, const glm::mat4x3& value)
    {
        GLint location = getUniformLocation(uniformName);
        //        typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    
    void GLProgram::setUniformWithMat4x3v(const std::string& uniformName, int count, const float* fv)
    {
        GLint location = getUniformLocation(uniformName);
        //        typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        glUniformMatrix4x3fv(location, count, GL_FALSE, fv);
    }
    
    void GLProgram::setUniformWithFloatV(const std::string& uniformName, int count, const float* fv)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform1fv(location, count, fv);
    }
    
    void GLProgram::setUniformWithIntV(const std::string& uniformName, int count, const int* iv)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform1iv(location, count, iv);
    }
    
    void GLProgram::setUniformWithVec2(const std::string& uniformName, GLfloat f1, GLfloat f2)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform2f(location, f1, f2);
    }
    
    void GLProgram::setUniformWithIVec2(const std::string& uniformName, int i1, int i2)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform2i(location, i1, i2);
    }
    
    void GLProgram::setUniformWithVec2V(const std::string& uniformName, int count, const GLfloat* fv)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform2fv(location, count, fv);
    }
    
    void GLProgram::setUniformWithIVec2V(const std::string& uniformName, int count, const int* iv)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform2iv(location, count, iv);
    }
    
    void GLProgram::setUniformWithVec3(const std::string& uniformName, GLfloat f1, GLfloat f2, GLfloat f3)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform3f(location, f1, f2, f3);
    }
    
    void GLProgram::setUniformWithIVec3(const std::string& uniformName, int i1, int i2, int i3)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform3i(location, i1, i2, i3);
    }
    
    void GLProgram::setUniformWithVec3V(const std::string& uniformName, int count, const GLfloat* fv)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform3fv(location, count, fv);
    }
    
    void GLProgram::setUniformWithIVec3V(const std::string& uniformName, int count, const int* iv)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform3iv(location, count, iv);
    }

	void GLProgram::setUniformWithVec4(const std::string& uniformName, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
	{
		GLint location = getUniformLocation(uniformName);
		glUniform4f(location, f1, f2, f3, f4);
	}
    
    void GLProgram::setUniformWithIVec4(const std::string& uniformName, int i1, int i2, int i3, int i4)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform4i(location, i1, i2, i3, i4);
    }
    
    void GLProgram::setUniformWithVec4V(const std::string& uniformName, int count, const GLfloat* fv)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform4fv(location, count, fv);
    }
    
    void GLProgram::setUniformWithIVec4V(const std::string& uniformName, int count, const int* iv)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform4iv(location, count, iv);
    }
    
    void GLProgram::setUniformWithTex2D(const std::string& uniformName, Texture2D* texture)
    {
		// 自动生成纹理单元
		GLuint textureUnit;
		if (m_mTextureUnits.find(uniformName) != m_mTextureUnits.end())
		{
			textureUnit = m_mTextureUnits[uniformName];
		}
		else
		{
			textureUnit = m_uTextureUnitIndex++;
			m_mTextureUnits[uniformName] = textureUnit;
            
            // 注意！！！！ 还要通过使用glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元。我们只需要设置一次即可
            setUniformWithInt(uniformName, textureUnit);
		}
        
        common::BROWSER_ASSERT(textureUnit<MAX_ACTIVE_TEXTURE, "texture unit value is too big, it is out off support range in function GLProgram::setUniformWithTex2d");
		
        // 绑定纹理到opengl
        GLStateCache::getInstance()->bindTexture2DN(textureUnit, texture->getTextureId());
    }
    
    void GLProgram::setUniformWithSamplerBuffer(const std::string& uniformName, TextureBuffer* textureBuffer)
    {
        // 自动生成纹理单元
        GLuint textureUnit;
        if (m_mTextureUnits.find(uniformName) != m_mTextureUnits.end())
        {
            textureUnit = m_mTextureUnits[uniformName];
        }
        else
        {
            textureUnit = m_uTextureUnitIndex++;
            m_mTextureUnits[uniformName] = textureUnit;
            
            // 注意！！！！ 还要通过使用glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元。我们只需要设置一次即可
            setUniformWithInt(uniformName, textureUnit);
        }
        
        common::BROWSER_ASSERT(textureUnit<MAX_ACTIVE_TEXTURE, "texture unit value is too big, it is out off support range in function GLProgram::setUniformSamplerBuffer");

        // 绑定纹理到opengl
        GLStateCache::getInstance()->bindSamplerBuffer(textureUnit, textureBuffer->getTextureId());
    }

	void GLProgram::setUniformWithImageBuffer(const std::string& uniformName, GLuint textureId, GLenum access, GLenum format)
	{
		// 自动生成纹理单元
		GLuint textureUnit;
		if (m_mTextureUnits.find(uniformName) != m_mTextureUnits.end())
		{
			textureUnit = m_mTextureUnits[uniformName];
		}
		else
		{
			textureUnit = m_uTextureUnitIndex++;
			m_mTextureUnits[uniformName] = textureUnit;

			// 注意！！！！ 还要通过使用glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元。我们只需要设置一次即可
			setUniformWithInt(uniformName, textureUnit);
		}

		common::BROWSER_ASSERT(textureUnit < MAX_ACTIVE_TEXTURE, "texture unit value is too big, it is out off support range in function GLProgram::setUniformWithImageBuffer");

		// 绑定纹理到opengl
		GLStateCache::getInstance()->bindImageBuffer(textureUnit, textureId, access, format);
	}
    
}
