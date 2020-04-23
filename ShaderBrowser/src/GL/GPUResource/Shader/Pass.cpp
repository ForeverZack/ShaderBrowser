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
	}

	void Pass::init(GLProgram* program)
	{
        program->retain();
		m_oGLProgram = program;
	}

	bool Pass::isGPUResourceLoaded()
	{
		return m_oGLProgram->getResouceState() == GRS_Loaded;
	}

	void Pass::usePass(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms)
	{
        // 注意在更新uniform变量的值之前，要先使用着色器glUseProgram
		m_oGLProgram->useProgram();

		// 更新属性
		for (auto itor = uniforms.begin(); itor != uniforms.end(); ++itor)
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
            m_oGLProgram->setUniformWithVec3(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_MAINCAMERA_WORLDPOS], cameraGlobalPos.x, cameraGlobalPos.y, cameraGlobalPos.z);
			m_oGLProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_VIEW_MATRIX], viewMatrix);
			m_oGLProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_PROJECTION_MATRIX], projectionMatrix);
		}

	}


}
