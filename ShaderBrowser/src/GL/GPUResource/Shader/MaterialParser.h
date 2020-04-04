#pragma once

#include <string>
#include <unordered_map>
#include "Plugins/rapidjson/document.h"
#include "GL/GPUResource/Shader/Material.h"
#include "GL/GLDefine.h"

using namespace rapidjson;

namespace customGL
{
	struct MaterialUnformParamter
	{
	public:
		std::string name;
		UniformValue::UniformValueType type;
		union U {
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
				std::string path;
			} tex2D;
			struct {
				std::vector<int> vec;
			} intv, ivec2v, ivec3v, ivec4v;
			struct {
				std::vector<float> vec;
			} floatv, v2fv, v3fv, v4fv, mat4v, mat4x3v, mat3v, mat3x4v;

			// �����Union�е���������˹��캯����������������������ͻᷢ�ֳ������ִ�������union����Ķ��������ڴ棬���Բ��ܶ��徲̬���������͵ı�����
			//	������union��Ҳ�������Ŵ��й��캯�������������͸��ƹ��캯���ȵ���Ķ��󣬵��ǿ��Դ�Ŷ�Ӧ�������ָ�롣�������޷���֤��Ĺ��캯�������������õ���ȷ�ĵ��ã��ɴˣ��Ϳ��ܳ����ڴ�й©��
			// ���ԣ���C++��ʹ��unionʱ����������C������ʹ��union�ķ�񣬾�����Ҫ��union���ж����������Ա��ⱨ��
			U() { memset(this, 0, sizeof(*this)); }
			~U() {}
			U& operator=(const U& other) {
				memcpy(this, &other, sizeof(*this));
				return *this;
			}
		} value;
	};

	struct MaterialParameters
	{
		// �������� 
		std::string name;
		// uniforms
		std::vector<MaterialUnformParamter> uniforms;
		// pass
		std::string vert;
		std::string frag;
		std::string vert_program;
		std::string frag_program;
	};

	/*
		Material�ṹ:
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
					"vert": "shader/default/xx.vert",
					"frag": "shader/default/xx.frag",
					"vert_program": "�������",
					"frag_program": "�������",
				},
			],
		}
	*/
    class MaterialParser
	{
	public:
		static MaterialParameters parseMaterialFileByContent(const std::string& content);

	protected:
		// ����uniform��Ӧ����
		static std::unordered_map<std::string, std::string> m_mBuiltinUniforms;
	};
}
