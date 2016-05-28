#ifndef TOUCHSHAPES_H
#define TOUCHSHAPES_H
//Cinder and OpenGL Includes
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

//Standard Library Includes
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

struct TouchShape
{
	virtual void draw() = 0;

	float size()
	{
		return mSize;
	}

	ColorA getColor()
	{
		return mColor;
	}

	bool getFilledShape()
	{
		return mFilledShapes;
	}

	float getSize()
	{
		return mSize;
	}

protected:
	bool mFilledShapes;
	ColorA mColor;
	float mSize;
};

//Struct for listing points, draws lines. 
struct TouchPoint : public TouchShape
{
	TouchPoint() {}

	TouchPoint(const vec2& initialPt, const ColorA& color, const float& size)
	{
		pointList.emplace_back(initialPt);
		mColor = color;
		mSize = size;
	}

	void addPoint(const vec2& pt)
	{
		pointList.emplace_back(pt);
	}

	void clearPoints()
	{
		pointList.clear();
	}

	vec2 getFirstPoint()
	{
		return pointList.front();
	}

	list<vec2> getPointList()
	{
		return pointList;
	}

	void draw() override
	{
		gl::lineWidth(mSize);
		gl::color(mColor);

		for (auto points : pointList)
		{
			gl::drawSolidCircle(points, mSize);
		}
	}

private:
	list<vec2> pointList;
};

//Struct for creating circles
struct TouchCircle : public TouchShape
{
	TouchCircle() {}

	TouchCircle(const vec2& center, const float& radius, const ColorA& color, const float& size, bool& filledShapes)
	{
		mRadius = radius;
		mCenter = center;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
	}

	void changeRadius(float& radius)
	{
		mRadius = radius;
	}

	float getRadius()
	{
		return mRadius;
	}

	void changePosition(vec2 newCenter)
	{
		mCenter = newCenter;
	}

	float getCenterX()
	{
		return mCenter.x;
	}

	float getCenterY()
	{
		return mCenter.y;
	}

	vec2 getCenter()
	{
		return mCenter;
	}

	void draw() override
	{
		gl::color(mColor);
		if (mFilledShapes)
		{
			gl::drawSolidCircle(mCenter, mRadius);
		}
		else gl::drawStrokedCircle(mCenter, mRadius, mSize * 2.0f);

		gl::color(1.0, 1.0, 1.0, 1.0);
	}

private:
	float mRadius;
	vec2 mCenter;
};

//Struct for creating rectangles
struct TouchRectangle : public TouchShape
{
	TouchRectangle() {}

	TouchRectangle(const int& x1, const int& y1, const int& x2, const int& y2, const ColorA& color, const float& size, bool& filledShapes)
	{
		mUpperLeftX = x1;
		mUpperLeftY = y1;
		mLowerRightX = x2;
		mLowerRightY = y2;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
	}

	void changePoint(int x, int y)
	{
		mLowerRightX = x;
		mLowerRightY = y;
	}

	int upperLeftX()
	{
		return mUpperLeftX;
	}

	int upperLeftY()
	{
		return mUpperLeftY;
	}

	int lowerRightX()
	{
		return mLowerRightX;
	}

	int lowerRightY()
	{
		return mLowerRightY;
	}

	void draw() override
	{
		//gl::lineWidth(mSize);
		gl::color(mColor);
		if (mFilledShapes)
		{
			gl::drawSolidRect(Rectf(mUpperLeftX, mUpperLeftY, mLowerRightX, mLowerRightY));
		}
		else gl::drawStrokedRect(Rectf(mUpperLeftX, mUpperLeftY, mLowerRightX, mLowerRightY), mSize);

		gl::color(1.0, 1.0, 1.0, 1.0);
	}

private:
	int mUpperLeftX;
	int mUpperLeftY;
	int mLowerRightX;
	int mLowerRightY;
};

//Struct for creating triangles
struct TouchTriangle : public TouchShape
{
	TouchTriangle() {}

	TouchTriangle(const vec2& point1, const vec2& point2, const vec2& point3, const vec2& baseCenter, const ColorA& color, const float& size, bool& filledShapes)
	{
		mPoint1 = point1;
		mPoint2 = point2;
		mPoint3 = point3;
		mBaseCenter = baseCenter;
		mColor = color;
		mSize = size;
		if (mSize == 1) mSize = 2;
		mFilledShapes = filledShapes;
	}

	void changeSize(float x, float y)
	{
		mPoint1.y = y;
		mPoint2.x = x;
		mPoint3.x = mBaseCenter.x + (mBaseCenter.x - mPoint2.x);
	}

	void draw() override
	{
		gl::lineWidth(mSize);
		gl::color(mColor);
		if (mFilledShapes)
		{
			gl::drawSolidTriangle(mPoint1, mPoint2, mPoint3);
		}
		else
		{
			gl::drawLine(mPoint1, mPoint2);
			gl::drawLine(mPoint2, mPoint3);
			gl::drawLine(mPoint3, mPoint1);

			gl::color(1.0, 1.0, 1.0, 1.0);
		}
	}

	vec2 getPoint1()
	{
		return mPoint1;
	}

	vec2 getPoint2()
	{
		return mPoint2;
	}

	vec2 getPoint3()
	{
		return mPoint3;
	}

	vec2 getBaseCenter()
	{
		return mBaseCenter;
	}

private:
	vec2 mPoint1;
	vec2 mPoint2;
	vec2 mPoint3;
	vec2 mBaseCenter;
};

#endif