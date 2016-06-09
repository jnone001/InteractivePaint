#pragma once
#include "TouchShape.h"

namespace touchpoints { namespace drawing
{
	struct TouchRectangle : public TouchShape
	{
		TouchRectangle();
		TouchRectangle(int x1, int y1, int x2, int y2, ColorA color, float size, bool filledShapes);
		void changePoint(int x, int y);
		int upperLeftX();
		int upperLeftY();
		int lowerRightX();
		int lowerRightY();
		void draw() override;
	private:
		int mUpperLeftX;
		int mUpperLeftY;
		int mLowerRightX;
		int mLowerRightY;
	};
}}