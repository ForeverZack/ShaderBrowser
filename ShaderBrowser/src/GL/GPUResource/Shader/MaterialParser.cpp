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
		{ GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE0], "sampler2D" },	// 纹理0 (主纹理)
		{ GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_TEXUTRE1], "sampler2D"},	// 纹理1 (附加纹理)
		{ GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_NORMALMAP], "sampler2D" },	// 法线贴图
		{ GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_ALBEDO_COLOR], "vec4"},	// albedo颜色
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

	// 字符串转OpenGL内部枚举
	std::unordered_map<std::string, GLenum> MaterialParser::m_mString2GLenum
	{
		{ "GL_NONE", GL_NONE },	// None
		// texture
		{ "GL_REPEAT", GL_REPEAT },	// 对纹理的默认行为。重复纹理图像。
		{ "GL_MIRRORED_REPEAT", GL_MIRRORED_REPEAT },	// 和GL_REPEAT一样，但每次重复图片是镜像放置的。
		{ "GL_CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE },	// 纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
		{ "GL_CLAMP_TO_BORDER", GL_CLAMP_TO_BORDER },	//超出的坐标为用户指定的边缘颜色。
		{ "GL_NEAREST", GL_NEAREST },	// GL_NEAREST（也叫邻近过滤，Nearest Neighbor Filtering）是OpenGL默认的纹理过滤方式。当设置为GL_NEAREST的时候，OpenGL会选择中心点最接近纹理坐标的那个像素。
		{ "GL_LINEAR", GL_LINEAR },	// GL_LINEAR（也叫线性过滤，(Bi)linear Filtering）它会基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色。一个纹理像素的中心距离纹理坐标越近，那么这个纹理像素的颜色对最终的样本颜色的贡献越大。
		{ "GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST },	// 使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
		{ "GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST },	// 使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
		{ "GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR },	// 在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
		{ "GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR },	// 在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
		// z test
		{ "GL_ALWAYS", GL_ALWAYS },	// 永远通过深度测试
		{ "GL_NEVER", GL_NEVER },	// 永远不通过深度测试
		{ "GL_LESS", GL_LESS },	// 在片段深度值小于缓冲的深度值时通过测试
		{ "GL_EQUAL", GL_EQUAL },	// 在片段深度值等于缓冲区的深度值时通过测试
		{ "GL_LEQUAL", GL_LEQUAL },	// 在片段深度值小于等于缓冲区的深度值时通过测试
		{ "GL_GREATER", GL_GREATER },	// 在片段深度值大于缓冲区的深度值时通过测试
		{ "GL_NOTEQUAL", GL_NOTEQUAL },	// 在片段深度值不等于缓冲区的深度值时通过测试
		{ "GL_GEQUAL", GL_GEQUAL },	// 在片段深度值大于等于缓冲区的深度值时通过测试
		// cull
		{ "GL_BACK", GL_BACK },	// 只剔除背向面
		{ "GL_FRONT", GL_FRONT },	// 只剔除正向面
		{ "GL_FRONT_AND_BACK", GL_FRONT_AND_BACK },	// 剔除正向面和背向面
		{ "GL_CCW", GL_CCW },	// 逆时针
		{ "GL_CW", GL_CW },	// 顺时针
		// stencil
		{ "GL_KEEP", GL_KEEP },	// 保持当前储存的模板值
		{ "GL_ZERO", GL_ZERO },	// 将模板值设置为0
		{ "GL_REPLACE", GL_REPLACE },	// 将模板值设置为glStencilFunc函数设置的ref值
		{ "GL_INCR", GL_INCR },	// 如果模板值小于最大值则将模板值加1
		{ "GL_INCR_WRAP", GL_INCR_WRAP },	// 与GL_INCR一样，但如果模板值超过了最大值则归零
		{ "GL_DECR", GL_DECR },	// 如果模板值大于最小值则将模板值减1
		{ "GL_DECR_WRAP", GL_DECR_WRAP },	// 与GL_DECR一样，但如果模板值小于0则将其设置为最大值
		{ "GL_INVERT", GL_INVERT },	// 按位翻转当前的模板缓冲值
		// blend		C_result = C_source∗F_source + C_destination∗F_destination
		{ "GL_ZERO", GL_ZERO },	// 因子等于0
		{ "GL_ONE", GL_ONE },	// 因子等于1
		{ "GL_SRC_COLOR", GL_SRC_COLOR },	// 因子等于源颜色向量C¯source
		{ "GL_ONE_MINUS_SRC_COLOR", GL_ONE_MINUS_SRC_COLOR },	// 因子等于1−C¯source
		{ "GL_DST_COLOR", GL_DST_COLOR },	// 因子等于目标颜色向量C¯destination
		{ "GL_ONE_MINUS_DST_COLOR", GL_ONE_MINUS_DST_COLOR },	// 因子等于1−C¯destination
		{ "GL_SRC_ALPHA", GL_SRC_ALPHA },	// 因子等于C¯source的alpha分量
		{ "GL_ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA },	// 因子等于1− C¯source的alpha分量
		{ "GL_DST_ALPHA", GL_DST_ALPHA },	// 因子等于C¯destination的alpha分量
		{ "GL_ONE_MINUS_DST_ALPHA", GL_ONE_MINUS_DST_ALPHA },	// 因子等于1− C¯destination的alpha分量
		{ "GL_CONSTANT_COLOR", GL_CONSTANT_COLOR },	// 因子等于常数颜色向量C¯constant
		{ "GL_ONE_MINUS_CONSTANT_COLOR", GL_ONE_MINUS_CONSTANT_COLOR },	// 因子等于1−C¯constant
		{ "GL_CONSTANT_ALPHA", GL_CONSTANT_ALPHA },	// 因子等于C¯constant的alpha分量
		{ "GL_ONE_MINUS_CONSTANT_ALPHA", GL_ONE_MINUS_CONSTANT_ALPHA },	// 因子等于1− C¯constant的alpha分量
		{ "GL_FUNC_ADD", GL_FUNC_ADD },	// 默认选项，将两个分量相加：C_result = Src + Dst。
		{ "GL_FUNC_SUBTRACT", GL_FUNC_SUBTRACT },	// 将两个分量相减： C_result = Src−Dst。
		{ "GL_FUNC_REVERSE_SUBTRACT", GL_FUNC_REVERSE_SUBTRACT },	// 将两个分量相减，但顺序相反：C_result = Dst−Src。
	};

	// 字符串转RenderQueue
	std::unordered_map<std::string, RenderQueue> MaterialParser::m_mString2RenderQueue
	{
		{ "Background", RenderQueue::Background },
		{ "SkyBox", RenderQueue::SkyBox },
		{ "Opaque", RenderQueue::Opaque },
		{ "Transparent", RenderQueue::Transparent },
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
			"renderQueue": 1000,		// 或 "renderQueue": "Transparent",

			// 深度测试
			"ztest": 
			{
				"enable": true,
				"func": "GL_LESS"
			},
			"ztest": false,
			"zwrite": true,
			// 裁切
			"cull": 
			{
				"enable": true,
				"cullface": "GL_BACK",
				"frontface": "GL_CCW"
			},
			"cull": 
			{
				"enable": true,
				"cullface": "GL_BACK"
			},
			"cull": false,
			// 模板
			"stencil": 
			{
				"enable": true,
				"func": "GL_EQUAL",
				"ref": 1,
				"mask": 255,
				"sfail": "GL_KEEP",
				"dpfail": "GL_KEEP",
				"dppass": "GL_REPLACE"
			},
			// 混合
			"blend":
			{
				"enable": true,
				"sfactor": "GL_SRC_ALPHA",
				"dfactor": "GL_ONE_MINUS_SRC_ALPHA",
				"safactor": "GL_SRC_ALPHA",
				"dafactor": "GL_ONE_MINUS_SRC_ALPHA",
				"equation": "GL_FUNC_ADD"
			},
			"blend":
			{
				"enable": true,
				"sfactor": "GL_SRC_ALPHA",
				"dfactor": "GL_ONE_MINUS_SRC_ALPHA"
			},
			"blend": true,


			"uniforms": 
			{
				"CGL_TEXTURE0": 
				{             
					"type": "sampler2D",             // 内置参数(m_mBuiltinUniforms)可省略type
					"wrap": "GL_REPEAT",	// 纹理环绕方式，默认为GL_CLAMP_TO_EDGE
					"filter": "GL_LINEAR",		// 纹理过滤方式，默认为GL_LINEAR
					"value": "res/texture/xxx.png"			// 或者缩写 "xxx.png"(当前目录下)  或 "./xxx.png"
				},
				"CGL_ALBEDO_COLOR":
				{
					"type": "vec4",
					"value": [1, 1, 1, 1]
				}
			},
			"pass": [
				{
					"name": "Pass1",	// 省略会生成"UndefineGLProgram_XXX"
					"vert": "shader/default/xx.vert",		 // vert\frag最后会直接读取为vert_program\frag_program
					"frag": "shader/default/xx.frag",	
					"vert_program": "代码代码",
					"frag_program": "代码代码",
					// 深度测试
					"ztest":
					{
					"enable": true,
					"func": "GL_LESS"
					},
					"ztest": false,
					"zwrite": true,
					// 裁切
					"cull":
					{
					"enable": true,
					"cullface": "GL_BACK",
					"frontface": "GL_CCW"
					},
					"cull":
					{
					"enable": true,
					"cullface": "GL_BACK"
					},
					"cull": false,
					// 模板
					"stencil":
					{
					"enable": true,
					"func": "GL_EQUAL",
					"ref": 1,
					"mask": 255,
					"sfail": "GL_KEEP",
					"dpfail": "GL_KEEP",
					"dppass": "GL_REPLACE"
					},
					// 混合
					"blend":
					{
					"enable": true,
					"sfactor": "GL_SRC_ALPHA",
					"dfactor": "GL_ONE_MINUS_SRC_ALPHA",
					"safactor": "GL_SRC_ALPHA",
					"dafactor": "GL_ONE_MINUS_SRC_ALPHA",
					"equation": "GL_FUNC_ADD"
					},
					"blend":
					{
					"enable": true,
					"sfactor": "GL_SRC_ALPHA",
					"dfactor": "GL_ONE_MINUS_SRC_ALPHA"
					},
					"blend": true,
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

		// renderQueue
		if (document.HasMember("renderQueue"))
		{
			if (document["renderQueue"].IsString())
			{
				const std::string& strRenderQueue = document["renderQueue"].GetString();
				BROWSER_ASSERT(m_mString2RenderQueue.find(strRenderQueue) != m_mString2RenderQueue.end(), "Material's renderQueue is not declared in RenderQueue in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				parameters->renderQueue = m_mString2RenderQueue[strRenderQueue];
			}
			else
			{
				BROWSER_ASSERT(document["renderQueue"].IsUint(), "Material's renderQueue must be unsigned int value or RenderQueue string in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				parameters->renderQueue = document["renderQueue"].GetUint();
			}
		}
		else
		{
			// 默认值  Opaque(1000)
			parameters->renderQueue = RenderQueue::Opaque;
		}

		// depth
		if (document.HasMember("ztest"))
		{
			if (document["ztest"].IsBool())
			{
				bool enable = document["ztest"].GetBool();
				parameters->ztest.enableZTest = enable;
			}
			else
			{
				BROWSER_ASSERT(document["ztest"].IsObject(), "Material's ztest must be object value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				if (document["ztest"].HasMember("enable"))
				{
					BROWSER_ASSERT(document["ztest"]["enable"].IsBool(), "Material's ztest enable must be bool value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					parameters->ztest.enableZTest = document["ztest"]["enable"].GetBool();
				}
				if (document["ztest"].HasMember("func"))
				{
					BROWSER_ASSERT(document["ztest"]["func"].IsString(), "Material's ztest enable must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					parameters->ztest.ZTestFunc = convertString2GLenum(document["ztest"]["func"].GetString(), parameters->ztest.ZTestFunc);
				}
			}
		}
		if (document.HasMember("zwrite"))
		{
			if (document["zwrite"].IsBool())
			{
				bool enable = document["zwrite"].GetBool();
				parameters->ztest.ZWrite = enable;
			}
		}

		// cull
		if (document.HasMember("cull"))
		{
			if (document["cull"].IsBool())
			{
				bool enable = document["cull"].GetBool();
				parameters->cull.enableCull = enable;
			}
			else
			{
				BROWSER_ASSERT(document["cull"].IsObject(), "Material's cull must be object value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				if (document["cull"].HasMember("enable"))
				{
					BROWSER_ASSERT(document["cull"]["enable"].IsBool(), "Material's cull enable must be bool value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					parameters->cull.enableCull = document["cull"]["enable"].GetBool();
				}
				if (document["cull"].HasMember("cullface"))
				{
					BROWSER_ASSERT(document["cull"]["cullface"].IsString(), "Material's cull cullface must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					parameters->cull.cullFace = convertString2GLenum(document["cull"]["cullface"].GetString(), parameters->cull.cullFace);
				}
				if (document["cull"].HasMember("frontface"))
				{
					BROWSER_ASSERT(document["cull"]["frontface"].IsString(), "Material's cull frontface must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					parameters->cull.frontFace = convertString2GLenum(document["cull"]["frontface"].GetString(), parameters->cull.frontFace);
				}
			}
		}

		// stencil
		if (document.HasMember("stencil"))
		{
			BROWSER_ASSERT(document["stencil"].IsObject(), "Material's stencil must be object value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
			BROWSER_ASSERT(document["stencil"].HasMember("enable"), "Material's stencil must has enable property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
			BROWSER_ASSERT(document["stencil"].HasMember("func"), "Material's stencil must has func property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
			BROWSER_ASSERT(document["stencil"].HasMember("ref"), "Material's stencil must has ref property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
			BROWSER_ASSERT(document["stencil"].HasMember("mask"), "Material's stencil must has mask property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
			BROWSER_ASSERT(document["stencil"].HasMember("sfail"), "Material's stencil must has sfail property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
			BROWSER_ASSERT(document["stencil"].HasMember("dpfail"), "Material's stencil must has dpfail property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
			BROWSER_ASSERT(document["stencil"].HasMember("dppass"), "Material's stencil must has dppass property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
			
			{
				BROWSER_ASSERT(document["stencil"]["enable"].IsBool(), "Material's stencil enable must be bool value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				parameters->stencil.enableStencilTest = document["stencil"]["enable"].GetBool();
			}
			{
				BROWSER_ASSERT(document["stencil"]["func"].IsString(), "Material's stencil func must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				parameters->stencil.stencilFuncParam->func = convertString2GLenum(document["stencil"]["func"].GetString(), parameters->stencil.stencilFuncParam->func);
			}
			{
				BROWSER_ASSERT(document["stencil"]["ref"].IsInt(), "Material's stencil ref must be int value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				parameters->stencil.stencilFuncParam->ref = document["stencil"]["ref"].GetInt();
			}
			{
				BROWSER_ASSERT(document["stencil"]["mask"].IsUint(), "Material's stencil mask must be uint value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				parameters->stencil.stencilFuncParam->mask = document["stencil"]["mask"].GetUint();
			}
			{
				BROWSER_ASSERT(document["stencil"]["sfail"].IsString(), "Material's stencil sfail must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				parameters->stencil.stencilOpParam->sfail = convertString2GLenum(document["stencil"]["sfail"].GetString(), parameters->stencil.stencilOpParam->sfail);
			}
			{
				BROWSER_ASSERT(document["stencil"]["dpfail"].IsString(), "Material's stencil dpfail must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				parameters->stencil.stencilOpParam->dpfail = convertString2GLenum(document["stencil"]["dpfail"].GetString(), parameters->stencil.stencilOpParam->dpfail);
			}
			{
				BROWSER_ASSERT(document["stencil"]["dppass"].IsString(), "Material's stencil dppass must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				parameters->stencil.stencilOpParam->dppass = convertString2GLenum(document["stencil"]["dppass"].GetString(), parameters->stencil.stencilOpParam->dppass);
			}

		}

		// blend
		if (document.HasMember("blend"))
		{
			if (document["blend"].IsBool())
			{
				bool enable = document["blend"].GetBool();
				parameters->blend.enableBlend = enable;
			}
			else
			{
				BROWSER_ASSERT(document["blend"].IsObject(), "Material's blend must be object value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				BROWSER_ASSERT(document["blend"].HasMember("enable"), "Material's blend must has enable property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				BROWSER_ASSERT(document["blend"].HasMember("sfactor"), "Material's blend must has sfactor property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
				BROWSER_ASSERT(document["blend"].HasMember("dfactor"), "Material's blend must has dfactor property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");

				{
					BROWSER_ASSERT(document["blend"]["enable"].IsBool(), "Material's blend enable must be bool value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					parameters->blend.enableBlend = document["blend"]["enable"].GetBool();
				}
				if (!document["blend"].HasMember("safactor") || !document["blend"].HasMember("dafactor"))
				{
					BROWSER_ASSERT(document["blend"]["sfactor"].IsString(), "Material's blend sfactor must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					BROWSER_ASSERT(document["blend"]["dfactor"].IsString(), "Material's blend dfactor must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					parameters->blend.blendFuncParam->sfactor = parameters->blend.blendFuncParam->safactor = document["blend"]["sfactor"].GetBool();
					parameters->blend.blendFuncParam->dfactor = parameters->blend.blendFuncParam->dafactor = document["blend"]["dfactor"].GetBool();
				}
				else
				{
					BROWSER_ASSERT(document["blend"].HasMember("safactor"), "Material's blend must has safactor property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					BROWSER_ASSERT(document["blend"].HasMember("dafactor"), "Material's blend must has dafactor property in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					BROWSER_ASSERT(document["blend"]["sfactor"].IsString(), "Material's blend sfactor must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					BROWSER_ASSERT(document["blend"]["dfactor"].IsString(), "Material's blend dfactor must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					BROWSER_ASSERT(document["blend"]["safactor"].IsString(), "Material's blend safactor must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					BROWSER_ASSERT(document["blend"]["dafactor"].IsString(), "Material's blend dafactor must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					parameters->blend.blendFuncParam->sfactor = convertString2GLenum(document["blend"]["sfactor"].GetString(), parameters->blend.blendFuncParam->sfactor);
					parameters->blend.blendFuncParam->dfactor = convertString2GLenum(document["blend"]["dfactor"].GetString(), parameters->blend.blendFuncParam->dfactor);
					parameters->blend.blendFuncParam->safactor = convertString2GLenum(document["blend"]["safactor"].GetString(), parameters->blend.blendFuncParam->safactor);
					parameters->blend.blendFuncParam->dafactor = convertString2GLenum(document["blend"]["dafactor"].GetString(), parameters->blend.blendFuncParam->dafactor);
				}
				if (document["blend"].HasMember("equation"))
				{
					BROWSER_ASSERT(document["blend"]["equation"].IsString(), "Material's blend equation must be string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					parameters->blend.blendEquation = convertString2GLenum(document["blend"]["equation"].GetString(), parameters->blend.blendEquation);
				}

			}
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
						strType = m_mBuiltinUniforms[uniformParam.name];
						BROWSER_ASSERT(strType != "" || m_mBuiltinUniforms[uniformParam.name] == strType, "Found builtin uniform type not right in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
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
							// path
							BROWSER_SAFE_RELEASE_POINTER(uniformParam.m_pString);
							uniformParam.m_pString = new std::string(value.GetString());
							*(uniformParam.m_pString) = FileUtils::getInstance()->tryGetAbsolutePathForFilename(*(uniformParam.m_pString), directory);
                            uniformParam.value.tex2D.path = &(*uniformParam.m_pString)[0];
							BROWSER_ASSERT(FileUtils::getInstance()->isDirectoryOrFileExist(*uniformParam.m_pString), "Uniform's texture file is not found in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
							parameters->textures_path.push_back(*(uniformParam.m_pString));	// 记录纹理路径，方便统一异步加载所有图片
							// wrap
							uniformParam.value.tex2D.wrap = TexParameters_DefaultWrap;
							if (uniformValue.HasMember("wrap"))
							{
								BROWSER_ASSERT(uniformValue["wrap"].IsString(), "Sampler2D's wrap property is not string (type sampler2D) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
								convertString2GLenum(uniformValue["wrap"].GetString(), TexParameters_DefaultWrap);
							}
							// filter
							uniformParam.value.tex2D.wrap = TexParameters_DefaultFilter;
							if (uniformValue.HasMember("filter"))
							{
								BROWSER_ASSERT(uniformValue["filter"].IsString(), "Sampler2D's filter property is not string (type sampler2D) in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
								convertString2GLenum(uniformValue["filter"].GetString(), TexParameters_DefaultFilter);
							}
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
					const std::string full_path = FileUtils::getInstance()->tryGetAbsolutePathForFilename(vert_path, directory);
					BROWSER_ASSERT(FileUtils::getInstance()->isDirectoryOrFileExist(full_path), "Pass's vert file is not found in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
					passParam.vert_program = Utils::readAbsolutePathFile(full_path.c_str());
                }
                else if (one_pass.HasMember("vert_program"))
                {
                    BROWSER_ASSERT(one_pass["vert_program"].IsString(), "Pass's vert_program is not a string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    passParam.vert_program = one_pass["vert_program"].GetString();
                }
				passParam.tags = GLProgram::parseShaderSourceCode(passParam.vert_program);	// 解析shader，将自定义的一些语法规则转成标准的glsl

                // fragment shader
                if (one_pass.HasMember("frag"))
                {
                    BROWSER_ASSERT(one_pass["frag"].IsString(), "Pass's frag is not a string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    std::string frag_path = one_pass["frag"].GetString();
					const std::string full_path = FileUtils::getInstance()->tryGetAbsolutePathForFilename(frag_path, directory);
					BROWSER_ASSERT(FileUtils::getInstance()->isDirectoryOrFileExist(full_path), "Pass's frag file is not found in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    passParam.frag_program = Utils::readFile(full_path.c_str());
                }
                else if (one_pass.HasMember("frag_program"))
                {
                    BROWSER_ASSERT(one_pass["frag_program"].IsString(), "Pass's frag_program is not a string value in function MaterialParser::parseMaterialFileByContent(const std::string& content)");
                    passParam.frag_program = one_pass["frag_program"].GetString();
                }
				passParam.tags |= GLProgram::parseShaderSourceCode(passParam.frag_program);	// 解析shader，将自定义的一些语法规则转成标准的glsl

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
						texture->setTexWrapParams(uniformParam.value.tex2D.wrap, uniformParam.value.tex2D.wrap);
						texture->setTexFilterParams(uniformParam.value.tex2D.filter, uniformParam.value.tex2D.filter);
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


	GLenum MaterialParser::convertString2GLenum(const std::string& sWrap, GLenum defaultEnum)
	{
		if (m_mString2GLenum.find(sWrap) != m_mString2GLenum.end())
		{
			return m_mString2GLenum[sWrap];
		}

		return defaultEnum;
	}

}
