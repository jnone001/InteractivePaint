#pragma once
#include "TouchShape.h"

namespace drawing {
	struct TouchPoint : public TouchShape
	{
	public:
		TouchPoint();
		TouchPoint(const vec2& initialPt, const ColorA& color, const float& size);
		void addPoint(const vec2& pt);
		void clearPoints();
		vec2 getFirstPoint();
		list<vec2> getPointList();
		void draw() override;
	private:
		list<vec2> pointList;
	};
}