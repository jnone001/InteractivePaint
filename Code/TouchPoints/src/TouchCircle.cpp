#include "TouchCircle.h"
#include <cinder/gl/scoped.h>
#include <cinder/gl/draw.h>

using namespace cinder;

namespace touchpoints { namespace drawing
{
	TouchCircle::TouchCircle() {}

	TouchCircle::TouchCircle(vec2 center, float radius, ColorA color, float size, bool filledShapes)
	{
		mRadius = radius;
		mCenter = center;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
		this->framesDrawn = 1;
	}

	TouchCircle::TouchCircle(vec2 center, float radius, ColorA color, float size, bool filledShapes, int framesDrawn)
	{
		mRadius = radius;
		mCenter = center;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
		this->framesDrawn = framesDrawn;
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
}}
