#include "UserInterface.h"
#include <cinder/Rand.h>

using namespace cinder::app;

namespace touchpoints { namespace ui
{
	UserInterface::UserInterface() {}

	UserInterface::UserInterface(int mWindowWidth, int mWindowHeight, drawing::Brush* brush, drawing::Illustrator* mIllustrator, devices::DeviceHandler* mDeviceHandler, std::shared_ptr<gl::Fbo> fbo, std::vector<std::shared_ptr<gl::Fbo>>* fboLayerList, std::vector<float>* fboLayerAlpha)
	{
		modeChangeFlag = true;
		windowWidth = mWindowWidth;
		windowHeight = mWindowHeight;
		mBrush = brush;
		illustrator = mIllustrator;
		deviceHandler = mDeviceHandler;
		uiFbo = fbo;
		uiFboFlag = true;
		modeButtons = true;
		brushButtons = false;
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
		for (auto layers : *layerList)
		{
			(*layerAlpha).emplace_back(1.0f);
		}
		keyboard = TouchKeyboard(mWindowWidth, mWindowHeight);
	}

	void UserInterface::drawShapesButtonsFbo()
	{
		shapeButtonsFbo->bindFramebuffer();
		for (int i = 0; i < 5; i++)
		{
			gl::color(1.0f, 1.0f, 1.0f);
			gl::drawSolidRect(Rectf(50, 50 * i + 50, 100, 50 * i + 100));
			gl::color(0.75f, 0.75f, .75f, 1.0f);
			gl::drawStrokedRect(Rectf(50, 50 * (i) + 50, 100, 50 * i + 100), uiOutlineSize);
		}

		gl::TextureRef texture = gl::Texture::create(loadImage(loadAsset("Eraser.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(55, 255, 95, 295));

		gl::color(mBrush->getColor());
		gl::lineWidth(2);
		gl::drawLine(vec2(55, 95), vec2(95, 55));

		if (mBrush->getFilledShapes()) gl::drawSolidCircle(vec2(75, 125), 15);
		else gl::drawStrokedCircle(vec2(75, 125), 15);

		if (mBrush->getFilledShapes()) gl::drawSolidRect(Rectf(60, 188, 90, 160));
		else gl::drawStrokedRect(Rectf(60, 188, 90, 160));

		if (mBrush->getFilledShapes()) gl::drawSolidTriangle(vec2(55, 240), vec2(95, 240), vec2(73, 205));
		else
		{
			gl::drawLine(vec2(55, 240), vec2(95, 240));
			gl::drawLine(vec2(95, 240), vec2(73, 205));
			gl::drawLine(vec2(73, 205), vec2(55, 240));
		}

		shapeButtonsFbo->unbindFramebuffer();
	}

	void UserInterface::drawDeviceButtonsFbo()
	{
		deviceButtonsFbo->bindFramebuffer();

		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		gl::lineWidth(5);
		gl::color(0.0f, 0.0f, 0.0f, 1.0f);
		gl::drawSolidRect(Rectf(windowWidth * .9, windowHeight * .56, windowWidth, windowHeight * .8));
		gl::color(1.0f, 1.0f, 1.0f, 1.0f);

		TextLayout layout1;
		layout1.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout1.setFont(Font("Arial", 50));
		layout1.setColor(Color(1, 1, 1));
		layout1.addLine(std::string(" MultiTouch"));
		Surface8u rendered = layout1.render(true, false);
		gl::Texture2dRef mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .9, windowHeight * .77, windowWidth, windowHeight * .8));

		TextLayout layout2;
		layout2.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout2.setFont(Font("Arial", 50));
		layout2.setColor(Color(1, 1, 1));
		layout2.addLine(std::string("       EyeX"));
		rendered = layout2.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .9, windowHeight * .74, windowWidth, windowHeight * .77));

		TextLayout layout3;
		layout3.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout3.setFont(Font("Arial", 50));
		layout3.setColor(Color(1, 1, 1));
		layout3.addLine(std::string("Leap Gesture"));
		rendered = layout3.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .9, windowHeight * .71, windowWidth, windowHeight * .74));

		TextLayout layout4;
		layout4.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout4.setFont(Font("Arial", 50));
		layout4.setColor(Color(1, 1, 1));
		layout4.addLine(std::string("  Leap Draw"));
		rendered = layout4.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .9, windowHeight * .68, windowWidth, windowHeight * .71));

		TextLayout layout5;
		layout5.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout5.setFont(Font("Arial", 50));
		layout5.setColor(Color(1, 1, 1));
		layout5.addLine(std::string("Leap Motion"));
		rendered = layout5.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .9, windowHeight * .65, windowWidth, windowHeight * .68));

		TextLayout layout6;
		layout6.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout6.setFont(Font("Arial", 50));
		layout6.setColor(Color(1, 1, 1));
		layout6.addLine(std::string("Real Sense Expressions"));
		rendered = layout6.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .9, windowHeight * .62, windowWidth, windowHeight * .65));

		TextLayout layout7;
		layout7.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout7.setFont(Font("Arial", 50));
		layout7.setColor(Color(1, 1, 1));
		layout7.addLine(std::string("Real Sense Draw"));
		rendered = layout7.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .9, windowHeight * .59, windowWidth, windowHeight * .62));

		TextLayout layout8;
		layout8.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout8.setFont(Font("Arial", 50));
		layout8.setColor(Color(1, 1, 1));
		layout8.addLine(std::string(" Real Sense "));
		rendered = layout8.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .9, windowHeight * .56, windowWidth, windowHeight * .59));

		gl::color(0.0f, 0.0f, 0.0f);

		if (deviceHandler->realSenseStatus()) gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth * .87, windowHeight * .56, windowWidth * .89, windowHeight * .59));

		if (deviceHandler->realSenseDraw()) gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth * .87, windowHeight * .59, windowWidth * .89, windowHeight * .62));

		if (deviceHandler->realSenseExpressions()) gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth * .87, windowHeight * .62, windowWidth * .89, windowHeight * .65));

		if (deviceHandler->leapStatus()) gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth * .87, windowHeight * .65, windowWidth * .89, windowHeight * .68));

		if (deviceHandler->leapDraw()) gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth * .87, windowHeight * .68, windowWidth * .89, windowHeight * .71));

		if (deviceHandler->leapGesture()) gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth * .87, windowHeight * .71, windowWidth * .89, windowHeight * .74));

		if (deviceHandler->eyeXStatus()) gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth * .87, windowHeight * .74, windowWidth * .89, windowHeight * .77));

		if (deviceHandler->multiTouchStatus()) gl::color(0.0, 1.0, 0.0);
		else gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth * .87, windowHeight * .77, windowWidth * .89, windowHeight * .8));

		gl::color(0.75, 0.75, .75, 1.0);

		gl::drawStrokedRect(Rectf(windowWidth * .87, windowHeight * .56, windowWidth * .89, windowHeight * .59));
		gl::drawStrokedRect(Rectf(windowWidth * .87, windowHeight * .59, windowWidth * .89, windowHeight * .62));
		gl::drawStrokedRect(Rectf(windowWidth * .87, windowHeight * .62, windowWidth * .89, windowHeight * .65));
		gl::drawStrokedRect(Rectf(windowWidth * .87, windowHeight * .65, windowWidth * .89, windowHeight * .68));
		gl::drawStrokedRect(Rectf(windowWidth * .87, windowHeight * .68, windowWidth * .89, windowHeight * .71));
		gl::drawStrokedRect(Rectf(windowWidth * .87, windowHeight * .71, windowWidth * .89, windowHeight * .74));
		gl::drawStrokedRect(Rectf(windowWidth * .87, windowHeight * .74, windowWidth * .89, windowHeight * .77));
		gl::drawStrokedRect(Rectf(windowWidth * .87, windowHeight * .77, windowWidth * .89, windowHeight * .8));

		gl::drawStrokedRect(Rectf(windowWidth * .87, windowHeight * .56, windowWidth * .89, windowHeight * .8), uiOutlineSize);

		deviceButtonsFbo->unbindFramebuffer();
	}

	void UserInterface::drawBrushButtonsFbo()
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		brushButtonsFbo->bindFramebuffer();

		/*LineSize Button*/
		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(100, 50, 200, 100));
		TextLayout layout1;
		layout1.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
		layout1.setFont(Font("Arial", 200));
		layout1.setColor(Color(1, 1, 1));
		layout1.addLine(std::string("Size"));
		Surface8u rendered = layout1.render(true, false);
		gl::Texture2dRef mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(Rectf(100, 50, 200, 100)));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(100, 50, 200, 100), uiOutlineSize);

		//Plus Button 
		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(200, 50, 250, 100));
		gl::TextureRef texture = gl::Texture::create(loadImage(loadAsset("BluePlus.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(200, 50, 250, 100));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(200, 50, 250, 100), uiOutlineSize);

		//Minus Button
		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(250, 50, 300, 100));

		texture = gl::Texture::create(loadImage(loadAsset("BlueMinus.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(250, 50, 300, 100));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(250, 50, 300, 100), uiOutlineSize);

		/*Transparency Button*/

		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(100, 100, 200, 150));
		TextLayout layout2;
		layout2.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
		layout2.setFont(Font("Arial", 200));
		layout2.setColor(Color(1, 1, 1));
		layout2.addLine(std::string("Alpha"));
		Surface8u rendered2 = layout2.render(true, false);
		gl::Texture2dRef mTexture2 = gl::Texture2d::create(rendered2);
		gl::color(Color::white());
		gl::draw(mTexture2, Rectf(Rectf(100, 100, 200, 150)));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(100, 100, 200, 150), uiOutlineSize);

		//Plus Button 

		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(200, 100, 250, 150));

		texture = gl::Texture::create(loadImage(loadAsset("BluePlus.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(200, 100, 250, 150));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(200, 100, 250, 150), uiOutlineSize);

		//Minus Button

		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(250, 100, 300, 150));

		texture = gl::Texture::create(loadImage(loadAsset("BlueMinus.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(250, 100, 300, 150));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(250, 100, 300, 150), uiOutlineSize);

		/*Filled Shapes Button*/

		/*Fill Shapes*/
		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(100, 150, 200, 200));

		TextLayout layout3;
		layout3.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
		layout3.setFont(Font("Arial", 200));
		layout3.setColor(Color(1, 1, 1));
		layout3.addLine(std::string("Filled"));
		Surface8u rendered3 = layout3.render(true, false);
		gl::Texture2dRef mTexture3 = gl::Texture2d::create(rendered3);
		gl::color(Color::white());
		gl::draw(mTexture3, Rectf(Rectf(100, 150, 200, 200)));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(100, 150, 200, 200), uiOutlineSize);

		//Enable and Disabled Button
		if (mBrush->getFilledShapes())
		{
			gl::color(0.0, 1.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(200, 150, 250, 200));
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(200, 150, 250, 200), uiOutlineSize);
		}
		else
		{
			gl::color(0.0, 0.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(200, 150, 250, 200));
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(200, 150, 250, 200), uiOutlineSize);
		}

		brushButtonsFbo->unbindFramebuffer();
	}

	bool UserInterface::getFps()
	{
		return FPS;
	}

	void UserInterface::endButtonPress(TouchEvent::Touch touch)
	{
		if (keyboard.getMovingKeyboard())
		{
			if (keyboard.checkMovingId(touch.getId())) keyboard.finishMoving();
		}
	}

	void UserInterface::uiSetup()
	{
		//Setup UI function. Sets up the UI, mainly draw calls to write the buttons to frame buffer objects.
		//Errors occured when this function was called in TouchPointsApp 'setup', so it is now called in the update call.
		gl::Fbo::Format format;

		//Draws Settings menu FBO
		gl::color(1.0, 1.0, 1.0, 1.0);

		settingsButtonsFbo = gl::Fbo::create(windowWidth, windowHeight, format);

		settingsButtonsFbo->bindFramebuffer();
		gl::lineWidth(3);

		//Frames Per Second Button
		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(windowWidth * .65, windowHeight * .95, windowWidth * .8, windowHeight));
		TextLayout layout1;
		layout1.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout1.setFont(Font("Arial", 200));
		layout1.setColor(Color(1, 1, 1));
		layout1.addLine(std::string("Frames Per Second"));
		Surface8u rendered = layout1.render(true, false);
		gl::Texture2dRef mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .65, windowHeight * .95, windowWidth * .8, windowHeight));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(windowWidth * .65, windowHeight * .95, windowWidth * .8, windowHeight), uiOutlineSize);

		//Cycle Background button
		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(windowWidth * .65, windowHeight * .9, windowWidth * .8, windowHeight * .95));
		TextLayout layout2;
		layout2.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
		layout2.setFont(Font("Arial", 200));
		layout2.setColor(Color(1, 1, 1));
		layout2.addLine(std::string("Cycle Background"));
		rendered = layout2.render(true, false);
		mTexture = gl::Texture2d::create(rendered);
		gl::color(Color::white());
		gl::draw(mTexture, Rectf(windowWidth * .65, windowHeight * .9, windowWidth * .8, windowHeight * .95));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(windowWidth * .65, windowHeight * .9, windowWidth * .8, windowHeight * .95), uiOutlineSize);

		settingsButtonsFbo->unbindFramebuffer();

		gl::color(1.0, 1.0, 1.0, 1.0);

		//Draws Device Buttons FBO
		deviceButtonsFbo = gl::Fbo::create(windowWidth, windowHeight, format);

		drawDeviceButtonsFbo();

		//Draws Mode Buttons FBO.
		modeButtonsFbo = gl::Fbo::create(windowWidth, windowHeight, format);

		modeButtonsFbo->bindFramebuffer();

		gl::color(0.3, 0.2, 0.5, 1.0);
		gl::drawSolidRect(Rectf(0, 0, 350, 50));

		//Color Button
		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(0, 2, 50, 50), uiOutlineSize);
		gl::color(0.3, 0.2, 0.5, 1.0);
		gl::drawSolidRect(Rectf(0, 2, 50, 50));

		gl::TextureRef texture = gl::Texture::create(loadImage(loadAsset("Colors.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(5, 5, 45, 45));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(0, 2, 100, 50), uiOutlineSize);

		//Shapes button
		gl::color(0.3, 0.2, 0.5, 1.0);
		gl::drawSolidRect(Rectf(50, 2, 100, 50));
		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(50, 2, 100, 50), uiOutlineSize);

		texture = gl::Texture::create(loadImage(loadAsset("Shapes.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(58, 8, 92, 42));

		//Brush button
		gl::color(0.3, 0.2, 0.5, 1.0);
		gl::drawSolidRect(Rectf(100, 2, 150, 50));

		texture = gl::Texture::create(loadImage(loadAsset("Brush.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(100, 0, 150, 50));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(100, 2, 150, 50), uiOutlineSize);

		//Symmetry Button

		gl::color(0.3, 0.2, 0.5, 1.0);
		gl::drawSolidRect(Rectf(150, 2, 200, 50));

		gl::color(0.75, 0.75, .75, 1.0);

		gl::drawLine(vec2(175, 2), vec2(175, 15));
		gl::drawLine(vec2(175, 20), vec2(175, 25));
		gl::drawLine(vec2(175, 30), vec2(175, 35));
		gl::drawLine(vec2(175, 40), vec2(175, 45));

		gl::drawStrokedRect(Rectf(150, 2, 200, 50), uiOutlineSize);

		//Layer Visualization Button
		texture = gl::Texture::create(loadImage(loadAsset("Layers.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(205, 5, 245, 45));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(200, 0, 250, 50), uiOutlineSize);

		//Text Button
		texture = gl::Texture::create(loadImage(loadAsset("Letter.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(255, 5, 295, 45));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(250, 2, 300, 50), uiOutlineSize);

		//Undo Button
		texture = gl::Texture::create(loadImage(loadAsset("Undo.png")));

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(texture, Rectf(305, 5, 345, 45));

		gl::color(0.75, 0.75, .75, 1.0);
		gl::drawStrokedRect(Rectf(300, 2, 350, 50), uiOutlineSize);

		modeButtonsFbo->unbindFramebuffer();

		//Sets Up Brush Buttons

		brushButtonsFbo = gl::Fbo::create(windowWidth, windowHeight, format);

		drawBrushButtonsFbo();

		//Sets up the draw calls for color buttons and writes them to an FBO.
		colorButtonsFbo = gl::Fbo::create(windowWidth, windowHeight, format);

		colorButtonsFbo->bindFramebuffer();
		int i = 0;
		for (auto myColor : mBrush->getColorList())
		{
			gl::color(myColor);
			gl::drawSolidRect(Rectf(0, 50 * (i) + 50, 50, 50 * (i) + 100));
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(0, 50 * (i) + 50, 50, 50 * i + 100), uiOutlineSize);
			i++;
		}
		colorButtonsFbo->unbindFramebuffer();

		//Draws Shapes Buttons FBO.
		gl::color(1.0, 1.0, 1.0, 1.0);

		shapeButtonsFbo = gl::Fbo::create(windowWidth, windowHeight, format);

		drawShapesButtonsFbo();

		gl::color(1.0, 1.0, 1.0, 1.0);
		//Loads the asset for transparent Background and writes it to the FBO.
		transparentBackgroundFbo = gl::Fbo::create(windowWidth, windowHeight, format);
		transparentBackgroundFbo->bindFramebuffer();
		cinder::gl::TextureRef tempText = gl::Texture::create(loadImage(loadAsset("TransparentBackground.png")));
		gl::draw(tempText, Rectf(0, 0, windowWidth, windowHeight));
		transparentBackgroundFbo->unbindFramebuffer();
		gl::color(1.0, 1.0, 1.0, 0.5);
		gl::draw(transparentBackgroundFbo->getColorTexture());
		keyboard.createKeyboard();
	}

	std::shared_ptr<gl::Fbo> UserInterface::getTransparentBackground()
	{
		return transparentBackgroundFbo;
	}

	bool UserInterface::isBackgroundTransparent()
	{
		return transparentBackground;
	}

	void UserInterface::incrementBackground()
	{
		//Checks the current color
		Color tempColor = backgroundList.front();

		//If the background istransparent we disable it and change color
		if (transparentBackground)
		{
			transparentBackground = false;
			backgroundList.pop_front();
			//Assign the new background color to the new front of list.
			backgroundColor = backgroundList.front();
			//Place the old background color back into the list
			backgroundList.emplace_back(tempColor);
			return;
		}

		if (tempColor == Color(256.0, 256.0, 256.0))
		{
			transparentBackground = true;
			return;
		}
		backgroundList.pop_front();
		//Assign the new background color to the new front of list.
		backgroundColor = backgroundList.front();
		//Place the old background color back into the list
		backgroundList.emplace_back(tempColor);
	}

	float UserInterface::getLayerAlpha(int layerNumber)
	{
		return (*layerAlpha)[layerNumber];
	}

	bool UserInterface::getUiFlag()
	{
		return uiFlag;
	}

	void UserInterface::toggleUiFlag()
	{
		if (uiFlag)
		{
			modeButtons = false;
			uiFboFlag = false;
			uiFlag = false;
		}
		else
		{
			uiFlag = true;
			uiFboFlag = true;
			modeButtons = true;
		}
	}

	void UserInterface::changeModeButtons(bool x)
	{
		modeButtons = x;
	}

	bool UserInterface::getUiFboFlag()
	{
		return uiFboFlag;
	}

	void UserInterface::clearLayers()
	{
		for (auto layer : *layerList)
		{
			layer->bindFramebuffer();
			//glClearColor(1.0,1.0,1.0, 0.0);
			glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 0.0);

			glClear(GL_COLOR_BUFFER_BIT);
			//gl::clear(Color(1.0,1.0,1.0)/*ColorA(backgroundColor)*/, 0.0);
			layer->unbindFramebuffer();
		}
		illustrator->clearTimeMachine();
	}

	void UserInterface::setModeChangeFlag()
	{
		modeChangeFlag = true;
	}

	Color UserInterface::getBackgroundColor()
	{
		return backgroundColor;
	}

	void UserInterface::changeBackgroundColor(Color background)
	{
		backgroundColor = background;
	}

	void UserInterface::slideButtons(TouchEvent::Touch touch)
	{
		int x = touch.getX();
		int y = touch.getY();
		if (layerVisualization)
		{
			int yDist = (*layerList).size() * 200 + 50;
			int size = 0;
			for (auto frame : (*layerList))
			{
				if (x > 200 && x < 250)
				{
					if (y > yDist - 200 && y < yDist)
					{
						(*layerAlpha)[size] = ((y - ((float)yDist - 200)) / ((float)yDist - ((float)yDist - 200)));
					}
				}
				yDist = yDist - 200;
				size = size + 1;
			}
		}
		if (keyboard.keyboardStatus())
		{
			if (keyboard.getMovingKeyboard())
			{
				if (keyboard.checkMovingId(touch.getId()))
				{
					keyboard.setAnchor(vec2(x, y));
				}
			}
		}
	}

	bool UserInterface::inInteractiveUi(float x, float y, uint32_t id)
	{
		//ONLY ON IF MULTITOUCH IS DISABLED!
		if (deviceHandler->multiTouchStatus() == false)
		{
			//If the multi-touch is 'disabled' we still allow the user to toggle devices on and off using the multitouch
			if (deviceButtons)
			{
				if (x < windowWidth && x > windowWidth * .87)
				{
					if (y > windowHeight * .56 && y < windowHeight * .59)
					{
						deviceHandler->toggleRealSense();
						drawDeviceButtonsFbo();
						modeChangeFlag = true;
						return true;
					}

					if (y > windowHeight * .59 && y < windowHeight * .62)
					{
						deviceHandler->toggleRealSenseDraw();
						drawDeviceButtonsFbo();
						modeChangeFlag = true;
						return true;
					}
					if (y > windowHeight * .62 && y < windowHeight * .65)
					{
						deviceHandler->toggleRealSenseExpressions();
						drawDeviceButtonsFbo();
						modeChangeFlag = true;
						return true;
					}
					if (y > windowHeight * .65 && y < windowHeight * .68)
					{
						deviceHandler->toggleLeap();
						drawDeviceButtonsFbo();
						modeChangeFlag = true;
						return true;
					}
					if (y > windowHeight * .68 && y < windowHeight * .71)
					{
						deviceHandler->toggleLeapDraw();
						drawDeviceButtonsFbo();
						return true;
					}
					if (y > windowHeight * .71 && y < windowHeight * .74)
					{
						deviceHandler->toggleLeapGesture();
						drawDeviceButtonsFbo();
						return true;
					}
					if (y > windowHeight * .74 && y < windowHeight * .77)
					{
						deviceHandler->toggleEyeX();
						drawDeviceButtonsFbo();
						if (deviceHandler->eyeXStatus() == false)
						{
							modeButtons = true;
						}
						modeChangeFlag = true;
						return true;
					}
					if (y > windowHeight * .77 && y < windowHeight * .8)
					{
						deviceHandler->toggleMultiTouch();
						drawDeviceButtonsFbo();
						modeChangeFlag = true;
						return true;
					}
				}
			}
			return true;
		}

		//modeButtons UI

		//Actual 'Interactive Ui'
		if (illustrator->getNumberOfActiveDrawings() == 0)
		{
			if (keyboard.getSettingText())
			{
				illustrator->saveCurrentFbo();
				keyboard.setTextAnchor(vec2(x, y));
				keyboard.turnOnKeyboard();
				return true;
			}

			//Checks keyboard input if its on
			if (keyboard.keyboardStatus())
			{
				//Checks if the user presses a keyboard key
				if (keyboard.onKeyboardButtons(vec2(x, y)))
				{
					//Checks if the keyboard is 'done' and needs to be cleaned up.
					if (keyboard.needsCleanup())
					{
						//Cleans up the keyboard by disabling it and deleting the string
						keyboard.endText();
						//Writes the keyboard text to the current Fbo.
						layerList->back()->bindFramebuffer();
						gl::color(1.0, 1.0, 1.0, 1.0);
						gl::draw(keyboard.getTextFbo()->getColorTexture());
						layerList->back()->unbindFramebuffer();
					}
					return true;
				}
				//If the keyboard is on we want to check if the user is trying to drag it
				if (keyboard.onKeyboardSurface(vec2(x, y)))
				{
					bool tempBool = true;
					keyboard.setMovingKeyboard(tempBool);
					keyboard.setMovingId(id, vec2(x, y));
					return true;
				}

				//If they are clicking outside the keyboard they want to 'shut it down'
				//Finish cleaning up the keyboard.
				keyboard.endText();
				layerList->back()->bindFramebuffer();
				gl::color(1.0, 1.0, 1.0, 1.0);
				gl::draw(keyboard.getTextFbo()->getColorTexture());
				layerList->back()->unbindFramebuffer();

				return true;
			}

			if (settingsButtons)
			{//Settings Buttons

				if (windowWidth * .65 < x && x < windowWidth * .8)
				{
					//FPS Buttons
					if (windowHeight * .9 < y && y < windowHeight * .95)
					{
						incrementBackground();
						return true;
					}

					//Cycle Background button
					if (windowHeight * .95 < y && y < windowHeight)
					{
						FPS = !FPS;
						return true;
					}
				}
			}
			if (uiFboFlag)
			{//Mode box FBO Flag.
				//Device Modes button
				if (x > windowWidth * .92 && x < windowWidth && y > windowHeight * .8 && y < windowHeight * .84)
				{
					deviceButtons = !deviceButtons;
					return true;
				}

				//Settings Button
				if (windowWidth * .8 < x && x < windowWidth * .83 && windowHeight * .95 < y && y < windowHeight)
				{
					settingsButtons = !settingsButtons;
					return true;
				}
			}

			if (modeButtons)
			{
				if (uiFboFlag)
				{
					//Color change button.
					if (x < 50 && y < 50)
					{
						colorButtons = !colorButtons;
						return true;
					}
					else if (x < 100 && y < 50)
					{
						shapeButtons = !shapeButtons;
						return true;
					}
					else if (x < 150 && y < 50)
					{
						//mBrush->changeFilledShapes(!mBrush->getFilledShapes());
						//modeChangeFlag = true;
						brushButtons = !brushButtons;
						return true;
					}
					else if (x < 200 && y < 50)
					{
						mBrush->getSymmetry()->toggleSymmetry();
						return true;
					}
					else if (x < 250 && y < 50)
					{
						layerVisualization = !layerVisualization;
						return true;
					}
					else if (x < 300 && y < 50)
					{
						keyboard.beginSettingTextAnchor();
						return true;
					}
					else if (x < 350 && y < 50)
					{
						illustrator->undoDraw(backgroundColor);
						return true;
					}
				}
			}
		}
		//Color buttons UI
		if (colorButtons)
		{
			for (int i = 0; i < 8; i++)
			{ // i < total colors
				if (x < 50 && y < (50 * i + 100))
				{
					//currColor = i;
					mBrush->changeStaticColor(static_cast<ourColors::ourColors>(i));
					colorButtons = false;
					drawShapesButtonsFbo();
					modeChangeFlag = true;
					return true;
				}
			}
		}

		if (layerVisualization)
		{
			int yDist = (*layerList).size() * 200 + 50;
			int size = 0;
			for (auto frame : (*layerList))
			{
				if (x > 200 && x < 600)
				{
					if (x < 250)
					{
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

		if (brushButtons)
		{
			//Line Size Plus button
			if (x > 200 && x < 250 && y > 51 && y < 100)
			{
				mBrush->increaseLineSize();
				modeChangeFlag = true;
				return true;
			}
			//Line Size minus button
			if (x > 250 && x < 300 && y > 50 && y < 100)
			{
				mBrush->decreaseLineSize();
				modeChangeFlag = true;
				return true;
			}
			//Transparency Plus button
			if (x > 200 && x < 250 && y > 101 && y < 150)
			{
				mBrush->increaseAlpha();
				modeChangeFlag = true;
				return true;
			}
			//Transparency minus button
			if (x > 250 && x < 300 && y > 101 && y < 150)
			{
				mBrush->decreaseAlpha();
				modeChangeFlag = true;
				return true;
			}
			//Filled Shapes
			if (x > 200 && x < 250 && y > 151 && y < 200)
			{
				mBrush->changeFilledShapes(!mBrush->getFilledShapes());
				drawBrushButtonsFbo();
				drawShapesButtonsFbo();
				modeChangeFlag = true;
				return true;
			}
		}

		if (shapeButtons)
		{
			if (x > 50 && x < 100 && y < 100)
			{
				mBrush->changeShape(Shape::Shape::Line);
				mBrush->deactivateEraser();
				shapeButtons = false;
				modeChangeFlag = true;
				return true;
			}
			if (x > 50 && x < 100 && y < 150)
			{
				mBrush->changeShape(Shape::Shape::Circle);
				mBrush->deactivateEraser();
				shapeButtons = false;
				modeChangeFlag = true;
				return true;
			}
			if (x > 50 && x < 100 && y < 200)
			{
				mBrush->changeShape(Shape::Shape::Rectangle);
				mBrush->deactivateEraser();
				shapeButtons = false;
				modeChangeFlag = true;
				return true;
			}
			if (x > 50 && x < 100 && y < 250)
			{
				mBrush->deactivateEraser();
				mBrush->changeShape(Shape::Shape::Triangle);
				shapeButtons = false;
				modeChangeFlag = true;
				return true;
			}
			if (x > 50 && x < 100 && y < 300)
			{
				if (mBrush->IsEraserActive())
				{
					mBrush->deactivateEraser();
				}
				else
				{
					mBrush->activateEraser();
				}

				shapeButtons = false;
				modeChangeFlag = true;
				return true;
			}
		}
		if (deviceButtons)
		{
			if (x < windowWidth && x > windowWidth * .87)
			{
				if (y > windowHeight * .56 && y < windowHeight * .59)
				{
					deviceHandler->toggleRealSense();
					drawDeviceButtonsFbo();
					modeChangeFlag = true;
					return true;
				}

				if (y > windowHeight * .59 && y < windowHeight * .62)
				{
					deviceHandler->toggleRealSenseDraw();
					drawDeviceButtonsFbo();
					modeChangeFlag = true;
					return true;
				}
				if (y > windowHeight * .62 && y < windowHeight * .65)
				{
					deviceHandler->toggleRealSenseExpressions();
					drawDeviceButtonsFbo();
					modeChangeFlag = true;
					return true;
				}
				if (y > windowHeight * .65 && y < windowHeight * .68)
				{
					deviceHandler->toggleLeap();
					drawDeviceButtonsFbo();
					modeChangeFlag = true;
					return true;
				}
				if (y > windowHeight * .68 && y < windowHeight * .71)
				{
					deviceHandler->toggleLeapDraw();
					drawDeviceButtonsFbo();
					return true;
				}
				if (y > windowHeight * .71 && y < windowHeight * .74)
				{
					deviceHandler->toggleLeapGesture();
					drawDeviceButtonsFbo();
					return true;
				}
				if (y > windowHeight * .74 && y < windowHeight * .77)
				{
					deviceHandler->toggleEyeX();
					drawDeviceButtonsFbo();
					if (deviceHandler->eyeXStatus() == false)
					{
						modeButtons = true;
					}
					modeChangeFlag = true;
					return true;
				}
				if (y > windowHeight * .77 && y < windowHeight * .8)
				{
					deviceHandler->toggleMultiTouch();
					drawDeviceButtonsFbo();
					modeChangeFlag = true;
					return true;
				}
			}
		}

		return false;
	}

	void UserInterface::modeRectangle()
	{
		if (mBrush->getRandColor())
		{
			ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, mBrush->getAlphaColor());

			gl::color(newColor);
			if (!mBrush->getFilledShapes()) gl::drawStrokedRect(Rectf(windowWidth * .85, windowHeight * .85, windowWidth * .95, windowHeight * .95), mBrush->getLineSize());
			else gl::drawSolidRect(Rectf(windowWidth * .85, windowHeight * .85, windowWidth * .95, windowHeight * .95));
		}

		else
		{
			ColorA newColor(mBrush->getColor(), mBrush->getAlphaColor());
			gl::color(newColor);
			if (!mBrush->getFilledShapes()) gl::drawStrokedRect(Rectf(windowWidth * .85, windowHeight * .85, windowWidth * .95, windowHeight * .95), mBrush->getLineSize());
			else gl::drawSolidRect(Rectf(windowWidth * .85, windowHeight * .85, windowWidth * .95, windowHeight * .95));
		}
	}

	void UserInterface::modeCircle()
	{
		if (mBrush->getRandColor())
		{
			ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, mBrush->getAlphaColor());
			gl::color(newColor);
			if (!mBrush->getFilledShapes()) gl::drawStrokedCircle(vec2(windowWidth * .9, windowHeight * .9), windowHeight * .05, mBrush->getLineSize() * 2.0f);
			else gl::drawSolidCircle(vec2(windowWidth * .9, windowHeight * .9), windowHeight * .05);
		}
		else
		{
			ColorA newColor(mBrush->getColor(), mBrush->getAlphaColor());
			gl::color(newColor);
			if (!mBrush->getFilledShapes()) gl::drawStrokedCircle(vec2(windowWidth * .9, windowHeight * .9), windowHeight * .05, mBrush->getLineSize() * 2.0f);
			else gl::drawSolidCircle(vec2(windowWidth * .9, windowHeight * .9), windowHeight * .05);
		}
	}

	void UserInterface::modeTriangle()
	{
		if (mBrush->getRandColor())
		{
			ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, mBrush->getAlphaColor());
			gl::color(newColor);
			if (!mBrush->getFilledShapes())
			{
				gl::lineWidth(mBrush->getLineSize());
				gl::drawLine(vec2(windowWidth * .85, windowHeight * .95), vec2(windowWidth * .95, windowHeight * .95));
				gl::drawLine(vec2(windowWidth * .95, windowHeight * .95), vec2(windowWidth * .9, windowHeight * .85));
				gl::drawLine(vec2(windowWidth * .9, windowHeight * .85), vec2(windowWidth * .85, windowHeight * .95));
			}
			else gl::drawSolidTriangle(vec2(windowWidth * .85, windowHeight * .95), vec2(windowWidth * .95, windowHeight * .95), vec2(windowWidth * .9, windowHeight * .85));
		}
		else
		{
			ColorA newColor(mBrush->getColor(), mBrush->getAlphaColor());
			gl::color(newColor);
			if (!mBrush->getFilledShapes())
			{
				gl::lineWidth(mBrush->getLineSize());
				gl::drawLine(vec2(windowWidth * .85, windowHeight * .95), vec2(windowWidth * .95, windowHeight * .95));
				gl::drawLine(vec2(windowWidth * .95, windowHeight * .95), vec2(windowWidth * .9, windowHeight * .85));
				gl::drawLine(vec2(windowWidth * .9, windowHeight * .85), vec2(windowWidth * .85, windowHeight * .95));
			}
			else gl::drawSolidTriangle(vec2(windowWidth * .85, windowHeight * .95), vec2(windowWidth * .95, windowHeight * .95), vec2(windowWidth * .9, windowHeight * .85));
		}
	}

	void UserInterface::modeLine()
	{
		if (mBrush->getRandColor())
		{
			//Performance Issues with drawing these 3 lines every frame!
			ColorA newColor1(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, mBrush->getAlphaColor());
			ColorA newColor2(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, mBrush->getAlphaColor());
			ColorA newColor3(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, mBrush->getAlphaColor());
			//gl::color(newColor1);
			drawing::TouchPoint newTouchPoints1(vec2(windowWidth * .85, windowHeight * .95), newColor1, mBrush->getLineSize());
			illustrator->missedPoints(windowWidth * .85, windowHeight * .95, windowWidth * .95, windowHeight * .85, newTouchPoints1);
			drawing::TouchPoint newTouchPoints2(vec2(windowWidth * .86, windowHeight * .90), newColor2, mBrush->getLineSize());
			illustrator->missedPoints(windowWidth * .86, windowHeight * .90, windowWidth * .92, windowHeight * .84, newTouchPoints2);
			drawing::TouchPoint newTouchPoints3(vec2(windowWidth * .88, windowHeight * .97), newColor3, mBrush->getLineSize());
			illustrator->missedPoints(windowWidth * .88, windowHeight * .97, windowWidth * .94, windowHeight * .90, newTouchPoints3);
			newTouchPoints1.draw();
			newTouchPoints2.draw();
			newTouchPoints3.draw();
		}
		else
		{
			ColorA newColor(mBrush->getColor(), mBrush->getAlphaColor());
			gl::color(newColor);
			drawing::TouchPoint newTouchPoints1(vec2(windowWidth * .85, windowHeight * .95), newColor, mBrush->getLineSize());
			illustrator->missedPoints(windowWidth * .85, windowHeight * .95, windowWidth * .95, windowHeight * .85, newTouchPoints1);
			drawing::TouchPoint newTouchPoints2(vec2(windowWidth * .86, windowHeight * .90), newColor, mBrush->getLineSize());
			illustrator->missedPoints(windowWidth * .86, windowHeight * .90, windowWidth * .92, windowHeight * .84, newTouchPoints2);
			drawing::TouchPoint newTouchPoints3(vec2(windowWidth * .88, windowHeight * .97), newColor, mBrush->getLineSize());
			illustrator->missedPoints(windowWidth * .88, windowHeight * .97, windowWidth * .94, windowHeight * .90, newTouchPoints3);
			newTouchPoints1.draw();
			newTouchPoints2.draw();
			newTouchPoints3.draw();
		}
	}

	void UserInterface::drawUi()
	{
		gl::color(1.0, 1.0, 1.0, 1.0);

		modeChangeFlag = true;

		//Draws to the UI FBO. Currently only houses 'modebox' in the fbo.
		if (modeChangeFlag)
		{
			modeChangeFlag = false;
			uiFbo->bindFramebuffer();

			gl::lineWidth(5);
			//Clears the framebuffer to redraw
			gl::clear(ColorA(0.0, 0.0, 0.0, 0.0));

			gl::color(.82, .82, .82, 1.0);
			gl::drawSolidRect(Rectf(windowWidth * .8, windowHeight * .8, windowWidth, windowHeight));
			gl::color(0.0, 0.0, 0.0);

			//Draw Device Mode button
			gl::color(0.1, 0.1, 0.1, 1.0);
			gl::drawSolidRect(Rectf(windowWidth * .92, windowHeight * .8, windowWidth, windowHeight * .83));

			TextLayout layout1;
			layout1.clear(ColorA(0.2f, 0.2f, 0.2f, 0.2f));
			layout1.setFont(Font("Arial", 50));
			layout1.setColor(Color(1, 1, 1));
			layout1.addLine(std::string("Device Modes"));
			Surface8u rendered = layout1.render(true, false);
			gl::Texture2dRef mTexture = gl::Texture2d::create(rendered);
			gl::color(Color::white());
			gl::draw(mTexture, Rectf(windowWidth * .92, windowHeight * .8, windowWidth, windowHeight * .83));
			gl::color(0.75, 0.75, .75, 1.0);
			gl::drawStrokedRect(Rectf(windowWidth * .92, windowHeight * .8, windowWidth, windowHeight * .83));

			//Draw outline of modeBox
			gl::color(0.75, 0.75, 0.75);
			gl::drawStrokedRect(Rectf(windowWidth * .8, windowHeight * .8, windowWidth, windowHeight), uiOutlineSize);

			//Draw 'Settings' box
			gl::TextureRef texture = gl::Texture::create(loadImage(loadAsset("Settings.png")));

			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::draw(texture, Rectf(windowWidth * .8, windowHeight * .95, windowWidth * .83, windowHeight));

			gl::color(0.0, 0.0, 0.0);

			//Switch to draw which shape is represented in the mode box.
			if (mBrush->IsEraserActive())
			{
				bool tempBool = false;
				drawing::TouchCircle(vec2(windowWidth * .9, windowHeight * .9), mBrush->getLineSize() * 2, Color(1.0, 1.0, 1.0), 1, tempBool).draw();
			}
			else
				switch (mBrush->getShape())
				{
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
			if (deviceHandler->multiTouchStatus())
			{
				gl::color(0.0, 0.0, 1.0);
				gl::drawSolidRect(Rectf(windowWidth * .81, windowHeight * .81, windowWidth * .83, windowHeight * .83));
			}
			if (deviceHandler->leapStatus())
			{
				gl::color(0.0, 1.0, 0.0);
				gl::drawSolidRect(Rectf(windowWidth * .84, windowHeight * .81, windowWidth * .86, windowHeight * .83));
			}
			if (deviceHandler->eyeXStatus())
			{
				gl::color(1.0, 0.0, 0.0);
				gl::drawSolidRect(Rectf(windowWidth * .87, windowHeight * .81, windowWidth * .89, windowHeight * .83));
			}
			if (deviceHandler->realSenseStatus())
			{
				gl::color(1.0, 1.0, 0.0);
				gl::drawSolidRect(Rectf(windowWidth * .90, windowHeight * .81, windowWidth * .92, windowHeight * .83));
			}
			uiFbo->unbindFramebuffer();
			//uiFboFlag = false;
		}
		if (uiFboFlag)
		{//Constantly drawn Ui buttons. If ui flag is off, we shut down the ui
			//Currently shuts down nothing because it needs to be developed in parralel to inInteractiveUi
		}

		if (settingsButtons)
		{
			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::draw(settingsButtonsFbo->getColorTexture());
		}

		if (modeButtons)
		{
			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::draw(modeButtonsFbo->getColorTexture());
		}

		if (brushButtons)
		{
			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::draw(brushButtonsFbo->getColorTexture());
		}

		if (colorButtons)
		{
			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::draw(colorButtonsFbo->getColorTexture());
		}
		if (shapeButtons)
		{
			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::draw(shapeButtonsFbo->getColorTexture());
		}

		if (mBrush->getSymmetry()->getSymmetryOn())
		{
			for (int i = 0; i < 50; i = i + 2)
			{
				gl::lineWidth(4);
				gl::color(0.75, 0.75, 0.75);
				gl::drawLine(vec2(windowWidth / 2, windowHeight - i * 50), vec2(windowWidth / 2, windowHeight - (i + 1) * 50));
			}
		}
		if (layerVisualization)
		{
			int y = layerList->size();
			y = y * 200 + 50;
			int layerNumber = 0;
			for (auto frame : *layerList)
			{
				gl::color(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0);
				gl::drawSolidRect(Rectf(200, (y - 200), 600, y));

				gl::color(0.75, 0.75, .75, 1.0);
				gl::drawStrokedRect(Rectf(200, y - 200, 600, y), uiOutlineSize);
				gl::color(1.0, 1.0, 1.0, (*layerAlpha)[layerNumber]);
				gl::draw(frame->getColorTexture(), Rectf(200, (y - 200), 600, y));
				gl::color(0.0, 0.0, 0.0, 1.0);
				gl::drawSolidRect(Rectf(200, y - 200, 250, y));
				gl::color(0.75, 0.75, .75, 1.0);
				gl::drawStrokedRect(Rectf(200, y - 200, 250, y), uiOutlineSize);
				gl::color(1.0, 1.0, 1.0, 1.0);

				//Draws indicator for alpha level
				gl::color(0.0, 1.0f, 0.0, 1.0f);
				gl::drawSolidCircle(vec2(225, (y - 200) + (*layerAlpha)[layerNumber] * (y - (y - 200))), 5.0f);

				for (int x = 0; x < 8; x++)
				{
					gl::color(1.0, 1.0, 1.0, 1.0 - .125 * x);
					gl::drawSolidRect(Rectf(200, (y - 200) + x * 25, 250, (y - 200) + (x + 1) * 25));
				}
				y = y - 200;
				layerNumber++;
			}
		}

		if (deviceButtons)
		{
			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::draw(deviceButtonsFbo->getColorTexture());
		}

		//Draws keyboard if it is on
		if (keyboard.keyboardStatus())
		{
			gl::color(1.0, 1.0, 1.0, 1.0);
			gl::draw(keyboard.getKeyboardFbo()->getColorTexture());
			gl::draw(keyboard.getTextFbo()->getColorTexture());
		}
		gl::color(1.0, 1.0, 1.0, 1.0);
		if (keyboard.getMovingKeyboard())
		{
			gl::draw((keyboard.getMovingKeyboardFbo()->getColorTexture()));
		}
	}
}}
