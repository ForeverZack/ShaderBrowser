#include "GLProgramCache.h"
#include "../../Tools/Utils.h"

namespace common
{    
    static GLProgramCache* m_oInstance = nullptr;
    GLProgramCache* GLProgramCache::getInstance()
    {
        if (!m_oInstance)
        {
            m_oInstance = new GLProgramCache();
        }
        return m_oInstance;
    }
    
    GLProgramCache::GLProgramCache()
    {
        // 清理
        m_vPrograms.clear();
    }
    
    GLProgramCache::~GLProgramCache()
    {
        
    }
    
    void GLProgramCache::addGLProgram(std::string name, GLProgram* program)
    {
        BROWSER_ASSERT(program, "GLProgram is not invalid in function GLProgramCache::addGLProgram");
        
        auto itor = m_vPrograms.find(name);
        if (itor != m_vPrograms.end())
        {
            BROWSER_WARNING(false, "GLProgram has been added, please check your program in function GLProgramCache::addGLProgram");
            
            m_vPrograms.erase(itor);
        }
        
        m_vPrograms.emplace(name, program);
    }
    
    GLProgram* GLProgramCache::getGLProgram(std::string name)
    {
        auto itor = m_vPrograms.find(name);
        if (itor != m_vPrograms.end())
        {
            return itor->second;
        }
        
        return nullptr;
    }
    
    
}
