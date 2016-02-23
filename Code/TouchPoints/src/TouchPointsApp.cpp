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

//Leap Includes

#include "Leap.h"
#include "LeapMath.h"

//Our own includes
//#include "TouchShapes.h"
#include "TouchShapes.cpp"

//include Images
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "String.h"

//Standard Library Includes
#include <vector>
#include <map>
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace cinder;

//#define EYEX
#ifdef EYEX

//Our Tobi EyeX Functionality. Cannot run with Tobii driver soooo...ifdef?

#include <stdio.h>
#include <conio.h>
#include <assert.h>
//#include "stdafx.h"
#include "eyex/EyeX.h"
#include "wtypes.h"

#pragma comment (lib, "Tobii.EyeX.Client.lib")
//Stores values of eyePosition(where we are looking) from tobii EyeX
float gazePositionX = 0.0f;
float gazePositionY = 0.0f;

// ID of the global interactor that provides our data stream; must be unique within the application.
static const TX_STRING InteractorId = "Twilight Sparkle";

// global variables
static TX_HANDLE g_hGlobalInteractorSnapshot = TX_EMPTY_HANDLE;

//Stored Desktop resolution variables.
int resolutionX;
int resolutionY;

#endif

#define COLOR_AMOUNT 7
#define BACKGROUND_COLORS 8

#define windowWidth  getWindowSize().x
#define windowHeight getWindowSize().y
#define FRAME_RATE 120

//Leap map 
map<uint32_t, vec2> pointsMap;

bool imageFlag = false;
gl::TextureRef imageTexture;
float fadeTime = 1.0;
int imageNum;


//Layers
int currLayer = 0;

float lineSize = 1.0f;

float r, g, b = 256.0f;

int currColor = 0;
int currBackground = 0;

float backgroundArray[BACKGROUND_COLORS][3] = { { 0.0f, 0.0f, 0.0f }, { 256.0f, 256.0f, 256.0f }, { 256.0f, 0.0f, 0.0f }, { 256.0f, 256.0f, 0.0f }, { 0.0f, 256.0f, 0.0f }, { 0.0f, 256.0f, 256.0f }, { 0.0f, 0.0f, 256.0f }, { 256.0f, 0.0f, 256.0f } };
float colorArray[COLOR_AMOUNT][3] = { { 256.0f, 256.0f, 256.0f }, { 256.0f, 0.0f, 0.0f }, { 256.0f, 256.0f, 0.0f }, { 0.0f, 256.0f, 0.0f }, { 0.0f, 256.0f, 256.0f }, { 0.0f, 0.0f, 256.0f }, { 256.0f, 0.0f, 256.0f } };

//TouchPointsApp, our libcinder app!
class TouchPointsApp : public App {
public:
	void setup() override;

	void mouseDown(MouseEvent event) override;

	void	touchesBegan(TouchEvent event) override;
	void	touchesMoved(TouchEvent event) override;
	void	touchesEnded(TouchEvent event) override;
	void	keyDown(KeyEvent event) override;
	void	leapDraw(Leap::Frame frame);
	void	drawUi();
	void	modeRectangle();
	void	modeCircle();
	void	modeTriangle();
	void	modeLine();
	void	drawImageTexture();
	void	loadImages(string imageName);
	void	saveImage(string imageType);

	//List of MODES
	bool randColor = false;
	bool eraserMode = false;
	bool lineDraw = true;
	bool circleDraw = false;
	bool rectDraw = false;
	bool triangleDraw = false;
	bool filledShapes = false;

	bool uiFboFlag = false;
	bool modeChangeFlag = true;

#ifdef EYEX


#endif

	void update() override;
	void draw() override;


private:

	//Leap Motion Controller
	Leap::Controller leapContr;
	Leap::Frame currentFrame;


	//Need to redo this area. We should have a list for active points, this list will contain ALL shapes.
	//Perhaps Templates? Need to attend C++ Workshop for this. 
	//The touchPoint should have a variable 'shape' which just tells it if it is a square, circle, triangle, line, etc.
	//Keep the 'eraser' points though because they need to be drawn separately last!
	//Might need a new function for the eraser mode so it can ALWAYS have the correct background color. 
	//Future playback feature may want eraser points in the same list as the other shapes as well. 


	list<TouchShape*>		  myShapes;


	map<uint32_t, TouchPoint>		myActivePoints;
	list<TouchPoint>				myPoints;
	map<uint32_t, TouchPoint>		myActivePointsEraser;
	list<TouchPoint>				myPointsEraser;
	map<uint32_t, TouchCircle>		myActiveCircles;
	list<TouchCircle>				myCircles;
	map<uint32_t, TouchRectangle>	myActiveRectangles;
	list<TouchRectangle>			myRectangles;
	map<uint32_t, TouchTriangle>	myActiveTriangles;
	list<TouchTriangle>				myTriangles;

	//Fbo's for Layering.
	//We may want to set a vector of framebuffers?
	//This way we can dynamically have multiple layers....????
	std::shared_ptr<gl::Fbo>		firstFbo;
	std::shared_ptr<gl::Fbo>		secondFbo;
	std::shared_ptr<gl::Fbo>		uiFbo;
	std::shared_ptr<gl::Fbo>		imageFbo;


};

/*Retrieves frames from leap motion Service*/
Leap::Frame getLeapFrame(Leap::Controller controller){
	return controller.frame();
}

void prepareSettings(TouchPointsApp::Settings *settings)
{
	// By default, multi-touch is disabled on desktop and enabled on mobile platforms.
	// You enable multi-touch from the SettingsFn that fires before the app is constructed.
	settings->setMultiTouchEnabled(true);

	// On mobile, if you disable multitouch then touch events will arrive via mouseDown(), mouseDrag(), etc.
	//	settings->setMultiTouchEnabled( false );
}

#ifdef EYEX
//eyeX Functions.


/*
* Initializes g_hGlobalInteractorSnapshot with an interactor that has the Gaze Point behavior.
*/
BOOL InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext)
{
	TX_HANDLE hInteractor = TX_EMPTY_HANDLE;
	TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED };
	BOOL success;

	success = txCreateGlobalInteractorSnapshot(
		hContext,
		InteractorId,
		&g_hGlobalInteractorSnapshot,
		&hInteractor) == TX_RESULT_OK;
	success &= txCreateGazePointDataBehavior(hInteractor, &params) == TX_RESULT_OK;

	txReleaseObject(&hInteractor);

	return success;
}

/*
* Callback function invoked when a snapshot has been committed.
*/
void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param)
{
	// check the result code using an assertion.
	// this will catch validation errors and runtime errors in debug builds. in release builds it won't do anything.

	TX_RESULT result = TX_RESULT_UNKNOWN;
	txGetAsyncDataResultCode(hAsyncData, &result);
	assert(result == TX_RESULT_OK || result == TX_RESULT_CANCELLED);
}

/*
* Callback function invoked when the status of the connection to the EyeX Engine has changed.
*/

void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam)
{
	switch (connectionState) {
	case TX_CONNECTIONSTATE_CONNECTED: {
		BOOL success;
		//printf("The connection state is now CONNECTED (We are connected to the EyeX Engine)\n");
		// commit the snapshot with the global interactor as soon as the connection to the engine is established.
		// (it cannot be done earlier because committing means "send to the engine".)
		success = txCommitSnapshotAsync(g_hGlobalInteractorSnapshot, OnSnapshotCommitted, NULL) == TX_RESULT_OK;
		if (!success) {
			//printf("Failed to initialize the data stream.\n");
		}
		else {
			//printf("Waiting for gaze data to start streaming...\n");
		}
	}
									   break;

	case TX_CONNECTIONSTATE_DISCONNECTED:
		//printf("The connection state is now DISCONNECTED (We are disconnected from the EyeX Engine)\n");
		break;

	case TX_CONNECTIONSTATE_TRYINGTOCONNECT:
		//printf("The connection state is now TRYINGTOCONNECT (We are trying to connect to the EyeX Engine)\n");
		break;

	case TX_CONNECTIONSTATE_SERVERVERSIONTOOLOW:
		//printf("The connection state is now SERVER_VERSION_TOO_LOW: this application requires a more recent version of the EyeX Engine to run.\n");
		break;

	case TX_CONNECTIONSTATE_SERVERVERSIONTOOHIGH:
		//printf("The connection state is now SERVER_VERSION_TOO_HIGH: this application requires an older version of the EyeX Engine to run.\n");
		break;
	}
}

/*
* Handles an event from the Gaze Point data stream.
*/
void OnGazeDataEvent(TX_HANDLE hGazeDataBehavior)
{
	TX_GAZEPOINTDATAEVENTPARAMS eventParams;
	if (txGetGazePointDataEventParams(hGazeDataBehavior, &eventParams) == TX_RESULT_OK) {


		/*
		int gazeMovementLimit = 10;

		if (eventParams.X - gazePositionX >= gazeMovementLimit)
		{
			gazePositionX += gazeMovementLimit;
		}
		else if (eventParams.X - gazePositionX <= gazeMovementLimit){
			gazePositionX -= gazeMovementLimit;
		}
		else {
			gazePositionX = eventParams.X;
		}

		if (eventParams.Y - gazePositionY >= gazeMovementLimit)
		{
			gazePositionY += gazeMovementLimit;
		}
		else if (eventParams.Y - gazePositionY <= gazeMovementLimit){
			gazePositionY -= gazeMovementLimit;
		}
		else {
			gazePositionY = eventParams.Y;
		}
		*/
		gazePositionX = eventParams.X;
		gazePositionY = eventParams.Y;



		//printf("Gaze Data: (%.1f, %.1f) timestamp %.0f ms\n", eventParams.X, eventParams.Y, eventParams.Timestamp);

		//gl::clear();
		//Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
		//gl::color(newColor);
		//vec2 mCenter{ eventParams.X, eventParams.Y };
		//myActiveCircles.insert(make_pair(touch.getId(), TouchCircle(touch.getPos(), 30.0f, newColor, lineSize, filledShapes)));
		//gl::drawStrokedCircle(mCenter, 10.0f, 10.0f);
	}
	else {
		//printf("Failed to interpret gaze data event packet.\n");
	}
}

/*
* Callback function invoked when an event has been received from the EyeX Engine.
*/
void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
{
	TX_HANDLE hEvent = TX_EMPTY_HANDLE;
	TX_HANDLE hBehavior = TX_EMPTY_HANDLE;


	txGetAsyncDataContent(hAsyncData, &hEvent);

	// NOTE. Uncomment the following line of code to view the event object. The same function can be used with any interaction object.
	//OutputDebugStringA(txDebugObject(hEvent));

	if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK) {
		OnGazeDataEvent(hBehavior);
		txReleaseObject(&hBehavior);
	}


	// NOTE since this is a very simple application with a single interactor and a single data stream, 
	// our event handling code can be very simple too. A more complex application would typically have to 
	// check for multiple behaviors and route events based on interactor IDs.

	txReleaseObject(&hEvent);

}

#endif

void TouchPointsApp::setup()
{
	//Sets max mulitouch points
	CI_LOG_I("MT: " << System::hasMultiTouch() << " Max points: " << System::getMaxMultiTouchPoints());
	glEnable(GL_LINE_SMOOTH);
	//setWindowSize(windowWidth, windowHeight);
	setFullScreen(1);

	//Sets window size and initializes framebuffers (layers).
	setWindowSize(windowWidth, windowHeight);
	gl::Fbo::Format format;
	firstFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	secondFbo = gl::Fbo::create(windowWidth, windowHeight, format);


	//Set up UI
	uiFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	//Set up image feedback fbo 
	imageFbo = gl::Fbo::create(windowWidth, windowHeight, format);


	//drawUi();

	setFrameRate(FRAME_RATE);

	//Sets up eyeX context
#ifdef EYEX
	//Get desktop resolution
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	resolutionX = desktop.right;
	resolutionY = desktop.bottom;






	TX_CONTEXTHANDLE hContext = TX_EMPTY_HANDLE;
	TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
	TX_TICKET hEventHandlerTicket = TX_INVALID_TICKET;
	BOOL success;

	// initialize and enable the context that is our link to the EyeX Engine.
	success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
	success &= txCreateContext(&hContext, TX_FALSE) == TX_RESULT_OK;
	success &= InitializeGlobalInteractorSnapshot(hContext);
	success &= txRegisterConnectionStateChangedHandler(hContext, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
	success &= txRegisterEventHandler(hContext, &hEventHandlerTicket, HandleEvent, NULL) == TX_RESULT_OK;
	success &= txEnableConnection(hContext) == TX_RESULT_OK;
	/*
	// let the events flow until a key is pressed.
	if (success) {
		printf("Initialization was successful.\n");
	}
	else {
		printf("Initialization failed.\n");
	}
	printf("Press any key to exit...\n");
	_getch();
	printf("Exiting.\n");
	*/
	/*

	HOW DO WE DELETE THE CONTEXT?
	// disable and delete the context.
	txDisableConnection(hContext);
	txReleaseObject(&g_hGlobalInteractorSnapshot);
	success = txShutdownContext(hContext, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
	success &= txReleaseContext(&hContext) == TX_RESULT_OK;
	success &= txUninitializeEyeX() == TX_RESULT_OK;
	if (!success) {
	printf("EyeX could not be shut down cleanly. Did you remember to release all handles?\n");
	}
	*/
#endif
}
/*Bresenhams Line Algorithm for smooth lines*/
void missedPoints(int xi, int yi, int xf, int yf, TouchPoint& points){
	int dx, dy, x, y, d, ds, dt;
	//Find Abs value of difference between x and y coords of two points
	dx = abs(xi - xf);
	dy = abs(yi - yf);
	//Ceck which coordinates have bigger range x or y 
	if (dx >= dy){
		d = 2 * dy - dx;
		ds = 2 * dy;
		dt = 2 * (dy - dx);
		//Figure out which point to use as starting point
		if (xi < xf){
			x = xi;
			y = yi;
		}
		else{
			x = xf;
			y = yf;
			xf = xi;
			yf = yi;
		}

		//Would be starting point passed to method
		points.addPoint(vec2(x, y));

		while (x < xf){
			if (d < 0){
				d += ds;
			}
			else{
				if (y < yf){
					y++;
					d += dt;
				}
				else{
					y--;
					d += dt;
				}
			}

			x++;

			points.addPoint(vec2(x, y));
		}
	}
	else{
		d = 2 * dx - dy;
		ds = 2 * dx;
		dt = 2 * (dx - dy);
		//Figure out which point to use as starting point
		if (yi < yf){
			x = xi;
			y = yi;
		}
		else{
			x = xf;
			y = yf;
			xf = xi;
			yf = yi;
		}

		points.addPoint(vec2(x, y));

		while (y < yf){
			if (d < 0){
				d += ds;
			}
			else{
				if (x > xf){
					x--;
					d += dt;
				}
				else{
					x++;
					d += dt;
				}
			}

			y++;

			points.addPoint(vec2(x, y));
		}
	}
}

static Area calcCenter(gl::TextureRef imageTexture){
	Area image = imageTexture->getBounds();
	Area window = getWindowBounds();
	Area center = Area::proportionalFit(image, window, true, false);

	return center;
}

void TouchPointsApp::leapDraw(Leap::Frame frame){
	//Get all pointables from current leap frame
	Leap::PointableList pointables = frame.pointables();
	//Gets a virtual rectangular prism which is within the field of vie of Leap 
	Leap::InteractionBox iBox = frame.interactionBox();

	//Temporary TouchPoint

	//Traverse all pointables 
	for (auto& points : pointables){
		//Normalize points from iBox
		Leap::Vector normalizedPosition = iBox.normalizePoint(points.stabilizedTipPosition());
		//Get x and y coordinate value form normalized value within given window 
		float leapXCoordinate = normalizedPosition.x * windowWidth;
		float leapYCoordinate = windowHeight - normalizedPosition.y * windowHeight;

		//Create a prevous vec2
		//vec2 prevPoint;

		if (points.touchDistance() > 0 && points.touchZone() != Leap::Pointable::Zone::ZONE_NONE)
		{
			gl::color(0, 1, 0, 1 - points.touchDistance());
			gl::drawSolidCircle(vec2(leapXCoordinate, leapYCoordinate), 40);
		}
		else if (points.touchDistance() <= 0)
		{
			//Check to see if id for pointable object is present
			auto result = pointsMap.find(points.id());
			//Checks to see if new pointable is drawing
			if (result == pointsMap.end()){

				//myPrevX = leapXCoordinate;
				//myPrevY = leapYCoordinate;

				if (randColor){
					Color newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
					myActivePoints.insert(make_pair(points.id(), TouchPoint(vec2(leapXCoordinate, leapYCoordinate), newColor, lineSize)));
				}
				else {
					Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
					myActivePoints.insert(make_pair(points.id(), TouchPoint(vec2(leapXCoordinate, leapYCoordinate), newColor, lineSize)));
				}

				pointsMap.emplace(points.id(), vec2(leapXCoordinate, leapYCoordinate));
				myActivePoints[points.id()].addPoint(vec2(leapXCoordinate, leapYCoordinate));

			}
			else{
				//New poitable is not drawing need to add new point 
				//prevPoint = result->second;
				//gl::clear(Color(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2]));
				missedPoints(leapXCoordinate, leapYCoordinate, myActivePoints[points.id()].getFirstPoint().x, myActivePoints[points.id()].getFirstPoint().y, myActivePoints[points.id()]);

				//myPrevX = leapXCoordinate;
				//myPrevY = leapYCoordinate;

				myPoints.push_back(myActivePoints[points.id()]);
				myActivePoints[points.id()].clearPoints();

				myActivePoints[points.id()].addPoint(vec2(leapXCoordinate, leapYCoordinate));



				if (currLayer == 0){

					(*firstFbo).bindFramebuffer();
					//for (int i = 0; i < 5; i++){
					for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
						oldPoints->draw();
						++oldPoints;
					}
					//}
					(*firstFbo).unbindFramebuffer();
				}
				else if (currLayer == 1){
					(*secondFbo).bindFramebuffer();
					//Draws the points multiple times to prevent flickering.
					//for (int i = 0; i < 5; i++){
					for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
						oldPoints->draw();
						++oldPoints;
					}
					//}
					(*secondFbo).unbindFramebuffer();
				}

				myPoints.clear();
			}
		}
		else
		{
			/*
			for (auto& point : line){
			//gl::color(1, 1, 0, 0);
			//gl::drawSolidCircle(point, 10);
			}
			*/

		}
	}

}

void TouchPointsApp::drawImageTexture(){

	(*imageFbo).bindFramebuffer();
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	gl::color(1.0, 1.0, 1.0, fadeTime);

	gl::draw(imageTexture);

	if (fadeTime == 0){
		imageFlag = false;
		fadeTime = 1;
	}

	fadeTime -= 0.001;

	(*imageFbo).unbindFramebuffer();

}

void TouchPointsApp::loadImages(string imageName){

	imageTexture = gl::Texture::create(loadImage(loadAsset(imageName)));
}	

void TouchPointsApp::saveImage(string imageType){

	writeImage(getHomeDirectory() / "cinder" / "Saved_Images" / (toString(imageNum) + imageType), copyWindowSurface());
	imageNum++;
	loadImages("Save" + imageType);
	cout << "Image " << imageNum << "Saved!";
}
/*Mode Change Functions*/
void TouchPointsApp::modeRectangle(){

	//(*uiFbo).unbindTexture(0, 36064U);
	//(*uiFbo).bindFramebuffer();
	if (randColor){
		Color newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		gl::color(newColor);
		//gl::lineWidth(10);
		//gl::drawSolidCircle(vec2(0,0), 10);
		//gl::drawSolidCircle(vec2(1920,1080), 100);
		if (!filledShapes)
			gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		else gl::drawSolidRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95));
		//(*uiFbo).unbindFramebuffer();
	}

	else{
		Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
		gl::color(newColor);
		//gl::lineWidth(10);
		//gl::drawSolidCircle(vec2(0,0), 10);
		//gl::drawSolidCircle(vec2(1920,1080), 100);
		if (!filledShapes)
			gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		else gl::drawSolidRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95));
		//(*uiFbo).unbindFramebuffer();
	}
}

void TouchPointsApp::modeCircle(){

	//(*uiFbo).unbindTexture(0, 36064U);
	//(*uiFbo).bindFramebuffer();
	//CM_HSV, Rand::randFloat(), 0.5f, 1.0f
	if (randColor){
		Color newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!filledShapes)
			gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, lineSize*2.0f);
		else
			gl::drawSolidCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05);

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		//(*uiFbo).unbindFramebuffer();
	}
	else{

		Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!filledShapes)
			gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, lineSize*2.0f);
		else
			gl::drawSolidCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05);

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		//(*uiFbo).unbindFramebuffer();
	}
}


void TouchPointsApp::modeTriangle(){

	//(*uiFbo).unbindTexture(0, 36064U);
	//(*uiFbo).bindFramebuffer();
	if (randColor){
		Color newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!filledShapes){
			//gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, lineSize*2.0f);
			TouchPoint uiTouchPoint(vec2(windowWidth*.85, windowHeight*.95), newColor, lineSize);
			uiTouchPoint.addPoint(vec2(windowWidth*.95, windowHeight*.95));
			uiTouchPoint.addPoint(vec2(windowWidth*.9, windowHeight*.85));
			uiTouchPoint.addPoint(vec2(windowWidth*.85, windowHeight*.95));

			missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.95, uiTouchPoint);
			missedPoints(windowWidth*.95, windowHeight*.95, windowWidth*.9, windowHeight*.85, uiTouchPoint);
			missedPoints(windowWidth*.9, windowHeight*.85, windowWidth*.85, windowHeight*.95, uiTouchPoint);
			uiTouchPoint.draw();
			/*
			gl::lineWidth(lineSize);
			gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.95));
			gl::drawLine(vec2(windowWidth*.95, windowHeight*.95), vec2(windowWidth*.9, windowHeight*.85));
			gl::drawLine(vec2(windowWidth*.9, windowHeight*.85), vec2(windowWidth*.85, windowHeight*.95));
			*/
		}
		else
			gl::drawSolidTriangle(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight *.95), vec2(windowWidth*.9, windowHeight*.85));

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		//(*uiFbo).unbindFramebuffer();
	}
	else{
		Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!filledShapes){

			//gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, lineSize*2.0f);
			TouchPoint uiTouchPoint(vec2(windowWidth*.85, windowHeight*.95), newColor, lineSize);
			uiTouchPoint.addPoint(vec2(windowWidth*.95, windowHeight*.95));
			uiTouchPoint.addPoint(vec2(windowWidth*.9, windowHeight*.85));
			uiTouchPoint.addPoint(vec2(windowWidth*.85, windowHeight*.95));

			missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.95, uiTouchPoint);
			missedPoints(windowWidth*.95, windowHeight*.95, windowWidth*.9, windowHeight*.85, uiTouchPoint);
			missedPoints(windowWidth*.9, windowHeight*.85, windowWidth*.85, windowHeight*.95, uiTouchPoint);
			uiTouchPoint.draw();

			/*
			gl::lineWidth(lineSize);
			gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.95));
			gl::drawLine(vec2(windowWidth*.95, windowHeight*.95), vec2(windowWidth*.9, windowHeight*.85));
			gl::drawLine(vec2(windowWidth*.9, windowHeight*.85), vec2(windowWidth*.85, windowHeight*.95));
			*/
		}
		else
			gl::drawSolidTriangle(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight *.95), vec2(windowWidth*.9, windowHeight*.85));

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		//(*uiFbo).unbindFramebuffer();
	}
}

void TouchPointsApp::modeLine(){

	if (randColor){
		//Performance Issues with drawing these 3 lines every frame!
		Color newColor1(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		Color newColor2(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		Color newColor3(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		//gl::color(newColor1);
		TouchPoint newTouchPoints1(vec2(windowWidth*.85, windowHeight*.95), newColor1, lineSize);
		missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.85, newTouchPoints1);
		TouchPoint newTouchPoints2(vec2(windowWidth*.86, windowHeight*.90), newColor2, lineSize);
		missedPoints(windowWidth*.86, windowHeight*.90, windowWidth*.92, windowHeight*.84, newTouchPoints2);
		TouchPoint newTouchPoints3(vec2(windowWidth*.88, windowHeight*.97), newColor3, lineSize);
		missedPoints(windowWidth*.88, windowHeight*.97, windowWidth*.94, windowHeight*.90, newTouchPoints3);
		newTouchPoints1.draw();
		newTouchPoints2.draw();
		newTouchPoints3.draw();


		/*
		Color newColor1(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		Color newColor2(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		Color newColor3(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
		gl::lineWidth(lineSize);
		gl::color(newColor1);
		gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.85));
		gl::color(newColor2);
		gl::drawLine(vec2(windowWidth*.86, windowHeight*.9), vec2(windowWidth*.92, windowHeight*.84));
		gl::color(newColor3);
		gl::drawLine(vec2(windowWidth*.88, windowHeight*.97), vec2(windowWidth*.94, windowHeight*.90));
		*/
	}
	else{

		Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
		gl::color(newColor);
		TouchPoint newTouchPoints1(vec2(windowWidth*.85, windowHeight*.95), newColor, lineSize);
		missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.85, newTouchPoints1);
		TouchPoint newTouchPoints2(vec2(windowWidth*.86, windowHeight*.90), newColor, lineSize);
		missedPoints(windowWidth*.86, windowHeight*.90, windowWidth*.92, windowHeight*.84, newTouchPoints2);
		TouchPoint newTouchPoints3(vec2(windowWidth*.88, windowHeight*.97), newColor, lineSize);
		missedPoints(windowWidth*.88, windowHeight*.97, windowWidth*.94, windowHeight*.90, newTouchPoints3);
		newTouchPoints1.draw();
		newTouchPoints2.draw();
		newTouchPoints3.draw();

		/*
		Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
		gl::lineWidth(lineSize);
		gl::color(newColor);
		gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.85));
		gl::drawLine(vec2(windowWidth*.86, windowHeight*.9), vec2(windowWidth*.92, windowHeight*.84));
		gl::drawLine(vec2(windowWidth*.88, windowHeight*.97), vec2(windowWidth*.94, windowHeight*.90));
		*/
	}

}
void TouchPointsApp::keyDown(KeyEvent event)
{
	if (event.getChar() == 'z') {
		// Toggle full screen when the user presses the 'f' key.
		if (lineSize != 1.0f)
			lineSize--;
		modeChangeFlag = true;
	}
	else if (event.getChar() == 'x') {
		// Clear the list of points when the user presses the space bar.
		if (lineSize != 15.0f)
			lineSize++;
		modeChangeFlag = true;

	}
	else if (event.getCode() == KeyEvent::KEY_ESCAPE) {
		// Exit full screen, or quit the application, when the user presses the ESC key.
		//if (isFullScreen())
		//setFullScreen(false);
		//else
		quit();
	}
	else if (event.getChar() == 'r')	//Turns on random color mode
	{
		//Turns on random color mode
		if (randColor == false)
			randColor = true;
		else randColor = false;

		modeChangeFlag = true;
	}
	else if (event.getChar() == 'q')	//Cycles through colors
	{
		if (currColor != 0)
			currColor--;
		else currColor = COLOR_AMOUNT - 1;

		modeChangeFlag = true;
	}
	else if (event.getChar() == 'w')	//Cycles through colors
	{
		if (currColor != COLOR_AMOUNT - 1)
			currColor++;
		else currColor = 0;

		modeChangeFlag = true;
	}
	else if (event.getChar() == 'e')	//Eraser mode
	{
		if (eraserMode == false)
			eraserMode = true;
		else eraserMode = false;

		modeChangeFlag = true;
	}
	else if (event.getChar() == 'c')	//Clear Screen (Broken with framebuffers).
	{
		//gl::color(1.0, 1.0, 1.0);
		//gl::drawSolidCircle(vec2(500,500), 100);
		//myPoints.clear();

		(*firstFbo).bindFramebuffer();
		//gl::clear(Color(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2]));
		//gl::ScopedScissor(vec2(0, windowHeight), vec2(windowWidth, windowHeight));
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*firstFbo).unbindFramebuffer();

		(*secondFbo).bindFramebuffer();
		//gl::ScopedScissor(vec2(0, windowHeight), vec2(windowWidth, windowHeight));
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*secondFbo).unbindFramebuffer();

		(*uiFbo).bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*uiFbo).unbindFramebuffer();

		//myCircles.clear();
		//myRectangles.clear();
		//myTriangles.clear();
		//myPointsEraser.clear();
		modeChangeFlag = true;
	}
	else if (event.getChar() == 'b')
	{
		if (currBackground != BACKGROUND_COLORS - 1)
			currBackground = currBackground + 1;
		else currBackground = 0;
	}
	else if (event.getChar() == 'u')
	{
		lineDraw = true;
		circleDraw = false;
		rectDraw = false;
		triangleDraw = false;
		modeChangeFlag = true;
	}
	else if (event.getChar() == 'i')
	{
		lineDraw = false;
		circleDraw = true;
		rectDraw = false;
		triangleDraw = false;
		modeChangeFlag = true;
	}
	else if (event.getChar() == 'f')
	{
		filledShapes = !filledShapes;
	}
	else if (event.getChar() == 'o')
	{
		lineDraw = false;
		circleDraw = false;
		rectDraw = true;
		triangleDraw = false;
		modeChangeFlag = true;
	}
	else if (event.getChar() == 'p')
	{
		lineDraw = false;
		circleDraw = false;
		rectDraw = false;
		triangleDraw = true;
		modeChangeFlag = true;
	}
	else if (event.getChar() == 'n'){
		saveImage(".png");
		imageFlag = true;
	}
	else if (event.getChar() == 'j') {
		saveImage(".jpeg");
		imageFlag = true;

	}
	else if (event.getChar() == 't') {
		saveImage(".tif");
		imageFlag = true;
	}
#ifdef EYEX
	else if (event.getChar() == ' ')
	{
		lineDraw = false;
		circleDraw = false;
		rectDraw = false;
		triangleDraw = false;
		if (gazePositionX <= resolutionX / 2 && gazePositionY <= resolutionY / 2)
		{
			lineDraw = true;
		}
		else if (gazePositionX >= resolutionX / 2 && gazePositionY <= resolutionY / 2)
		{
			circleDraw = true;
		}
		else if (gazePositionX <= resolutionX / 2 && gazePositionY >= resolutionY / 2)
		{
			rectDraw = true;
		}
		else if (gazePositionX >= resolutionX / 2 && gazePositionY >= resolutionY / 2)
		{
			triangleDraw = true;
		}

	}
#endif
	else if (event.getChar() == 'l')
	{

		if (currLayer == 0){
			currLayer = 1;
			//gl::drawSolidCircle(getWindowCenter(), 10);
		}
		else if (currLayer == 1)
		{
			currLayer = 0;
			//gl::drawSolidCircle(getWindowCenter(), 150);
		}
	}

}

void TouchPointsApp::touchesBegan(TouchEvent event)
{
	//CI_LOG_I(event);

	//If we are in line draw mode
	for (const auto &touch : event.getTouches()) {
		if (eraserMode){
			Color newColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2]);
			myActivePointsEraser.insert(make_pair(touch.getId(), TouchPoint(touch.getPos(), newColor, lineSize * 2)));
		}
		else if (lineDraw){
			if (randColor){
				Color newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
				myActivePoints.insert(make_pair(touch.getId(), TouchPoint(touch.getPos(), newColor, lineSize)));
			}
			else {
				Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
				myActivePoints.insert(make_pair(touch.getId(), TouchPoint(touch.getPos(), newColor, lineSize)));
			}

		}
		else if (circleDraw){
			if (randColor){
				Color newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
				myActiveCircles.insert(make_pair(touch.getId(), TouchCircle(touch.getPos(), 30.0f, newColor, lineSize, filledShapes)));
			}
			else {
				Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
				myActiveCircles.insert(make_pair(touch.getId(), TouchCircle(touch.getPos(), 30.0f, newColor, lineSize, filledShapes)));
			}

		}
		else if (rectDraw){
			if (randColor){
				Color newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
				myActiveRectangles.insert(make_pair(touch.getId(), TouchRectangle(touch.getPos().x, touch.getPos().y, touch.getPos().x, touch.getPos().y, newColor, lineSize, filledShapes)));
			}
			else {
				Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
				myActiveRectangles.insert(make_pair(touch.getId(), TouchRectangle(touch.getPos().x, touch.getPos().y, touch.getPos().x, touch.getPos().y, newColor, lineSize, filledShapes)));
			}
		}
		else if (triangleDraw){
			if (randColor){
				Color newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
				myActiveTriangles.insert(make_pair(touch.getId(), TouchTriangle(touch.getPos(), touch.getPos(), touch.getPos(), touch.getPos(), newColor, lineSize, filledShapes)));
			}
			else {
				Color newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
				myActiveTriangles.insert(make_pair(touch.getId(), TouchTriangle(touch.getPos(), touch.getPos(), touch.getPos(), touch.getPos(), newColor, lineSize, filledShapes)));
			}
		}

	}
}

void TouchPointsApp::touchesMoved(TouchEvent event)
{
	//CI_LOG_I(event);
	if (eraserMode){
		for (const auto &touch : event.getTouches()) {
			//myActivePointsEraser[touch.getId()].addPoint(touch.getPos());
			missedPoints(touch.getPrevX(), touch.getPrevY(), touch.getX(), touch.getY(), myActivePointsEraser[touch.getId()]);

		}
	}
	else if (lineDraw){
		for (const auto &touch : event.getTouches()) {

			/*
			float a = touch.getPrevX() - touch.getX();
			a = a * a;
			float b = touch.getPrevY() - touch.getY();
			b = b * b;
			float c = sqrt(a + b);

			if (.25 * c > myActivePoints[touch.getId()].size())
			*/
			missedPoints(touch.getPrevX(), touch.getPrevY(), touch.getX(), touch.getY(), myActivePoints[touch.getId()]);
			myPoints.push_back(myActivePoints[touch.getId()]);
			myActivePoints[touch.getId()].clearPoints();

			if (currLayer == 0){


				(*firstFbo).bindFramebuffer();
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
				(*secondFbo).bindFramebuffer();
				//Draws the points multiple times to prevent flickering.
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*secondFbo).unbindFramebuffer();
			}

			myPoints.clear();
			//myActivePoints[touch.getId()].addPoint(touch.getPos());

		}
	}
	else if (circleDraw){
		for (const auto &touch : event.getTouches()) {

			float a;
			float b;
			float c;
			a = touch.getPos().x - myActiveCircles[touch.getId()].getCenterX();
			b = touch.getPos().y - myActiveCircles[touch.getId()].getCenterY();
			a = a*a;
			b = b*b;
			c = sqrt(a + b);
			myActiveCircles[touch.getId()].changeRadius(c);
			/*
			myActiveCircles[touch.getId()].changeRadius(sqrt(((touch.getPos().x - myActiveCircles[touch.getId()].getCenterX())^2) +
			((touch.getPos().y - myActiveCircles[touch.getId()].getCenterY()) ^ 2)));
			*/
			//myActiveCircles[touch.getId()].changeRadius( TouchPointsApp::alterRadius(touch.getId(), touch.getPos()));
		}
	}
	else if (rectDraw)
	{
		for (const auto &touch : event.getTouches()){
			myActiveRectangles[touch.getId()].changePoint(touch.getPos().x, touch.getPos().y);

		}

	}
	else if (triangleDraw)
	{
		for (const auto &touch : event.getTouches()){
			myActiveTriangles[touch.getId()].changeSize(touch.getPos().x, touch.getPos().y);

		}

	}
}

void TouchPointsApp::touchesEnded(TouchEvent event)
{
	//CI_LOG_I(event);
	for (const auto &touch : event.getTouches()) {
		if (eraserMode){
			// myActivePoints[touch.getId()].startDying();
			myPointsEraser.push_back(myActivePointsEraser[touch.getId()]);
			myActivePointsEraser.erase(touch.getId());
			if (currLayer == 0){


				(*firstFbo).bindFramebuffer();
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myPointsEraser.begin(); oldPoints != myPointsEraser.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
				(*secondFbo).bindFramebuffer();
				//Draws the points multiple times to prevent flickering.
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myPointsEraser.begin(); oldPoints != myPointsEraser.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*secondFbo).unbindFramebuffer();
			}

			myPointsEraser.clear();
		}
		else if (lineDraw){
			myActivePoints.erase(touch.getId());
			// myActivePoints[touch.getId()].startDying();
			/*
			myPoints.push_back(myActivePoints[touch.getId()]);
			myActivePoints.erase(touch.getId());

			if (currLayer == 0){


			(*firstFbo).bindFramebuffer();
			//for (int i = 0; i < 5; i++){
			for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
			oldPoints->draw();
			++oldPoints;
			}
			//}
			(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
			(*secondFbo).bindFramebuffer();
			//Draws the points multiple times to prevent flickering.
			//for (int i = 0; i < 5; i++){
			for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
			oldPoints->draw();
			++oldPoints;
			}
			//}
			(*secondFbo).unbindFramebuffer();
			}

			myPoints.clear();
			*/
		}
		else if (circleDraw){
			myCircles.push_back(myActiveCircles[touch.getId()]);
			myActiveCircles.erase(touch.getId());

			if (currLayer == 0){


				(*firstFbo).bindFramebuffer();
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myCircles.begin(); oldPoints != myCircles.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
				(*secondFbo).bindFramebuffer();
				//Draws the points multiple times to prevent flickering.
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myCircles.begin(); oldPoints != myCircles.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*secondFbo).unbindFramebuffer();
			}

			myCircles.clear();
		}
		else if (rectDraw){
			myRectangles.push_back(myActiveRectangles[touch.getId()]);
			myActiveRectangles.erase(touch.getId());
			if (currLayer == 0){


				(*firstFbo).bindFramebuffer();
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myRectangles.begin(); oldPoints != myRectangles.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
				(*secondFbo).bindFramebuffer();
				//Draws the points multiple times to prevent flickering.
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myRectangles.begin(); oldPoints != myRectangles.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*secondFbo).unbindFramebuffer();
			}

			myRectangles.clear();
		}
		else if (triangleDraw){
			myTriangles.push_back(myActiveTriangles[touch.getId()]);
			myActiveTriangles.erase(touch.getId());
			if (currLayer == 0){


				(*firstFbo).bindFramebuffer();
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myTriangles.begin(); oldPoints != myTriangles.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
				(*secondFbo).bindFramebuffer();
				//Draws the points multiple times to prevent flickering.
				//for (int i = 0; i < 5; i++){
				for (auto oldPoints = myTriangles.begin(); oldPoints != myTriangles.end();) {
					oldPoints->draw();
					++oldPoints;
				}
				//}
				(*secondFbo).unbindFramebuffer();
			}

			myTriangles.clear();
		}
	}
}

void TouchPointsApp::mouseDown(MouseEvent event)
{
}

void TouchPointsApp::update(){
}

void TouchPointsApp::drawUi(){
	//(*uiFbo).unbindTexture();


	//gl::clear(GL_BACK);
	//if (gazePositionX > 1920 && gazePositionY > 1080)
	if (modeChangeFlag)
	{
		modeChangeFlag = false;
		(*uiFbo).bindFramebuffer();
		uiFboFlag = true;
		//int q, w, e, r;
		//	q = windowWidth*.8;
		//w = windowHeight*.8;
		//e = windowHeight*.2;
		//r = windowWidth*.2;
		//vec2 bill(q, w);
		//gl::scissor(bill, vec2(r, e));
		//Clears the framebuffer before drawing.
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		gl::color(1.0, 0.9, 0.5);
		//gl::lineWidth(10);
		//gl::drawSolidCircle(vec2(0,0), 10);
		//gl::drawSolidCircle(vec2(1920,1080), 100);

		gl::drawStrokedRect(Rectf(windowWidth*.8, windowHeight*.8, windowWidth, windowHeight), 5);
		gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.8, windowHeight*.8, windowWidth, windowHeight));
		if (rectDraw) modeRectangle();
		else if (circleDraw) modeCircle();
		else if (triangleDraw) modeTriangle();
		else if (lineDraw) modeLine();
		(*uiFbo).unbindFramebuffer();
		uiFboFlag = false;
	}
}

void TouchPointsApp::draw()
{
	gl::enableAlphaBlending();
	//Add a vector instead of the 3 ref to arrays.
	gl::clear(Color(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2]));


	//drawUi();
	//gl::draw(uiFbo->getColorTexture());
	




	currentFrame = getLeapFrame(leapContr);
	leapDraw(currentFrame);

	/*Draws image that provides feedback */
	if(imageFlag){

		Area center = calcCenter(imageTexture);

		drawImageTexture();
		gl::draw(imageFbo->getColorTexture(),center);

	}


#ifdef EYEX
	gl::color(1.0, 1.0, 1.0);
	vec2 gaze1(gazePositionX - 10, gazePositionY);
	vec2 gaze2(gazePositionX + 10, gazePositionY);

	gl::drawStrokedCircle(gaze1, 10.0f, 10.0f);
	gl::drawStrokedCircle(gaze2, 10.0f, 10.0f);

	//UI Box
	/*
	if (gazePositionX >= windowWidth*.8 && gazePositionY <= windowHeight*.2)
	{
		gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.8, 0, windowWidth, windowHeight*.2));
	}
	*/
	//gl::color(1.0, 1.0, 1.0);
	//gl::drawSolidRect(Rectf(1400, 0, 1920, 500));
#endif
	//gl::color(0.5,0.7,0.1);
	//Frame Buffers Setup
	//Draws the framebuffer
	/*
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	GLuint texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);

	glTexImage2D(
	GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(
	GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0
	);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	gl::drawStrokedRect(Rectf(0, 500, 500, 500), 10);
	gl::drawSolidRect(Rectf(-0.5, -0.5, 500, 500));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	*/

	//Rectf(-0.5, -0.5, 0.5, 0.5);
	vec2 hilary{ 100, 100 };
	vec2 clinton{ 500, 1000 };
	vec2 biden{ 900, 100 };



	//Rect(x1,y1,x2,y2);
	//gl::drawStrokedRect(Rectf(0,500,500,500), 10);
	//gl::drawSolidRect(Rectf(-0.5, -0.5, 500, 500) );
	//gl::drawSolidTriangle(hilary, clinton, biden);
	/*
	for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
	oldPoints->draw();
	++oldPoints;
	}
	*/
	/*
	for (auto oldPoints = myCircles.begin(); oldPoints != myCircles.end();) {
	oldPoints->draw();
	++oldPoints;
	}
	for (auto oldPoints = myRectangles.begin(); oldPoints != myRectangles.end();) {
	oldPoints->draw();
	++oldPoints;
	}
	for (auto oldPoints = myTriangles.begin(); oldPoints != myTriangles.end();) {
	oldPoints->draw();
	++oldPoints;
	}
	*/
	if (currLayer == 0)
	{
		//Must always draw framebuffers as a set color!
		gl::color(1.0, 1.0, 1.0);
		//Layer 1 is on top, second is below
		gl::draw(secondFbo->getColorTexture());
		gl::draw(firstFbo->getColorTexture());

		//gl::draw(uiFbo->getColorTexture());
	}
	else if (currLayer == 1)
	{
		gl::color(1.0, 1.0, 1.0);
		//Layer 2 is on top.
		gl::draw(firstFbo->getColorTexture());
		gl::draw(secondFbo->getColorTexture());
		//gl::draw(uiFbo->getColorTexture());

	}

	drawUi();

	gl::color(1.0, 1.0, 1.0);

	//if (gazePositionX > 1920 && gazePositionY > 1080)
	{
		gl::draw(uiFbo->getColorTexture());
	}
	/*
	for (auto &activePoint : myActivePoints) {
	activePoint.second.draw();
	}
	*/

	for (auto &activePoint : myActiveCircles) {
		activePoint.second.draw();
	}
	for (auto &activePoint : myActiveRectangles) {
		activePoint.second.draw();
	}
	for (auto &activePoint : myActiveTriangles) {
		activePoint.second.draw();
	}
	for (auto &activePoint : myActivePointsEraser) {
		activePoint.second.draw();
	}

	/*
	for (auto oldPoints = myPointsEraser.begin(); oldPoints != myPointsEraser.end();) {
	oldPoints->draw();
	++oldPoints;
	}
	*/





}

CINDER_APP(TouchPointsApp, RendererGl, prepareSettings)
