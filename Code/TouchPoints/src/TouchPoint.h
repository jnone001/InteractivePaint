#pragma once
#include "TouchShape.h"

namespace touchpoints { namespace drawing
{
	struct TouchPoint : public TouchShape
	{
	public:
		TouchPoint();
		TouchPoint(vec2 initialPt, ColorA color, float size);
		void addPoint(const vec2& pt);
		void clearPoints();
		vec2 getFirstPoint();
		std::list<vec2> getPointList();
		void draw() override;
	private:
		std::list<vec2> pointList;
	};
}}
