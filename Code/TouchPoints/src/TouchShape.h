#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

namespace touchpoints { namespace drawing
{
	struct TouchShape
	{
	public:
		virtual void draw() = 0;
		float size();
		ColorA getColor();
		bool getFilledShape();
		float getSize();
	protected:
		bool mFilledShapes;
		ColorA mColor;
		float mSize;
	};
}}