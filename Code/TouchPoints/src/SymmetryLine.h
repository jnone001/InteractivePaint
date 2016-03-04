#ifndef SYMMETRYLINE_H
#define SYMMETRYLINE_H
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

#include "TouchShapes.h"

struct SymmetryLine{

	SymmetryLine(){}
	//Creates a symmetric line about the 
	SymmetryLine(float x, bool ySymmetric){
		if (ySymmetric){
			point1 = vec2(x, 0);
			point2 = vec2(x, 1);
		}
		else{
			point1 = vec2(0, x);
			point2 = vec2(1, x);
		}

	}

	TouchPoint symmetricLine(TouchPoint line);
	TouchCircle symmetricCircle(TouchCircle circle);
	TouchRectangle symmetricRectangle(TouchRectangle rectangle);
	TouchTriangle symmetricTriangle(TouchTriangle triangle);
	vec2 symmetricPoint(vec2 point);


private:
	vec2 point1;
	vec2 point2;
	//float slope;


};

TouchPoint SymmetryLine::symmetricLine(TouchPoint line){

	auto points = line.getPointList();
	vec2 firstPoint = SymmetryLine::symmetricPoint(points.front());
	TouchPoint symmetricTouch(firstPoint, line.getColor(), line.getSize());
	for (auto touches : points){
		symmetricTouch.addPoint(SymmetryLine::symmetricPoint(touches));
	}

	return symmetricTouch;


};

vec2 SymmetryLine::symmetricPoint(vec2 point){
	if (point1.x - point2.x == 0){
		auto symmetricX = point1.x - (point.x - point1.x);
		return vec2(symmetricX, point.y);
	}
	else if (point1.y - point2.y == 0)
	{
		auto symmetricY = point1.y - (point.y - point1.y);
		return vec2(point.x, symmetricY);
	}
	return vec2(0, 0);

};

TouchCircle SymmetryLine::symmetricCircle(TouchCircle circle){

	vec2 symCenter = SymmetryLine::symmetricPoint(circle.getCenter());
	bool x = circle.getFilledShape();
	TouchCircle symmetricCircle(symCenter, circle.getRadius(), circle.getColor(), circle.getSize(), x);
	return symmetricCircle;
}

TouchRectangle SymmetryLine::symmetricRectangle(TouchRectangle rectangle){

	vec2 symUpperLeft = SymmetryLine::symmetricPoint(vec2(rectangle.upperLeftX(), rectangle.upperLeftY()));
	vec2 symLowerRight = SymmetryLine::symmetricPoint(vec2(rectangle.lowerRightX(), rectangle.lowerRightY()));

	int x1 = symUpperLeft.x;
	int y1 = symUpperLeft.y;
	int x2 = symLowerRight.x;
	int y2 = symLowerRight.y;

	bool symBool = rectangle.getFilledShape();

	TouchRectangle symmetricRectangle(x1, y1, x2, y2, rectangle.getColor(), rectangle.getSize(), symBool);
	return symmetricRectangle;

}

TouchTriangle SymmetryLine::symmetricTriangle(TouchTriangle triangle){

	vec2 symPoint1 = symmetricPoint(triangle.getPoint1());
	vec2 symPoint2 = symmetricPoint(triangle.getPoint2());
	vec2 symPoint3 = symmetricPoint(triangle.getPoint3());
	vec2 symBaseCenter = symmetricPoint(triangle.getBaseCenter());
	bool symBool = triangle.getFilledShape();

	TouchTriangle symmetricTriangle(symPoint1, symPoint2, symPoint3, symBaseCenter, triangle.getColor(), triangle.getSize(), symBool);
	return symmetricTriangle;
}

#endif