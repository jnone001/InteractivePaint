#include "ImageHandler.h"

namespace touchpoints { namespace drawing
{
	ImageHandler::ImageHandler() {}

	ImageHandler::ImageHandler(std::vector<std::shared_ptr<cinder::gl::Fbo>>* fboLayerList, std::vector<float>* fboLayerAlpha)
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

	bool ImageHandler::getIconFlag()
	{
		return iconFlag;
	}

	//Function that will display the currently loaded icon. 
	//Has a fade built in based on how long it's been since the icon was loaded.
	void ImageHandler::displayIcon()
	{
		if (iconFlag == false) return;
		cinder::gl::color(1.0, 1.0, 1.0, fadeTime);
		cinder::gl::draw(imageTexture, cinder::Rectf(cinder::app::getWindowSize().x / 2 + 55, cinder::app::getWindowSize().y / 2 - 55, cinder::app::getWindowSize().x / 2 - 100, cinder::app::getWindowSize().y / 2 + 100));
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

	void ImageHandler::changeImageType(std::string newImageType)
	{
		imageType = newImageType;
	}

	void ImageHandler::loadIcon(std::string icon)
	{
		imageTexture = cinder::gl::Texture::create(cinder::loadImage(cinder::app::loadAsset(icon)));
		fadeTime = 1.0f;
		start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		iconFlag = true;
	}

	void ImageHandler::displayStartIcon()
	{
		if (iconStartFlag == false) return;
		cinder::gl::color(1.0, 1.0, 1.0, startFadeTime);
		cinder::gl::draw(imageTexture, cinder::Rectf(cinder::app::getWindowSize().x / 2 - 500, cinder::app::getWindowSize().y / 2 - 255, cinder::app::getWindowSize().x / 2 + 455, cinder::app::getWindowSize().y / 2 + 300)); //ms is fade time in milliseconds.
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

	void ImageHandler::loadStartIcon(std::string icon)
	{
		imageTexture = cinder::gl::Texture::create(cinder::loadImage(cinder::app::loadAsset(icon)));
		startFadeTime = 1.0f;
		start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		startUpFlag = false;
		iconStartFlag = true;
	}

	bool ImageHandler::getStartUpFlag()
	{
		return startUpFlag;
	}

	void ImageHandler::saveCanvas(cinder::vec2 windowSize, cinder::ColorA background)
	{
		cinder::gl::Fbo::Format format;
		std::shared_ptr<cinder::gl::Fbo> saveImageFbo;
		saveImageFbo = cinder::gl::Fbo::create(windowSize.x, windowSize.y, format);

		(*saveImageFbo).bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		cinder::gl::color(background);
		cinder::gl::drawSolidRect(cinder::Rectf(0, 0, windowSize.x, windowSize.y));
		cinder::gl::color(1.0, 1.0, 1.0, 1.0);
		int x = 0;
		for (auto frames : *mLayerList)
		{
			cinder::gl::color(1.0, 1.0, 1.0, (*layerAlpha)[x]);
			cinder::gl::draw(frames->getColorTexture());
			x++;
		}
		(*saveImageFbo).unbindFramebuffer();
		writeImage(cinder::getHomeDirectory() / "Interactive_Paint" / "Saved_Images" / (cinder::toString(imageNum) + "." + imageType), (saveImageFbo->getColorTexture())->createSource());
		imageNum++;
		loadIcon("Save.png");
		std::cout << "Image " << imageNum << "Saved!";
	}
}}
