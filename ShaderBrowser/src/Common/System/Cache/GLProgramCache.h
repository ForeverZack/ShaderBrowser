#pragma once

#include "BaseCache.h"
#include <unordered_map>
#include "GL/GPUResource/Shader/GLProgram.h"

using namespace customGL;

namespace common
{
	class GLProgramCache : public BaseMapCache<std::string, GLProgram*, GLProgramCache>
	{
	public:
		GLProgramCache();
		~GLProgramCache();
        
    public:
        // 初始化
        void init();
        // 添加
		GLProgram* addGLProgram(const std::string& name, GLProgram* program);
		GLProgram* addGLProgram(const std::string& name, const std::string& vertFilename, const std::string& fragFilename);
		GLProgram* addGLProgramBySource(const std::string& name, const std::string& vertSource, const std::string& fragSource);
        // 获取
        GLProgram* getGLProgram(const std::string& name);
        // 获取GLProgram的拷贝
        GLProgram* getGLProgramCopy(const std::string& name);

	};
}

