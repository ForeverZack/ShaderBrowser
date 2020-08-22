#ifndef COMS_RENDER_MATERIAL_H
#define COMS_RENDER_MATERIAL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "GL/GPUResource/BaseGPUResource.h"
#include "GL/GPUResource/Shader/Pass.h"
#include "GL/GPUResource/Shader/GLProgram.h"
#include "Common/Tools/Utils.h"
#include "GL/GPUResource/Model/Mesh.h"
#include "Browser/Components/Camera/Camera.h"
#include "Browser/Components/Transform/Transform.h"
#include "Common/CommonDefine.h"
#include "GL/GPUResource/Texture/Texture2D.h"
#include "GL/GPUResource/Texture/RenderTexture.h"
#include "GL/GPUResource/Texture/TextureBuffer.h"

using namespace std;
using namespace browser;

namespace customGL
{
    class MaterialParameters;
    
    class Material : public Reference
	{
	public:
		// 默认材质名称
		static const char* DEFAULT_MATERIAL_NAME;

	public:
		static Material* create(const MaterialParameters* parameters);
        static Material* createMaterialByProgramName(const std::string& materialName, const std::string& programName);

	public:
		Material(const std::string& materialName);
		~Material();

	public:
		// 初始化材质
		void init();
        // 添加pass
        void addPass(Pass* pass);
		// 使用材质的第几个Pass (渲染线程调用)
		// TODO: 这里实际上应该根据pass的类型来决定，而不是根据index ( 这里的类型也跟渲染队列有关，比方说如果在进行阴影深度贴图的渲染，应该使用对应的简单的pass )
		void useMaterial(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms, int index = 0);
		// GLProgram是否在GPU中加载完成
		bool isGPUResourceLoaded();

        // 添加\设置uniform
        void setUniformInt(const std::string& uniformName, int value);
        void setUniformFloat(const std::string& uniformName, float value);
        void setUniformMat3(const std::string& uniformName, const glm::mat3& value);
        void setUniformMat3x4(const std::string& uniformName, const glm::mat3x4& value);
        void setUniformMat4(const std::string& uniformName, const glm::mat4& value);
        void setUniformMat4x3(const std::string& uniformName, const glm::mat4x3& value);
        void setUniformFloatV(const std::string& uniformName, int size, const float* value);
        void setUniformV2f(const std::string& uniformName, const glm::vec2& value);
        void setUniformV3f(const std::string& uniformName, const glm::vec3& value);
        void setUniformV4f(const std::string& uniformName, const glm::vec4& value);
        void setUniformTex2D(const std::string& uniformName, Texture2D* texture);
        void setUniformTex2D(const std::string& uniformName, RenderTexture* texture);
        void setUniformSamplerBuffer(const std::string& uniformName, TextureBuffer* textureBuffer);
        void setUniformIntV(const std::string& uniformName, int count, const int* value);
        void setUniformIVec2(const std::string& uniformName, const glm::ivec2& value);
        void setUniformVec2V(const std::string& uniformName, int count, const float* value);
        void setUniformIVec2V(const std::string& uniformName, int count, const int* value);
        void setUniformIVec3(const std::string& uniformName, const glm::ivec3& value);
        void setUniformVec3V(const std::string& uniformName, int count, const float* value);
        void setUniformIVec3V(const std::string& uniformName, int count, const int* value);
        void setUniformIVec4(const std::string& uniformName, const glm::ivec4& value);
        void setUniformVec4V(const std::string& uniformName, int count, const float* value);
        void setUniformIVec4V(const std::string& uniformName, int count, const int* value);
        void setUniformMat4V(const std::string& uniformName, int count, const float* value);
        void setUniformMat4x3V(const std::string& uniformName, int count, const float* value);
        void setUniformMat3V(const std::string& uniformName, int count, const float* value);
        void setUniformMat3x4V(const std::string& uniformName, int count, const float* value);
        void resetUniformsDirty();  // 将所有UniformValue的dirty标记重置为false
        

		REGISTER_PROPERTY_GET(Pass*, m_pPrePass, PrePass)
		REGISTER_PROPERTY_GET(Pass*, m_pShadowCasterPass, ShadowCasterPass)
        REGISTER_PROPERTY_GET_SET(unsigned int, m_uMaterialId, MaterialId)
		REGISTER_PROPERTY_CONSTREF_GET(std::vector<Pass*>, m_vPass, Pass)
		REGISTER_PROPERTY_CONSTREF_GET(std::string, m_sMaterialName, MaterialName)
		REGISTER_PROPERTY_CONSTREF_GET_SET(std::string, m_sFilePath, FilePath)
        REGISTER_PROPERTY_GET_SET(unsigned int, m_uSharedId, SharedId)
        REGISTER_PROPERTY_GET_SET(bool, m_bDefaultMaterialFlag, DefaultMaterialFlag)
        REGISTER_PROPERTY_GET(unsigned int, m_eQueue, RenderQueue)
		REGISTER_PROPERTY_GET_SET(browser::Camera*, m_oCurCamera, CurCamera)
		REGISTER_PROPERTY_GET_SET(bool, m_bDirty, Dirty)
		REGISTER_PROPERTY_GET_SET(unsigned short, m_uViewMatrixDirtyTag, ViewMatrixDirtyTag)
		REGISTER_PROPERTY_GET_SET(unsigned short, m_uProjectionMatrixDirtyTag, ProjectionMatrixDirtyTag)
		void setRenderQueue(unsigned int queue)
		{
			m_eQueue = queue;
			// 在设置RenderQueue的时候设置是否透明
			m_bTransparentFlag = queue >= RenderQueue::Transparent;
		}
		const std::unordered_map<std::string, UniformValue>& getUniforms() 
		{
			return m_mUniforms;
		}
		size_t getPassCount()
		{
			return m_vPass.size();
		}
		void setPrePass(Pass* pass);
		void setShadowCasterPass(Pass* pass);
		bool isSupportPrePass();
		bool isSupportShadowCaster();

		// depth test
		REGISTER_PROPERTY_SET(bool, m_bEnableZTest, EnableZTest)
		REGISTER_PROPERTY_SET(GLenum, m_eZTestFunc, ZTestFunc)
		REGISTER_PROPERTY_SET(bool, m_bZWrite, ZWrite)
		// stencil test
		void setEnableStencilTest(bool enable);
		void setStencilFuncParameter(GLint ref, GLuint mask);
		void setStencilFuncParameter(GLenum func, GLint ref, GLuint mask);
		void setStencilOpParameter(GLenum sfail, GLenum dpfail, GLenum dppass);
		// blend
		void setEnableBlend(bool enable);
		void setBlendFuncParameter(GLenum sfactor, GLenum dfactor);
		void setBlendFuncParameter(GLenum sfactor, GLenum dfactor, GLenum safactor, GLenum dafactor);

	private:
        // id
        unsigned int m_uMaterialId;
		// 名称
		std::string m_sMaterialName;
		// 对应材质文件绝对路径
		std::string m_sFilePath;
        // 渲染队列
        unsigned int m_eQueue;
        // uniform数据
        std::unordered_map<std::string, UniformValue> m_mUniforms;
        // Pass队列
        std::vector<Pass*> m_vPass;
        // 是否使用gpuInstance
        bool m_bGpuInstance;
        // 共享材质Id  TODO: 如果不是共享材质，也可以从这个id找到对应的共享材质
        unsigned int m_uSharedId;
        // 是否是默认材质
        bool m_bDefaultMaterialFlag;
        // 是否透明
        bool m_bTransparentFlag;

		// 以下是一些特殊Pass:
		// pre-pass
		Pass* m_pPrePass;
		// shadow caster pass
		Pass* m_pShadowCasterPass;

        
		// depth
		// 深度测试开关		开启：glEnable(GL_DEPTH_TEST);		关闭：glDisable(GL_DEPTH_TEST);
		bool m_bEnableZTest;
        // 深度测试方法		glDepthFunc(GL_LESS);
		/*
			函数						描述
			GL_ALWAYS			永远通过深度测试
			GL_NEVER			永远不通过深度测试
			GL_LESS				在片段深度值小于缓冲的深度值时通过测试
			GL_EQUAL			在片段深度值等于缓冲区的深度值时通过测试
			GL_LEQUAL			在片段深度值小于等于缓冲区的深度值时通过测试
			GL_GREATER		在片段深度值大于缓冲区的深度值时通过测试
			GL_NOTEQUAL	在片段深度值不等于缓冲区的深度值时通过测试
			GL_GEQUAL			在片段深度值大于等于缓冲区的深度值时通过测试
		*/
		GLenum m_eZTestFunc;
		// 深度写入		开启：glDepthMask(GL_TRUE);	关闭：glDepthMask(GL_FALSE);
		bool m_bZWrite;
        
		// stencil
        // 模板测试开关
        bool m_bEnableStencilTest;
		// 模板测试方法		glStencilFunc(GLenum func, GLint ref, GLuint mask)
		/*
			一共包含三个参数：
					func：设置模板测试函数(Stencil Test Function)。这个测试函数将会应用到已储存的模板值上和glStencilFunc函数的ref值上。可用的选项有：GL_NEVER、GL_LESS、GL_LEQUAL、GL_GREATER、GL_GEQUAL、GL_EQUAL、GL_NOTEQUAL和GL_ALWAYS。它们的语义和深度缓冲的函数类似。
					ref：设置了模板测试的参考值(Reference Value)。模板缓冲的内容将会与这个值进行比较。
					mask：设置一个掩码，它将会与参考值和储存的模板值在测试比较它们之前进行与(AND)运算。初始情况下所有位都为1。
		*/
		std::shared_ptr<StencilFuncParameter> m_pStencilFuncParam;
		// 模板缓冲更新		glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)
		/*
			一共包含三个选项，我们能够设定每个选项应该采取的行为：
					sfail：模板测试失败时采取的行为。
					dpfail：模板测试通过，但深度测试失败时采取的行为。
					dppass：模板测试和深度测试都通过时采取的行为。

			行为						描述
			GL_KEEP				保持当前储存的模板值
			GL_ZERO				将模板值设置为0
			GL_REPLACE		将模板值设置为glStencilFunc函数设置的ref值
			GL_INCR				如果模板值小于最大值则将模板值加1
			GL_INCR_WRAP	与GL_INCR一样，但如果模板值超过了最大值则归零
			GL_DECR				如果模板值大于最小值则将模板值减1
			GL_DECR_WRAP	与GL_DECR一样，但如果模板值小于0则将其设置为最大值
			GL_INVERT			按位翻转当前的模板缓冲值
		*/
		std::shared_ptr<StencilOpParameter> m_pStencilOpParam;

		// Blend
        // 混合开关
		bool m_bEnableBlend;
		// 混合因子		
		/* 
			OpenGL中的混合是通过下面这个方程来实现的：
					C_result = C_source∗F_source + C_destination∗F_destination
			其中:
					C_source：源颜色向量。这是源自纹理的颜色向量。
					C_destination：目标颜色向量。这是当前储存在颜色缓冲中的颜色向量。
					F_source：源因子值。指定了alpha值对源颜色的影响。
					F_destination：目标因子值。指定了alpha值对目标颜色的影响。

			glBlendFunc(GLenum sfactor, GLenum dfactor)函数接受两个参数，来设置源和目标因子	或  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
			这个函数和我们之前设置的那样设置了RGB分量，但这样只能让最终的alpha分量被源颜色向量的alpha值所影响到。*/
		/*
			选项			值
			GL_ZERO	因子等于0
			GL_ONE	因子等于1
			GL_SRC_COLOR	因子等于源颜色向量C¯source
			GL_ONE_MINUS_SRC_COLOR	因子等于1−C¯source
			GL_DST_COLOR	因子等于目标颜色向量C¯destination
			GL_ONE_MINUS_DST_COLOR	因子等于1−C¯destination
			GL_SRC_ALPHA	因子等于C¯source的alpha分量
			GL_ONE_MINUS_SRC_ALPHA	因子等于1− C¯source的alpha分量
			GL_DST_ALPHA	因子等于C¯destination的alpha分量
			GL_ONE_MINUS_DST_ALPHA	因子等于1− C¯destination的alpha分量
			GL_CONSTANT_COLOR	因子等于常数颜色向量C¯constant
			GL_ONE_MINUS_CONSTANT_COLOR	因子等于1−C¯constant
			GL_CONSTANT_ALPHA	因子等于C¯constant的alpha分量
			GL_ONE_MINUS_CONSTANT_ALPHA	因子等于1− C¯constant的alpha分量
		*/
		std::shared_ptr<BlendFuncParameter> m_pBlendFuncParam;
		// 混合操作(运算符)		glBlendEquation(GLenum mode)允许我们设置运算符，默认选项为GL_FUNC_ADD
		/*
			它提供了三个选项：
				GL_FUNC_ADD：默认选项，将两个分量相加：C_result=Src+Dst。
				GL_FUNC_SUBTRACT：将两个分量相减： C_result=Src−Dst。
				GL_FUNC_REVERSE_SUBTRACT：将两个分量相减，但顺序相反：C_result=Dst−Src。
		*/
		GLenum m_eBlendEquation;
        
        
        
        // 一些其他属性
        // 当前渲染用的相机
        browser::Camera* m_oCurCamera;
		// 材质脏标记	(处理材质修改时，重新传入通用参数)
		bool m_bDirty;
		// view, projection 脏标记 (处理Camera)
		unsigned short m_uViewMatrixDirtyTag;
		unsigned short m_uProjectionMatrixDirtyTag;
        
        
        
	};
}

#endif
