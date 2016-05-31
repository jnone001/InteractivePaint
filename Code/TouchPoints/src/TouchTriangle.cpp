#include "TouchTriangle.h"

namespace touchpoints { namespace drawing
{
	TouchTriangle::TouchTriangle() {}

	TouchTriangle::TouchTriangle(const vec2& point1, const vec2& point2, const vec2& point3, const vec2& baseCenter, const ColorA& color, const float& size, bool& filledShapes)
	{
		mPoint1 = point1;
		mPoint2 = point2;
		mPoint3 = point3;
		mBaseCenter = baseCenter;
		mColor = color;
		mSize = size;
		if (mSize == 1) mSize = 2;
		mFilledShapes = filledShapes;
	}

	void TouchTriangle::changeSize(float x, float y)
	{
		mPoint1.y = y;
		mPoint2.x = x;
		mPoint3.x = mBaseCenter.x + (mBaseCenter.x - mPoint2.x);
	}

	void TouchTriangle::draw()
	{
		gl::lineWidth(mSize);
		gl::color(mColor);
		if (mFilledShapes)
		{
			gl::drawSolidTriangle(mPoint1, mPoint2, mPoint3);
		}
		else
		{
			gl::drawLine(mPoint1, mPoint2);
			gl::drawLine(mPoint2, mPoint3);
			gl::drawLine(mPoint3, mPoint1);

			gl::color(1.0, 1.0, 1.0, 1.0);
		}
	}

	vec2 TouchTriangle::getPoint1()
	{
		return mPoint1;
	}

	vec2 TouchTriangle::getPoint2()
	{
		return mPoint2;
	}

	vec2 TouchTriangle::getPoint3()
	{
		return mPoint3;
	}

	vec2 TouchTriangle::getBaseCenter()
	{
		return mBaseCenter;
	}
}}
