#pragma once
#include "TouchShape.h"

namespace touchpoints { namespace drawing
{
	struct TouchRectangle : public TouchShape
	{
		TouchRectangle();
		TouchRectangle(int x1, int y1, int x2, int y2, ColorA color, float size, bool filledShapes);
		TouchRectangle(int x1, int y1, int x2, int y2, ColorA color, float size, bool filledShapes, int framesDrawn);
		inline int upperLeftX() { return mUpperLeftX; }
		inline int upperLeftY() { return mUpperLeftY; }
		inline int lowerRightX() { return mLowerRightX; }
		inline int lowerRightY() { return mLowerRightY; }
		void draw() override;
	private:
		int mUpperLeftX;
		int mUpperLeftY;
		int mLowerRightX;
		int mLowerRightY;
	};
}}