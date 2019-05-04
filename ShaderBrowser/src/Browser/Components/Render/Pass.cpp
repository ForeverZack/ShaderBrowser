#include "Pass.h"
#include "GL/Texture2D.h"
#include "GL/GLDefine.h"

namespace browser
{
	Pass* Pass::createPass(GLProgram* program)
	{
		Pass* pass = new Pass();
		pass->init(program);
        

		return pass;
	}

	Pass::Pass()
		: m_oGLProgram(nullptr)
	{
	}

	Pass::~Pass()
	{
        if (m_oGLProgram)
        {
            m_oGLProgram->release();
        }
        
        m_mUniforms = nullptr;
	}

	void Pass::init(GLProgram* program)
	{
        program->retain();
		m_oGLProgram = program;
	}
    
    void Pass::setUniformsFromMaterial(std::unordered_map<std::string, UniformValue>* uniforms)
    {
        m_mUniforms = uniforms;
    }

	void Pass::usePass()
	{
        // 注意在更新uniform变量的值之前，要先使用着色器glUseProgram
		m_oGLProgram->useProgram();

		// 更新属性
		for (auto itor = m_mUniforms->begin(); itor != m_mUniforms->end(); ++itor)
		{
			itor->second.updateGLProgramUniformValue(m_oGLProgram, itor->first);
		}

	}


}
