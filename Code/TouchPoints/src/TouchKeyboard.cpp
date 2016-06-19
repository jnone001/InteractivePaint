#include "TouchKeyboard.h"

namespace touchpoints { namespace ui
{
	TouchKeyboard::TouchKeyboard() {}

	TouchKeyboard::TouchKeyboard(int windowWidth, int windowHeight)
	{
		printString = "";
		anchor = cinder::vec2(windowWidth * .2, windowHeight * .6);
		cinder::gl::Fbo::Format format;
		keyboardFbo = cinder::gl::Fbo::create(windowWidth, windowHeight, format);
		movingKeyboardFbo = cinder::gl::Fbo::create(windowWidth, windowHeight, format);
		textFbo = cinder::gl::Fbo::create(windowWidth, windowHeight, format);

		//Our list of Fonts
		fontList.emplace_back("Arial");
		fontList.emplace_back("Times New Roman");
		fontList.emplace_back("Papyrus");
		fontList.emplace_back("Symbol");
		font = fontList.front();
	}

	void TouchKeyboard::cycleFont()
	{
		fontList.emplace_back(fontList.front());
		fontList.pop_front();
		font = fontList.front();
		createKeyboard();
	}

	//Tells anyone if the keyboard is 'done' writing text. Indicating that the program should quickly save any data it needs from the keyboard.
	bool TouchKeyboard::needsCleanup()
	{
		return textCleanup;
	}

	//Clears the string and disables all text writing features.
	void TouchKeyboard::endText()
	{
		textFbo->bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		cinder::TextLayout layout1;
		layout1.clear(cinder::ColorA(0.2f, 0.2f, 0.2f, 0.0f));
		layout1.setFont(cinder::Font(font, 62));
		layout1.setColor(cinder::Color(0, 0, 0));
		layout1.addLine(printString);
		cinder::Surface8u rendered = layout1.render(true, false);
		textTexture = cinder::gl::Texture2d::create(rendered);
		cinder::gl::color(cinder::Color::white());
		cinder::gl::draw(textTexture, textAnchor);
		textFbo->unbindFramebuffer();

		printString = "";
		keyboardOn = false;
		textCleanup = false;
	}

	//Returns where the text is going to be written
	cinder::vec2 TouchKeyboard::getTextAnchor()
	{
		return textAnchor;
	}

	//Turns on keyboard
	void TouchKeyboard::turnOnKeyboard()
	{
		keyboardOn = true;
		drawText();
	}

	//Sets where you want to write the text
	void TouchKeyboard::setTextAnchor(cinder::vec2 pos)
	{
		textAnchor = pos;
		settingTextAnchor = false;
	}

	bool TouchKeyboard::getSettingText()
	{
		return settingTextAnchor;
	}

	void TouchKeyboard::beginSettingTextAnchor()
	{
		settingTextAnchor = true;
	}

	std::shared_ptr<cinder::gl::Fbo> TouchKeyboard::getTextFbo()
	{
		return textFbo;
	}

	//Draws the current printString to the textFbo
	void TouchKeyboard::drawText()
	{
		textFbo->bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		cinder::TextLayout layout1;
		layout1.clear(cinder::ColorA(0.2f, 0.2f, 0.2f, 0.0f));
		layout1.setFont(cinder::Font(font, 62));
		layout1.setColor(cinder::Color(0, 0, 0));
		layout1.addLine(printString + "|");
		cinder::Surface8u rendered = layout1.render(true, false);
		textTexture = cinder::gl::Texture2d::create(rendered);
		cinder::gl::color(cinder::Color::white());
		cinder::gl::draw(textTexture, textAnchor);
		textFbo->unbindFramebuffer();
	}

	void TouchKeyboard::finishMoving()
	{
		createKeyboard();
		movingKeyboard = false;
		moverId = 0;
	}

	void TouchKeyboard::setMovingId(uint32_t id, cinder::vec2 pos)
	{
		userFingerDistance = cinder::vec2(anchor.x - pos.x, anchor.y - pos.y);
		moverId = id;
	}

	bool TouchKeyboard::checkMovingId(uint32_t id)
	{
		return id == moverId;
	}

	void TouchKeyboard::setAnchor(cinder::vec2 pos)
	{
		anchor = cinder::vec2(pos.x + userFingerDistance.x, pos.y + userFingerDistance.y);
		keyboardFbo->bindFramebuffer();
		glClearColor(0.0, 0.0, 0.0, 0.0);

		glClear(GL_COLOR_BUFFER_BIT);
		cinder::gl::color(0.0, 0.0, 0.0, 0.8);

		cinder::gl::drawSolidRect(cinder::Rectf(anchor.x - 120, anchor.y - 25, anchor.x + 25 + 40 + 10 * 75 + 3 * 20 + 100, anchor.y + 25 + 5 * 75));
		keyboardFbo->unbindFramebuffer();
		//createKeyboard();
	}

	std::shared_ptr<cinder::gl::Fbo> TouchKeyboard::getMovingKeyboardFbo()
	{
		return movingKeyboardFbo;
	}

	bool TouchKeyboard::getMovingKeyboard()
	{
		return movingKeyboard;
	}

	void TouchKeyboard::setMovingKeyboard(bool moving)
	{
		movingKeyboard = moving;
	}

	bool TouchKeyboard::keyboardStatus()
	{
		return keyboardOn;
	}

	cinder::vec2 TouchKeyboard::getAnchor()
	{
		return anchor;
	}

	//Writes text to a given position and a string. Font, color etc hard coded inside here.
	void TouchKeyboard::writeText(cinder::vec2 startPos, cinder::vec2 endPos, std::string text)
	{
		cinder::TextLayout layout1;
		layout1.clear(cinder::ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout1.setFont(cinder::Font(font, 200));
		layout1.setColor(cinder::Color(1, 1, 1));
		layout1.addLine(text);
		cinder::Surface8u rendered = layout1.render(true, false);
		cinder::gl::Texture2dRef mTexture = cinder::gl::Texture2d::create(rendered);
		cinder::gl::color(cinder::Color::white());
		cinder::gl::draw(mTexture, cinder::Rectf(startPos.x, startPos.y, endPos.x, endPos.y));
	}

	void TouchKeyboard::createKeyboard()
	{
		std::string keys;
		if (!shiftOn)
		{
			keys = "1234567890qwertyuiopasdfghjkl;zxcvbnm,./";
		}
		else
		{
			keys = "!@#$%^&*()QWERTYUIOPASDFGHJKL:ZXCVBNM<>?";
		}
		keyboardFbo->bindFramebuffer();
		glClearColor(0.0, 0.0, 0.0, 0.0);

		glClear(GL_COLOR_BUFFER_BIT);

		//Draw the entire outline of the Keyboard.
		//This Rectf parameters are used as the locations for onKeyboardSurface
		cinder::gl::color(0.0, 0.0, 0.0, 0.8);

		cinder::gl::drawSolidRect(cinder::Rectf(anchor.x - 120, anchor.y - 25, anchor.x + 25 + 40 + 10 * 75 + 3 * 20 + 100, anchor.y + 25 + 5 * 75));

		cinder::vec2 start;
		cinder::vec2 stop;
		std::string myString;

		int i;
		int j;
		for (j = 0; j < 4; j++)
		{
			//Draws boxes for Tilda, tab, shift buttons, starting keys in the row.
			cinder::gl::color(0.75, 0.75, 0.75, 1.);
			cinder::gl::drawSolidRect(cinder::Rectf(anchor.x - 100, anchor.y + j * 75, anchor.x + j * 20, anchor.y + (j + 1) * 75));
			cinder::gl::color(0.0, 0.0, 0.0, 1.0);
			cinder::gl::drawStrokedRect(cinder::Rectf(anchor.x - 100, anchor.y + j * 75, anchor.x + j * 20, anchor.y + (j + 1) * 75), 3);

			//Prepares to draw text onto the boxes
			start = cinder::vec2(anchor.x - 100, anchor.y + j * 75);
			stop = cinder::vec2(anchor.x + j * 20, anchor.y + (j + 1) * 75);
			switch (j)
			{
			case 0:
				if (shiftOn)
				{
					myString = " ~ ";
				}
				else myString = " `   ";
				break;
			case 1:
				myString = " Tab";
				break;
			case 2:
				myString = " Font";
				break;
			case 3:
				myString = " Shift";
				break;
			}

			writeText(start, stop, myString);

			//Draws boxes for backspace, \, enter and done buttons. Ending keys in the row

			cinder::gl::color(0.75, 0.75, 0.75, 1.);
			cinder::gl::drawSolidRect(cinder::Rectf(anchor.x + 20 * j + 9 * 75, anchor.y + j * 75, anchor.x + 20 * j + ((3 - j) * 20) + 100 + (9 + 1) * 75, anchor.y + (j + 1) * 75));
			cinder::gl::color(0.0, 0.0, 0.0, 1.0);
			cinder::gl::drawStrokedRect(cinder::Rectf(anchor.x + 20 * j + 9 * 75, anchor.y + j * 75, anchor.x + 20 * j + ((3 - j) * 20) + 100 + (9 + 1) * 75, anchor.y + (j + 1) * 75), 3);

			//Prepares to draw text  onto boxes
			start = cinder::vec2(anchor.x + 20 * j + 10 * 75, anchor.y + j * 75);
			stop = cinder::vec2(anchor.x + 20 * j + ((3 - j) * 20) + 100 + (9 + 1) * 75, anchor.y + (j + 1) * 75);
			switch (j)
			{
			case 0:
				myString = "Backspace";
				break;
			case 1:
				if (shiftOn)
				{
					myString = " | ";
				}
				else myString = " \\    ";
				break;
			case 2:
				myString = "Enter";
				break;
			case 3:
				myString = "Done";
				break;
			}

			writeText(start, stop, myString);

			//Draw the first 10 keys of a row.
			for (i = 0; i < 10; i++)
			{
				cinder::gl::color(.75, .75, .75, 1.0);

				cinder::gl::drawSolidRect(cinder::Rectf(anchor.x + 20 * j + i * 75, anchor.y + j * 75, anchor.x + 20 * j + (i + 1) * 75, anchor.y + (j + 1) * 75));
				cinder::gl::color(0.0, 0.0, 0.0, 1.0);
				cinder::gl::drawStrokedRect(cinder::Rectf(anchor.x + 20 * j + i * 75, anchor.y + j * 75, anchor.x + 20 * j + (i + 1) * 75, anchor.y + (j + 1) * 75), 3);
				cinder::TextLayout layout1;
				layout1.clear(cinder::ColorA(0.2f, 0.2f, 0.2f, 0.2f));
				layout1.setFont(cinder::Font(font, 200));
				layout1.setColor(cinder::Color(1, 1, 1));
				std::string nextKey;
				std::stringstream ss;
				ss << keys[i + j * 10];
				ss >> nextKey;
				layout1.addLine(nextKey);
				cinder::Surface8u rendered = layout1.render(true, false);
				cinder::gl::Texture2dRef mTexture = cinder::gl::Texture2d::create(rendered);
				cinder::gl::color(cinder::Color::white());
				cinder::gl::draw(mTexture, cinder::Rectf(anchor.x + 20 * j + i * 75, anchor.y + j * 75, anchor.x + 20 * j + (i + 1) * 75, anchor.y + (j + 1) * 75));
			}

			//Manually draw any additional keys needed in the row
		}

		//Draw Space Bar.
		cinder::gl::color(0.75, 0.75, 0.75, 1.);
		cinder::gl::drawSolidRect(cinder::Rectf(anchor.x, anchor.y + j * 75, anchor.x + ((3 - j) * 20) + 100 + (9 + 1) * 75, anchor.y + (j + 1) * 75));
		cinder::gl::color(0.0, 0.0, 0.0, 1.0);
		cinder::gl::drawStrokedRect(cinder::Rectf(anchor.x, anchor.y + j * 75, anchor.x + ((3 - j) * 20) + 100 + (9 + 1) * 75, anchor.y + (j + 1) * 75), 3);

		start = cinder::vec2(anchor.x + 200, anchor.y + j * 75);
		stop = cinder::vec2(anchor.x - 100 + (9 + 1) * 75, anchor.y + (j + 1) * 75);
		myString = "Space";
		writeText(start, stop, myString);
		keyboardFbo->unbindFramebuffer();
	}

	void TouchKeyboard::appendString(std::string text)
	{
		printString += text;
	}

	void TouchKeyboard::drawKeyboard()
	{
		cinder::gl::color(1.0, 1.0, 1.0, 1.0);
		cinder::gl::draw(keyboardFbo->getColorTexture());
	}

	std::shared_ptr<cinder::gl::Fbo> TouchKeyboard::getKeyboardFbo()
	{
		return keyboardFbo;
	}

	bool TouchKeyboard::onKeyboardSurface(cinder::vec2 touch)
	{
		if (anchor.y - 25 < touch.y && touch.y < anchor.y + 25 + 5 * 75)
		{
			if (anchor.x - 120 < touch.x && touch.x < anchor.x + 25 + 40 + 10 * 75 + 3 * 20 + 100)
			{
				return true;
			}
		}
		return false;
	}

	bool TouchKeyboard::onKeyboardButtons(cinder::vec2 touch)
	{
		//Checks if a interaction is located on the keyboards surface.
		//Mostly used for touch interactions currently.
		int x = touch.x;
		int y = touch.y;
		int key = -1;
		//First row of Keys
		//Increment J myself, then put these in there. easy pz.
		int j = 0;
		if (anchor.y + j * 75 < y && y < anchor.y + (j + 1) * 75)
		{
			//Checks for '`' and Backspace
			if (anchor.x - 100 < x && x < anchor.x + j * 20)
			{
				if (shiftOn)
				{
					appendString("~");
				}
				else appendString("`");
				drawText();
				return true;
			}
			if (anchor.x + 20 * j + 10 * 75 < x && x < anchor.x + 20 * j + ((3 - j) * 20) + 100 + (9 + 1) * 75)
			{
				if (!printString.empty())
				{
					printString.pop_back();
				}
				drawText();
				return true;
			}

			//Checks for interaction on the button line
			int i;
			for (i = 0; i < 10; i++)
			{
				if (anchor.x + 20 * j + i * 75 < x && x < anchor.x + 20 * j + (i + 1) * 75)
				{
					key = i;
					break;
				}
			}
			switch (key)
			{
			case 0:
				if (shiftOn)
				{
					appendString("!");
				}
				else appendString("1");
				drawText();
				return true;
			case 1:
				if (shiftOn)
				{
					appendString("@");
				}
				else appendString("2");
				drawText();
				return true;
			case 2:
				if (shiftOn)
				{
					appendString("#");
				}
				else appendString("3");
				drawText();
				return true;
			case 3:
				if (shiftOn)
				{
					appendString("$");
				}
				else appendString("4");
				drawText();
				return true;
			case 4:
				if (shiftOn)
				{
					appendString("%");
				}
				else appendString("5");
				drawText();
				return true;
			case 5:
				if (shiftOn)
				{
					appendString("^");
				}
				else appendString("6");
				drawText();
				return true;
			case 6:
				if (shiftOn)
				{
					appendString("&");
				}
				else appendString("7");
				drawText();
				return true;
			case 7:
				if (shiftOn)
				{
					appendString("*");
				}
				else appendString("8");
				drawText();
				return true;
			case 8:
				if (shiftOn)
				{
					appendString("(");
				}
				else appendString("9");
				drawText();
				return true;
			case 9:
				if (shiftOn)
				{
					appendString(")");
				}
				else appendString("0");
				drawText();
				return true;
			}
		}
		j++;
		if (anchor.y + j * 75 < y && y < anchor.y + (j + 1) * 75)
		{
			//Checks for 'Tab' and '\'
			if (anchor.x - 100 < x && x < anchor.x + j * 20)
			{
				appendString("     ");
				drawText();
				return true;
			}
			if (anchor.x + 20 * j + 10 * 75 < x && x < anchor.x + 20 * j + ((3 - j) * 20) + 100 + (9 + 1) * 75)
			{
				if (shiftOn)
				{
					appendString("|");
				}
				else appendString("\\");
				drawText();
				return true;
			}

			int i;
			for (i = 0; i < 10; i++)
			{
				if (anchor.x + 20 * j + i * 75 < x && x < anchor.x + 20 * j + (i + 1) * 75)
				{
					key = i;
					break;
				}
			}
			switch (key)
			{
			case 0:
				if (shiftOn)
				{
					appendString("Q");
				}
				else appendString("q");
				drawText();
				return true;
			case 1:
				if (shiftOn)
				{
					appendString("W");
				}
				else appendString("w");
				drawText();
				return true;
			case 2:
				if (shiftOn)
				{
					appendString("E");
				}
				else appendString("e");
				drawText();
				return true;
			case 3:
				if (shiftOn)
				{
					appendString("R");
				}
				else appendString("r");
				drawText();
				return true;
			case 4:
				if (shiftOn)
				{
					appendString("T");
				}
				else appendString("t");
				drawText();
				return true;
			case 5:
				if (shiftOn)
				{
					appendString("Y");
				}
				else appendString("y");
				drawText();
				return true;
			case 6:
				if (shiftOn)
				{
					appendString("U");
				}
				else appendString("u");
				drawText();
				return true;
			case 7:
				if (shiftOn)
				{
					appendString("I");
				}
				else appendString("i");
				drawText();
				return true;
			case 8:
				if (shiftOn)
				{
					appendString("O");
				}
				else appendString("o");
				drawText();
				return true;
			case 9:
				if (shiftOn)
				{
					appendString("P");
				}
				else appendString("p");
				drawText();
				return true;
			}
		}
		j++;
		if (anchor.y + j * 75 < y && y < anchor.y + (j + 1) * 75)
		{
			//Checks for 'Font' and Enter
			if (anchor.x - 100 < x && x < anchor.x + j * 20)
			{
				cycleFont();
				drawText();
				return true;
			}
			if (anchor.x + 20 * j + 10 * 75 < x && x < anchor.x + 20 * j + ((3 - j) * 20) + 100 + (9 + 1) * 75)
			{
				//Enter functionality not yet implemented.
				appendString("\n");
				drawText();
				return true;
			}
			int i;
			for (i = 0; i < 10; i++)
			{
				if (anchor.x + 20 * j + i * 75 < x && x < anchor.x + 20 * j + (i + 1) * 75)
				{
					key = i;
					break;
				}
			}
			switch (key)
			{
			case 0:
				if (shiftOn)
				{
					appendString("A");
				}
				else appendString("a");
				drawText();
				return true;
			case 1:
				if (shiftOn)
				{
					appendString("S");
				}
				else appendString("s");
				drawText();
				return true;
			case 2:
				if (shiftOn)
				{
					appendString("D");
				}
				else appendString("d");
				drawText();
				return true;
			case 3:
				if (shiftOn)
				{
					appendString("F");
				}
				else appendString("f");
				drawText();
				return true;
			case 4:
				if (shiftOn)
				{
					appendString("G");
				}
				else appendString("g");
				drawText();
				return true;
			case 5:
				if (shiftOn)
				{
					appendString("H");
				}
				else appendString("h");
				drawText();
				return true;
			case 6:
				if (shiftOn)
				{
					appendString("J");
				}
				else appendString("j");
				drawText();
				return true;
			case 7:
				if (shiftOn)
				{
					appendString("K");
				}
				else appendString("k");
				drawText();
				return true;
			case 8:
				if (shiftOn)
				{
					appendString("L");
				}
				else appendString("l");
				drawText();
				return true;
			case 9:
				if (shiftOn)
				{
					appendString(":");
				}
				else appendString(";");
				drawText();
				return true;
			}
		}
		j++;
		if (anchor.y + j * 75 < y && y < anchor.y + (j + 1) * 75)
		{
			//Checks for Shift and 'Done' key
			if (anchor.x - 100 < x && x < anchor.x + j * 20)
			{
				shiftOn = !shiftOn;
				createKeyboard();
				drawText();
				return true;
			}
			if (anchor.x + 20 * j + 10 * 75 < x && x < anchor.x + 20 * j + ((3 - j) * 20) + 100 + (9 + 1) * 75)
			{
				textCleanup = true;
				drawText();
				return true;
			}

			int i;
			for (i = 0; i < 10; i++)
			{
				if (anchor.x + 20 * j + i * 75 < x && x < anchor.x + 20 * j + (i + 1) * 75)
				{
					key = i;
					break;
				}
			}
			switch (key)
			{
			case 0:
				if (shiftOn)
				{
					appendString("Z");
				}
				else appendString("z");
				drawText();
				return true;
			case 1:
				if (shiftOn)
				{
					appendString("X");
				}
				else appendString("x");
				drawText();
				return true;
			case 2:
				if (shiftOn)
				{
					appendString("C");
				}
				else appendString("c");
				drawText();
				return true;
			case 3:
				if (shiftOn)
				{
					appendString("V");
				}
				else appendString("v");
				drawText();
				return true;
			case 4:
				if (shiftOn)
				{
					appendString("B");
				}
				else appendString("b");
				drawText();
				return true;
			case 5:
				if (shiftOn)
				{
					appendString("N");
				}
				else appendString("n");
				drawText();
				return true;
			case 6:
				if (shiftOn)
				{
					appendString("M");
				}
				else appendString("m");
				drawText();
				return true;
			case 7:
				if (shiftOn)
				{
					appendString("<");
				}
				else appendString(",");
				drawText();
				return true;
			case 8:
				if (shiftOn)
				{
					appendString(">");
				}
				else appendString(".");
				drawText();
				return true;
			case 9:
				if (shiftOn)
				{
					appendString("?");
				}
				else appendString("/");
				drawText();
				return true;
			}
		}

		j++;
		//Checks space button
		if (anchor.y + j * 75 < y && y < anchor.y + (j + 1) * 75)
		{
			if (anchor.x < x && x < anchor.x + ((3 - j) * 20) + 100 + 10 * 75)
			{
				appendString(" ");
				return true;
			}
		}
		return false;
	}
}}
