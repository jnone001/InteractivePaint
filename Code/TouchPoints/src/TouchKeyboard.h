#ifndef TOUCHKEYBOARD_H
#define TOUCHKEYBOARD_H
//Cinder and OpenGL Includes
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/gl/Fbo.h"

//Our own includes
//#include "TouchShapes.h"


//Standard Library Includes
#include <vector>
#include <map>
#include <list>

struct TouchKeyboard{

	//Creates a keyboard
	TouchKeyboard(){
		printString = "";
		anchor = cinder::vec2(250, 100);
		cinder::gl::Fbo::Format format;
		keyboardFbo = cinder::gl::Fbo::create(1920, 1080, format);
		movingKeyboardFbo = cinder::gl::Fbo::create(1920, 1080, format);
	}

	void appendString(std::string text);
	void drawKeyboard();
	bool onKeyboardSurface(cinder::vec2 touch);
	void createKeyboard();
	std::shared_ptr<cinder::gl::Fbo> getKeyboardFbo();
	std::shared_ptr<cinder::gl::Fbo> getMovingKeyboardFbo();
	cinder::vec2 getAnchor();
	bool keyboardStatus();
	void setMovingKeyboard(bool moving);
	bool getMovingKeyboard();
	bool checkMovingId(uint32_t id);
	void setMovingId(uint32_t id);
	void setAnchor(cinder::vec2 pos);
	void finishMoving();
private:
	std::string		printString;
	cinder::vec2	anchor;
	std::shared_ptr<cinder::gl::Fbo>	keyboardFbo;
	std::shared_ptr<cinder::gl::Fbo>	movingKeyboardFbo;
	bool	keyboardOn = false;
	bool	shiftOn = true;
	bool	movingKeyboard = false;
	uint32_t moverId = 0;
	


};
void TouchKeyboard::finishMoving(){
	createKeyboard();
	movingKeyboard = false;
	moverId = 0;
}
void TouchKeyboard::setMovingId(uint32_t id){
	moverId = id;

}
bool TouchKeyboard::checkMovingId(uint32_t id){
	return id == moverId;

}
void TouchKeyboard::setAnchor(vec2 pos){
	anchor = pos;
	keyboardFbo->bindFramebuffer();
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT);
	cinder::gl::color(0.0, 0.0, 0.0, 1.0);

	gl::drawSolidRect(cinder::Rectf(anchor.x - 120, anchor.y - 25, anchor.x + 25 + 40 + 10 * 75, anchor.y + 25 + 4 * 75));
	keyboardFbo->unbindFramebuffer();
	//createKeyboard();
}
std::shared_ptr<cinder::gl::Fbo> TouchKeyboard::getMovingKeyboardFbo()
{
	return movingKeyboardFbo;
}

bool TouchKeyboard::getMovingKeyboard(){
	return movingKeyboard;
}
void TouchKeyboard::setMovingKeyboard(bool moving){
	movingKeyboard = moving;
}
bool TouchKeyboard::keyboardStatus(){
	return keyboardOn;
}

cinder::vec2 TouchKeyboard::getAnchor(){
	return anchor;
}

void TouchKeyboard::createKeyboard(){
	
	string keys;
	if (!shiftOn){
		keys = "1234567890qwertyuiopasdfghjkl;zxcvbnm,./";
	}
	else{
		keys = "!@#$%^&*()QWERTYUIOPASDFGHJKL:ZXCVBNM<>?";
	}
	keyboardFbo->bindFramebuffer();
	glClearColor(0.0,0.0,0.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT);
	cinder::gl::color(0.0, 0.0, 0.0, 1.0);
	
	gl::drawSolidRect(cinder::Rectf(anchor.x - 120, anchor.y - 25, anchor.x + 25 + 40 + 10 * 75, anchor.y +  25+ 4 * 75));
	


	int i;
	int j;
	for (j = 0; j < 4; j++){
		cinder::gl::color(0.75, 0.75, 0.75, 1.);
		cinder::gl::drawSolidRect(Rectf(anchor.x - 100, anchor.y+j*75, anchor.x + j*20, anchor.y +(j+1) *75));
		cinder::gl::color(0.0, 0.0, 0.0, 1.0);
		cinder::gl::drawStrokedRect(Rectf(anchor.x - 100, anchor.y + j * 75, anchor.x + j * 20, anchor.y + (j + 1) * 75),3);
		for (i = 0; i < 10; i++)
		{
			cinder::gl::color(.75, .75, .75, 1.0);
			
			cinder::gl::drawSolidRect(cinder::Rectf(anchor.x  + 20*j + i * 75, anchor.y + j * 75, anchor.x +20*j+ (i + 1) * 75, anchor.y + (j + 1) * 75));
			cinder::gl::color(0.0, 0.0, 0.0, 1.0);
			cinder::gl::drawStrokedRect(cinder::Rectf(anchor.x +20*j + i * 75, anchor.y + j * 75, anchor.x +20*j+ (i + 1) * 75, anchor.y + (j + 1) * 75), 3);
			TextLayout layout1;
			layout1.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
			layout1.setFont(Font("Arial", 100));
			layout1.setColor(Color(1, 1, 1));
			string nextKey;
			stringstream ss;
			ss << keys[i + j * 10];
			ss >> nextKey;
			layout1.addLine(nextKey);
			Surface8u rendered = layout1.render(true, false);
			gl::Texture2dRef mTexture = gl::Texture2d::create(rendered);
			gl::color(Color::white());
			gl::draw(mTexture, cinder::Rectf(anchor.x + 20 * j + i * 75, anchor.y + j * 75, anchor.x + 20* j + (i + 1) * 75, anchor.y + (j + 1) * 75));


		}
	}
	keyboardFbo->unbindFramebuffer();
}
void TouchKeyboard::appendString(std::string text){
	printString += text;
}

void TouchKeyboard::drawKeyboard(){
	cinder::gl::color(1.0, 1.0, 1.0, 1.0);
	cinder::gl::draw(keyboardFbo->getColorTexture());
}

std::shared_ptr<cinder::gl::Fbo> TouchKeyboard::getKeyboardFbo(){
	return keyboardFbo;
}

bool TouchKeyboard::onKeyboardSurface(cinder::vec2 touch){
	//Checks if a interaction is located on the keyboards surface.
	//Mostly used for touch interactions currently.
	int x = touch.x;
	int y = touch.y;
	//First row of Keys
	if (anchor.y < y && y < anchor.y + 50)
	{
		int i;
		for (i = 0; i < 10; i++)
		{

			if (anchor.x + i*50 < x && x < anchor.x + (i + 1) *50){
				break;
			}
		}
		switch (i){
		case 0:
			appendString("1");
			return true;
			break;
		case 1:
			appendString("2");
			return true;
			break;
		case 2:
			appendString("3");
			return true;
			break;
		case 3:
			appendString("4");
			return true;
			break;
		case 4:
			appendString("5");
			return true;
			break;
		case 5:
			appendString("6");
			return true;
			break;
		case 6:
			appendString("7");
			return true;
			break;
		case 7:
			appendString("8");
			return true;
			break;
		case 8:
			appendString("9");
			return true;
			break;
		case 9:
			appendString("0");
			return true;
			break;

		}
	}
	if (anchor.y +50 < y && y < anchor.y + 100)
	{
		int i;
		for (i = 0; i < 9; i++)
		{

			if (anchor.x + i * 50 < x && x < anchor.x + (i + 1) * 50){
				break;
			}
		}
		switch (i){
		case 0:
			appendString("q");
			return true;
			break;
		case 1:
			appendString("w");
			return true;
			break;
		case 2:
			appendString("e");
			return true;
			break;
		case 3:
			appendString("r");
			return true;
			break;
		case 4:
			appendString("t");
			return true;
			break;
		case 5:
			appendString("y");
			return true;
			break;
		case 6:
			appendString("u");
			return true;
			break;
		case 7:
			appendString("i");
			return true;
			break;
		case 8:
			appendString("o");
			return true;
			break;
		case 9:
			appendString("p");
			return true;
			break;

		}
	}
	if (anchor.y + 100 < y && y < anchor.y + 150)
	{
		int i;
		for (i = 0; i < 9; i++)
		{

			if (anchor.x + i * 50 < x < anchor.x + (i + 1) * 50){
				break;
			}
		}
		switch (i){
		case 0:
			appendString("a");
			return true;
			break;
		case 1:
			appendString("s");
			return true;
			break;
		case 2:
			appendString("d");
			return true;
			break;
		case 3:
			appendString("f");
			return true;
			break;
		case 4:
			appendString("g");
			return true;
			break;
		case 5:
			appendString("h");
			return true;
			break;
		case 6:
			appendString("j");
			return true;
			break;
		case 7:
			appendString("k");
			return true;
			break;
		case 8:
			appendString("l");
			return true;
			break;
		case 9:
			appendString(";");
			return true;
			break;

		}
	}
	if (anchor.y + 150 < y && y < anchor.y + 200)
	{
		int i;
		for (i = 0; i < 9; i++)
		{

			if (anchor.x + i * 50 < x < anchor.x + (i + 1) * 50){
				break;
			}
		}
		switch (i){
		case 0:
			appendString("z");
			return true;
			break;
		case 1:
			appendString("x");
			return true;
			break;
		case 2:
			appendString("c");
			return true;
			break;
		case 3:
			appendString("v");
			return true;
			break;
		case 4:
			appendString("b");
			return true;
			break;
		case 5:
			appendString("n");
			return true;
			break;
		case 6:
			appendString("m");
			return true;
			break;
		case 7:
			appendString(",");
			return true;
			break;
		case 8:
			appendString(".");
			return true;
			break;
		case 9:
			appendString("/");
			return true;
			break;

		}
	}

	//if (((anchor.x + 25 + 40 + 10 * 75) >= x && x >= (anchor.x - 120)) && ((anchor.y + 25 + 4 * 75) >= y && y >= (anchor.y - 25))){
		//return true;
	//}
	return false;
}
#endif