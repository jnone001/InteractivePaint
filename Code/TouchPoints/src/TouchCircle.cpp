#include "TouchCircle.h"

namespace drawing {
	TouchCircle::TouchCircle() {}

	TouchCircle::TouchCircle(const vec2& center, const float& radius, const ColorA& color, const float& size, bool& filledShapes)
	{
		mRadius = radius;
		mCenter = center;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
	}

	void TouchCircle::changeRadius(float& radius)
	{
		mRadius = radius;
	}

	float TouchCircle::getRadius()
	{
		return mRadius;
	}

	void TouchCircle::changePosition(vec2 newCenter)
	{
		mCenter = newCenter;
	}

	float TouchCircle::getCenterX()
	{
		return mCenter.x;
	}

	float TouchCircle::getCenterY()
	{
		return mCenter.y;
	}

	vec2 TouchCircle::getCenter()
	{
		return mCenter;
	}

	void TouchCircle::draw()
	{
		gl::color(mColor);
		if (mFilledShapes)
		{
			gl::drawSolidCircle(mCenter, mRadius);
		}
		else gl::drawStrokedCircle(mCenter, mRadius, mSize * 2.0f);

		gl::color(1.0, 1.0, 1.0, 1.0);
	}
}