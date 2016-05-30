#include "TouchRectangle.h"

namespace drawing {
	TouchRectangle::TouchRectangle() {}

	TouchRectangle::TouchRectangle(const int& x1, const int& y1, const int& x2, const int& y2, const ColorA& color, const float& size, bool& filledShapes)
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
}