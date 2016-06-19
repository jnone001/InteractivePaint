#include "Brush.h"

namespace touchpoints { namespace drawing
{
	Brush::Brush() {}

	Brush::Brush(Shape::Shape shape, cinder::ColorA color, float alphaColor, int lineSize, bool filledShapes, bool randColor, bool isEraserActive, SymmetryLine* mySymmetry)
	{
		mShape = shape;
		mColor = color;
		mLineSize = lineSize;
		mFilledShapes = filledShapes;
		mRandColor = randColor;
		mMySymmetry = mySymmetry;
		isEraserActive = isEraserActive;
		mAlphaColor = alphaColor;
		mStaticColor = ourColors::ourColors::Black;
		//Fills our colorList with all our colors
		Color newColor1(0.0f, 0.0f, 0.0f);
		colorList.emplace_back(newColor1);
		Color newColor2(256.0f, 0.0f, 0.0f);
		colorList.emplace_back(newColor2);
		Color newColor3(256.0f, 256.0f, 0.0f);
		colorList.emplace_back(newColor3);
		Color newColor4(0.0f, 256.0f, 0.0f);
		colorList.emplace_back(newColor4);
		Color newColor5(0.0f, 256.0f, 256.0f);
		colorList.emplace_back(newColor5);
		Color newColor6(0.0f, 0.0f, 256.0f);
		colorList.emplace_back(newColor6);
		Color newColor7(256.0f, 0.0f, 256.0f);
		colorList.emplace_back(newColor7);
		Color newColor8(1.0f, 0.3f, 0.0);
		colorList.emplace_back(newColor8);
	}

	int drawing::Brush::getCurrentColor()
	{
		return currColor;
	}

	int Brush::getCurrentShape()
	{
		return mShape;
	}

	void Brush::incrementShape()
	{
		if (mShape == Shape::Shape::Line)
		{
			mShape = Shape::Shape::Circle;
		}
		else if (mShape == Shape::Shape::Circle)
		{
			mShape = Shape::Shape::Rectangle;
		}
		else if (mShape == Shape::Shape::Rectangle)
		{
			mShape = Shape::Shape::Triangle;
		}
		else if (mShape == Shape::Shape::Triangle)
		{
			if (mFilledShapes == true)
			{
				mShape = Shape::Shape::Line;
				mFilledShapes = false;
			}
			else
			{
				mFilledShapes = true;
				mShape = Shape::Shape::Circle;
			}
		}
	}

	std::vector<Color> Brush::getColorList()
	{
		return colorList;
	}

	ColorA Brush::getColor()
	{
		return mColor;
	}

	int Brush::getLineSize()
	{
		return mLineSize;
	}

	Shape::Shape Brush::getShape()
	{
		return mShape;
	}

	bool Brush::getFilledShapes()
	{
		return mFilledShapes;
	}

	SymmetryLine* Brush::getSymmetry()
	{
		return mMySymmetry;
	}

	bool Brush::getRandColor()
	{
		return mRandColor;
	}

	bool Brush::IsEraserActive()
	{
		return isEraserActive;
	}

	float Brush::getAlphaColor()
	{
		return mAlphaColor;
	}

	//Change brush 
	void Brush::changeColor(ColorA color)
	{
		mColor = color;
	}

	void Brush::changeLineSize(int lineSize)
	{
		mLineSize = lineSize;
	}

	void Brush::increaseLineSize()
	{
		if (mLineSize != 15.0f)
			mLineSize++;
	}

	void Brush::decreaseLineSize()
	{
		if (mLineSize != 1.0f)
			mLineSize--;
	}

	void Brush::changeShape(Shape::Shape shape)
	{
		mShape = shape;
	}

	void Brush::changeFilledShapes(bool filledShapes)
	{
		mFilledShapes = filledShapes;
	}

	void Brush::changeRandColor(bool randColor)
	{
		mRandColor = randColor;
	}

	void Brush::activateEraser()
	{
		isEraserActive = true;
	}

	void Brush::deactivateEraser()
	{
		isEraserActive = false;
	}

	void Brush::changeAlphaColor(float alphaColor)
	{
		mAlphaColor = alphaColor;
	}

	void Brush::increaseAlpha()
	{
		if (mAlphaColor != 1.0)
		{
			mAlphaColor += 0.1;
			mColor = ColorA(mColor, mAlphaColor);
		}
	}

	void Brush::decreaseAlpha()
	{
		if (mAlphaColor > 0.0)
		{
			mAlphaColor -= 0.1;
			mColor = ColorA(mColor, mAlphaColor);
		}
	}

	void Brush::incrementColor()
	{
		currColor++;
		if (currColor == colorList.size())
		{
			currColor = 0;
		}
		mColor = ColorA(colorList[currColor], mAlphaColor);
	}

	void Brush::decrementColor()
	{
		currColor--;
		if (currColor == -1)
		{
			currColor = colorList.size() - 1;
		}
		mColor = ColorA(colorList[currColor], mAlphaColor);
	}

	void Brush::changeStaticColor(ourColors::ourColors staticColor)
	{
		if (staticColor == ourColors::ourColors::White)
		{
			mStaticColor = ourColors::ourColors::White;
			mColor = ColorA(1.0, 1.0, 1.0, mAlphaColor);
		}
		if (staticColor == ourColors::ourColors::Red)
		{
			mStaticColor = ourColors::ourColors::Red;
			mColor = ColorA(1.0, 0.0, 0.0, mAlphaColor);
		}
		if (staticColor == ourColors::ourColors::Yellow)
		{
			mStaticColor = ourColors::ourColors::Yellow;
			mColor = ColorA(1.0, 1.0, 0.0, mAlphaColor);
		}
		if (staticColor == ourColors::ourColors::Green)
		{
			mStaticColor = ourColors::ourColors::Green;
			mColor = ColorA(0.0, 256.0, 0.0, mAlphaColor);
		}
		if (staticColor == ourColors::ourColors::Aqua)
		{
			mStaticColor = ourColors::ourColors::Aqua;
			mColor = ColorA(0.0, 1.0, 1.0, mAlphaColor);
		}
		if (staticColor == ourColors::ourColors::Blue)
		{
			mStaticColor = ourColors::ourColors::Blue;
			mColor = ColorA(0.0, 0.0, 1.0, mAlphaColor);
		}
		if (staticColor == ourColors::ourColors::Purple)
		{
			mStaticColor = ourColors::ourColors::Purple;
			mColor = ColorA(1.0, 0.0, 1.0, mAlphaColor);
		}
		if (staticColor == ourColors::ourColors::Orange)
		{
			mStaticColor = ourColors::ourColors::Orange;
			mColor = ColorA(1.0, 0.3, 0.0, mAlphaColor);
		}
		if (staticColor == ourColors::ourColors::Black)
		{
			mStaticColor = ourColors::ourColors::Black;
			mColor = ColorA(0.0, 0.0, 0.0, mAlphaColor);
		}
	}
}}
