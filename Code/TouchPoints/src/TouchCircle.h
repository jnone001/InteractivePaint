#pragma once
#include "TouchShape.h"

namespace touchpoints { namespace drawing
{
	struct TouchCircle : public TouchShape
	{
		TouchCircle();
		TouchCircle(vec2 center, float radius, ColorA color, float size, bool filledShapes);
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
}}
