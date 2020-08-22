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
		REGISTER_PROPERTY_SET(GLenum, m_eBlendEquation, BlendEquation)

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
		// 深度测试开关
		bool m_bEnableZTest;
        // 深度测试方法
		GLenum m_eZTestFunc;
		// 深度写入
		bool m_bZWrite;
        
		// stencil
        // 模板测试开关
        bool m_bEnableStencilTest;
		// 模板测试方法
		std::shared_ptr<StencilFuncParameter> m_pStencilFuncParam;
		// 模板缓冲更新
		std::shared_ptr<StencilOpParameter> m_pStencilOpParam;

		// Blend
        // 混合开关
		bool m_bEnableBlend;
		// 混合因子		
		std::shared_ptr<BlendFuncParameter> m_pBlendFuncParam;
		// 混合操作(运算符)
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
