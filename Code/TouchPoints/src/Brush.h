#pragma once
#include "cinder/app/App.h"
#include <vector>
#include "SymmetryLine.h"
#include "Enums.h"

namespace drawing {
	struct Brush {
	public:
		Brush();
		Brush(Shape::Shape shape, cinder::ColorA color, float alphaColor, int lineSize, bool filledShapes, bool randColor, bool eraserMode, SymmetryLine *mySymmetry);
		cinder::ColorA getColor();
		int getLineSize();
		Shape::Shape getShape();
		bool getFilledShapes();
		SymmetryLine* getSymmetry();
		bool getRandColor();
		bool getEraserMode();
		float getAlphaColor();
		//Change brush 
		void changeColor(cinder::ColorA color);
		void changeLineSize(int lineSize);
		void increaseLineSize();
		void decreaseLineSize();
		void changeShape(Shape::Shape shape);
		void changeFilledShapes(bool filledShapes);
		void incrementShape();
		void changeRandColor(bool randColor);
		void changeEraserMode(bool eraserMode);
		void changeAlphaColor(float alphaColor);
		void increaseAlpha();
		void decreaseAlpha();
		void incrementColor();
		void changeStaticColor(ourColors::ourColors staticColor);
		int getCurrentColor();
		int getCurrentShape();
		void Brush::decrementColor();
		std::vector<Color> getColorList();

	private:
		cinder::ColorA mColor;
		int mLineSize;
		Shape::Shape mShape;
		bool mFilledShapes;
		SymmetryLine* mMySymmetry;
		bool mRandColor;
		bool mEraserMode;
		float mAlphaColor;
		ourColors::ourColors mStaticColor;
		std::vector<Color> colorList;
		int currShape = 0;
		int currColor = 0;
	};
}