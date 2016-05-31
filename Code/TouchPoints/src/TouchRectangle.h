#pragma once
#include "TouchShape.h"

namespace touchpoints { namespace drawing
{
	struct TouchRectangle : public TouchShape
	{
		TouchRectangle();
		TouchRectangle(const int& x1, const int& y1, const int& x2, const int& y2, const ColorA& color, const float& size, bool& filledShapes);
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