#pragma once
#include "cinder/app/App.h"

using namespace cinder;

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