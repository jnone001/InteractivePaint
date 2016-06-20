#include "DrawEvent.h"

namespace touchpoints { namespace drawing
{
	DrawEvent::DrawEvent() : hasStartPoint(false), hasEndPoint(false), maxLifeSpan(0), currentAge(0) {}

	DrawEvent::DrawEvent(vec2 startPoint, Guid guid, bool shouldFinalizeShape, int maxLifeSpan) :
		startPoint(startPoint), shapeGuid(guid), shouldFinalizeShape(shouldFinalizeShape),
		hasStartPoint(true), hasEndPoint(false), maxLifeSpan(maxLifeSpan), currentAge(0) {}

	DrawEvent::DrawEvent(vec2 startPoint, Guid guid, bool shouldFinalizeShape, int maxLifeSpan, int currentAge) :
		startPoint(startPoint), shapeGuid(guid), shouldFinalizeShape(shouldFinalizeShape),
		hasStartPoint(true), hasEndPoint(false), maxLifeSpan(maxLifeSpan), currentAge(currentAge) {}

	DrawEvent::DrawEvent(vec2 startPoint, vec2 parentStartPoint, Guid guid, bool shouldFinalizeShape, int maxLifeSpan) :
		startPoint(startPoint), parentStartPoint(parentStartPoint), shapeGuid(guid), shouldFinalizeShape(shouldFinalizeShape),
		hasStartPoint(true), hasEndPoint(false), maxLifeSpan(maxLifeSpan), currentAge(0) {}

	DrawEvent::DrawEvent(vec2 startPoint, vec2 parentStartPoint, Guid guid, bool shouldFinalizeShape,
		int maxLifeSpan, int currentAge) : startPoint(startPoint), parentStartPoint(parentStartPoint), 
		shapeGuid(guid), shouldFinalizeShape(shouldFinalizeShape), hasStartPoint(true), 
		hasEndPoint(false), maxLifeSpan(maxLifeSpan), currentAge(currentAge) {}
	
	void DrawEvent::SetEndPoint(vec2 value)
	{
		endPoint = value;
		hasEndPoint = true;
	}
}}