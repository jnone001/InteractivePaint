#include "TouchPoint.h"
#include <cinder/gl/Context.h>
#include <cinder/gl/draw.h>

using namespace cinder;

namespace touchpoints { namespace drawing
{
	TouchPoint::TouchPoint() {}

	TouchPoint::TouchPoint(vec2 initialPt, ColorA color, float size)
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

		std::list<vec2> TouchPoint::getPointList()
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
}}