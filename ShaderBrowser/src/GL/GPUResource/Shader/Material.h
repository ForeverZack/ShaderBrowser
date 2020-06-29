#ifndef COMS_RENDER_MATERIAL_H
#define COMS_RENDER_MATERIAL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
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
		static Material* createMaterial(const std::string& materialName = DEFAULT_MATERIAL_NAME);
		static Material* create(const MaterialParameters* parameters);
        static Material* createMaterialByProgramName(const std::string& programName, const std::string& materialName);

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
        REGISTER_PROPERTY_GET_SET(unsigned int, m_eQueue, RenderQueue)
		REGISTER_PROPERTY_GET_SET(browser::Camera*, m_oCurCamera, CurCamera)
		REGISTER_PROPERTY_GET_SET(bool, m_bTransformDirty, TransformDirty)
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

        
        // 深度测试
        bool m_bOpenZTest;
        // ...
        
        // 模板测试
        bool m_bStencilTest;
        // ...

        // 混合
        // ...
        
        
        
        // 一些其他属性
        // 当前渲染用的相机
        browser::Camera* m_oCurCamera;
		// Transform脏标记
		bool m_bTransformDirty;
        
        
        
	};
}

#endif
