#include "VerticalIsoscelesTriangle.h"
#include "Line.h"

namespace touchpoints { namespace math
{
	VerticalIsoscelesTriangle::VerticalIsoscelesTriangle(vec2 oneCorner, vec2 oppositeCorner, bool isPointingDown) : isPointingDown(isPointingDown)
	{
		boundingRectangle = Rect(oneCorner, oppositeCorner);

		auto topMidPoint = FindMidPoint(boundingRectangle.GetTopLeftCorner(), boundingRectangle.GetTopRightCorner());
		auto bottomMidPoint = FindMidPoint(boundingRectangle.GetBottomLeftCorner(), boundingRectangle.GetBottomRightCorner());

		if(isPointingDown)
		{
			baseVertexLeft = boundingRectangle.GetTopLeftCorner();
			baseVertexRight = boundingRectangle.GetTopRightCorner();
			oppositeBaseVertex = bottomMidPoint;
			baseCenter = topMidPoint;
		}
		else
		{
			baseVertexLeft = boundingRectangle.GetBottomLeftCorner();
			baseVertexRight = boundingRectangle.GetBottomRightCorner();
			oppositeBaseVertex = topMidPoint;
			baseCenter = bottomMidPoint;
		}
	}
}}
