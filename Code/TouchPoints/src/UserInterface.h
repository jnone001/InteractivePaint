#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "Brush.h"
#include "Illustrator.h"


struct UserInterface{

	UserInterface(){}
	//Creates a User Interface
	UserInterface(int mWindowWidth, int mWindowHeight, bool leapRun, bool eyeXRun, Brush* brush, Illustrator* mIllustrator, std::shared_ptr<gl::Fbo> fbo, std::vector<std::shared_ptr<gl::Fbo>>* fboLayerList){
		modeChangeFlag = true;
		windowWidth = mWindowWidth;
		windowHeight = mWindowHeight;
		mBrush = brush;
		illustrator = mIllustrator;
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
		backgroundColor = Color(0.0, 0.0, 0.0);
		layerList = fboLayerList;



		
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
	//Functions to get brush values



private:
	Brush* mBrush;
	Illustrator* illustrator;
	std::shared_ptr<gl::Fbo> uiFbo;

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
	bool layerVisualization;
	bool uiFlag;
	//bool radialActive = false;
	//vec2 radialCenter = vec2(0.0f, 0.0f);

	std::vector<std::shared_ptr<gl::Fbo>>* layerList;
	Color backgroundColor;


};
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

bool UserInterface::inInteractiveUi(int x, int y)
{
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
			//gl::color(0.9, 0.85, 0.65);
			//gl::drawStrokedRect(Rectf(400, y - 200, 600, y), 10);

			if (x > 400 && x < 600)
			{
				if (y > yDist - 200 && y < yDist)
				{
					std::shared_ptr<gl::Fbo> temp = (*layerList)[2];
					(*layerList)[2] = (*layerList)[size];
					(*layerList)[size] = temp;
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
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 150){
			(*mBrush).changeShape(Shape::Shape::Circle);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 200){
			(*mBrush).changeShape(Shape::Shape::Rectangle);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 250){
			(*mBrush).changeShape(Shape::Shape::Triangle);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
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
		//uiFboFlag = true;

		//glClearColor(0.0,0.0,0.0,  0.0);
		//glClear(GL_COLOR_BUFFER_BIT);

		gl::clear(ColorA(0.0,0.0,0.0,0.0));


		gl::color(1.0, 0.9, 0.5);
		//gl::lineWidth(10);
		//gl::drawSolidCircle(vec2(0,0), 10);
		//gl::drawSolidCircle(vec2(1920,1080), 100);

		gl::drawStrokedRect(Rectf(windowWidth*.8, windowHeight*.8, windowWidth, windowHeight), 5);
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
		if (System::hasMultiTouch()){
			gl::color(0.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(windowWidth*.81, windowHeight * .81, windowWidth*.83, windowHeight *.83));
		}
		if (leapRunning){
			gl::color(0.0, 1.0, 0.0);
			gl::drawSolidRect(Rectf(windowWidth*.84, windowHeight * .81, windowWidth*.86, windowHeight *.83));
		}
		if (eyeXRunning){
			gl::color(1.0, 0.0, 0.0);
			gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight * .81, windowWidth*.89, windowHeight *.83));
		}
		(*uiFbo).unbindFramebuffer();
		//uiFboFlag = false;
	}
	if (uiFboFlag){//Constantly drawn Ui buttons. If ui flag is off, we shut down the ui


		if (modeButtons){
			//Color Button
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(0, 2, 100, 50), 10);
			gl::color(1.0, 1.0, 0);
			gl::drawSolidRect(Rectf(0, 0, 20, 50));
			gl::color(0.0, 1.0, 0.0);
			gl::drawSolidRect(Rectf(20, 0, 35, 50));
			gl::color(1.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(35, 0, 50, 50));

			//Shapes button
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(50, 2, 100, 50), 10);
			gl::color(0.0, 1.0, 1.0);
			gl::drawSolidRect(Rectf(50, 0, 100, 50));

			//FilledShapes button
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(100, 2, 150, 50), 10);
			gl::color(1.0, 1.0, 1.0);
			gl::drawSolidRect(Rectf(100, 0, 150, 50));

			//Line Size
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(150, 2, 200, 50), 10);
			gl::color(0.0, 0.0, 0.0);
			gl::drawSolidRect(Rectf(150, 0, 200, 50));

			gl::color(1.0, 1.0, 1.0);
			gl::lineWidth(5);
			gl::drawLine(vec2(160, 25), vec2(190, 25));
			gl::drawLine(vec2(175, 10), vec2(175, 40));

			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(200, 2, 250, 50), 10);
			gl::color(0.0, 0.0, 0.0);
			gl::drawSolidRect(Rectf(200, 0, 250, 50));

			gl::color(1.0, 1.0, 1.0);
			//gl::lineWidth((*mBrush).getLineSize());
			gl::drawLine(vec2(210, 25), vec2(240, 25));
			//gl::drawLine(vec2(225, 10), vec2(225, 40));

			//More transparency button
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(250, 2, 300, 50), 10);
			gl::color(1.0, 0.0, 1.0, 1.0);
			gl::drawSolidRect(Rectf(250, 0, 300, 50));
			//Less transparent button
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(300, 2, 350, 50), 10);
			gl::color(0.0, 1.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(300, 0, 350, 50));

			//Symmetry Button
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(350, 2, 400, 50), 10);
			gl::drawLine(vec2(375, 2), vec2(375, 15));
			gl::drawLine(vec2(375, 20), vec2(375, 25));
			gl::drawLine(vec2(375, 30), vec2(375, 35));
			gl::drawLine(vec2(375, 40), vec2(375, 45));

			//Layer Visualization Button
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(400, 2, 450, 50), 10);
			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::drawStrokedRect(Rectf(410, 10, 430, 30), 1);
			gl::drawStrokedRect(Rectf(420, 20, 440, 40), 1);


		}
		if (colorButtons){
			int i = 0;
			for (auto myColor : (*mBrush).getColorList())
			{
				gl::color(0.9, 0.85, 0.65);
				gl::drawStrokedRect(Rectf(0, 50 * (i)+50, 50, 50 * i + 100), 10);
				gl::color(myColor);
				gl::drawSolidRect(Rectf(0, 50 * (i)+50, 50, 50 * (i)+100));
				i++;
			}
		}
		if (shapeButtons){

			for (int i = 0; i < 4; i++){

				gl::color(0.9, 0.85, 0.65);
				gl::drawStrokedRect(Rectf(50, 50 * (i)+50, 100, 50 * i + 100), 10);
				gl::color(0.0, 0.0, 0.0);
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
		for (auto frame : (*layerList)){
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(400, y - 200, 600, y), 10);
			frame->blitToScreen(Area(vec2(0, 0), vec2(1920, 1080)), Area(vec2(400, 1080 - (y - 200)), vec2(600, 1080 - y)), GL_NEAREST, GL_COLOR_BUFFER_BIT);
			y = y - 200;
		}
		//(*layerList)[0]->blitToScreen(Area(vec2(0, 0), vec2(1920, 1080)), Area(vec2(500, 500), vec2(1000, 800)), GL_NEAREST, GL_COLOR_BUFFER_BIT);
		//(*layerList)[1]->blitToScreen(Area(vec2(0, 0), vec2(1920, 1080)), Area(vec2(300, 100), vec2(400, 400)), GL_NEAREST, GL_COLOR_BUFFER_BIT);
	}

	//gl::color(1.0, 1.0, 1.0);
	//if (uiFboFlag) gl::draw(uiFbo->getColorTexture());
}


#endif