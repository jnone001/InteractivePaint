#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H
//Cinder and OpenGL Includes
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppBase.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include <chrono>

//Standard Library Includes
#include <vector>

struct ImageHandler
{
	ImageHandler() {}

	//Creates an ImageHandler
	ImageHandler(std::vector<std::shared_ptr<gl::Fbo>>* fboLayerList, std::vector<float>* fboLayerAlpha)
	{
		mLayerList = fboLayerList;
		iconFlag = false;
		fadeTime = 1.0f;
		startFadeTime = 1.0f;
		imageType = "png";
		imageNum = 0;
		layerAlpha = fboLayerAlpha;
		startUpFlag = true;
		iconStartFlag = false;
	}

	void saveCanvas(vec2 windowSize, ColorA background);
	void loadIcon(string icon);
	void displayIcon();
	void changeImageType(string imageType);
	bool getIconFlag();
	bool getStartUpFlag();
	void displayStartIcon();
	void loadStartIcon(string icon);

private:
	std::vector<std::shared_ptr<gl::Fbo>>* mLayerList;
	std::vector<float>* layerAlpha;
	bool iconFlag;
	bool startUpFlag;
	bool iconStartFlag;
	float fadeTime;
	float startFadeTime;
	string imageType;
	int imageNum;
	cinder::gl::TextureRef imageTexture;
	cinder::gl::TextureRef startImageTexture;
	std::chrono::milliseconds start;
};

bool ImageHandler::getIconFlag()
{
	return iconFlag;
}

//Function that will display the currently loaded icon. 
//Has a fade built in based on how long it's been since the icon was loaded.
void ImageHandler::displayIcon()
{
	if (iconFlag == false) return;
	gl::color(1.0, 1.0, 1.0, fadeTime);
	gl::draw(imageTexture, Rectf(getWindowSize().x / 2 + 55, getWindowSize().y / 2 - 55, getWindowSize().x / 2 - 100, getWindowSize().y / 2 + 100));
	//ms is fade time in milliseconds.
	std::chrono::milliseconds ms{2000};
	std::chrono::milliseconds dif{std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - start};
	long longMs = ms.count();
	long longDif = dif.count();
	fadeTime = 1.0f * (longMs - longDif) / longMs;
	if (dif >= ms)
	{
		iconFlag = false;
	}
}

void ImageHandler::changeImageType(string newImageType)
{
	imageType = newImageType;
}

void ImageHandler::loadIcon(string icon)
{
	imageTexture = gl::Texture::create(loadImage(loadAsset(icon)));
	fadeTime = 1.0f;
	start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	iconFlag = true;
}

void ImageHandler::displayStartIcon()
{
	if (iconStartFlag == false) return;
	gl::color(1.0, 1.0, 1.0, startFadeTime);
	gl::draw(imageTexture, Rectf(getWindowSize().x / 2 - 500, getWindowSize().y / 2 - 255, getWindowSize().x / 2 + 455, getWindowSize().y / 2 + 300)); //ms is fade time in milliseconds.
	std::chrono::milliseconds ms{5000};
	std::chrono::milliseconds msDelay{2000};

	std::chrono::milliseconds dif{std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - start};
	long longMs = ms.count();
	long longDif = dif.count();
	long longMsDelay = msDelay.count();
	if (longDif > longMsDelay)
	{
		startFadeTime = 1.0f * (longMs - (longDif - longMsDelay)) / longMs;
	}

	if (dif >= ms + msDelay)
	{
		iconStartFlag = false;
	}
}

void ImageHandler::loadStartIcon(string icon)
{
	imageTexture = gl::Texture::create(loadImage(loadAsset(icon)));
	startFadeTime = 1.0f;
	start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	startUpFlag = false;
	iconStartFlag = true;
}

bool ImageHandler::getStartUpFlag()
{
	return startUpFlag;
}

void ImageHandler::saveCanvas(vec2 windowSize, ColorA background)
{
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
	for (auto frames : *mLayerList)
	{
		gl::color(1.0, 1.0, 1.0, (*layerAlpha)[x]);
		gl::draw(frames->getColorTexture());
		x++;
	}
	(*saveImageFbo).unbindFramebuffer();
	writeImage(cinder::getHomeDirectory() / "Interactive_Paint" / "Saved_Images" / (cinder::toString(imageNum) + "." + imageType), (saveImageFbo->getColorTexture())->createSource());
	imageNum++;
	loadIcon("Save.png");
	cout << "Image " << imageNum << "Saved!";
}

#endif
