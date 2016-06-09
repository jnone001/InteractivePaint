#include "TouchRectangle.h"
#include <cinder/gl/scoped.h>
#include <cinder/gl/draw.h>

using namespace cinder;

namespace touchpoints { namespace drawing
{
	TouchRectangle::TouchRectangle() {}

	TouchRectangle::TouchRectangle(int x1, int y1, int x2, int y2, ColorA color, float size, bool filledShapes)
	{
		mUpperLeftX = x1;
		mUpperLeftY = y1;
		mLowerRightX = x2;
		mLowerRightY = y2;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
	}

	void TouchRectangle::changePoint(int x, int y)
	{
		mLowerRightX = x;
		mLowerRightY = y;
	}

	int TouchRectangle::upperLeftX()
	{
		return mUpperLeftX;
	}

	int TouchRectangle::upperLeftY()
	{
		return mUpperLeftY;
	}

	int TouchRectangle::lowerRightX()
	{
		return mLowerRightX;
	}

	int TouchRectangle::lowerRightY()
	{
		return mLowerRightY;
	}

	void TouchRectangle::draw()
	{
		gl::color(mColor);
		if (mFilledShapes)
		{
			gl::drawSolidRect(Rectf(mUpperLeftX, mUpperLeftY, mLowerRightX, mLowerRightY));
		}
		else gl::drawStrokedRect(Rectf(mUpperLeftX, mUpperLeftY, mLowerRightX, mLowerRightY), mSize);

		gl::color(1.0, 1.0, 1.0, 1.0);
	}
}}
