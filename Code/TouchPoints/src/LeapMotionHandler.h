#pragma once
#include "Leap.h"
#include "cinder/Vector.h"
#include <map>
#include "ImageHandler.h"
#include "UserInterface.h"

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
		void leapDraw(bool& lockCurrentFrame, bool& proxActive, drawing::Illustrator& illustrator);
		void gestRecognition(bool isDrawing, bool& processing, bool& proxActive,
		                     int& currShape, bool& imageFlag, drawing::Brush& brush, ui::UserInterface& gui,
		                     drawing::ImageHandler& imageHandler, gl::Fbo* proxFbo);

		inline Leap::Frame GetCurrentFrame() const
		{
			return this->currentFrame;
		};

		inline void SetCurrentFrame()
		{
			this->currentFrame = this->leapController.frame();
		};

	private:
		void drawProx(bool& proxActive, gl::Fbo* proxFbo);

		int windowWidth;
		int windowHeight;
		std::map<uint32_t, vec2> pointsMap;
		std::map<uint32_t, bool> activePointsMap;
		Leap::Controller leapController;
		Leap::Frame currentFrame;
		Leap::GestureList gestureList;
		Leap::Listener leapListener;
	};
}}
