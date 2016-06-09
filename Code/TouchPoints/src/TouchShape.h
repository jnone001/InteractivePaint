#pragma once
#include "cinder/app/App.h"
#include "IDrawable.h"

using namespace cinder;

namespace touchpoints { namespace drawing
{
	struct TouchShape : public IDrawable
	{
	public:
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