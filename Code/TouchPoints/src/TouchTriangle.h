#pragma once
#include "TouchShape.h"

namespace touchpoints { namespace drawing
{
	struct TouchTriangle : public TouchShape
	{
		TouchTriangle();
		TouchTriangle(vec2 point1, vec2 point2, vec2 point3, vec2 baseCenter, ColorA color, float size, bool filledShapes);
		void changeSize(float x, float y);
		void draw() override;
		vec2 getPoint1();
		vec2 getPoint2();
		vec2 getPoint3();
		vec2 getBaseCenter();
	private:
		vec2 mPoint1;
		vec2 mPoint2;
		vec2 mPoint3;
		vec2 mBaseCenter;
	};
}}
