#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppBase.h"
#include <chrono>
#include <vector>

namespace touchpoints { namespace drawing
{
	struct ImageHandler
	{
	public:
		ImageHandler();
		ImageHandler(std::vector<std::shared_ptr<cinder::gl::Fbo>>* fboLayerList, std::vector<float>* fboLayerAlpha);
		void saveCanvas(cinder::vec2 windowSize, cinder::ColorA background);
		void loadIcon(std::string icon);
		void displayIcon();
		void changeImageType(std::string imageType);
		bool getIconFlag();
		bool getStartUpFlag();
		void displayStartIcon();
		void loadStartIcon(std::string icon);
	private:
		std::vector<std::shared_ptr<cinder::gl::Fbo>>* mLayerList;
		std::vector<float>* layerAlpha;
		bool iconFlag;
		bool startUpFlag;
		bool iconStartFlag;
		float fadeTime;
		float startFadeTime;
		std::string imageType;
		int imageNum;
		cinder::gl::TextureRef imageTexture;
		cinder::gl::TextureRef startImageTexture;
		std::chrono::milliseconds start;
	};
}}