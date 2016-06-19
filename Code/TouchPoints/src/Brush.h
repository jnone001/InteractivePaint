#pragma once
#include "cinder/app/App.h"
#include <vector>
#include "SymmetryLine.h"
#include "Enums.h"

namespace touchpoints { namespace drawing
{
	struct Brush
	{
	public:
		Brush();
		Brush(Shape::Shape shape, ColorA color, float alphaColor, int lineSize, bool filledShapes, bool randColor, bool eraserMode, SymmetryLine* mySymmetry);
		ColorA getColor();
		int getLineSize();
		Shape::Shape getShape();
		bool getFilledShapes();
		SymmetryLine* getSymmetry();
		bool getRandColor();
		bool IsEraserActive();
		float getAlphaColor();
		//Change brush 
		void changeColor(ColorA color);
		void changeLineSize(int lineSize);
		void increaseLineSize();
		void decreaseLineSize();
		void changeShape(Shape::Shape shape);
		void changeFilledShapes(bool filledShapes);
		void incrementShape();
		void changeRandColor(bool randColor);
		void activateEraser();
		void deactivateEraser();
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
		ColorA mColor;
		int mLineSize;
		Shape::Shape mShape;
		bool mFilledShapes;
		SymmetryLine* mMySymmetry;
		bool mRandColor;
		bool isEraserActive;
		float mAlphaColor;
		ourColors::ourColors mStaticColor;
		std::vector<Color> colorList;
		int currShape = 0;
		int currColor = 0;
	};
}}
