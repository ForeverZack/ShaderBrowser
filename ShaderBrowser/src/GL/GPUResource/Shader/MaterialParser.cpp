#include "MaterialParser.h"
#include "Common/Tools/Utils.h"
#include "Common/Tools/FileUtils.h"
#include "Common/System/Cache/TextureCache.h"
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
    
    // 字符串类型 转 UniformValueType
    std::unordered_map<std::string, UniformValue::UniformValueType> MaterialParser::m_mStrType2UniformValueType
    {
        { "sampler2D", UniformValue::UniformValueType::UniformValueType_Sampler2D },
        { "float", UniformValue::UniformValueType::UniformValueType_Float },
        { "vec2", UniformValue::UniformValueType::UniformValueType_Vec2 },
        { "vec3", UniformValue::UniformValueType::UniformValueType_Vec3 },
        { "vec4", UniformValue::UniformValueType::UniformValueType_Vec4 },
        { "int", UniformValue::UniformValueType::UniformValueType_Int },
        { "ivec2", UniformValue::UniformValueType::UniformValueType_IVec2 },
        { "ivec3", UniformValue::UniformValueType::UniformValueType_IVec3 },
        { "ivec4", UniformValue::UniformValueType::UniformValueType_IVec4 },
        { "mat4", UniformValue::UniformValueType::UniformValueType_Mat4 },
        { "mat4x3", UniformValue::UniformValueType::UniformValueType_Mat4x3 },
        { "mat3", UniformValue::UniformValueType::UniformValueType_Mat3 },
        { "mat3x4", UniformValue::UniformValueType::UniformValueType_Mat3x4 },
        { "float[]", UniformValue::UniformValueType::UniformValueType_FloatV },
        { "int[]", UniformValue::UniformValueType::UniformValueType_IntV },
    };


	MaterialUniformParamter::MaterialUniformParamter()
		: m_pString(nullptr)
		, m_pIntV(nullptr)
		, m_pFloatV(nullptr)
	{
	}

	MaterialUniformParamter::MaterialUniformParamter(const MaterialUniformParamter& param)
	{
		name = param.name;
		type = param.type;
		value = param.value;
		// 复制指针数据
		if (param.checkStringNeed() && param.m_pString)
		{
			m_pString = new std::string(*param.m_pString);
		}
		if (param.checkIntVectorNeed() && param.m_pIntV && param.m_pIntV->size() > 0)
		{
			m_pIntV = new std::vector<int>(&(*param.m_pIntV)[0], &(*param.m_pIntV)[0] + param.m_pIntV->size());
		}
		if (param.checkFloatVectorNeed() && param.m_pFloatV && param.m_pFloatV->size() > 0)
		{
			m_pFloatV = new std::vector<float>(&(*param.m_pFloatV)[0], &(*param.m_pFloatV)[0] + param.m_pFloatV->size());
		}
	}

	MaterialUniformParamter::MaterialUniformParamter(MaterialUniformParamter&& param) noexcept
	{
		name = param.name;
		type = param.type;
		value = param.value;
		// 接管指针数据
		if (param.checkStringNeed() && param.m_pString)
		{
			m_pString = param.m_pString;
			param.m_pString = nullptr;
		}
        if (param.checkIntVectorNeed() && param.m_pIntV)
		{
			m_pIntV = param.m_pIntV;
			param.m_pIntV = nullptr;
		}
        if (param.checkFloatVectorNeed() && param.m_pFloatV)
		{
			m_pFloatV = param.m_pFloatV;
			param.m_pFloatV = nullptr;
		}
	}

	MaterialUniformParamter::~MaterialUniformParamter()
	{
        if (checkStringNeed())
        {
            BROWSER_SAFE_RELEASE_POINTER(m_pString);
        }
		if (checkIntVectorNeed())
        {
            BROWSER_SAFE_RELEASE_POINTER(m_pIntV);
        }
		if (checkFloatVectorNeed())
        {
            BROWSER_SAFE_RELEASE_POINTER(m_pFloatV);
        }
	}
    
    bool MaterialUniformParamter::checkStringNeed() const
    {
        return type == UniformValue::UniformValueType::UniformValueType_Sampler2D;
    }
    
    bool MaterialUniformParamter::checkIntVectorNeed() const
    {
        return type==UniformValue::UniformValueType::UniformValueType_IntV
            || type==UniformValue::UniformValueType::UniformValueType_IVec2V
            || type==UniformValue::UniformValueType::UniformValueType_IVec3V
            || type==UniformValue::UniformValueType::UniformValueType_IVec4V;
    }
    
    bool MaterialUniformParamter::checkFloatVectorNeed() const
    {
        return type==UniformValue::UniformValueType::UniformValueType_FloatV
            || type==UniformValue::UniformValueType::UniformValueType_Vec2V
            || type==UniformValue::UniformValueType::UniformValueType_Vec3V
            || type==UniformValue::UniformValueType::UniformValueType_Vec4V
            || type==UniformValue::UniformValueType::UniformValueType_Mat4V
            || type==UniformValue::UniformValueType::UniformValueType_Mat4x3V
            || type==UniformValue::UniformValueType::UniformValueType_Mat3V
            || type==UniformValue::UniformValueType::UniformValueType_Mat3x4V;
    }
    
    unsigned long MaterialParser::m_uGeneratorId = 0;
    
    MaterialParameters* MaterialParser::parseMaterialFile(const std::string& filepath, shared_ptr<const char*> extra/* = nullptr*/)
    {
		std::string directory;
		const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(filepath, directory);
		std::string content = Utils::readAbsolutePathFile(full_path.c_str());
		return MaterialParser::parseMaterialFileByContent(content, directory);
    }
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
					"vert": "shader/default/xx.vert",
					"frag": "shader/default/xx.frag",
					"vert_program": "代码代码",
					"frag_program": "代码代码",
				},
			],
		}
	*/
	MaterialParameters* MaterialParser::parseMaterialFileByContent(const std::string& content, const std::string& directory)
	{
		MaterialParameters* parameters = new MaterialParameters();
		parameters->directory = directory;

		Document document;
		document.Parse(content.c_str());
        
        BROWSER_ASSERT(!document.HasParseError(), "Material file parse error in function MaterialParser::parseMaterialFileByContent(const std::string& content)");

		// name
		if (document.HasMember("name"))
		{
			BROWSER_ASSERT(document["name"].IsString(), "Material's name must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
			parameters->name = document["name"].GetString();
		}

		// uniforms
		if (document.HasMember("uniforms"))
		{
            rapidjson::Value& uniforms = document["uniforms"];
            std::string strType;
            for(auto itor=uniforms.MemberBegin(); itor!=uniforms.MemberEnd(); ++itor)
            {
                rapidjson::Value& uniformName = itor->name;
                rapidjson::Value& uniformValue = itor->value;
                
                MaterialUniformParamter uniformParam;
                
                // name
                {
                    BROWSER_ASSERT(uniformName.IsString(), "Uniform's name must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    uniformParam.name = uniformName.GetString();
                }
                
                // type
                {
                    strType = "";
                    if (uniformValue.HasMember("type"))
                    {
                        BROWSER_ASSERT(uniformValue["type"].IsString(), "Uniform's type must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                        strType = uniformValue["type"].GetString();
                    }
                    // 检测是否是内置变量
                    if (m_mBuiltinUniforms.find(uniformParam.name) != m_mBuiltinUniforms.end())
                    {
                        BROWSER_ASSERT(strType!="" || m_mBuiltinUniforms[uniformParam.name]==strType, "Found builtin uniform type not right in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                        strType = m_mBuiltinUniforms[uniformParam.name];
                    }
                    BROWSER_ASSERT(m_mStrType2UniformValueType.find(strType)!=m_mStrType2UniformValueType.end(), "Cannot find the uniform value type in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    uniformParam.type = m_mStrType2UniformValueType[strType];
                }

                // value
                {
                    BROWSER_ASSERT(uniformValue.HasMember("value"), "Uniform's value is empty in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    rapidjson::Value& value = uniformValue["value"];
                    switch (uniformParam.type)
                    {
                    case UniformValue::UniformValueType::UniformValueType_Float:
                        {
                            BROWSER_ASSERT(value.IsFloat(), "Uniform's value is not float (type float) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            uniformParam.value.floatValue = value.GetFloat();
                        }
                        break;
                    
                    case UniformValue::UniformValueType::UniformValueType_Vec2:
                        {
                            BROWSER_ASSERT(value.IsArray() && value.GetArray().Size()>=2, "Uniform's value is not array (type vec2) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            const rapidjson::Value::Array& var_array = value.GetArray();
                            uniformParam.value.v2f = glm::vec2(var_array[0].GetFloat(), var_array[1].GetFloat());
                        }
                        break;
                            
                    case UniformValue::UniformValueType::UniformValueType_Vec3:
                        {
                            BROWSER_ASSERT(value.IsArray() && value.GetArray().Size()>=3, "Uniform's value is not array (type vec3) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            const rapidjson::Value::Array& var_array = value.GetArray();
                            uniformParam.value.v3f = glm::vec3(var_array[0].GetFloat(), var_array[1].GetFloat(), var_array[2].GetFloat());
                        }
                        break;
                            
                    case UniformValue::UniformValueType::UniformValueType_Vec4:
                        {
                            BROWSER_ASSERT(value.IsArray() && value.GetArray().Size()>=4, "Uniform's value is not array (type vec4) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            const rapidjson::Value::Array& var_array = value.GetArray();
                            uniformParam.value.v4f = glm::vec4(var_array[0].GetFloat(), var_array[1].GetFloat(), var_array[2].GetFloat(), var_array[3].GetFloat());
                        }
                        break;
                            
                    case UniformValue::UniformValueType::UniformValueType_Int:
                        {
                            BROWSER_ASSERT(value.IsInt(), "Uniform's value is not int (type int) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            uniformParam.value.intValue = value.GetInt();
                        }
                        break;
                            
                    case UniformValue::UniformValueType::UniformValueType_IVec2:
                        {
                            BROWSER_ASSERT(value.IsArray() && value.GetArray().Size()>=2, "Uniform's value is not array (type ivec2) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            const rapidjson::Value::Array& var_array = value.GetArray();
                            uniformParam.value.ivec2 = glm::ivec2(var_array[0].GetInt(), var_array[1].GetInt());
                        }
                        break;
                            
                    case UniformValue::UniformValueType::UniformValueType_IVec3:
                        {
                            BROWSER_ASSERT(value.IsArray() && value.GetArray().Size()>=3, "Uniform's value is not array (type ivec3) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            const rapidjson::Value::Array& var_array = value.GetArray();
                            uniformParam.value.ivec3 = glm::ivec3(var_array[0].GetInt(), var_array[1].GetInt(), var_array[2].GetInt());
                        }
                        break;
                            
                    case UniformValue::UniformValueType::UniformValueType_IVec4:
                        {
                            BROWSER_ASSERT(value.IsArray() && value.GetArray().Size()>=4, "Uniform's value is not array (type ivec4) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            const rapidjson::Value::Array& var_array = value.GetArray();
                            uniformParam.value.ivec4 = glm::ivec4(var_array[0].GetInt(), var_array[1].GetInt(), var_array[2].GetInt(), var_array[3].GetInt());
                        }
                        break;
                            
                    case UniformValue::UniformValueType::UniformValueType_Sampler2D:
                        {
                            BROWSER_ASSERT(value.IsString(), "Uniform's value is not string (type sampler2D) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
							BROWSER_SAFE_RELEASE_POINTER(uniformParam.m_pString);
							uniformParam.m_pString = new std::string(value.GetString());
							*(uniformParam.m_pString) = FileUtils::getInstance()->tryGetAbsolutePathForFilename(*(uniformParam.m_pString), directory);
                            uniformParam.value.tex2D.path = &(*uniformParam.m_pString)[0];
							BROWSER_ASSERT(FileUtils::getInstance()->isDirectoryOrFileExist(*uniformParam.m_pString), "Uniform's texture file is not found in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            parameters->textures_path.push_back(*(uniformParam.m_pString));
                        }
                        break;
                            
                    case UniformValue::UniformValueType::UniformValueType_Mat4:
                        {
                            BROWSER_ASSERT(value.IsArray() && value.GetArray().Size()>=16, "Uniform's value is not array (type mat4) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            const rapidjson::Value::Array& var_array = value.GetArray();
                            uniformParam.value.mat4 = glm::mat4(var_array[0].GetFloat(), var_array[4].GetFloat(), var_array[8].GetFloat(), var_array[12].GetFloat(),
                                                                var_array[1].GetFloat(), var_array[5].GetFloat(), var_array[9].GetFloat(), var_array[13].GetFloat(),
                                                                var_array[2].GetFloat(), var_array[6].GetFloat(), var_array[10].GetFloat(), var_array[14].GetFloat(),
                                                                var_array[3].GetFloat(), var_array[7].GetFloat(), var_array[11].GetFloat(), var_array[15].GetFloat());
                        }
                        break;
                    
                    case UniformValue::UniformValueType::UniformValueType_Mat4x3:
                        {
                            BROWSER_ASSERT(value.IsArray() && value.GetArray().Size()>=12, "Uniform's value is not array (type mat4x3) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            const rapidjson::Value::Array& var_array = value.GetArray();
                            uniformParam.value.mat4x3 = glm::mat4x3(var_array[0].GetFloat(), var_array[3].GetFloat(), var_array[6].GetFloat(), var_array[9].GetFloat(),
                                                                    var_array[1].GetFloat(), var_array[4].GetFloat(), var_array[7].GetFloat(), var_array[10].GetFloat(),
                                                                    var_array[2].GetFloat(), var_array[5].GetFloat(), var_array[8].GetFloat(), var_array[11].GetFloat());
                        }
                        break;
                            
                    case UniformValue::UniformValueType::UniformValueType_Mat3:
                        {
                            BROWSER_ASSERT(value.IsArray() && value.GetArray().Size()>=9, "Uniform's value is not array (type mat3) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                            const rapidjson::Value::Array& var_array = value.GetArray();
                            uniformParam.value.mat3 = glm::mat3(var_array[0].GetFloat(), var_array[3].GetFloat(), var_array[6].GetFloat(),
                                                                var_array[1].GetFloat(), var_array[4].GetFloat(), var_array[7].GetFloat(),
                                                                var_array[2].GetFloat(), var_array[5].GetFloat(), var_array[8].GetFloat());
                        }
                        break;
                                                                
					case UniformValue::UniformValueType::UniformValueType_Mat3x4:
						{
							BROWSER_ASSERT(value.IsArray() && value.GetArray().Size() >= 12, "Uniform's value is not array (type mat3x4) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
							const rapidjson::Value::Array& var_array = value.GetArray();
							uniformParam.value.mat3x4 = glm::mat3x4(var_array[0].GetFloat(), var_array[4].GetFloat(), var_array[8].GetFloat(),
								var_array[1].GetFloat(), var_array[5].GetFloat(), var_array[9].GetFloat(),
								var_array[2].GetFloat(), var_array[6].GetFloat(), var_array[10].GetFloat(),
								var_array[3].GetFloat(), var_array[7].GetFloat(), var_array[11].GetFloat());
						}
						break;

					case UniformValue::UniformValueType::UniformValueType_FloatV:
					{
						BROWSER_ASSERT(value.IsArray(), "Uniform's value is not array (type float[]) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
						const rapidjson::Value::Array& var_array = value.GetArray();
						BROWSER_SAFE_RELEASE_POINTER(uniformParam.m_pFloatV);
						uniformParam.m_pFloatV = new std::vector<float>();
						uniformParam.m_pFloatV->reserve(var_array.Size());
						for (int i = 0; i < var_array.Size(); ++i)
						{
							uniformParam.m_pFloatV->push_back(var_array[i].GetFloat());
						}
						uniformParam.value.floatv.pointer = &(*uniformParam.m_pFloatV)[0];
						uniformParam.value.floatv.count = var_array.Size();
					}
					break;

					case UniformValue::UniformValueType::UniformValueType_IntV:
					{
						BROWSER_ASSERT(value.IsArray(), "Uniform's value is not array (type int[]) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
						const rapidjson::Value::Array& var_array = value.GetArray();
						BROWSER_SAFE_RELEASE_POINTER(uniformParam.m_pIntV);
						uniformParam.m_pIntV = new std::vector<int>();
						uniformParam.m_pIntV->reserve(var_array.Size());
						for (int i = 0; i < var_array.Size(); ++i)
						{
							uniformParam.m_pIntV->push_back(var_array[i].GetInt());
						}
						uniformParam.value.intv.pointer = &(*uniformParam.m_pIntV)[0];
						uniformParam.value.intv.count = var_array.Size();
					}
					break;
                                                                
                    }
                }
            
                parameters->uniforms.push_back(std::move(uniformParam));
            }
            
		}
        
        // pass
        BROWSER_ASSERT(document.HasMember("pass"), "Matieral must has 'pass' parameter, please check your program in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
        {
            rapidjson::Value& pass = document["pass"];
            BROWSER_ASSERT(pass.IsArray() && pass.GetArray().Size()>0, "Material's parameter pass size must bigger than 0, please check your program in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
            const rapidjson::Value::Array& passes = pass.GetArray();
            parameters->passes.reserve(passes.Size());
            for(int i=0; i<passes.Size(); ++i)
            {
                MaterialPassParamter passParam;
                
                rapidjson::Value& one_pass = passes[i];
                
                // glprogram name
                if (one_pass.HasMember("name"))
                {
                    BROWSER_ASSERT(one_pass["name"].IsString(), "Pass's name is not a string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    passParam.name = one_pass["name"].GetString();
                }
                else
                {
                    passParam.name = "UndefineGLProgram_"+std::to_string(m_uGeneratorId++);
                }
                // vertex shader
                if (one_pass.HasMember("vert"))
                {
                    BROWSER_ASSERT(one_pass["vert"].IsString(), "Pass's vert is not a string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    std::string vert_path = one_pass["vert"].GetString();
					const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(vert_path);
					BROWSER_ASSERT(FileUtils::getInstance()->isDirectoryOrFileExist(full_path), "Pass's vert file is not found in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					passParam.vert_program = Utils::readAbsolutePathFile(full_path.c_str());
                }
                else if (one_pass.HasMember("vert_program"))
                {
                    BROWSER_ASSERT(one_pass["vert_program"].IsString(), "Pass's vert_program is not a string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    passParam.vert_program = one_pass["vert_program"].GetString();
                }
				GLProgram::convertSourceCodeInclude(passParam.vert_program);	// 替换shader中的inc

                // fragment shader
                if (one_pass.HasMember("frag"))
                {
                    BROWSER_ASSERT(one_pass["frag"].IsString(), "Pass's frag is not a string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    std::string frag_path = one_pass["frag"].GetString();
					const std::string full_path = FileUtils::getInstance()->getAbsolutePathForFilename(frag_path);
					BROWSER_ASSERT(FileUtils::getInstance()->isDirectoryOrFileExist(full_path), "Pass's frag file is not found in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    passParam.frag_program = Utils::readFile(full_path.c_str());
                }
                else if (one_pass.HasMember("frag_program"))
                {
                    BROWSER_ASSERT(one_pass["frag_program"].IsString(), "Pass's frag_program is not a string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    passParam.frag_program = one_pass["frag_program"].GetString();
                }
				GLProgram::convertSourceCodeInclude(passParam.frag_program);	// 替换shader中的inc

                parameters->passes.push_back(std::move(passParam));
            }
        }

		return parameters;
	}
    
    void MaterialParser::setupMaterialUniforms(const std::vector<MaterialUniformParamter> uniforms, Material* material)
    {
        for (auto itor=uniforms.begin(); itor!=uniforms.end(); ++itor)
        {
            const MaterialUniformParamter& uniformParam = *itor;
            switch(uniformParam.type)
            {
                case UniformValue::UniformValueType::UniformValueType_Float:
                    {
                        material->setUniformFloat(uniformParam.name, uniformParam.value.floatValue);
                    }
                    break;
                
                case UniformValue::UniformValueType::UniformValueType_Vec2:
                    {
                        material->setUniformV2f(uniformParam.name, uniformParam.value.v2f);
                    }
                    break;
                        
                case UniformValue::UniformValueType::UniformValueType_Vec3:
                    {
                        material->setUniformV3f(uniformParam.name, uniformParam.value.v3f);
                    }
                    break;
                        
                case UniformValue::UniformValueType::UniformValueType_Vec4:
                    {
                        material->setUniformV4f(uniformParam.name, uniformParam.value.v4f);
                    }
                    break;
                        
                case UniformValue::UniformValueType::UniformValueType_Int:
                    {
                        material->setUniformInt(uniformParam.name, uniformParam.value.intValue);
                    }
                    break;
                        
                case UniformValue::UniformValueType::UniformValueType_IVec2:
                    {
                        material->setUniformIVec2(uniformParam.name, uniformParam.value.ivec2);
                    }
                    break;
                        
                case UniformValue::UniformValueType::UniformValueType_IVec3:
                    {
                        material->setUniformIVec3(uniformParam.name, uniformParam.value.ivec3);
                    }
                    break;
                        
                case UniformValue::UniformValueType::UniformValueType_IVec4:
                    {
                        material->setUniformIVec4(uniformParam.name, uniformParam.value.ivec4);
                    }
                    break;
                        
                case UniformValue::UniformValueType::UniformValueType_Sampler2D:
                    {
                        Texture2D* texture = TextureCache::getInstance()->getTexture(uniformParam.value.tex2D.path);
                        material->setUniformTex2D(uniformParam.name, texture);
                    }
                    break;
                        
                case UniformValue::UniformValueType::UniformValueType_Mat4:
                    {
                        material->setUniformMat4(uniformParam.name, uniformParam.value.mat4);
                    }
                    break;
                
                case UniformValue::UniformValueType::UniformValueType_Mat4x3:
                    {
                        material->setUniformMat4x3(uniformParam.name, uniformParam.value.mat4x3);
                    }
                    break;
                        
                case UniformValue::UniformValueType::UniformValueType_Mat3:
                    {
                        material->setUniformMat3(uniformParam.name, uniformParam.value.mat3);
                    }
                    break;
                                                            
                case UniformValue::UniformValueType::UniformValueType_Mat3x4:
                    {
                        material->setUniformMat3x4(uniformParam.name, uniformParam.value.mat3x4);
                    }
                    break;

                case UniformValue::UniformValueType::UniformValueType_FloatV:
                {
                    material->setUniformFloatV(uniformParam.name, uniformParam.value.floatv.count, uniformParam.value.floatv.pointer);
                }
                break;

                case UniformValue::UniformValueType::UniformValueType_IntV:
                {
                    material->setUniformIntV(uniformParam.name, uniformParam.value.intv.count, uniformParam.value.intv.pointer);
                }
                break;
            }
        }
    }

}
