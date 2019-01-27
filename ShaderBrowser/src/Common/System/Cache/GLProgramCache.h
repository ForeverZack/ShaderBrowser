#pragma once

#include <unordered_map>
#include "../../../GL/GLProgram.h"

using namespace customGL;

namespace common
{
	class GLProgramCache
	{
    public:
        static GLProgramCache* getInstance();

	public:
		GLProgramCache();
		~GLProgramCache();
        
    public:
        // 添加
        void addGLProgram(std::string name, GLProgram* program);
        // 获取
        GLProgram* getGLProgram(std::string name);

	private:
        std::unordered_map<std::string, GLProgram*> m_vPrograms;
	};
}

