#pragma once
#include "cinder/Vector.h"
#include <cmath>

using namespace cinder;

namespace touchpoints { namespace math
{
	inline float CalculateDistance(vec2 startPoint, vec2 endPoint)
	{
		auto diffXSquared = pow((endPoint.x - startPoint.x), 2);
		auto diffYSquared = pow((endPoint.y - startPoint.y), 2);
		return sqrt(diffXSquared + diffYSquared);
	}

	inline vec2 FindMidPoint(vec2 startPoint, vec2 endPoint)
	{
		auto newX = (startPoint.x + endPoint.x) / 2;
		auto newY = (startPoint.y + endPoint.y) / 2;
		return vec2(newX, newY);
	}
}}
