#pragma once
#include "cinder/app/App.h"
#include <list>
#include "AllShapes.h"

namespace touchpoints { namespace drawing
{
	struct SymmetryLine
	{
	public:
		SymmetryLine();
		SymmetryLine(float x, bool ySymmetric);
		TouchPoint symmetricLine(TouchPoint line);
		TouchCircle symmetricCircle(TouchCircle circle);
		TouchRectangle symmetricRectangle(TouchRectangle rectangle);
		TouchTriangle symmetricTriangle(TouchTriangle triangle);
		vec2 symmetricPoint(vec2 point);
		void toggleSymmetry();
		bool getSymmetryOn();

	private:
		vec2 point1;
		vec2 point2;
		bool symmetryOn;
	};
}}
