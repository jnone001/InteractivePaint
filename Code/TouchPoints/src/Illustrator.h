#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/Fbo.h"
#include <vector>
#include <list>
#include "AllShapes.h"
#include "Brush.h"
#include <stack>
#include <queue>
#include "DrawEvent.h"
#include <unordered_map>
#include "GuidHash.h"

using namespace std;

namespace touchpoints { namespace drawing
{
	struct Illustrator
	{
	public:
		Illustrator();
		Illustrator(Brush* brush, vector<shared_ptr<gl::Fbo>>* layerList);
		void beginTouchShapes(uint32_t myId, vec2 myPos);
		void movingTouchShapes(uint32_t myId, vec2 myPos, vec2 prevPos);
		void endTouchShapes(uint32_t myId);
		void missedPoints(int xi, int yi, int xf, int yf, TouchPoint& points);
		int getNumberOfActiveDrawings();
		//Time Machine
		void saveCurrentFbo();
		void undoDraw(Color background);
		void clearTimeMachine();
		void drawActiveShapes();

		//manually add shapes to draw
		void addToActiveCircles(TouchCircle activeCircle, Guid key);
		void addToTemporaryCircles(TouchCircle tempCircle);
		void addToTemporaryCircles(vector<TouchCircle> tempCircles);
		void addToActiveTriangles(TouchVerticalIsoscelesTriangle activeTriangle, Guid key);
		void addToTemporaryTriangles(TouchVerticalIsoscelesTriangle tempTriangle);
		void addToTemporaryTriangles(vector<TouchVerticalIsoscelesTriangle> tempTriangles);
		void addToActiveRectangles(TouchRectangle activeRectangle, Guid key);
		void addToTemporaryRectangles(TouchRectangle tempRectangle);
		void addToTemporaryRectangles(vector<TouchRectangle> tempRectangles);
		void addToActivePoints(TouchPoint activePoints, Guid key);
		void addToTemporaryPoints(TouchPoint tempPoints);
		void addToTemporaryPoints(vector<TouchPoint> tempPoints);
		
		void addDrawEventToQueue(DrawEvent event);
		void addDrawEventsToQueue(vector<DrawEvent> events);
		void processDrawEventQueue();
		//draws all active shapes
		void drawActive() const;
		//draws all temporary shapes and immediatedly disposes of them
		void drawTemporary();
	private:
		void circleEventHandler(DrawEvent event);
		void triangleEventHandler(DrawEvent event);
		void rectangleEventHandler(DrawEvent event);
		void lineEventHandler(DrawEvent event);

		vector<shared_ptr<gl::Fbo>>* mLayerList;
		Brush* mBrush;
		int numberOfActiveDrawings;

		GuidGenerator guidGenerator;

		//A hash map of each shape that is drawn each frame
		//...and also a vector of temporary shapes drawn only one frame
		unordered_map<Guid, TouchCircle> activeCirclesMap;
		vector<TouchCircle> temporaryCircles;
		unordered_map<Guid, TouchVerticalIsoscelesTriangle> activeTrianglesMap;
		vector<TouchVerticalIsoscelesTriangle> temporaryTriangles;
		unordered_map<Guid, TouchRectangle> activeRectanglesMap;
		vector<TouchRectangle> temporaryRectangles;
		unordered_map<Guid, TouchPoint> activePointsMap;
		vector<TouchPoint> temporaryPoints;

		queue<DrawEvent> drawEventQueue;

		map<uint32_t, TouchPoint> myActivePoints;
		vector<TouchPoint> myPoints;
		map<uint32_t, TouchCircle> myActiveCirclesEraser;
		vector<TouchCircle> myCirclesEraser;
		map<uint32_t, TouchCircle> myActiveCircles;
		vector<TouchCircle> myCircles;
		map<uint32_t, TouchRectangle> myActiveRectangles;
		vector<TouchRectangle> myRectangles;
		map<uint32_t, TouchVerticalIsoscelesTriangle> myActiveTriangles;
		vector<TouchVerticalIsoscelesTriangle> myTriangles;

		//TimeMachine 
		map<shared_ptr<gl::Fbo>, list<shared_ptr<gl::Fbo>>> myTimeMachine;
		//Stack that tells which item in map to pop
		stack<shared_ptr<gl::Fbo>> undoOrder;
		int undoCount = 0;
	};
}}
