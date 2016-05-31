#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/Fbo.h"
#include <vector>
#include <list>
#include "AllShapes.h"
#include "Brush.h"
#include <stack>

using namespace std;

namespace touchpoints {
	namespace drawing {
		struct Illustrator
		{
		public:
			Illustrator();
			Illustrator(Brush* brush, vector<shared_ptr<gl::Fbo>>* layerList);
			void beginTouchShapes(uint32_t myId, vec2 myPos);
			void movingTouchShapes(uint32_t myId, vec2 myPos, vec2 prevPos);
			void endTouchShapes(uint32_t myId);
			void missedPoints(int xi, int yi, int xf, int yf, TouchPoint& points);
			int getActiveDrawings();
			//Time Machine
			void saveCurrentFbo();
			void undoDraw(Color background);
			void clearTimeMachine();
			void drawActiveShapes();

		private:
			vector<shared_ptr<gl::Fbo>>* mLayerList;
			Brush* mBrush;
			int activeDrawings;
			map<uint32_t, TouchPoint> myActivePoints;
			list<TouchPoint> myPoints;
			map<uint32_t, TouchPoint> myActivePointsEraser;
			list<TouchPoint> myPointsEraser;
			map<uint32_t, TouchCircle> myActiveCirclesEraser;
			list<TouchCircle> myCirclesEraser;
			map<uint32_t, TouchCircle> myActiveCircles;
			list<TouchCircle> myCircles;
			map<uint32_t, TouchRectangle> myActiveRectangles;
			list<TouchRectangle> myRectangles;
			map<uint32_t, TouchTriangle> myActiveTriangles;
			list<TouchTriangle> myTriangles;

			//TimeMachine 
			map<shared_ptr<gl::Fbo>, list<shared_ptr<gl::Fbo>>> myTimeMachine;
			//Stack that tells which item in map to pop
			stack<shared_ptr<gl::Fbo>> undoOrder;
			int undoCount = 0;
		};
	}
}