#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include <list>

namespace touchpoints { namespace ui
{
	struct TouchKeyboard
	{
	public:
		TouchKeyboard();
		TouchKeyboard(int windowWidth, int windowHeight);
		void appendString(std::string text);
		void drawKeyboard();
		bool onKeyboardSurface(cinder::vec2 touch);
		void createKeyboard();
		std::shared_ptr<cinder::gl::Fbo> getKeyboardFbo();
		std::shared_ptr<cinder::gl::Fbo> getMovingKeyboardFbo();
		cinder::vec2 getAnchor();
		bool keyboardStatus();
		void turnOnKeyboard();
		void setMovingKeyboard(bool moving);
		bool getMovingKeyboard();
		bool checkMovingId(uint32_t id);
		void setMovingId(uint32_t id, cinder::vec2 pos);
		void setAnchor(cinder::vec2 pos);
		void finishMoving();
		void drawText();
		void setTextAnchor(cinder::vec2 pos);
		bool getSettingText();
		void beginSettingTextAnchor();
		cinder::vec2 getTextAnchor();
		bool needsCleanup();
		void endText();
		std::shared_ptr<cinder::gl::Fbo> getTextFbo();
		void cycleFont();
		void writeText(cinder::vec2 startPos, cinder::vec2 endPos, std::string text);
		bool onKeyboardButtons(cinder::vec2 touch);
	private:
		std::string printString;
		std::string font;
		std::list<std::string> fontList;
		cinder::vec2 anchor;
		cinder::vec2 userFingerDistance;
		cinder::vec2 textAnchor = cinder::vec2(1000, 500);
		std::shared_ptr<cinder::gl::Fbo> keyboardFbo;
		std::shared_ptr<cinder::gl::Fbo> movingKeyboardFbo;
		std::shared_ptr<cinder::gl::Fbo> textFbo;
		cinder::gl::Texture2dRef textTexture;
		bool keyboardOn = false;
		bool shiftOn = false;
		bool settingTextAnchor = false;
		bool movingKeyboard = false;
		bool caps = false;
		bool textCleanup = false;
		uint32_t moverId = 0;
	};
}}
