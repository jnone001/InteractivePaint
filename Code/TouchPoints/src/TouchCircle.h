#pragma once
#include "TouchShape.h"

namespace drawing {
	struct TouchCircle : public TouchShape
	{
		TouchCircle();
		TouchCircle(const vec2& center, const float& radius, const ColorA& color, const float& size, bool& filledShapes);
		void changeRadius(float& radius);
		float getRadius();
		void changePosition(vec2 newCenter);
		float getCenterX();
		float getCenterY();
		vec2 getCenter();
		void draw() override;
	private:
		float mRadius;
		vec2 mCenter;
	};
}