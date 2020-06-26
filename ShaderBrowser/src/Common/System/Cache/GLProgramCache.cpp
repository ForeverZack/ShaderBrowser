#include "GLProgramCache.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"

namespace common
{    

    GLProgramCache::GLProgramCache()
    {

    }
    
    GLProgramCache::~GLProgramCache()
    {
        
    }
    
    void GLProgramCache::init()
    {
        // 添加渲染引擎自带的默认着色器
        // 默认网格模型着色器
        GLProgramCache::getInstance()->addGLProgram(GLProgram::DEFAULT_GLPROGRAM_NAME, "shaders/default/model_default.vert", "shaders/default/model_default.frag");
		// 默认带骨骼动画的网格模型着色器
		GLProgramCache::getInstance()->addGLProgram(GLProgram::DEFAULT_SKELETON_GLPROGRAM_NAME, "shaders/default/skeleton_model_default.vert", "shaders/default/skeleton_model_default.frag");
        // 默认lines着色器
        GLProgramCache::getInstance()->addGLProgram(GLProgram::DEFAULT_LINES_GLPROGRAM_NAME, "shaders/default/lines_default.vert", "shaders/default/lines_default.frag");
    }
    
	GLProgram* GLProgramCache::addGLProgram(const std::string& name, GLProgram* program)
    {
        BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::addGLProgram(std::string name, GLProgram* program)");
        
        program->retain();
		if (add(name, program))
		{
            program->release();
            BROWSER_WARNING(false, "GLProgram has been added, please check your program in function GLProgramCache::addGLProgram(std::string name, GLProgram* program)");
		}

		return program;
    }

	GLProgram* GLProgramCache::addGLProgram(const std::string& name, const std::string& vertFilename, const std::string& fragFilename)
	{
		const std::string vert_full_path = FileUtils::getInstance()->getAbsolutePathForFilename(vertFilename);
		const std::string frag_full_path = FileUtils::getInstance()->getAbsolutePathForFilename(fragFilename);
		GLProgram* program = GLProgram::create(vert_full_path.c_str(), frag_full_path.c_str());
		BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::addGLProgram(std::string name, const std::string& vertFilename, const std::string& fragFilename)");
        program->retain();
		if (add(name, program))
		{
            program->release();
			BROWSER_WARNING(false, "GLProgram has been added, please check your program in function GLProgramCache::addGLProgram(std::string name, const std::string& vertFilename, const std::string& fragFilename)");
		}

		return program;
	}
    
	GLProgram* GLProgramCache::addGLProgramBySource(const std::string& name, const std::string& vertSource, const std::string& fragSource)
    {
        GLProgram* program = GLProgram::createBySource(vertSource, fragSource);

        BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::addGLProgramBySource(const std::string& name, const std::string& vertSource, const std::string& fragSource)");
        program->retain();
        if (add(name, program))
        {
            program->release();
            BROWSER_WARNING(false, "GLProgram has been added, please check your program in function GLProgramCache::addGLProgramBySource(const std::string& name, const std::string& vertSource, const std::string& fragSource)");
        }

		return program;
	}
    
    GLProgram* GLProgramCache::getGLProgram(const std::string& name)
    {
		GLProgram* program = get(name);
        
		BROWSER_WARNING(program, "GLProgram is not invalid in function GLProgramCache::getGLProgram");

		return program;
	}
    
    GLProgram* GLProgramCache::getGLProgramCopy(const std::string& name)
    {
        GLProgram* program = get(name);
        
        BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::getGLProgramCopy");
        
        return program->clone();
    }
    
    
}
