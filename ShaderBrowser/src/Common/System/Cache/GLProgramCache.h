#pragma once

#include "BaseCache.h"
#include <unordered_map>
#include "../../../GL/GLProgram.h"

using namespace customGL;

namespace common
{
	class GLProgramCache : public BaseMapCache<std::string, GLProgram, GLProgramCache>
	{
	public:
		GLProgramCache();
		~GLProgramCache();
        
    public:
        // 添加
        void addGLProgram(std::string name, GLProgram* program);
        // 获取
        GLProgram* getGLProgram(std::string name);

	};
}

