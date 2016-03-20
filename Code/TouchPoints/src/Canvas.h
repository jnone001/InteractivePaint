#ifndef CANVAS_H
#define CANVAS_H
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
#include "SymmetryLine.h"
enum Shape { Line, Circle, Rectangle, Triangle };
struct Brush{

	Brush(){}
	//Creates a brush
	Brush(Shape shape, ColorA color, int lineSize, bool filledShapes, bool randColor, SymmetryLine mySymmetry){
		mShape = shape;
		mColor = color;
		mLineSize = lineSize;
		mFilledShapes = filledShapes;
		mRandColor = randColor;
		mMySymmetry = mySymmetry;
	}


	TouchPoint symmetricLine(TouchPoint line);
	TouchCircle symmetricCircle(TouchCircle circle);
	TouchRectangle symmetricRectangle(TouchRectangle rectangle);
	TouchTriangle symmetricTriangle(TouchTriangle triangle);
	vec2 symmetricPoint(vec2 point);

	//Functions to get brush values
	ColorA getColor();
	int getLineSize();
	Shape getShape();
	bool getFilledShapes();
	SymmetryLine getSymmetry();
	bool getRandColor();
	//Change brush 
	void changeColor(ColorA color);
	void changeLineSize(int lineSize);
	void changeShape(Shape shape);
	void changeFilledShapes(bool filledShapes);
	void voidChangeRandColor(bool randColor);



private:
	ColorA mColor;
	int mLineSize;
	Shape mShape;
	bool mFilledShapes;
	SymmetryLine mMySymmetry;
	bool mRandColor;


};

#endif