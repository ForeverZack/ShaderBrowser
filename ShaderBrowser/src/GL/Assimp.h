#pragma once
#include <glm/glm.hpp>

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
    
}
