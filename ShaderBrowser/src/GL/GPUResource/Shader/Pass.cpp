#include "Pass.h"
#include "GL/GPUResource/Texture/Texture2D.h"
#include "GL/GLDefine.h"

namespace customGL
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

	void Pass::usePass(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
	{
        // 注意在更新uniform变量的值之前，要先使用着色器glUseProgram
		m_oGLProgram->useProgram();

		// 更新属性 (TODO: 这一步需要移到GPU命令中去)
		for (auto itor = m_mUniforms->begin(); itor != m_mUniforms->end(); ++itor)
		{
			itor->second.updateGLProgramUniformValue(m_oGLProgram, itor->first);
		}

		// 更新mvp
		if (transformDirty)
		{
			m_oGLProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_MODEL_MATRIX], modelMatrix);
		}
		if (cameraDirty)
		{
			m_oGLProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_VIEW_MATRIX], viewMatrix);
			m_oGLProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_PROJECTION_MATRIX], projectionMatrix);
		}

	}


}
