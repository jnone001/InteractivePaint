#include "TouchVerticalIsoscelesTriangle.h"
#include <cinder/gl/Context.h>
#include <cinder/gl/draw.h>

using namespace cinder;

namespace touchpoints { namespace drawing
{
	TouchVerticalIsoscelesTriangle::TouchVerticalIsoscelesTriangle() {}

	TouchVerticalIsoscelesTriangle::TouchVerticalIsoscelesTriangle(vec2 baseVertexLeft, vec2 baseVertexRight, vec2 oppositeBaseVertex, 
		vec2 baseCenter, ColorA color, float size, bool filledShapes) : baseVertexLeft(baseVertexLeft), baseVertexRight(baseVertexRight),
		oppositeBaseVertex(oppositeBaseVertex), baseCenter(baseCenter)
	{
		mColor = color;
		mSize = size;
		if (mSize == 1) mSize = 2;
		mFilledShapes = filledShapes;
		this->framesDrawn = 1;
	}

	TouchVerticalIsoscelesTriangle::TouchVerticalIsoscelesTriangle(vec2 baseVertexLeft, vec2 baseVertexRight, vec2 oppositeBaseVertex, 
		vec2 baseCenter, ColorA color, float size, bool filledShapes, int framesDrawn) : baseVertexLeft(baseVertexLeft), 
		baseVertexRight(baseVertexRight), oppositeBaseVertex(oppositeBaseVertex), baseCenter(baseCenter)
	{
		mColor = color;
		mSize = size;
		if (mSize == 1) mSize = 2;
		mFilledShapes = filledShapes;
		this->framesDrawn = framesDrawn;
	}

	void TouchVerticalIsoscelesTriangle::draw()
	{
		gl::lineWidth(mSize);
		gl::color(mColor);
		if (mFilledShapes)
		{
			gl::drawSolidTriangle(baseVertexLeft, baseVertexRight, oppositeBaseVertex);
		}
		else
		{
			gl::drawLine(baseVertexLeft, baseVertexRight);
			gl::drawLine(baseVertexRight, oppositeBaseVertex);
			gl::drawLine(oppositeBaseVertex, baseVertexLeft);

			gl::color(1.0, 1.0, 1.0, 1.0);
		}
	}
}}
