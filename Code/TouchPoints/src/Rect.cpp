#include "Rect.h"
#include <algorithm>

namespace  touchpoints { namespace math
{
	Rect::Rect() : Rect(vec2(0,0), vec2(0,0)) {}
	Rect::Rect(vec2 oneCorner, vec2 oppositeCorner)
	{
		//(0,0) is in the top left corner
		auto left = std::min(oneCorner.x, oppositeCorner.x);
		auto right = std::max(oneCorner.x, oppositeCorner.x);
		auto top = std::min(oneCorner.y, oppositeCorner.y);
		auto bottom = std::max(oneCorner.y, oppositeCorner.y);

		topLeftCorner = vec2(left, top);
		topRightCorner = vec2(right, top);
		bottomRightCorner = vec2(right, bottom);
		bottomLeftCorner = vec2(bottom, left);
	}
}}