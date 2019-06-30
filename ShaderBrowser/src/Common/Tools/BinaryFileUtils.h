#pragma once

#include <vector>
#include <string>
#include "BaseSingleton.h"

namespace common
{

	class BinaryFileUtils : public BaseSingleton<BinaryFileUtils>
	{
	public:
		BinaryFileUtils();
		~BinaryFileUtils();

	public:
		// test
		void serialize();
		void deserialize();

	private:


	};

}
