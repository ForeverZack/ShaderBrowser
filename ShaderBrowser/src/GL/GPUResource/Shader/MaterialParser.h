#pragma once

#include <string>
#include <unordered_map>
#include "Plugins/rapidjson/document.h"
#include "GL/GPUResource/Shader/Material.h"

using namespace rapidjson;

namespace customGL
{
    class MaterialParser
	{
	public:
		static Material* parseMaterialFileByContent(const std::string& content);

	protected:
		// ����uniform��Ӧ����
		static std::unordered_map<std::string, std::string> m_mBuiltinUniforms;
	};
}
