#include "TouchShape.h"

namespace drawing {
	float TouchShape::size() {
		return mSize;
	}
	ColorA TouchShape::getColor()
	{
		return mColor;
	}
	bool TouchShape::getFilledShape()
	{
		return mFilledShapes;
	}
	float TouchShape::getSize()
	{
		return mSize;
	}
}