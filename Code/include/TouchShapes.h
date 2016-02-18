
//Cinder and OpenGL Includes
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/gl/Fbo.h"

//Leap Includes
#include "Leap.h"
#include "LeapMath.h"


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


protected:
	bool			mFilledShapes;
	Color			mColor;
	float			mSize;

};

//Struct for listing points, draws lines. 
struct TouchPoint : public TouchShape
{
	TouchPoint(const vec2 &initialPt, const Color &color, const float &size);
	void addPoint(vec2 &pt);
	void draw() override;

private :
	list<vec2> pointList;
};


//Struct for creating circles
struct TouchCircle : public TouchShape
{
	TouchCircle();
	TouchCircle(const vec2 &center, const float &radius, const Color &color, const float &size, bool &filledShapes);
	float getCenterX;
	float getCenterY;
	void changeRadius(float &radius);
	void draw() override;

private:
	float mRadius;
	vec2 mCenter;
};

//Struct for creating rectangles
struct TouchRectangle : public TouchShape
{
	TouchRectangle(const int &x1, const int &y1, const int &x2, const int &y2, const Color &color, const float &size, bool &filledShapes);
	void changePoint(int x, int y);
	int upperLeftX();
	int upperLeftY();
	void draw() override;

private:
	int mUpperLeftX;
	int mUpperLeftY;
	int mLowerRightX;
	int mLowerRightY;
};

//Struct for creating triangles
struct TouchTriangle : public TouchShape
{

	TouchTriangle(const vec2 &point1, const vec2 &point2, const vec2 &point3, const vec2 &baseCenter, const Color &color, const float &size, bool &filledShapes);
	void changeSize(float x, float y);
	void draw() override;


private:
	vec2 mPoint1;
	vec2 mPoint2;
	vec2 mPoint3;
	vec2 mBaseCenter;
};


