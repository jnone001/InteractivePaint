#include "TouchShape.h"

namespace touchpoints { namespace drawing
{
	float TouchShape::size() const
	{
		return mSize;
	}

	ColorA TouchShape::getColor() const
	{
		return mColor;
	}

	bool TouchShape::getFilledShape() const
	{
		return mFilledShapes;
	}

	float TouchShape::getSize() const
	{
		return mSize;
	}
}}
