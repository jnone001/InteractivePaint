#include "TouchRectangle.h"
#include <cinder/gl/scoped.h>
#include <cinder/gl/draw.h>

using namespace cinder;

namespace touchpoints { namespace drawing
{
	TouchRectangle::TouchRectangle() : TouchRectangle(0, 0, 0, 0, ColorA(0, 0, 0, 0), 0.0f, false) {}

	TouchRectangle::TouchRectangle(int x1, int y1, int x2, int y2, ColorA color, float size, bool filledShapes)
	{
		mUpperLeftX = x1;
		mUpperLeftY = y1;
		mLowerRightX = x2;
		mLowerRightY = y2;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
		this->framesDrawn = 1;
	}

	TouchRectangle::TouchRectangle(int x1, int y1, int x2, int y2, ColorA color, float size, bool filledShapes, int framesDrawn)
	{
		mUpperLeftX = x1;
		mUpperLeftY = y1;
		mLowerRightX = x2;
		mLowerRightY = y2;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
		this->framesDrawn = framesDrawn;
	}

	void TouchRectangle::draw()
	{
		gl::color(mColor);
		if (mFilledShapes)
		{
			gl::drawSolidRect(Rectf(mUpperLeftX, mUpperLeftY, mLowerRightX, mLowerRightY));
		}
		else
		{
			gl::drawStrokedRect(Rectf(mUpperLeftX, mUpperLeftY, mLowerRightX, mLowerRightY), mSize);
		}

		gl::color(1.0, 1.0, 1.0, 1.0);
	}
}}
