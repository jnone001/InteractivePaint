#include "TouchPoint.h"

namespace drawing {
	TouchPoint::TouchPoint() {}

	TouchPoint::TouchPoint(const vec2& initialPt, const ColorA& color, const float& size)
	{
		pointList.emplace_back(initialPt);
		mColor = color;
		mSize = size;
	}

	void TouchPoint::addPoint(const vec2& pt)
	{
		pointList.emplace_back(pt);
	}

	void TouchPoint::clearPoints()
	{
		pointList.clear();
	}

	vec2 TouchPoint::getFirstPoint()
	{
		return pointList.front();
	}

	list<vec2> TouchPoint::getPointList()
	{
		return pointList;
	}

	void TouchPoint::draw()
	{
		gl::lineWidth(mSize);
		gl::color(mColor);

		for (auto points : pointList)
		{
			gl::drawSolidCircle(points, mSize);
		}
	}
}