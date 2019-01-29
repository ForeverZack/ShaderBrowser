#include "GLProgramCache.h"
#include "../../Tools/Utils.h"
#include "../../Tools/FileUtils.h"

namespace common
{    

    GLProgramCache::GLProgramCache()
    {

    }
    
    GLProgramCache::~GLProgramCache()
    {
        
    }
    
    void GLProgramCache::addGLProgram(std::string name, GLProgram* program)
    {
        BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::addGLProgram(std::string name, GLProgram* program)");
		if (add(name, program))
		{
            BROWSER_WARNING(false, "GLProgram has been added, please check your program in function GLProgramCache::addGLProgram(std::string name, GLProgram* program)");
		}
    }

	void GLProgramCache::addGLProgram(std::string name, const std::string& vertFilename, const std::string& fragFilename)
	{
		const std::string vert_full_path = FileUtils::getInstance()->getAbsolutePathForFilename(vertFilename);
		const std::string frag_full_path = FileUtils::getInstance()->getAbsolutePathForFilename(fragFilename);
		GLProgram* program = GLProgram::create(vert_full_path.c_str(), frag_full_path.c_str());
		BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::addGLProgram(std::string name, const std::string& vertFilename, const std::string& fragFilename)");
		if (add(name, program))
		{
			BROWSER_WARNING(false, "GLProgram has been added, please check your program in function GLProgramCache::addGLProgram(std::string name, const std::string& vertFilename, const std::string& fragFilename)");
		}
	}
    
    GLProgram* GLProgramCache::getGLProgram(std::string name)
    {
		GLProgram* program = get(name);
        
		BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::getGLProgram");
		return program;
	}
    
    
}
