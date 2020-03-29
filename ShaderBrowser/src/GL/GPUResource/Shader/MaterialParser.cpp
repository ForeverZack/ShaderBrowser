#include "MaterialParser.h"
#include "GL/GPUResource/Shader/GLProgram.h"

namespace customGL
{
	// 初始化内置uniform对应类型
	std::unordered_map<std::string, std::string> MaterialParser::m_mBuiltinUniforms
	{
		{ GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0], "sampler2D" },
		{ GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE1], "sampler2D"},
		{ GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_ALBEDO_COLOR], "vec4"},
	};

	Material* MaterialParser::parseMaterialFileByContent(const std::string& content)
	{
		Material* material = Material::createMaterial();

		Document document;
		document.Parse(content.c_str());
		
	//	std::string json = "{\"vert\": \"xxxx\",     \"frag\": \"yyyy\",     \"uniforms\": {         \"CGL_TEXTURE0\": {             \"type\": \"texture\",             \"value\": \"res/texture\"         },         \"CGL_COLOR\": {             \"type\": \"vec3\",             \"value\": [1,1,1]         }     } }";
	//	rapidjson::Document d;
	//	d.Parse(json.c_str());

	//	bool has = d.HasMember("bbb");
	//	bool has2 = d.HasMember("vert");
	//	rapidjson::Value& val = d["vert"];
	//	bool isStr = val.IsString();
	//	string aaa = val.GetString();
	//	rapidjson::Value& obj = d["uniforms"];
	//	bool isStr2 = obj.IsObject();
	//	bool isArry = obj["CGL_COLOR"]["value"].IsArray();
	//	rapidjson::Value::Array& arr = obj["CGL_COLOR"]["value"].GetArray();

		return material;
	}

}
