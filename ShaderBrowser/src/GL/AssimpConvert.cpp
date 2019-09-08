#include "AssimpConvert.h"

namespace Assimp
{
	extern glm::mat4 ConvertToGLM( const aiMatrix4x4 & matrix )
	{
		// glm::mat4 is column based like opengl
		// aiMatrix4x4 is row based

		return glm::mat4(
			matrix.a1, matrix.b1, matrix.c1, matrix.d1,
			matrix.a2, matrix.b2, matrix.c2, matrix.d2,
			matrix.a3, matrix.b3, matrix.c3, matrix.d3,
			matrix.a4, matrix.b4, matrix.c4, matrix.d4 );
	}

	extern glm::vec3 ConvertToGLM( const aiVector3D & vector )
	{
		return glm::vec3( vector.x, vector.y, vector.z );
	}

	extern glm::vec4 ConvertToGLM( const aiVector3D & vector, const float w )
	{
		return glm::vec4( vector.x, vector.y, vector.z, w );
	}

	extern glm::vec4 ConvertToGLM(const aiColor4D & vector)
	{
		return glm::vec4(vector.r, vector.g, vector.b, vector.a);
	}

	extern glm::quat ConvertToGLM( const aiQuaternion & quat )
	{
		return glm::quat( quat.w, quat.x, quat.y, quat.z );
	}

	extern std::string ConvertToSTD( const aiString & string )
	{
		return std::string( string.C_Str( ) );
	}
}
