#include "GLProgramCache.h"
#include "../../Tools/Utils.h"

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
        BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::addGLProgram");
		if (add(name, program))
		{
            BROWSER_WARNING(false, "GLProgram has been added, please check your program in function GLProgramCache::addGLProgram");
		}
    }
    
    GLProgram* GLProgramCache::getGLProgram(std::string name)
    {
		GLProgram* program = get(name);
        
		BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::getGLProgram");
		return program;
	}
    
    
}
