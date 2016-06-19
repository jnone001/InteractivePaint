#pragma once
#include "Leap.h"
#include "cinder/Vector.h"
#include "ImageHandler.h"
#include "UserInterface.h"
#include "DrawEvent.h"

#define TOTAL_SYMBOLS 7

using namespace cinder;

namespace touchpoints { namespace devices
{
	class LeapMotionHandler
	{
	public:
		LeapMotionHandler();
		LeapMotionHandler(int windowWidth, int windowHeight);

		void InitLeapMotion();
		void enableGest(Leap::Controller controller);
		void leapSave(ui::UserInterface& gui, drawing::ImageHandler& imageHandler);
		void leapColorChange(drawing::Brush& brush, ui::UserInterface& gui,
		    drawing::ImageHandler& imageHandler);
		void leapShapeChange(int& currShape, bool& imageFlag, drawing::Brush& brush,
		    ui::UserInterface& gui, drawing::ImageHandler& imageHandler);
		void leapDraw(bool& lockCurrentFrame, bool proxActive, drawing::Illustrator& illustrator);
		void gestRecognition(bool isDrawing, bool& processing, bool& proxActive,
			int& currShape, bool& imageFlag, drawing::Brush& brush, ui::UserInterface& gui,
			drawing::ImageHandler& imageHandler, gl::Fbo* proxFbo);
		inline Leap::Frame GetCurrentFrame() const { return this->currentFrame; }
		inline void SetCurrentFrame() { this->currentFrame = this->leapController.frame(); }
	private:
		void drawProx(bool& proxActive, gl::Fbo* proxFbo);

		//keeping 2 lists of draw events, some that fire constantly while drawing(temporary draw events)
		//and others that fire only once a touch id draws and then exits draw zone(finalizing the draw event)
		unordered_map<int, drawing::DrawEvent> finalizeableDrawEvents;
		unordered_map<int, drawing::DrawEvent> temporaryDrawEvents;

		int windowWidth;
		int windowHeight;
		Leap::Controller leapController;
		Leap::Frame currentFrame;
		Leap::GestureList gestureList;
		Leap::Listener leapListener;
		ColorA distanceToColor(float distance);
	};
}}
