#pragma once
#include "TouchShape.h"

namespace touchpoints { namespace drawing
{
	//Isosceles Triangle that points either up or down
	//in other words its base is parallel to the x-axis
	struct TouchVerticalIsoscelesTriangle : public TouchShape
	{
		TouchVerticalIsoscelesTriangle();
		TouchVerticalIsoscelesTriangle(vec2 point1, vec2 point2, vec2 point3, vec2 baseCenter, ColorA color, float size, bool filledShapes);
		TouchVerticalIsoscelesTriangle(vec2 point1, vec2 point2, vec2 point3, vec2 baseCenter, ColorA color, float size, bool filledShapes, int framesDrawn);
		void draw() override;
		inline vec2 GetBaseVertexLeft() { return baseVertexLeft; }
		inline vec2 GetBaseVertexRight() { return baseVertexRight; }
		inline vec2 GetOppositeBaseVertex() { return oppositeBaseVertex; }
		inline vec2 GetBaseCenter() { return baseCenter; }
	private:
		vec2 baseVertexLeft;
		vec2 baseVertexRight;
		vec2 oppositeBaseVertex;
		vec2 baseCenter;
	};
}}
