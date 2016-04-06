#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H
//Cinder and OpenGL Includes
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppBase.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include <math.h>
#include <chrono>
#include <ctime>
#include "String.h"

//Our own includes
//#include "TouchShapes.h"

//Standard Library Includes
#include <vector>
#include <map>
#include <list>
#include <time.h>
#include "Enums.h"

struct ImageHandler{

	ImageHandler(){}
	//Creates an ImageHandler
	ImageHandler(std::vector<std::shared_ptr<gl::Fbo>>* fboLayerList, std::vector<float>* fboLayerAlpha){
		mLayerList = fboLayerList;
		iconFlag = false;
		fadeTime = 1.0f;
		imageType = "png";
		imageNum = 0;
		layerAlpha = fboLayerAlpha;
	}

	void saveCanvas(vec2 windowSize, ColorA background);
	void loadIcon(string icon);
	void displayIcon();
	void changeImageType(string imageType);
	bool getIconFlag();



private:
	std::vector<std::shared_ptr<gl::Fbo>>* mLayerList;
	std::vector<float>* layerAlpha;
	bool iconFlag;
	float fadeTime;
	string imageType;
	int imageNum;
	cinder::gl::TextureRef imageTexture;
	//std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::milliseconds start;



};
bool ImageHandler::getIconFlag(){
	return iconFlag;
}
void ImageHandler::displayIcon(){
	if (iconFlag == false)
		return;
	gl::color(1.0, 1.0, 1.0, fadeTime);
	gl::draw(imageTexture, Rectf(getWindowSize().x / 2 + 55, getWindowSize().y / 2 - 55, getWindowSize().x / 2 - 100, getWindowSize().y / 2 + 100));
	//ms is fade time in milliseconds.
	std::chrono::milliseconds ms{ 2000 };
	std::chrono::milliseconds dif{ std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - start };
	long longMs = ms.count();
	long longDif = dif.count();
	fadeTime = 1.0f*(longMs - longDif)/longMs;
	if (dif  >= ms){
		iconFlag = false;
	}
	//fadeTime -= 0.009;

	/*
	if (fadeTime <= 0.1){
		iconFlag = false;
//		processing = false;
		fadeTime = 1;
	}

	fadeTime -= 0.009;
	*/
}
void ImageHandler::changeImageType(string newImageType){
	imageType = newImageType;
}
void ImageHandler::loadIcon(string icon){
	imageTexture = gl::Texture::create(loadImage(loadAsset(icon)));
	fadeTime = 1.0f;
	start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	iconFlag = true;
}

void ImageHandler::saveCanvas(vec2 windowSize, ColorA background){
	gl::Fbo::Format format;
	std::shared_ptr<gl::Fbo> saveImageFbo;
	saveImageFbo = gl::Fbo::create(windowSize.x, windowSize.y, format);

	(*saveImageFbo).bindFramebuffer();
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	gl::color(background);
	gl::drawSolidRect(Rectf(0, 0, windowSize.x, windowSize.y));
	gl::color(1.0, 1.0, 1.0, 1.0);
	int x = 0;
	for (auto frames : *mLayerList){
		gl::color(1.0, 1.0, 1.0, (*layerAlpha)[x]);
		gl::draw(frames->getColorTexture());
		x++;
	}
	(*saveImageFbo).unbindFramebuffer();
	//writeImage(getHomeDirectory() / "Interactive Paint" / "Saved_Images" / (toString(imageNum) + imageType), copyWindowSurface());
	writeImage(cinder::getHomeDirectory() / "Interactive_Paint" / "Saved_Images" / (cinder::toString(imageNum) + "." + imageType), (saveImageFbo->getColorTexture())->createSource());
	imageNum++;
	loadIcon("Save.png");
	cout << "Image " << imageNum << "Saved!";
}



#endif