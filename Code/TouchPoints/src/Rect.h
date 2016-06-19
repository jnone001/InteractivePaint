#pragma once
#include "cinder/Vector.h"

using namespace cinder;

namespace touchpoints { namespace math
{
	class Rect
	{
	public:
		Rect();
		Rect(vec2 oneCorner, vec2 oppositeCorner);
		inline vec2 GetTopLeftCorner() { return topLeftCorner; }
		inline vec2 GetTopRightCorner() { return topRightCorner; }
		inline vec2 GetBottomRightCorner() { return bottomRightCorner; }
		inline vec2 GetBottomLeftCorner() { return bottomLeftCorner; }
	private:
		vec2 topLeftCorner;
		vec2 topRightCorner;
		vec2 bottomRightCorner;
		vec2 bottomLeftCorner;
	};
}}
