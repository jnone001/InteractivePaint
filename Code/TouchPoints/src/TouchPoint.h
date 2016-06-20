#pragma once
#include "TouchShape.h"

using namespace std;

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
		vector<vec2> getPointList() const;
		void draw() override;
	private:
		vector<vec2> findMissedPoints(vec2 initialPoint, vec2 finalPoint);
		vector<vec2> pointList;
	};
}}
