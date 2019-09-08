#pragma once
#include <string>
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Assimp
{
    extern glm::mat4 ConvertToGLM( const aiMatrix4x4 & matrix );
    extern glm::vec3 ConvertToGLM( const aiVector3D & vector );
    extern glm::vec4 ConvertToGLM( const aiVector3D & vector, const float w );
    extern glm::vec4 ConvertToGLM(const aiColor4D & vector);
    extern glm::quat ConvertToGLM( const aiQuaternion & quat );
    extern std::string ConvertToSTD( const aiString & string );
}
