#pragma once

// How to scale down a range of numbers with a known min and max value
namespace customGL
{
	class Rescale
	{
	public:
		Rescale(float srcMin, float srcMax, float dstMin, float dstMax)
			: srcMin(srcMin)
			, srcMax(srcMax)
			, dstMin(dstMin)
			, dstMax(dstMax)
		{
		}

	public:
		float Scale(float x) const
		{
			//assert( x >= srcMin && x <= srcMax );
			const float numerator = (dstMax - dstMin)*(x - srcMin);
			const float denominator = (srcMax - srcMin);

			return (numerator / denominator) + dstMin;
		}

		float Unscale(float x) const
		{
			//assert( x >= dstMin && x <= dstMax );
			const float numerator = (srcMax - srcMin)*(x - dstMin);
			const float denominator = (dstMax - dstMin);

			return (numerator / denominator) + srcMin;
		}

	private:
		float srcMin, srcMax;
		float dstMin, dstMax;

	};
}
