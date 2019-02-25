#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Assimp
{
	template<typename T>
	void KeyPairFind(
		const double time,
		const T * keys,
		const std::uint32_t numKeys,
		const T ** leftKey,
		const T ** rightKey )
	{
		if( numKeys == 1 )
		{
			*leftKey = *rightKey = &keys[0];
			return;
		}

		if( keys[numKeys - 1].mTime < time )
		{
			*leftKey = *rightKey = &keys[numKeys - 1];
			return;
		}

		for( std::uint32_t t = 0; t < numKeys; t++ )
		{
			if( keys[t].mTime >= time )
			{
				*leftKey = &keys[(t - 1) % (numKeys + 1)];
				*rightKey = &keys[t];
				break;
			}
		}
	}

	template<typename R, typename T>
	R InterpolationGet(
		const double time,
		const T * keys,
		const std::uint32_t numKeys,
		const bool interpolateAnimation )
	{
		const T * leftKey, * rightKey;
		const auto interpolator = Assimp::Interpolator<R>( );
		R translation;

		Assimp::KeyPairFind( time, keys, numKeys, &leftKey, &rightKey );
		if( leftKey == rightKey || !interpolateAnimation )
		{
			// Do not interpolate
			translation = leftKey->mValue;
		}
		else
		{
			// Interpolate
			const auto progress = time - leftKey->mTime;
			const auto differenz = rightKey->mTime - leftKey->mTime;
			const auto weight = (progress) / (differenz);

			interpolator(
				translation,
				leftKey->mValue, rightKey->mValue,
				static_cast<float>( weight ) );
		}

		return translation;
	}

	glm::mat4 ConvertToGLM( const aiMatrix4x4 & matrix )
	{
		// glm::mat4 is column based like opengl
		// aiMatrix4x4 is row based

		return glm::mat4(
			matrix.a1, matrix.b1, matrix.c1, matrix.d1,
			matrix.a2, matrix.b2, matrix.c2, matrix.d2,
			matrix.a3, matrix.b3, matrix.c3, matrix.d3,
			matrix.a4, matrix.b4, matrix.c4, matrix.d4 );
	}

	glm::vec3 ConvertToGLM( const aiVector3D & vector )
	{
		return glm::vec3( vector.x, vector.y, vector.z );
	}

	glm::vec4 ConvertToGLM( const aiVector3D & vector, const float w )
	{
		return glm::vec4( vector.x, vector.y, vector.z, w );
	}

	glm::quat ConvertToGLM( const aiQuaternion & quat )
	{
		return glm::quat( quat.w, quat.x, quat.y, quat.z );
	}

	std::string ConvertToSTD( const aiString & string )
	{
		return std::string( string.C_Str( ) );
	}
}
