#pragma once
#include "cinder/Vector.h"
#include "Rect.h"

using namespace cinder;

namespace touchpoints { namespace math
{
	//Isosceles Triangle that points either up or down
	//in other words its base is parallel to the x-axis
	class VerticalIsoscelesTriangle
	{
	public:
		VerticalIsoscelesTriangle(vec2 oneCorner, vec2 oppositeCorner, bool isPointingDown);
		inline bool GetIsPointingDown() { return isPointingDown; }
		inline vec2 GetBaseVertexLeft() { return baseVertexLeft; }
		inline vec2 GetBaseVertexRight() { return baseVertexRight; }
		inline vec2 GetOppositeBaseVertex() { return oppositeBaseVertex; }
		inline vec2 GetBaseCenter() { return baseCenter; }
		inline Rect GetBoundingRectangle() { return boundingRectangle; }
	private:
		bool isPointingDown;
		vec2 baseVertexLeft;
		vec2 baseVertexRight;
		vec2 oppositeBaseVertex;
		vec2 baseCenter;
		Rect boundingRectangle;
	};
}}
