#pragma once
#include "TouchShape.h"

namespace drawing {
	struct TouchTriangle : public TouchShape
	{
		TouchTriangle();
		TouchTriangle(const vec2& point1, const vec2& point2, const vec2& point3, const vec2& baseCenter, const ColorA& color, const float& size, bool& filledShapes);
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
}