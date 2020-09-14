#pragma once

#include "GL/GPUResource/Shader/GLProgram.h"
#include "Common/Tools/BaseSingleton.h"

using namespace common;

namespace customGL
{
    class GLStateCache : public BaseSingleton<GLStateCache>
	{
	public:
		GLStateCache();
		~GLStateCache();
        
    public:
		// 刷新 (这里刷新，主要是因为引用了imgui插件，其中绑定了字符集纹理，会破坏本系统的逻辑，所以每帧更新一次，清空记录的纹理)
		void update(float deltaTime);
        // 绑定纹理到opengl
        void bindTexture2DN(GLuint textureUnit, GLuint textureId);
		void bindSamplerBuffer(GLuint textureUnit, GLuint textureId);
		void bindImageBuffer(GLuint textureUnit, GLuint textureId, GLenum access, GLenum format);
        
        // 设置深度测试
        void setZTest(bool enable, GLenum depthFunc = GL_LESS, GLenum zwrite = GL_TRUE);
		// 设置面剔除
		void setCull(bool enable, GLenum cullFace = GL_BACK, GLenum frontFace = GL_CCW);
		// 设置模板测试
		void setStencilEnable(bool enable);
		void setStencilFuncParameter(GLenum func, GLint ref, GLuint mask);
		void setStencilOpParameter(GLenum sfail, GLenum dpfail, GLenum dppass);
		// 设置混合
		void setBlendEnalbe(bool enable);
		void setBlendFuncParameter(GLenum sfactor, GLenum dfactor, GLenum safactor, GLenum dafactor);
		void setBlendEquation(GLenum equation);

    private:
        // textureId队列
        GLuint m_vTexIds[MAX_ACTIVE_TEXTURE];

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

		// cull
		// 面剔除开关
		bool m_bEnableCull;
		// 剔除面的类型	glCullFace(GL_FRONT);
		/*
			GL_BACK：只剔除背向面。
			GL_FRONT：只剔除正向面。
			GL_FRONT_AND_BACK：剔除正向面和背向面。
		*/
		GLenum m_eCullFace;
		// 正面是顺时针还是逆时针 (默认值是逆时针)		glFrontFace(GL_CCW);
		/*
			GL_CCW: 逆时针
			GL_CW: 顺时针
		*/
		GLenum m_eFrontFace;


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
		StencilFuncParameter m_oStencilFuncParam;
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
		StencilOpParameter m_oStencilOpParam;


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
		BlendFuncParameter m_oBlendFuncParam;
		// 混合操作(运算符)		glBlendEquation(GLenum mode)允许我们设置运算符，默认选项为GL_FUNC_ADD
		/*
		它提供了三个选项：
		GL_FUNC_ADD：默认选项，将两个分量相加：C_result=Src+Dst。
		GL_FUNC_SUBTRACT：将两个分量相减： C_result=Src−Dst。
		GL_FUNC_REVERSE_SUBTRACT：将两个分量相减，但顺序相反：C_result=Dst−Src。
		*/
		GLenum m_eBlendEquation;
        
	};
}

