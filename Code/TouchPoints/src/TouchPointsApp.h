#pragma once
#include "cinder/app/App.h"
#include "cinder/System.h"
#include "cinder/Log.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppBase.h"
#include <chrono>
#include <vector>
#include <map>
#include <list>
#include "SymmetryLine.h"
#include "Illustrator.h"
#include "Enums.h"
#include "Brush.h"
#include "UserInterface.h"
#include "ImageHandler.h"
#include "RealSenseHandler.h"
#include "libusb.h"
#include "DeviceHandler.h"
#include "EyeXHandler.h"
#include "LeapMotionHandler.h"

using namespace ci;
using namespace std;
using namespace cinder;
using namespace cinder::app;

namespace touchpoints { namespace app 
{
#define FRAME_RATE 120

	//TouchPointsApp, our libcinder app!
	class TouchPointsApp : public App
	{
	public:
		void setup() override;

		static void prepareSettings(TouchPointsApp::Settings* settings);

		void touchesBegan(TouchEvent event) override;
		void touchesMoved(TouchEvent event) override;
		void touchesEnded(TouchEvent event) override;
		void keyDown(KeyEvent event) override;
		void drawRadial();
		void setDefaultMode(Mode::DefaultModes mode);

		int windowWidth = getWindowSize().x;
		int windowHeight = getWindowSize().y;

		bool processing = false;
		bool radialActive = false;
		gl::TextureRef imageTexture;
		/*Code for Proximity Menu*/
		bool proxActive = false;
		float lineSize = 1.0f;
		float r, g, b = 256.0f;
		int currShape = 0;
		bool lockCurrentFrame = false;
		bool imageFlag = false;
		bool isDrawing = false;
		bool leapDrawFlag = true;

		//List of drawUI Flags
		bool modeButtons = true;
		bool colorButtons = false;
		bool shapeButtons = false;
		bool uiFboFlag = false;
		bool layerVisualization = false;
		bool startUpFlag = true;

		//Variables to keep track of performance. It is cleared once every second.
		int frames = 0;
		int fps = 0;

		void update() override;
		void draw() override;
	private:
		//Setup
		bool setupComplete = false;
		//Needs organizing
		drawing::Brush brush;
		drawing::Illustrator illustrator;
		ui::UserInterface gui;
		devices::DeviceHandler deviceHandler;
		drawing::ImageHandler imageHandler;
		devices::RealSenseHandler realSenseHandler;
		devices::EyeXHandler eyeXHandler;
		devices::LeapMotionHandler leapMotionHandler;

		//Keeps time for the last time we checked for connected or disconnected devices
		std::chrono::milliseconds lastDeviceCheck;

		vec2 radialCenter = vec2(windowWidth * .5, windowHeight * .5);

		//Symmetry lines
		drawing::SymmetryLine mySymmetry;

		//Gesture functions for multitouch
		bool findMultiTouchGestures(TouchEvent::Touch previousPoint, TouchEvent::Touch currentPoint);

		//Variables for point buffering
		map<uint32_t, list<TouchEvent::Touch>> bufferTouches;
		TouchEvent::Touch previousTouch;

		//Need to redo this area. We should have a list for active points, this list will contain ALL shapes.
		//Perhaps Templates? Need to attend C++ Workshop for this.
		//The touchPoint should have a variable 'shape' which just tells it if it is a square, circle, triangle, line, etc.
		//Keep the 'eraser' points though because they need to be drawn separately last!
		//Might need a new function for the eraser mode so it can ALWAYS have the correct background color.
		//Future playback feature may want eraser points in the same list as the other shapes as well.

		list<drawing::TouchShape*> myShapes;
		int activeDrawings;

		map<uint32_t, drawing::TouchPoint> myActivePoints;
		list<drawing::TouchPoint> myPoints;
		map<uint32_t, drawing::TouchPoint> myActivePointsEraser;
		list<drawing::TouchPoint> myPointsEraser;
		map<uint32_t, drawing::TouchCircle> myActiveCirclesEraser;
		list<drawing::TouchCircle> myCirclesEraser;
		map<uint32_t, drawing::TouchCircle> myActiveCircles;
		list<drawing::TouchCircle> myCircles;
		map<uint32_t, drawing::TouchRectangle> myActiveRectangles;
		list<drawing::TouchRectangle> myRectangles;
		map<uint32_t, drawing::TouchTriangle> myActiveTriangles;
		list<drawing::TouchTriangle> myTriangles;

		//Fbo's for Layering.
		//We may want to set a vector of framebuffers?
		//This way we can dynamically have multiple layers....????
		shared_ptr<gl::Fbo> firstFbo;
		shared_ptr<gl::Fbo> secondFbo;
		shared_ptr<gl::Fbo> thirdFbo;
		shared_ptr<gl::Fbo> activeFbo;
		shared_ptr<gl::Fbo> iconFbo;
		shared_ptr<gl::Fbo> saveImageFbo;

		shared_ptr<gl::Fbo> backgroundFbo;
		vector<std::shared_ptr<gl::Fbo>> layerList;
		vector<float> layerAlpha;

		//Fbo for UserInterface
		shared_ptr<gl::Fbo> uiFbo;
		shared_ptr<gl::Fbo> imageFbo;
		shared_ptr<gl::Fbo> radialFbo;
		//Proxy Menu Fbo
		shared_ptr<gl::Fbo> proxFbo;
		shared_ptr<gl::Fbo> fingerLocationFbo;
	};
}}