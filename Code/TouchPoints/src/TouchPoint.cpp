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
		if(pointList.size() > 0)
		{
			auto lastPoint = pointList.back();
			auto missedPoints = findMissedPoints(lastPoint, pt);
			pointList.insert(pointList.end(), missedPoints.begin(), missedPoints.end());
		}
		pointList.push_back(pt);
	}

	void TouchPoint::clearPoints()
	{
		pointList.clear();
	}

	vec2 TouchPoint::getFirstPoint()
	{
		return pointList.front();
	}

	vector<vec2> TouchPoint::getPointList() const
	{
		return pointList;
	}

	vector<vec2> TouchPoint::findMissedPoints(vec2 initialPoint, vec2 finalPoint)
	{
		auto xi = initialPoint.x;
		auto yi = initialPoint.y;
		auto xf = finalPoint.x;
		auto yf = finalPoint.y;

		auto missedPoints = vector<vec2>();

		int dx, dy, x, y, d, ds, dt;
		//Find Abs value of difference between x and y coords of two points
		dx = abs(xi - xf);
		dy = abs(yi - yf);
		//Check which coordinates have bigger range x or y
		if (dx >= dy)
		{
			d = 2 * dy - dx;
			ds = 2 * dy;
			dt = 2 * (dy - dx);
			//Figure out which point to use as starting point
			if (xi < xf)
			{
				x = xi;
				y = yi;
			}
			else
			{
				x = xf;
				y = yf;
				xf = xi;
				yf = yi;
			}

			//Would be starting point passed to method
			missedPoints.push_back(vec2(x, y));

			while (x < xf)
			{
				if (d < 0)
				{
					d += ds;
				}
				else
				{
					if (y < yf)
					{
						y++;
						d += dt;
					}
					else
					{
						y--;
						d += dt;
					}
				}

				x++;

				missedPoints.push_back(vec2(x, y));
			}
		}
		else
		{
			d = 2 * dx - dy;
			ds = 2 * dx;
			dt = 2 * (dx - dy);
			//Figure out which point to use as starting point
			if (yi < yf)
			{
				x = xi;
				y = yi;
			}
			else
			{
				x = xf;
				y = yf;
				xf = xi;
				yf = yi;
			}

			missedPoints.push_back(vec2(x, y));

			while (y < yf)
			{
				if (d < 0)
				{
					d += ds;
				}
				else
				{
					if (x > xf)
					{
						x--;
						d += dt;
					}
					else
					{
						x++;
						d += dt;
					}
				}

				y++;

				missedPoints.push_back(vec2(x, y));
			}
		}

		return missedPoints;
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