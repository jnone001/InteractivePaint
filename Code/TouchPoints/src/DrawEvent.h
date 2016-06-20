#pragma once
#include "cinder/Vector.h"
#include "Guid.h"

using namespace cinder;

namespace touchpoints { namespace drawing
{
	class DrawEvent
	{
	public:
		DrawEvent();
		DrawEvent(vec2 startPoint, Guid guid, bool shouldFinalizeShape, int maxLifeSpan);
		DrawEvent(vec2 startPoint, Guid guid, bool shouldFinalizeShape, int maxLifeSpan, int currentAge);
		DrawEvent(vec2 startPoint, vec2 parentStartPoint, Guid guid, bool shouldFinalizeShape, int maxLifeSpan);
		DrawEvent(vec2 startPoint, vec2 parentStartPoint, Guid guid, bool shouldFinalizeShape, int maxLifeSpan, int currentAge);
		inline vec2 GetStartPoint() { return startPoint; }
		inline vec2 GetEndPoint() { return endPoint; }
		inline vec2 GetParentStartPoint() { return parentStartPoint; }
		inline Guid GetShapeGuid() { return shapeGuid; }
		inline bool ShouldFinalizeShape() { return shouldFinalizeShape; }
		inline bool HasStartPoint() { return hasStartPoint; }
		inline bool HasEndPoint() { return hasEndPoint; }
		inline void IncrementCurrentAge() { currentAge++; }
		inline bool ShouldBeAlive() { return maxLifeSpan >= currentAge; }
		inline void ResetCurrentAge() { currentAge = 0; }
		void SetEndPoint(vec2 value);
	private:
		vec2 startPoint;
		vec2 endPoint;
		//for shapes that should draw based off of where an initial draw event began
		//e.g. shapes that havent been finilized yet
		vec2 parentStartPoint;
		//id to group together a sequence of DrawEvents
		Guid shapeGuid;
		//flag to let the illustrator know whether to this shape should be commited
		bool shouldFinalizeShape;
		bool hasStartPoint;
		bool hasEndPoint;
		//this is to help you manage the lifespan of this object, you can increment it every update loop
		//and make sure it hasnt lived past the maxLifeSpan you sent in the constructor by calling ShouldBeALive()
		int currentAge;
		int maxLifeSpan;
	};
}}
