

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "Brush.h"
#include "Illustrator.h"
#include "libusb.h"
#include <stdio.h>
#include "DeviceHandler.h"






struct UserInterface{

	UserInterface(){}
	//Creates a User Interface
	UserInterface(int mWindowWidth, int mWindowHeight, bool leapRun, bool eyeXRun, Brush* brush, Illustrator* mIllustrator, DeviceHandler* mDeviceHandler, std::shared_ptr<gl::Fbo> fbo, std::vector<std::shared_ptr<gl::Fbo>>* fboLayerList, std::vector<float>* fboLayerAlpha){
		modeChangeFlag = true;
		windowWidth = mWindowWidth;
		windowHeight = mWindowHeight;
		mBrush = brush;
		illustrator = mIllustrator;
		deviceHandler = mDeviceHandler;
		uiFbo = fbo;
		leapRunning = leapRun;
		eyeXRunning = eyeXRun;
		/*
		if (eyeXRunning){
			modeButtons = false;
			uiFlag = false;
		}
		else {
			uiFlag = true;
			modeButtons = true;
		}
		*/
		uiFboFlag = true;
		modeButtons = true;
		colorButtons = false;
		shapeButtons = false;
		layerVisualization = false;
		deviceButtons = false;
		transparentBackground = true;

		//Adds our backgroundColors to the list. 
		backgroundList.emplace_back(Color(0.0f, 0.0f, 0.0f));
		backgroundList.emplace_back(Color(256.0f, 256.0f, 256.0f));
		backgroundList.emplace_back(Color(256.0f, 0.0f, 0.0f));
		backgroundList.emplace_back(Color(256.0f, 256.0f, 0.0f));
		backgroundList.emplace_back(Color(0.0f, 256.0f, 0.0f));
		backgroundList.emplace_back(Color(0.0f, 256.0f, 256.0f));
		backgroundList.emplace_back(Color(0.0f, 0.0f, 256.0f)); 
		backgroundList.emplace_back(Color(256.0f, 0.0f, 256.0f));

		backgroundColor = backgroundList.front();
		incrementBackground();
		layerList = fboLayerList;
		layerAlpha = fboLayerAlpha;
		int x = 0;
		for (auto layers : *layerList){
			(*layerAlpha).emplace_back(1.0f);
		}
		
		

	}

	void modeRectangle();
	void modeCircle();
	void modeTriangle();
	void modeLine();
	void drawUi();
	bool inInteractiveUi(int x, int y);
	Color getBackgroundColor();
	void setModeChangeFlag();
	void UserInterface::changeBackgroundColor(Color background);
	void clearLayers();
	bool getUiFboFlag();
	void changeModeButtons(bool x);
	void toggleUiFlag();
	bool getUiFlag();
	bool isBackgroundTransparent();
	std::shared_ptr<gl::Fbo> getTransparentBackground();
	//UI Setup does all the drawing that cannot be done inside a libcinder 'setup' call, such as drawing to Framebuffers.
	void uiSetup();


	void slideButtons(int x, int y);
	float getLayerAlpha(int layerNumber);
	void incrementBackground();

	//Functions to get brush values



private:
	Brush* mBrush;
	Illustrator* illustrator;
	DeviceHandler * deviceHandler;
	std::shared_ptr<gl::Fbo> uiFbo;

	//Stores the 'Checkerboard pattern for background'
	std::shared_ptr<gl::Fbo> transparentBackgroundFbo;

	int windowWidth;
	int windowHeight;

	//Mode Flags
	bool modeChangeFlag;
	bool leapRunning;
	bool eyeXRunning;
	bool uiFboFlag;
	bool modeButtons;
	bool colorButtons;
	bool shapeButtons;
	bool deviceButtons;
	bool layerVisualization;
	bool uiFlag;
	bool undoFlag;
	bool transparentBackground;
	//bool radialActive = false;
	//vec2 radialCenter = vec2(0.0f, 0.0f);

	std::vector<std::shared_ptr<gl::Fbo>>* layerList;

	std::vector<float>* layerAlpha;
	list<Color> backgroundList; 
	Color backgroundColor;


};

void UserInterface::uiSetup(){

	//Loads the asset for transparent Background
	gl::Fbo::Format format;
	transparentBackgroundFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	transparentBackgroundFbo->bindFramebuffer();
	cinder::gl::TextureRef tempText = gl::Texture::create(loadImage(loadAsset("TransparentBackground.png")));
	gl::draw(tempText, Rectf(0, 0, windowWidth, windowHeight));
	transparentBackgroundFbo->unbindFramebuffer();
	gl::color(1.0, 1.0, 1.0, 0.5);
	gl::draw(transparentBackgroundFbo->getColorTexture());

}

std::shared_ptr<gl::Fbo> UserInterface::getTransparentBackground(){
	return transparentBackgroundFbo;
}

bool UserInterface::isBackgroundTransparent(){
	return transparentBackground;
}

void UserInterface::incrementBackground(){
	//Pops the current color
	Color tempColor = backgroundList.front();
	backgroundList.pop_front();
	//Assign the new background color to the new front of list.
	backgroundColor = backgroundList.front();
	//Place the old background color back into the list
	backgroundList.emplace_back(tempColor);
}
float UserInterface::getLayerAlpha(int layerNumber){
	return (*layerAlpha)[layerNumber];
}
bool UserInterface::getUiFlag(){
	return uiFlag;
}
void UserInterface::toggleUiFlag(){
	if (uiFlag){
		modeButtons = false;
		uiFboFlag = false;
		uiFlag = false;
	}
	else{
		uiFlag = true;
		uiFboFlag = true;
		modeButtons = true;
	}

}
void UserInterface::changeModeButtons(bool x){
	modeButtons = x;
}

bool UserInterface::getUiFboFlag(){
	return uiFboFlag;
}

void UserInterface::clearLayers(){

	for (auto layer : *layerList)
	{
		(*layer).bindFramebuffer();
		//glClearColor(1.0,1.0,1.0, 0.0);
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 0.0);

		glClear(GL_COLOR_BUFFER_BIT);
		//gl::clear(Color(1.0,1.0,1.0)/*ColorA(backgroundColor)*/, 0.0);
		(*layer).unbindFramebuffer();
	}
	(*illustrator).clearTimeMachine();
}

void UserInterface::setModeChangeFlag(){
	modeChangeFlag = true;
}

Color UserInterface::getBackgroundColor(){
	return backgroundColor;
}

void UserInterface::changeBackgroundColor(Color background){
	backgroundColor = background;
}
void UserInterface::slideButtons(int x, int y){
	if (layerVisualization){
		int yDist = (*layerList).size();
		yDist = (*layerList).size() * 200 + 50;
		int size = 0;
		for (auto frame : (*layerList)){
			//gl::color(0.75, 0.75, .75, 1.0);
			//gl::drawStrokedRect(Rectf(400, y - 200, 600, y), 10);

			if (x > 400 && x < 450)
			{
				if (y > yDist - 200 && y < yDist)
				{
					(*layerAlpha)[size] = ((y - ((float)yDist - 200)) / ((float)yDist - ((float)yDist - 200)));
					//(*layerAlpha)[size] = 0.5f;
				}
			}
			yDist = yDist - 200;
			size = size + 1;
		}
	}
}
bool UserInterface::inInteractiveUi(int x, int y)
{

	if (deviceHandler->multiTouchStatus() == false){
		//If the multi-touch is 'disabled' we still allow the user to toggle devices on and off using the multitouch
		if (deviceButtons){
			if (x < windowWidth && x > windowWidth *.87){
				if (y > windowHeight * .59 && y < windowHeight * .62){
					deviceHandler->toggleRealSense();
					modeChangeFlag = true;
				}
				if (y > windowHeight * .62 && y < windowHeight * .65){
					deviceHandler->toggleRealSenseExpressions();
					modeChangeFlag = true;
				}
				if (y > windowHeight * .65 && y < windowHeight * .68){
					deviceHandler->toggleLeap();
					modeChangeFlag = true;
				}
				if (y > windowHeight * .68 && y < windowHeight * .71){
					deviceHandler->toggleLeapDraw();
				}
				if (y > windowHeight * .71 && y < windowHeight * .74){
					deviceHandler->toggleLeapGesture();
				}
				if (y > windowHeight * .74 && y < windowHeight * .77){
					deviceHandler->toggleEyeX();
					if (deviceHandler->eyeXStatus() == false){
						modeButtons = true;
					}
					modeChangeFlag = true;
				}
				if (y > windowHeight * .77 && y < windowHeight * .8){
					deviceHandler->toggleMultiTouch();
					modeChangeFlag = true;
				}
			}

		}
		return true;
	}
	/*
	if (radialActive){

		if ((((radialCenter.x - 100) - 30) < x && x < ((radialCenter.x - 100) + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30))){
			(*mBrush).changeStaticColor();
			return true;
		}

		if ((((radialCenter.x + 100) - 30) < x && x < ((radialCenter.x + 100) + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30))){
			(*mBrush).incrementShape();
			return true;
		}

		if ((((radialCenter.x) - 30) < x && x < ((radialCenter.x) + 30)) && ((radialCenter.y - 100) - 30) < y && y < (radialCenter.y - 100) + 30){
			(*(*mBrush).getSymmetry()).toggleSymmetry();
			return true;
		}

		if ((((radialCenter.x) - 30) < x && x < ((radialCenter.x) + 30)) && ((radialCenter.y + 100) - 30) < y && y < (radialCenter.y + 100) + 30){
			uiFboFlag = !uiFboFlag;
			return true;
		}

		if ((radialCenter.x - 30) < x && x < ((radialCenter.x + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30))){
			radialActive = false;
			return true;
		}
	}
	*/
	
	//modeButtons UI
	
	if ((*illustrator).getActiveDrawings() == 0){

		if (uiFboFlag){
			if (x > windowWidth*.92 && x < windowWidth && y > windowHeight*.8 && y < windowHeight*.83){
				deviceButtons = !deviceButtons;
			}
		}

		if (modeButtons){
			//Color change button.
			if (uiFboFlag){

				if (x < 50 && y < 50)
				{
					colorButtons = !colorButtons;
					return true;
				}
				else if (x < 100 && y < 50){
					shapeButtons = !shapeButtons;
					return true;
				}
				else if (x < 150 && y < 50){
					(*mBrush).changeFilledShapes(!(*mBrush).getFilledShapes());
					modeChangeFlag = true;
					return true;
				}
				else if (x < 200 && y < 50){
					(*mBrush).increaseLineSize();
					//else lineSize = 1;
					modeChangeFlag = true;
					return true;
				}
				else if (x < 250 && y < 50){
					(*mBrush).decreaseLineSize();
					//else lineSize = 15;
					modeChangeFlag = true;
					return true;
				}
				else if (x < 300 && y < 50){
					(*mBrush).decreaseAlpha();
					modeChangeFlag = true;
					return true;
				}
				else if (x < 350 && y < 50){
					(*mBrush).increaseAlpha();
					modeChangeFlag = true;
					return true;
				}
				else if (x < 400 && y < 50){
					(*(*mBrush).getSymmetry()).toggleSymmetry();
					return true;
				}
				else if (x < 450 && y < 50){
					layerVisualization = !layerVisualization;
					return true;
				}
				else if (x < 500 && y < 50 ){
					(*illustrator).undoDraw(backgroundColor);
					return true;
				}
			}
		}
	}
	//Color buttons UI
	if (colorButtons){
		for (int i = 0; i < 8; i++){ // i < total colors
			if (x < 50 && y < (50 * i + 100))
			{
				//currColor = i;
				(*mBrush).changeStaticColor(static_cast<ourColors::ourColors>(i));
				colorButtons = false;
				modeChangeFlag = true;
				return true;
			}
		}
	}
	if (layerVisualization){
		int yDist = (*layerList).size();
		yDist = (*layerList).size() * 200 + 50;
		int size = 0;
		for (auto frame : (*layerList)){
			//gl::color(0.75, 0.75, .75, 1.0);
			//gl::drawStrokedRect(Rectf(400, y - 200, 600, y), 10);

			

			if (x > 400 && x < 800)
			{
				if (x < 450){
					//Signals that the user is pressing layerVisualization 'slide button'
					return true;
				}
				if (y > yDist - 200 && y < yDist)
				{
					//Swaps layers
					std::shared_ptr<gl::Fbo> temp = (*layerList)[2];
					(*layerList)[2] = (*layerList)[size];
					(*layerList)[size] = temp;


		
					//Swaps alpha value of layers
					float tempAlpha = (*layerAlpha)[2];
					(*layerAlpha)[2] = (*layerAlpha)[size];
					(*layerAlpha)[size] = tempAlpha;

					return true;

				}
			}
			yDist = yDist - 200;
			size = size + 1;
		}
	}
	if (shapeButtons){
		if (x > 50 && x < 100 && y < 100){
			(*mBrush).changeShape(Shape::Shape::Line);
			(*mBrush).changeEraserMode(false);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 150){
			(*mBrush).changeShape(Shape::Shape::Circle);
			(*mBrush).changeEraserMode(false);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 200){
			(*mBrush).changeShape(Shape::Shape::Rectangle);
			(*mBrush).changeEraserMode(false);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 250){
			(*mBrush).changeEraserMode(false);
			(*mBrush).changeShape(Shape::Shape::Triangle);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 300){
			if (mBrush->getEraserMode())
				(*mBrush).changeEraserMode(false);
			else mBrush->changeEraserMode(true);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
	}
	if (deviceButtons){
		if (x < windowWidth && x > windowWidth *.87){
			if (y > windowHeight * .59 && y < windowHeight * .62){
				deviceHandler->toggleRealSense();
				modeChangeFlag = true;
			}
			if (y > windowHeight * .62 && y < windowHeight * .65){
				deviceHandler->toggleRealSenseExpressions();
				modeChangeFlag = true;
			}
			if (y > windowHeight * .65 && y < windowHeight * .68){
				deviceHandler->toggleLeap();
				modeChangeFlag = true;
			}
			if (y > windowHeight * .68 && y < windowHeight * .71){
				deviceHandler->toggleLeapDraw();
			}
			if (y > windowHeight * .71 && y < windowHeight * .74){
				deviceHandler->toggleLeapGesture();
			}
			if (y > windowHeight * .74 && y < windowHeight * .77){
				deviceHandler->toggleEyeX();
				if (deviceHandler->eyeXStatus() == false){
					modeButtons = true;
				}
				modeChangeFlag = true;
			}
			if (y > windowHeight * .77 && y < windowHeight * .8){
				deviceHandler->toggleMultiTouch();
				modeChangeFlag = true;
			}




		}

	}

	return false;
}


void UserInterface::modeRectangle(){

	if ((*mBrush).getRandColor()){
		ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
		
		gl::color(newColor);
		//gl::lineWidth(10);
		//gl::drawSolidCircle(vec2(0,0), 10);
		//gl::drawSolidCircle(vec2(1920,1080), 100);
		if (!(*mBrush).getFilledShapes())
			gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), (*mBrush).getLineSize());
		else gl::drawSolidRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95));
		//(*uiFbo).unbindFramebuffer();
	}

	else{
		ColorA newColor((*mBrush).getColor(), (*mBrush).getAlphaColor());
		gl::color(newColor);
		//gl::lineWidth(10);
		//gl::drawSolidCircle(vec2(0,0), 10);
		//gl::drawSolidCircle(vec2(1920,1080), 100);
		if (!(*mBrush).getFilledShapes())
			gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), (*mBrush).getLineSize());
		else gl::drawSolidRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95));
		//(*uiFbo).unbindFramebuffer();
	}
}

void UserInterface::modeCircle(){

	if ((*mBrush).getRandColor()){
		ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!(*mBrush).getFilledShapes())
			gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, (*mBrush).getLineSize()*2.0f);
		else
			gl::drawSolidCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05);

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), (*mBrush).getLineSize());
		//(*uiFbo).unbindFramebuffer();
	}
	else{

		ColorA newColor((*mBrush).getColor(), (*mBrush).getAlphaColor());
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!(*mBrush).getFilledShapes())
			gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, (*mBrush).getLineSize()*2.0f);
		else
			gl::drawSolidCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05);

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), (*mBrush).getLineSize());
		//(*uiFbo).unbindFramebuffer();
	}
}

void UserInterface::modeTriangle(){

	//(*uiFbo).unbindTexture(0, 36064U);
	//(*uiFbo).bindFramebuffer();
	if ((*mBrush).getRandColor()){
		ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!(*mBrush).getFilledShapes()){
			/*"Better" Triagnles
			//gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, (*mBrush).getLineSize()*2.0f);
			TouchPoint uiTouchPoint(vec2(windowWidth*.85, windowHeight*.95), newColor, (*mBrush).getLineSize());
			uiTouchPoint.addPoint(vec2(windowWidth*.95, windowHeight*.95));
			uiTouchPoint.addPoint(vec2(windowWidth*.9, windowHeight*.85));
			uiTouchPoint.addPoint(vec2(windowWidth*.85, windowHeight*.95));

			missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.95, uiTouchPoint);
			missedPoints(windowWidth*.95, windowHeight*.95, windowWidth*.9, windowHeight*.85, uiTouchPoint);
			missedPoints(windowWidth*.9, windowHeight*.85, windowWidth*.85, windowHeight*.95, uiTouchPoint);
			uiTouchPoint.draw();
			*/

			gl::lineWidth((*mBrush).getLineSize());
			gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.95));
			gl::drawLine(vec2(windowWidth*.95, windowHeight*.95), vec2(windowWidth*.9, windowHeight*.85));
			gl::drawLine(vec2(windowWidth*.9, windowHeight*.85), vec2(windowWidth*.85, windowHeight*.95));

		}
		else
			gl::drawSolidTriangle(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight *.95), vec2(windowWidth*.9, windowHeight*.85));

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), (*mBrush).getLineSize());
		//(*uiFbo).unbindFramebuffer();
	}
	else{
		ColorA newColor((*mBrush).getColor(), (*mBrush).getAlphaColor());
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!(*mBrush).getFilledShapes()){
			/*"Better triangles maybe later"
			//gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, (*mBrush).getLineSize()*2.0f);
			TouchPoint uiTouchPoint(vec2(windowWidth*.85, windowHeight*.95), newColor, (*mBrush).getLineSize());
			uiTouchPoint.addPoint(vec2(windowWidth*.95, windowHeight*.95));
			uiTouchPoint.addPoint(vec2(windowWidth*.9, windowHeight*.85));
			uiTouchPoint.addPoint(vec2(windowWidth*.85, windowHeight*.95));

			missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.95, uiTouchPoint);
			missedPoints(windowWidth*.95, windowHeight*.95, windowWidth*.9, windowHeight*.85, uiTouchPoint);
			missedPoints(windowWidth*.9, windowHeight*.85, windowWidth*.85, windowHeight*.95, uiTouchPoint);
			uiTouchPoint.draw();
			*/

			gl::lineWidth((*mBrush).getLineSize());
			gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.95));
			gl::drawLine(vec2(windowWidth*.95, windowHeight*.95), vec2(windowWidth*.9, windowHeight*.85));
			gl::drawLine(vec2(windowWidth*.9, windowHeight*.85), vec2(windowWidth*.85, windowHeight*.95));

		}
		else
			gl::drawSolidTriangle(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight *.95), vec2(windowWidth*.9, windowHeight*.85));

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), (*mBrush).getLineSize());
		//(*uiFbo).unbindFramebuffer();
	}
}

void UserInterface::modeLine(){

	if ((*mBrush).getRandColor()){
		//Performance Issues with drawing these 3 lines every frame!
		ColorA newColor1(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
		ColorA newColor2(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
		ColorA newColor3(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
		//gl::color(newColor1);
		TouchPoint newTouchPoints1(vec2(windowWidth*.85, windowHeight*.95), newColor1, (*mBrush).getLineSize());
		(*illustrator).missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.85, newTouchPoints1);
		TouchPoint newTouchPoints2(vec2(windowWidth*.86, windowHeight*.90), newColor2, (*mBrush).getLineSize());
		(*illustrator).missedPoints(windowWidth*.86, windowHeight*.90, windowWidth*.92, windowHeight*.84, newTouchPoints2);
		TouchPoint newTouchPoints3(vec2(windowWidth*.88, windowHeight*.97), newColor3, (*mBrush).getLineSize());
		(*illustrator).missedPoints(windowWidth*.88, windowHeight*.97, windowWidth*.94, windowHeight*.90, newTouchPoints3);
		newTouchPoints1.draw();
		newTouchPoints2.draw();
		newTouchPoints3.draw();


		/*
		Color newColor1(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		Color newColor2(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		Color newColor3(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		gl::lineWidth((*mBrush).getLineSize());
		gl::color(newColor1);
		gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.85));
		gl::color(newColor2);
		gl::drawLine(vec2(windowWidth*.86, windowHeight*.9), vec2(windowWidth*.92, windowHeight*.84));
		gl::color(newColor3);
		gl::drawLine(vec2(windowWidth*.88, windowHeight*.97), vec2(windowWidth*.94, windowHeight*.90));
		*/
	}
	else{

		ColorA newColor((*mBrush).getColor(), (*mBrush).getAlphaColor());
		gl::color(newColor);
		TouchPoint newTouchPoints1(vec2(windowWidth*.85, windowHeight*.95), newColor, (*mBrush).getLineSize());
		(*illustrator).missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.85, newTouchPoints1);
		TouchPoint newTouchPoints2(vec2(windowWidth*.86, windowHeight*.90), newColor, (*mBrush).getLineSize());
		(*illustrator).missedPoints(windowWidth*.86, windowHeight*.90, windowWidth*.92, windowHeight*.84, newTouchPoints2);
		TouchPoint newTouchPoints3(vec2(windowWidth*.88, windowHeight*.97), newColor, (*mBrush).getLineSize());
		(*illustrator).missedPoints(windowWidth*.88, windowHeight*.97, windowWidth*.94, windowHeight*.90, newTouchPoints3);
		newTouchPoints1.draw();
		newTouchPoints2.draw();
		newTouchPoints3.draw();

		/*
		Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
		gl::lineWidth((*mBrush).getLineSize());
		gl::color(newColor);
		gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.85));
		gl::drawLine(vec2(windowWidth*.86, windowHeight*.9), vec2(windowWidth*.92, windowHeight*.84));
		gl::drawLine(vec2(windowWidth*.88, windowHeight*.97), vec2(windowWidth*.94, windowHeight*.90));
		*/
	}
}

void UserInterface::drawUi(){
	//(*uiFbo).unbindTexture();


	//gl::clear(GL_BACK);
	//if (gazePositionX > 1920 && gazePositionY > 1080)
	if (modeChangeFlag)//Draws to the UI FBO. Currently only houses 'modebox' in the fbo.
	{
		modeChangeFlag = false;
		(*uiFbo).bindFramebuffer();


		gl::lineWidth(5);
		//Clears the framebuffer to redraw
		gl::clear(ColorA(0.0,0.0,0.0,0.0));

		gl::color(.82,.82,.82 ,1.0);
		gl::drawSolidRect(Rectf(windowWidth*.8, windowHeight*.8, windowWidth, windowHeight));
		gl::color(0.0, 0.0, 0.0);

		//Draw Device Mode button
		TextLayout layout1;
		layout1.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout1.setFont(Font("Arial", 50));
		layout1.setColor(Color(1, 1, 1));
		layout1.addLine(std::string("Device Modes"));
		Surface8u rendered = layout1.render(true, false);
		gl::Texture2dRef mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth*.92, windowHeight*.8, windowWidth, windowHeight*.83));
		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(windowWidth*.92, windowHeight*.8, windowWidth, windowHeight*.83));

		//Draw outline of modeBox
		
		gl::color(0.75, 0.75, 0.75);
		gl::drawStrokedRect(Rectf(windowWidth*.8, windowHeight*.8, windowWidth, windowHeight), 10);
		gl::color(0.0, 0.0, 0.0);

		//gl::drawSolidRect(Rectf(windowWidth*.8, windowHeight*.8, windowWidth, windowHeight));

		if ((*mBrush).getEraserMode()){
			bool tempBool = false;
			TouchCircle(vec2(windowWidth*.9, windowHeight *.9), (*mBrush).getLineSize() * 2, Color(1.0, 1.0, 1.0), 1, tempBool).draw();
		}
		else switch ((*mBrush).getShape()){
		case Shape::Shape::Rectangle:
			modeRectangle();
			break;
		case Shape::Shape::Circle:
			modeCircle();
			break;
		case Shape::Shape::Triangle:
			modeTriangle();
			break;
		case Shape::Shape::Line:
			modeLine();
			break;
		}


		//auto maxTouches = System::getMaxMultiTouchPoints();
		if ((*deviceHandler).multiTouchStatus()){
			gl::color(0.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(windowWidth*.81, windowHeight * .81, windowWidth*.83, windowHeight *.83));
		}
		if ((*deviceHandler).leapStatus()){
			gl::color(0.0, 1.0, 0.0);
			gl::drawSolidRect(Rectf(windowWidth*.84, windowHeight * .81, windowWidth*.86, windowHeight *.83));
		}
		if ((*deviceHandler).eyeXStatus()){
			gl::color(1.0, 0.0, 0.0);
			gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight * .81, windowWidth*.89, windowHeight *.83));
		}
		if ((*deviceHandler).realSenseStatus()){
			gl::color(1.0, 1.0, 0.0);
			gl::drawSolidRect(Rectf(windowWidth*.90, windowHeight * .81, windowWidth*.92, windowHeight *.83));
		}
		(*uiFbo).unbindFramebuffer();
		//uiFboFlag = false;
	}
	if (uiFboFlag){//Constantly drawn Ui buttons. If ui flag is off, we shut down the ui


		if (modeButtons){

			gl::color(0.0, 0.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(0, 0, 500, 50));
			//Color Button
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(0, 2, 100, 50), 10);
			gl::color(1.0, 1.0, 0);
			gl::drawSolidRect(Rectf(0, 0, 20, 50));
			gl::color(0.0, 1.0, 0.0);
			gl::drawSolidRect(Rectf(20, 0, 35, 50));
			gl::color(1.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(35, 0, 50, 50));
			
			//Shapes button
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(50, 2, 100, 50), 10);
			gl::color(0.0, 1.0, 1.0);
			gl::drawSolidRect(Rectf(50, 0, 100, 50));

			//FilledShapes button
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(100, 2, 150, 50), 10);
			gl::color(1.0, 1.0, 1.0);
			gl::drawSolidRect(Rectf(100, 0, 150, 50));

			//Line Size
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(150, 2, 200, 50), 10);
			gl::color(0.0, 0.0, 0.0);
			gl::drawSolidRect(Rectf(150, 0, 200, 50));

			gl::color(1.0, 1.0, 1.0);
			gl::lineWidth(5);
			gl::drawLine(vec2(160, 25), vec2(190, 25));
			gl::drawLine(vec2(175, 10), vec2(175, 40));

			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(200, 2, 250, 50), 10);
			gl::color(0.0, 0.0, 0.0);
			gl::drawSolidRect(Rectf(200, 0, 250, 50));

			gl::color(1.0, 1.0, 1.0);
			//gl::lineWidth((*mBrush).getLineSize());
			gl::drawLine(vec2(210, 25), vec2(240, 25));
			//gl::drawLine(vec2(225, 10), vec2(225, 40));

			//More transparency button
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(250, 2, 300, 50), 10);
			gl::color(1.0, 0.0, 1.0, 1.0);
			gl::drawSolidRect(Rectf(250, 0, 300, 50));
			//Less transparent button
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(300, 2, 350, 50), 10);
			gl::color(0.0, 1.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(300, 0, 350, 50));

			//Symmetry Button
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(350, 2, 400, 50), 10);
			gl::drawLine(vec2(375, 2), vec2(375, 15));
			gl::drawLine(vec2(375, 20), vec2(375, 25));
			gl::drawLine(vec2(375, 30), vec2(375, 35));
			gl::drawLine(vec2(375, 40), vec2(375, 45));

			//Layer Visualization Button
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(400, 2, 450, 50), 10);
			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::drawStrokedRect(Rectf(410, 10, 430, 30), 1);
			gl::drawStrokedRect(Rectf(420, 20, 440, 40), 1);


			//Undo Button
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(450, 2, 500, 50), 10);
			



		}

		if (colorButtons){
			int i = 0;
			for (auto myColor : (*mBrush).getColorList())
			{
				gl::color(0.75, 0.75, .75, 1.0);
				gl::drawStrokedRect(Rectf(0, 50 * (i)+50, 50, 50 * i + 100), 10);
				gl::color(myColor);
				gl::drawSolidRect(Rectf(0, 50 * (i)+50, 50, 50 * (i)+100));
				i++;
			}
		}
		if (shapeButtons){

			for (int i = 0; i < 5; i++){

				gl::color(0.75, 0.75, .75, 1.0);
				gl::drawStrokedRect(Rectf(50, 50 * (i)+50, 100, 50 * i + 100), 10);
				gl::color(1.0, 1.0, 1.0);
				gl::drawSolidRect(Rectf(50, 50 * i + 50, 100, 50 * i + 100));
			}

			gl::color((*mBrush).getColor());
			gl::lineWidth(2);
			gl::drawLine(vec2(55, 95), vec2(95, 55));

			if ((*mBrush).getFilledShapes())
				gl::drawSolidCircle(vec2(75, 125), 15);
			else gl::drawStrokedCircle(vec2(75, 125), 15);

			if ((*mBrush).getFilledShapes())
				gl::drawSolidRect(Rectf(60, 188, 90, 160));
			else gl::drawStrokedRect(Rectf(60, 188, 90, 160));


			if ((*mBrush).getFilledShapes())
				gl::drawSolidTriangle(vec2(55, 245), vec2(95, 245), vec2(73, 205));
			else {

				gl::drawLine(vec2(55, 245), vec2(95, 245));
				gl::drawLine(vec2(95, 245), vec2(73, 205));
				gl::drawLine(vec2(73, 205), vec2(55, 245));
			}
		}

	}
	if ((*(*mBrush).getSymmetry()).getSymmetryOn()){
		for (int i = 0; i < 50; i = i + 2){
			gl::lineWidth(3);
			gl::color(1.0, 1.0, 1.0);
			gl::drawLine(vec2(windowWidth / 2, windowHeight - i * 50), vec2(windowWidth / 2, windowHeight - (i + 1) * 50));
		}
	}
	if (layerVisualization){

		//gl::draw((*layerList)[0]->blitFromScreen(const Area &srcArea, const Area &dstArea, GLenum filter = GL_NEAREST, GLbitfield mask = GL_COLOR_BUFFER_BIT);
		//Area(vec2(0, 0), vec2(1920, 1080));
		//gl::drawSolidRect(Rectf(500, 500, 800, 800));
		int y = (*layerList).size();
		y = y * 200 + 50;
		int layerNumber = 0;
		for (auto frame : (*layerList)){
			gl::color(backgroundColor.r,backgroundColor.g, backgroundColor.b, 1.0);
			gl::drawSolidRect(Rectf(400,(y - 200), 800, y));

			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(400, y - 200, 800, y), 10);
			gl::color(1.0, 1.0, 1.0, (*layerAlpha)[layerNumber]);
			//frame->blitToScreen(Area(vec2(0, 0), vec2(1920, 1080)), Area(vec2(400, 1080 - (y - 200)), vec2(800, 1080 - y)), GL_NEAREST, GL_COLOR_BUFFER_BIT);
			gl::draw(frame->getColorTexture(), Rectf(400, (y - 200), 800, y));
			gl::color(0.0,0.0,0.0,1.0);
			gl::drawSolidRect(Rectf(400, y - 200, 450, y));
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(400, y - 200, 450, y), 10);
			gl::color(1.0, 1.0, 1.0, 1.0);
			

			//Draws indicator for alpha level
			gl::color(0.0, 1.0f, 0.0, 1.0f);
			gl::drawSolidCircle(vec2(425, (y - 200) + (*layerAlpha)[layerNumber] * (y - (y - 200))), 5.0f);

			for (int x = 0; x < 8; x++)
			{
				gl::color(1.0, 1.0, 1.0, 1.0 - .125 * x);
				gl::drawSolidRect(Rectf(400, (y - 200) + x*25, 450, (y - 200) + (x + 1) * 25));
			}
			y = y - 200;
			layerNumber++;
		}
		//(*layerList)[0]->blitToScreen(Area(vec2(0, 0), vec2(1920, 1080)), Area(vec2(500, 500), vec2(1000, 800)), GL_NEAREST, GL_COLOR_BUFFER_BIT);
		//(*layerList)[1]->blitToScreen(Area(vec2(0, 0), vec2(1920, 1080)), Area(vec2(300, 100), vec2(400, 400)), GL_NEAREST, GL_COLOR_BUFFER_BIT);
	}

	//gl::color(1.0, 1.0, 1.0);
	//if (uiFboFlag) gl::draw(uiFbo->getColorTexture());
	if (deviceButtons){

		gl::lineWidth(5);

		TextLayout layout1;
		layout1.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout1.setFont(Font("Arial", 50));
		layout1.setColor(Color(1, 1, 1));
		unsigned char japanese[] = { 0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E, 0 };
		layout1.addLine(std::string(" MultiTouch") );
		Surface8u rendered = layout1.render(true, false);
		gl::Texture2dRef mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth*.9, windowHeight*.77,windowWidth, windowHeight*.8));

		TextLayout layout2;
		layout2.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout2.setFont(Font("Arial", 50));
		layout2.setColor(Color(1, 1, 1));
		layout2.addLine(std::string("       EyeX"));
		rendered = layout2.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth*.9, windowHeight*.74, windowWidth, windowHeight*.77));

		TextLayout layout3;
		layout3.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout3.setFont(Font("Arial", 50));
		layout3.setColor(Color(1, 1, 1));
		layout3.addLine(std::string("Leap Gesture"));
		rendered = layout3.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth*.9, windowHeight*.71, windowWidth, windowHeight*.74));

		TextLayout layout4;
		layout4.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout4.setFont(Font("Arial", 50));
		layout4.setColor(Color(1, 1, 1));
		layout4.addLine(std::string("  Leap Draw"));
		rendered = layout4.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth*.9, windowHeight*.68, windowWidth, windowHeight*.71));

		TextLayout layout5;
		layout5.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout5.setFont(Font("Arial", 50));
		layout5.setColor(Color(1, 1, 1));
		layout5.addLine(std::string("Leap Motion"));
		rendered = layout5.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth*.9, windowHeight*.65, windowWidth, windowHeight*.68));

		TextLayout layout6;
		layout6.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout6.setFont(Font("Arial", 50));
		layout6.setColor(Color(1, 1, 1));
		layout6.addLine(std::string("Real Sense Expressions"));
		rendered = layout6.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth*.9, windowHeight*.62, windowWidth, windowHeight*.65));

		TextLayout layout7;
		layout7.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout7.setFont(Font("Arial", 50));
		layout7.setColor(Color(1, 1, 1));
		layout7.addLine(std::string(" Real Sense"));
		rendered = layout7.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth*.9, windowHeight*.59, windowWidth, windowHeight*.62));

		gl::color(0.0,0.0,0.0);




		if (deviceHandler->realSenseStatus())
			gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight*.59, windowWidth*.89, windowHeight*.62));
		if (deviceHandler->realSenseExpressions())
			gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight*.62, windowWidth*.89, windowHeight*.65));
		if (deviceHandler->leapStatus())
			gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight*.65, windowWidth*.89, windowHeight*.68));
		if (deviceHandler->leapDraw())
			gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight*.68, windowWidth*.89, windowHeight*.71));
		if (deviceHandler->leapGesture())
			gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight*.71, windowWidth*.89, windowHeight*.74));
		if (deviceHandler->eyeXStatus())
			gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight*.74, windowWidth*.89, windowHeight*.77));
		if (deviceHandler->multiTouchStatus())
			gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight*.77, windowWidth*.89, windowHeight*.8));

		


		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(windowWidth*.87, windowHeight*.59, windowWidth*.89, windowHeight*.62));
		gl::drawStrokedRect(Rectf(windowWidth*.87, windowHeight*.62, windowWidth*.89, windowHeight*.65));
		gl::drawStrokedRect(Rectf(windowWidth*.87, windowHeight*.65, windowWidth*.89, windowHeight*.68));
		gl::drawStrokedRect(Rectf(windowWidth*.87, windowHeight*.68, windowWidth*.89, windowHeight*.71));
		gl::drawStrokedRect(Rectf(windowWidth*.87, windowHeight*.71, windowWidth*.89, windowHeight*.74));
		gl::drawStrokedRect(Rectf(windowWidth*.87, windowHeight*.74, windowWidth*.89, windowHeight*.77));
		gl::drawStrokedRect(Rectf(windowWidth*.87, windowHeight*.77, windowWidth*.89, windowHeight*.8));

	}
	
}

#endif