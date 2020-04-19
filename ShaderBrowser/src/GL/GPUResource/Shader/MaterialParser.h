#pragma once

#include <string>
#include <unordered_map>
#include <set>
#include <memory>
#include "Plugins/rapidjson/document.h"
#include "GL/GPUResource/Shader/Material.h"
#include "GL/GLDefine.h"

using namespace rapidjson;

namespace customGL
{
	class MaterialUniformParamter
	{
	public:
		MaterialUniformParamter();
		MaterialUniformParamter(const MaterialUniformParamter& param);
        MaterialUniformParamter(MaterialUniformParamter&& param) noexcept;
		~MaterialUniformParamter();

	public:
		std::string name;
		UniformValue::UniformValueType type;
		union U
        {
			float floatValue;
			glm::vec2 v2f;
			glm::vec3 v3f;
			glm::vec4 v4f;
			int intValue;
			glm::ivec2 ivec2;
			glm::ivec3 ivec3;
			glm::ivec4 ivec4;
			glm::mat3 mat3;
			glm::mat3x4 mat3x4;
			glm::mat4 mat4;
			glm::mat4x3 mat4x3;
			struct {
				const int* pointer;	// 从自身std::vector<int>中取址
				GLsizei count;
			} intv, ivec2v, ivec3v, ivec4v;
			struct {
				const float* pointer;
				GLsizei count;
			} floatv, v2fv, v3fv, v4fv, mat4v, mat4x3v, mat3v, mat3x4v;
			struct {
				const char* path;
			} tex2D;

			// 如果在Union中的类型添加了构造函数，或者添加析构函数，就会发现程序会出现错误。由于union里面的东西共享内存，所以不能定义静态、引用类型的变量。
			//	由于在union里也不允许存放带有构造函数、析构函数和复制构造函数等的类的对象，但是可以存放对应的类对象指针。编译器无法保证类的构造函数和析构函数得到正确的调用，由此，就可能出现内存泄漏。
			// 所以，在C++中使用union时，尽量保持C语言中使用union的风格，尽量不要让union带有对象。这样可以避免报错
			U()
            {
                memset(this, 0, sizeof(*this));
            }
			~U() {}
			U& operator=(const U& other)
            {
				memcpy(this, &other, sizeof(*this));
				return *this;
			}
		} value;
        
        // 是否需要数组数据
        bool checkStringNeed() const;
        bool checkIntVectorNeed() const;
        bool checkFloatVectorNeed() const;
		// 数组数据
		std::string* m_pString;
		std::vector<int>* m_pIntV;
		std::vector<float>* m_pFloatV;
	};
    
    struct MaterialPassParamter
    {
        std::string name;   // program name
//        std::string vert;     // 这两个字段不用，直接读取使用program
//        std::string frag;
        std::string vert_program;
        std::string frag_program;
    };

	class MaterialParameters
	{
    public:
		// 材质名称 
		std::string name;
		// uniforms
		std::vector<MaterialUniformParamter> uniforms;
		// pass
        std::vector<MaterialPassParamter> passes;
        // 所有纹理的路径(方便异步加载)
        std::set<std::string> textures_path;
	};

	/*
		Material结构:
		{
			"name": "Standard",
			"uniforms":
			{
				"CGL_TEXTURE0":
				{
					"type": "sampler2D",
					"value": "res/texture/xxx.png"
				},
				"CGL_ALBEDO_COLOR":
				{
					"type": "vec4",
					"value": [1, 1, 1, 1]
				}
			},
			"pass": [
				{
                    "name": "xxx",
					"vert": "shader/default/xx.vert",   // vert\frag最后会直接读取为vert_program\frag_program
					"frag": "shader/default/xx.frag",
					"vert_program": "代码代码",
					"frag_program": "代码代码",
				},
			],
		}
	*/
    class MaterialParser
	{
	public:
        // 通过材质文件路径解析材质
        static MaterialParameters* parseMaterialFile(const std::string& fullpath, shared_ptr<const char*> extra = nullptr);
        // 通过材质文件内容解析材质
		static MaterialParameters* parseMaterialFileByContent(const std::string& content);
        // 将MaterialUniformParamter设置到Material上
        static void setupMaterialUniforms(const std::vector<MaterialUniformParamter> uniforms, Material* material);

	protected:
		// 内置uniform对应类型
		static std::unordered_map<std::string, std::string> m_mBuiltinUniforms;
        // 字符串类型 转 UniformValueType
        static std::unordered_map<std::string, UniformValue::UniformValueType> m_mStrType2UniformValueType;
        // 未定义GLProgram名称id
        static unsigned long m_uGeneratorId;
	};
}
