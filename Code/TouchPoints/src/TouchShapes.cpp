
//Cinder and OpenGL Includes
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/gl/Fbo.h"

//Our own includes
//#include "TouchShapes.h"


//Standard Library Includes
#include <vector>
#include <map>
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;



struct TouchShape {
	//TouchShape();
	virtual void draw() = 0;
	float size(){
		return mSize;
	}


protected:
	bool			mFilledShapes;
	ColorA			mColor;
	float			mSize;

};

//Struct for listing points, draws lines. 
struct TouchPoint : public TouchShape
{
	TouchPoint(){}
	TouchPoint(const vec2 &initialPt, const ColorA &color, const float &size)
	{
		//mLine.push_back(initialPt);
		pointList.emplace_back(initialPt);
		mColor = color;
		mSize = size;
	}

	void addPoint(const vec2 &pt) {
		//mLine.push_back(pt);
		pointList.emplace_back(pt);
	}

	void clearPoints(){
		pointList.clear();
	}

	vec2 getFirstPoint(){
		return pointList.front();
	}

	void draw() override
	{

		//gl::drawSolidCircle(getWindowCenter(), 100);
		//gl::drawSolidRect(Rectf(-0.5, -0.5, 500, 500));


		gl::lineWidth(mSize);
		//auto allPoints = mLine.getPoints();
		gl::color(mColor);
		
		for (auto points : pointList){
			gl::drawSolidCircle(points, mSize);
		}

		
		

		/*
		gl::begin(GL_LINE_STRIP);
		for (const vec2 &point : pointList) {
			gl::vertex(point);
		}
		gl::end();
		*/

		//gl::draw(mLine);

		//	auto oldVertex = allPoints.
		/*
		auto oldVertex = mLine.
		while (mLine.next())
		gl::drawLine()

		for (const auto &touch : event.getTouches())
		*/
	}


private:
	list<vec2> pointList;
	//PolyLine2f mLine;
};


//Struct for creating circles
struct TouchCircle : public TouchShape
{
	TouchCircle(){}
	TouchCircle(const vec2 &center, const float &radius, const ColorA &color, const float &size, bool &filledShapes)
		//:  mRadius(radius), mCenter(center)
	{
		//mLine.push_back(initialPt);
		mRadius = radius;
		mCenter = center;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
	}

	//void addPoint(const vec2 &pt) { mLine.push_back(pt); }
	void changeRadius(float &radius){
		mRadius = radius;
	}
	
	void changePosition(vec2 newCenter)
	{
		mCenter = newCenter;
	}

	float getCenterX(){
		return mCenter.x;
	}
	float getCenterY(){
		return mCenter.y;
	}
	void draw() override
	{
		//gl::lineWidth(mSize);
		gl::color(mColor);
		if (mFilledShapes){
			gl::drawSolidCircle(mCenter, mRadius);
		}
		else gl::drawStrokedCircle(mCenter, mRadius, mSize*2.0f);
		

		gl::color(1.0, 1.0, 1.0, 1.0);

	}

private:
	float mRadius;
	vec2 mCenter;
};

//Struct for creating rectangles
struct TouchRectangle : public TouchShape
{
	TouchRectangle(){}
	TouchRectangle(const int &x1, const int &y1, const int &x2, const int &y2, const ColorA &color, const float &size, bool &filledShapes)
		//: mUpperLeftX(x1), mUpperLeftY(y1), mLowerRightX(x2), mLowerRightY(y2)
	{
		//mLine.push_back(initialPt);
		mUpperLeftX = x1;
		mUpperLeftY = y1;
		mLowerRightX = x2;
		mLowerRightY = y2;
		mColor = color;
		mSize = size;
		mFilledShapes = filledShapes;
	}

	//void addPoint(const vec2 &pt) { mLine.push_back(pt); }
	void changePoint(int x, int y){
		mLowerRightX = x;
		mLowerRightY = y;
	}

	int upperLeftX(){
		return mUpperLeftX;
	}

	int upperLeftY(){
		return mUpperLeftY;
	}

	void draw() override
	{
		//gl::lineWidth(mSize);
		gl::color(mColor);
		if (mFilledShapes){
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
	TouchTriangle(){}
	TouchTriangle(const vec2 &point1, const vec2 &point2, const vec2 &point3, const vec2 &baseCenter, const ColorA &color, const float &size, bool &filledShapes)
		//: mPoint1(point1), mPoint2(point2), mPoint3(point3), mBaseCenter(baseCenter)
	{
		//mLine.push_back(initialPt);
		mPoint1 = point1;
		mPoint2 = point2;
		mPoint3 = point3;
		mBaseCenter = baseCenter;
		mColor = color;
		mSize = size;
		if (mSize == 1)
			mSize = 2;
		mFilledShapes = filledShapes;
	}

	//void addPoint(const vec2 &pt) { mLine.push_back(pt); }
	void changeSize(float x, float y){
		mPoint1.y = y;
		mPoint2.x = x;
		mPoint3.x = mBaseCenter.x + (mBaseCenter.x - mPoint2.x);
	}

	void draw() override
	{
		
		gl::lineWidth(mSize);
		gl::color(mColor);
		if (mFilledShapes){
			gl::drawSolidTriangle(mPoint1, mPoint2, mPoint3);
		}
		else
		{
			
			gl::drawLine(mPoint1, mPoint2);
			gl::drawLine(mPoint2, mPoint3);
			gl::drawLine(mPoint3, mPoint1);



			gl::color(1.0, 1.0, 1.0, 1.0);
			/*
			TouchPoint uiTouchPoint(mPoint1, mColor, mSize);
			uiTouchPoint.addPoint(mPoint2);
			uiTouchPoint.addPoint(mPoint3);
			uiTouchPoint.addPoint(mPoint1);

			missedPoints123(mPoint1.x, mPoint1.y, mPoint2.x, mPoint2.y, uiTouchPoint);
			missedPoints123(mPoint1.x, mPoint1.y, mPoint2.x, mPoint2.y, uiTouchPoint);
			missedPoints123(mPoint1.x, mPoint1.y, mPoint2.x, mPoint2.y, uiTouchPoint);
			uiTouchPoint.draw();
			*/
			


			//gl::drawSolidTriangle(mPoint1, mPoint2, mPoint3);
		}

	}
private:
	vec2 mPoint1;
	vec2 mPoint2;
	vec2 mPoint3;
	vec2 mBaseCenter;
};