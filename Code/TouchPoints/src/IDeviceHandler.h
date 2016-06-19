#pragma once
#include "../src/IDrawable.h"
#include <vector>

namespace touchpoints {
	namespace devices
	{
		class IDeviceHandler
		{
		public:
			virtual std::vector<drawing::IDrawable> checkForInput() = 0;

		};
	}
}
