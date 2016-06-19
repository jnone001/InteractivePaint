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
		std::vector<vec2> getPointList() const;
		void draw() override;
	private:
		std::vector<vec2> pointList;
	};
}}
