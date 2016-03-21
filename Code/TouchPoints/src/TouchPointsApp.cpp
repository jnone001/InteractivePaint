

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
#include <math.h>
#include <chrono>
#include <ctime>


//Standard Library Includes
#include <vector>
#include <map>
#include <list>


//Our own includes
//#include "TouchShapes.h"
#include "SymmetryLine.h"
#include "TouchShapes.h"
#include "Illustrator.h"
#include "Enums.h"
#include "Brush.h"
#include "UserInterface.h"


#include "libusb.h"
#include <stdio.h>
#include "DeviceHandler.h"

//Leap Includes
#include "Leap.h"
#include "LeapMath.h"


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

//Device connection status
bool eyeXRunning = false;
bool leapRunning = false;
//int multiTouchConnected;
//int leapConnected;

#define EYEX
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

#define COLOR_AMOUNT 8
#define BACKGROUND_COLORS 8
#define TOTAL_SYMBOLS 7
#define TOTAL_SHAPES 7

#define COLOR_ZERO "White.png"
#define COLOR_ONE "Red.png"
#define COLOR_TWO "Yellow.png"
#define COLOR_THREE "Green.png"
#define COLOR_FOUR "Torquise.png"
#define COLOR_FIVE "Blue.png"
#define COLOR_SIX "Purple.png"
#define COLOR_SEVEN "Orange.png"

#define SHAPE_LINE "LineIcon.png"
#define SHAPE_Circle "Circle.png"
#define SHAPE_Rectangle "Rectangle.png"
#define SHAPE_Triangle "Triangle.png"
#define SHAPE_Filled_Circle "FilledCircle.png"
#define SHAPE_Filled_Rectangle "FilledRectangle.png"
#define SHAPE_Filled_Triangle "FilledTriangle.png"

#define SWIPE_GESTURE 8
//#define windowWidth  getWindowSize().x
//#define windowHeight getWindowSize().y

#define windowWidth  1919
#define windowHeight 1079



#define FRAME_RATE 120

//Leap map
map<uint32_t, vec2> pointsMap;
map<uint32_t, bool> activePointsMap;



bool imageFlag = false;
bool drawing = false;
bool lockCurrentFrame = false;
bool leapDrawFlag = true;
bool processing = false;
bool radialActive = false;
gl::TextureRef imageTexture;
int imageNum;
float fadeTime = 1;


bool modeChangeFlag = true;

//Layers
int currLayer = 0;

float lineSize = 1.0f;
float currentAlpha = 1.0;

float r, g, b = 256.0f;

int currColor = 0;
int currShape = 0;
int currBackground = 0;

float backgroundArray[BACKGROUND_COLORS][3] = { { 0.0f, 0.0f, 0.0f }, { 256.0f, 256.0f, 256.0f }, { 256.0f, 0.0f, 0.0f }, { 256.0f, 256.0f, 0.0f }, { 0.0f, 256.0f, 0.0f }, { 0.0f, 256.0f, 256.0f }, { 0.0f, 0.0f, 256.0f }, { 256.0f, 0.0f, 256.0f } };
float colorArray[COLOR_AMOUNT][3] = { { 256.0f, 256.0f, 256.0f }, { 256.0f, 0.0f, 0.0f }, { 256.0f, 256.0f, 0.0f }, { 0.0f, 256.0f, 0.0f }, { 0.0f, 256.0f, 256.0f }, { 0.0f, 0.0f, 256.0f }, { 256.0f, 0.0f, 256.0f }, { 1.0f, 0.3f, 0.0 } };

string symbolArray[TOTAL_SYMBOLS];



//Global EyeX Handler
TX_CONTEXTHANDLE hContext = TX_EMPTY_HANDLE;


class LeapListener : public Leap::Listener{
public:
	virtual void onConnect(const Leap::Controller&);
	//virtual void onFrame(const Leap::Controller&);
	virtual void onDisconnect(const Leap::Controller&);
};


//TouchPointsApp, our libcinder app!
class TouchPointsApp : public App {
public:
	void setup() override;

	void mouseDown(MouseEvent event) override;

	void	touchesBegan(TouchEvent event) override;
	void	touchesMoved(TouchEvent event) override;
	void	touchesEnded(TouchEvent event) override;
	void	keyDown(KeyEvent event) override;

	//Shapes Enum
	//enum Shape { Line, Circle, Rectangle, Triangle };


	//Drawing Functions for drawUi
	void	modeRectangle();
	void	modeCircle();
	void	modeTriangle();
	void	modeLine();

	//Function to change Shapes we are drawing
	//void	brush.changeShape(Shape x);

	//Functions to alter our alpha variable.
	void	increaseAlpha();
	void	decreaseAlpha();

	//Ui Functions
	void	drawUi();
	bool	inInteractiveUi(int x, int y);
	void	drawRadial();


	void	drawImageTexture();
	void	loadImages(string imageName);
	void	saveImage(string imageType);
	void	saveFboImage(std::shared_ptr<gl::Fbo> myFbo);


	/*Leap related functions*/
	void	enableGest(Leap::Controller controller);
	void	leapSave();
	void	leapColorChange();
	void	leapShapeChange();
	void	leapDraw(Leap::Frame frame);
	void 	gestRecognition(Leap::Frame frame, Leap::Controller controller);


	//List of MODES
	/*
	bool brush.getRandColor() = false;
	bool eraserMode = false;
	bool lineDraw = true;
	bool circleDraw = false;
	bool rectDraw = false;
	bool triangleDraw = false;
	bool filledShapes = false;
	*/

	//List of drawUI Flags
	bool modeButtons = true;
	bool colorButtons = false;
	bool shapeButtons = false;
	bool uiFboFlag = false;
	bool layerVisualization = false;


	void modeChangeFlagTrue();



#ifdef EYEX


#endif

	void update() override;
	void draw() override;


private:
	//Needs organizing
	Brush brush;
	Illustrator illustrator;
	UserInterface ui;
	DeviceHandler deviceHandler;

	
	//Leap Motion Controller
	Leap::Controller leapContr;
	Leap::Frame currentFrame;
	Leap::GestureList gestList;
	LeapListener	myLeapListener;

	//EyeX
	vec2 radialCenter = vec2(windowWidth *.5, windowHeight*.5);

	TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
	TX_TICKET hEventHandlerTicket = TX_INVALID_TICKET;
	TX_TICKET hGazeTrackingStateChangedTicket = TX_INVALID_TICKET;
	TX_EYEXAVAILABILITY availability;
	BOOL success;
	TX_HANDLE hStateBag = TX_EMPTY_HANDLE;

	//Symmetry lines
	SymmetryLine mySymmetry;
	//bool symmetryOn = false;

	/*
	//Functions to draw touch shapes with leap and multitouch
	void beginTouchShapes(uint32_t myId, vec2 myPos);
	void movingTouchShapes(uint32_t myId, vec2 myPos, vec2 prevPos);
	void endTouchShapes(uint32_t myId);
	*/
	//Gesture functions for multitouch

	bool findMultiTouchGestures(TouchEvent::Touch previousPoint, TouchEvent::Touch currentPoint);

	//Variables for point buffering
	map<uint32_t, list<TouchEvent::Touch>> bufferTouches;
	TouchEvent::Touch previousTouch;






	//Need to redo this area. We should have a list for active points, this list will contain ALL shapes.
	//Perhaps Templates? Need to attend C++ Workshop for this.
	//The touchPoint should have a variable 'shape' which just tells it if it is a square, circle, triangle, line, etc.
	//Keep the 'eraser' points though because they need to be drawn separately last!
	//Might need a new function for the eraser mode so it can ALWAYS have the correct background color.
	//Future playback feature may want eraser points in the same list as the other shapes as well.

	
	list<TouchShape*>		  myShapes;


	//
	
	int activeDrawings;


	map<uint32_t, TouchPoint>		myActivePoints;
	list<TouchPoint>				myPoints;
	map<uint32_t, TouchPoint>		myActivePointsEraser;
	list<TouchPoint>				myPointsEraser;
	map<uint32_t, TouchCircle>		myActiveCirclesEraser;
	list<TouchCircle>				myCirclesEraser;
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
	std::shared_ptr<gl::Fbo>		thirdFbo;
	std::shared_ptr<gl::Fbo>		activeFbo;
	std::shared_ptr<gl::Fbo>		iconFbo;

	std::vector<std::shared_ptr<gl::Fbo>> layerList;

	std::shared_ptr<gl::Fbo>		uiFbo;
	std::shared_ptr<gl::Fbo>		imageFbo;
	std::shared_ptr<gl::Fbo>		radialFbo;

};

/*Retrieves frames from leap motion Service*/
Leap::Frame getLeapFrame(Leap::Controller controller){
	return controller.frame();
}


void TouchPointsApp::modeChangeFlagTrue(){
	modeChangeFlag = true;
}

//Leapmotion listeners
void LeapListener::onConnect(const Leap::Controller& controller){
	leapRunning = true;
	modeChangeFlag = true;

}
void LeapListener::onDisconnect(const Leap::Controller& controller){
	leapRunning = false;
	modeChangeFlag = true;
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

void OnStateReceived(TX_HANDLE hStateBag)
{
	TX_BOOL success;
	TX_INTEGER eyeTrackingState;
	TX_SIZE2 displaySize;
	TX_SIZE2 screenBounds;
	TX_SIZE stringSize = 0;
	TX_STRING currentProfileName;
	TX_INTEGER presenceData;
	TX_INTEGER gazeTracking;


	success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_EYETRACKINGSTATE, &eyeTrackingState) == TX_RESULT_OK);
	if (success) {
		switch (eyeTrackingState) {
		case TX_EYETRACKINGDEVICESTATUS_TRACKING:
			eyeXRunning = true;
			modeChangeFlag = true;
			break;

		case TX_EYETRACKINGDEVICESTATUS_DEVICENOTCONNECTED:
			eyeXRunning = false;
			modeChangeFlag = true;
			break;

		default:
			printf(":DDD");
		}
	}

	/*

	success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_GAZETRACKING, &gazeTracking) == TX_RESULT_OK);
	if (success) {
	(gazeTracking == TX_GAZETRACKING_GAZETRACKED) ? eyeXRunning = true : eyeXRunning = false;
	modeChangeFlag = true;
	}
	*/


}
void TX_CALLCONVENTION OnEngineStateChanged(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
{
	TX_RESULT result = TX_RESULT_UNKNOWN;
	TX_HANDLE hStateBag = TX_EMPTY_HANDLE;

	if (txGetAsyncDataResultCode(hAsyncData, &result) == TX_RESULT_OK &&
		txGetAsyncDataContent(hAsyncData, &hStateBag) == TX_RESULT_OK) {
		OnStateReceived(hStateBag);
		txReleaseObject(&hStateBag);
	}
}

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
		//eyeXRunning = true;
		//printf("The connection state is now CONNECTED (We are connected to the EyeX Engine)\n");
		// commit the snapshot with the global interactor as soon as the connection to the engine is established.
		// (it cannot be done earlier because committing means "send to the engine".)
		success = txCommitSnapshotAsync(g_hGlobalInteractorSnapshot, OnSnapshotCommitted, NULL) == TX_RESULT_OK;
		txGetStateAsync(hContext, TX_STATEPATH_EYETRACKING, OnEngineStateChanged, NULL);
		if (!success) {

		}
		else {
			//printf("Waiting for gaze data to start streaming...\n");
		}
	}
									   break;

	case TX_CONNECTIONSTATE_DISCONNECTED:
		//printf("The connection state is now DISCONNECTED (We are disconnected from the EyeX Engine)\n");
		eyeXRunning = false;
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


/* 'Checking whether if the eye tracker is connected'
void TouchPointsApp::HandleNotification(InteractionNotification notification)
{
if (NotificationType.SettingsChanged == notification.NotificationType)
{
OnSettingsReceived((SettingsBag)notification.Data);
}
}

void TouchPointsApp::OnSettingsReceived(SettingsBag data)
{
EyeTrackingDeviceStatus deviceStatus;
var success = data.TryGetSettingValue(out deviceStatus, "eyetracking.state");

if (success)
{
Console.WriteLine("Eye Tracking Device Status is: " + deviceStatus);

}
}
*/

#endif



void TouchPointsApp::setup()
{
	//Sets max mulitouch points
	auto testvar1 = System::hasMultiTouch();
	int32_t testvar2 = System::getMaxMultiTouchPoints();


	CI_LOG_I("MT: " << System::hasMultiTouch() << " Max points: " << System::getMaxMultiTouchPoints());
	glEnable(GL_LINE_SMOOTH);
	//setWindowSize(windowWidth, windowHeight);
	setFullScreen(1);

	//Sets window size and initializes framebuffers (layers).
	setWindowSize(windowWidth, windowHeight);
	gl::Fbo::Format format;
	firstFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	secondFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	thirdFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	activeFbo = gl::Fbo::create(windowWidth, windowHeight, format);

	iconFbo = gl::Fbo::create(windowWidth, windowHeight, format);

	//Set up UI
	uiFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	//Set up image feedback fbo
	imageFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	//Set up fbo for proxy menu
	radialFbo = gl::Fbo::create(windowWidth, windowHeight, format);

	//Enable all Leap Gestures
	TouchPointsApp::enableGest(leapContr);

	leapContr.config().setFloat("Gesture.Swipe.MinLength", 150.0);
	leapContr.config().setFloat("Gesture.Swipe.MinVelocity", 500.0);
	leapContr.config().save();

	leapContr.addListener(myLeapListener);

	setFrameRate(FRAME_RATE);

	//activeDrawings = 0;

	//Set up symmetry line

	mySymmetry = SymmetryLine(windowWidth / 2, true);

	//Sets up layers
	layerList.emplace_back(firstFbo);
	layerList.emplace_back(secondFbo);
	layerList.emplace_back(thirdFbo);
	//Set up Brush
	bool tempFalse = false;
	bool tempTrue = true;
	ColorA newColor = ColorA(1.0f, 1.0f, 1.0f, 1.0f);
	float tempFloat = 1.0f;
	int tempInt = 1;
	Shape::Shape myShape = Shape::Shape::Line;
	brush = Brush(myShape, newColor, tempFloat , tempInt, tempFalse, tempFalse, tempFalse, &mySymmetry);
	illustrator = Illustrator(&brush, &layerList);
	deviceHandler = DeviceHandler();

	//Set up UI
	ui = UserInterface(windowWidth, windowHeight, leapRunning, eyeXRunning, &brush,  &illustrator, &deviceHandler, uiFbo, &layerList);

	//Sets up eyeX context
#ifdef EYEX
	//Get desktop resolution
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	resolutionX = desktop.right;
	resolutionY = desktop.bottom;



	//	hcontext.RegisterNotificationHandler(HandleNotification);
	//	hcontext.RegisterSettingObserver(SettingsPaths.EyeTracking);



	// initialize and enable the context that is our link to the EyeX Engine.
	success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
	success &= txCreateContext(&hContext, TX_FALSE) == TX_RESULT_OK;
	success &= InitializeGlobalInteractorSnapshot(hContext);
	success &= txRegisterConnectionStateChangedHandler(hContext, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
	success &= txRegisterEventHandler(hContext, &hEventHandlerTicket, HandleEvent, NULL) == TX_RESULT_OK;
	success &= txEnableConnection(hContext) == TX_RESULT_OK;
	success &= txRegisterStateChangedHandler(hContext, &hGazeTrackingStateChangedTicket, TX_STATEPATH_GAZETRACKING, OnEngineStateChanged, NULL) == TX_RESULT_OK;

	if (success){
		//eyeXRunning = true;
	}
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
void missingPoints(int xi, int yi, int xf, int yf, TouchPoint& points){


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
	Area center = Area::proportionalFit(window, image, true, false);
	//Area center(vec2(800, 800), vec2(1100, 1100));
	return center;
}

void TouchPointsApp::enableGest(Leap::Controller controller){

	controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
	controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
	controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);

}

void TouchPointsApp::gestRecognition(Leap::Frame frame, Leap::Controller controller){

	if (!drawing){
		//List of all gestures
		gestList = frame.gestures();
		//Process gestures...
		if (!processing){
			//for (int g = 0; g < gestures.count(); ++g)
			for (Leap::Gesture gesture : gestList) {
				//Gesture gesture = gestList[g];

				switch (gesture.type()) {
				case Leap::Gesture::TYPE_CIRCLE:
				{
					Leap::CircleGesture circle = gesture;

					if (circle.pointable().direction().angleTo(circle.normal()) <= M_PI / 2) {
						//clockwise circle
						processing = true;
						leapShapeChange();
					}
					else {
						//counterclockwise circle
						processing = true;
						leapShapeChange();

					}

					// Calculate angle swept since last frame

					float sweptAngle = 0;
					if (circle.state() != Leap::Gesture::STATE_START) {
						Leap::CircleGesture previousUpdate = Leap::CircleGesture(controller.frame(1).gesture(circle.id()));
						sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * M_PI;
					}

					break;
				}
				case Leap::Gesture::TYPE_SWIPE:
				{
					processing = true;
					leapColorChange();
					break;
				}
				case Leap::Gesture::TYPE_KEY_TAP:
				{
					processing = true;
					leapSave();
					break;

				}
				case Leap::Gesture::TYPE_SCREEN_TAP:
				{
					Leap::ScreenTapGesture screentap = gesture;

					break;
				}
				default:
					std::cout << std::string(2, ' ') << "Unknown gesture type." << std::endl;
					break;
				}
			}
		}
	}
}

void TouchPointsApp::leapSave(){

	saveImage(".png");
	imageFlag = true;
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
			/*LEAP DRAW ALL SHAPES CODE. NOT READY FOR IMPLEMENTATION*/
			if(pointsMap.find(points.id()) != pointsMap.end()){
			pointsMap.erase(points.id());
			//myActivePoints[points.id()].clearPoints();
			activePointsMap.erase(points.id());
			illustrator.endTouchShapes(points.id());
			}
			
		}
		else if (points.touchDistance() <= 0)
		{

			lockCurrentFrame = true;

			//Check to see if id for pointable object is present
			auto result = pointsMap.find(points.id());
			//Checks to see if new pointable is drawing
			if (result == pointsMap.end()){

				//myPrevX = leapXCoordinate;
				//myPrevY = leapYCoordinate;
				/*
				if (brush.getRandColor()){
					ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, currentAlpha);
					myActivePoints.insert(make_pair(points.id(), TouchPoint(vec2(leapXCoordinate, leapYCoordinate), newColor, lineSize)));
				}
				else {
					ColorA newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2], currentAlpha);
					myActivePoints.insert(make_pair(points.id(), TouchPoint(vec2(leapXCoordinate, leapYCoordinate), newColor, lineSize)));
				}
				*/
				/*LEAP DRAW ALL SHAPES CODE. NOT READY FOR IMPLEMENTATION*/
				illustrator.beginTouchShapes(points.id(), vec2(leapXCoordinate, leapYCoordinate));
				
				pointsMap.emplace(points.id(), vec2(leapXCoordinate, leapYCoordinate));
				activePointsMap.emplace(points.id(), true);
				//myActivePoints[points.id()].addPoint(vec2(leapXCoordinate, leapYCoordinate));


			}
			else{
				//New poitable is not drawing need to add new point
				//prevPoint = result->second;
				//gl::clear(Color(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2]));

				//missingPoints(leapXCoordinate, leapYCoordinate, myActivePoints[points.id()].getFirstPoint().x, myActivePoints[points.id()].getFirstPoint().y, myActivePoints[points.id()]);

				//myPrevX = leapXCoordinate;
				//myPrevY = leapYCoordinate;
				/*
				myPoints.push_back(myActivePoints[points.id()]);
				myActivePoints[points.id()].clearPoints();

				myActivePoints[points.id()].addPoint(vec2(leapXCoordinate, leapYCoordinate));


				(*(layerList).back()).bindFramebuffer();
				for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
					oldPoints->draw();
					if ((*brush.getSymmetry()).getSymmetryOn())
						(*((brush).getSymmetry())).symmetricLine(*oldPoints).draw();
					++oldPoints;
				}
				(*(layerList).back()).unbindFramebuffer();


				
				if (currLayer == 0){

					(*firstFbo).bindFramebuffer();
					//for (int i = 0; i < 5; i++){
					for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
						oldPoints->draw();
						if ((*brush.getSymmetry()).getSymmetryOn())
							mySymmetry.symmetricLine(*oldPoints).draw();
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
						if ((*brush.getSymmetry()).getSymmetryOn())
							mySymmetry.symmetricLine(*oldPoints).draw();
						++oldPoints;
					}
					//}
					(*secondFbo).unbindFramebuffer();
				}
				

				myPoints.clear();
				*/
				/*LEAP DRAW ALL SHAPES CODE. NOT READY FOR IMPLEMENTATION*/
				
				illustrator.movingTouchShapes(points.id(), vec2(leapXCoordinate, leapYCoordinate), pointsMap[points.id()]);
				//pointsMap.emplace(points.id(), vec2(leapXCoordinate, leapYCoordinate));
				pointsMap[points.id()] = vec2(leapXCoordinate, leapYCoordinate);
				//myActivePoints[points.id()].addPoint(vec2(leapXCoordinate, leapYCoordinate));
				

			}


		}
		else
		{
			/*LEAP DRAW ALL SHAPES CODE. NOT READY FOR IMPLEMENTATION
			if (pointsMap.find(points.id()) != pointsMap.end()){
			pointsMap.erase(points.id());
			myActivePoints[points.id()].clearPoints();
			endTouchShapes(points.id());
			}
			*/

		}
	}
	for(auto points : activePointsMap){
		if ( points.second ){
			points.second = false;
		}
		else {
			illustrator.endTouchShapes(points.first);
			activePointsMap.erase(points.first);
			pointsMap.erase(points.first);
		}
	}

}

void TouchPointsApp::leapShapeChange(){



	if (currShape != TOTAL_SYMBOLS - 1){
		currShape++;
	}
	else{
		currShape = 0;
	}
	brush.incrementShape();
	ui.setModeChangeFlag();
	switch (brush.getCurrentShape()){
	case 0:{
		//brush.changeShape(Shape::Shape::Line);
		loadImages(SHAPE_LINE);
		imageFlag = true;
		//Might not need this since brush.changeShape has it
		//modeChangeFlag = true;
		break;
	}
	case 1:{
		bool tempBool = false;
		//brush.changeFilledShapes(tempBool);
		//brush.changeShape(Shape::Shape::Circle);
		if (!brush.getFilledShapes())
		loadImages(SHAPE_Circle);
		else loadImages(SHAPE_Filled_Circle);
		imageFlag = true;
		//modeChangeFlag = true;
		break;
	}
	case 2:{
			  
			   //brush.changeFilledShapes(tempBool);
		//brush.changeShape(Shape::Shape::Rectangle);
		if (!brush.getFilledShapes())
		loadImages(SHAPE_Rectangle);
		else loadImages(SHAPE_Filled_Rectangle);
		imageFlag = true;
		//modeChangeFlag = true;
		break;
	}
	case 3:{
			   
			   //brush.changeFilledShapes(tempBool);
		//brush.changeShape(Shape::Shape::Triangle);
		if (!brush.getFilledShapes())
		loadImages(SHAPE_Triangle);
		else loadImages(SHAPE_Filled_Triangle);
		imageFlag = true;
		//modeChangeFlag = true;
		break;
	}
	default:{
		//std::cout << std::string(2, ' ') << "Unknown gesture type." << std::endl;
		break;
	}

	}
}

void TouchPointsApp::leapColorChange(){

	if (currColor != COLOR_AMOUNT - 1){
		currColor++;
	}
	else {
		currColor = 0;
	}

	brush.incrementColor();
	ui.setModeChangeFlag();
	//Provides correct image to provide feedback
	switch (brush.getCurrentColor()){
	case 0:{
		loadImages(COLOR_ZERO);
		imageFlag = true;
		modeChangeFlag = true;
		break;
	}
	case 1:{
		loadImages(COLOR_ONE);
		imageFlag = true;
		modeChangeFlag = true;
		break;
	}
	case 2:{
		loadImages(COLOR_TWO);
		imageFlag = true;
		modeChangeFlag = true;
		break;
	}
	case 3:{
		loadImages(COLOR_THREE);
		imageFlag = true;
		modeChangeFlag = true;
		break;
	}
	case 4:{
		loadImages(COLOR_FOUR);
		imageFlag = true;
		modeChangeFlag = true;
		break;
	}
	case 5:{
		loadImages(COLOR_FIVE);
		imageFlag = true;
		modeChangeFlag = true;
		break;
	}
	case 6:{
		loadImages(COLOR_SIX);
		imageFlag = true;
		modeChangeFlag = true;
		break;
	}
	case 7:{
		loadImages(COLOR_SEVEN);
		imageFlag = true;
		modeChangeFlag = true;
		break;
	}
	default:{
		//std::cout << std::string(2, ' ') << "Unknown gesture type." << std::endl;
		break;
	}

	}
}

void TouchPointsApp::drawImageTexture(){

	(*imageFbo).bindFramebuffer();

	glClearColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	gl::color(1.0, 1.0, 1.0, fadeTime);

	gl::draw(imageTexture, Rectf(0, 0, 1920, 1080));

	if (fadeTime <= 0.1){
		imageFlag = false;
		processing = false;
		fadeTime = 1;
	}

	fadeTime -= 0.009;


	(*imageFbo).unbindFramebuffer();

}

void TouchPointsApp::drawRadial(){
	gl::color(1.0, 1.0, 1.0, 1.0);
	(*radialFbo).bindFramebuffer();
	glClearColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	(*radialFbo).unbindFramebuffer();

	//Draw to radial menu buffer
	(*radialFbo).bindFramebuffer();
	//Make background transparent
	glClearColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//Center Circle
	gl::color(1.0, 0.9, 0.5);
	gl::drawStrokedCircle(radialCenter, 30.0f, 2.0f);

	gl::color(1.0, 1.0, 1.0, 0.8);

	gl::drawSolidCircle(radialCenter, 29.0f);

	//Draws outer ring
	gl::color(1.0, 1.0, 1.0);
	gl::drawStrokedCircle(radialCenter, 100.0f, 60.0f);


	gl::color(1.0, 1.0, 1.0, 0.5);
	gl::drawStrokedCircle(radialCenter, 131.0f, 2.0f, 300);


	gl::color(1.0, 1.0, 1.0,0.8);
	gl::drawStrokedCircle(radialCenter, 69.0f, 2.0f, 300);
		


	//Draw left icon
	
	imageTexture = gl::Texture::create(loadImage(loadAsset("AllColor.png")));
	(*iconFbo).bindFramebuffer();
	gl::clear(ColorA(1.0, 1.0, 1.0, 1.0));
	gl::draw(imageTexture, Rectf(0,0,1920,1080));
	(*iconFbo).unbindFramebuffer();
	(*radialFbo).bindFramebuffer();
	
	iconFbo->blitTo(radialFbo, Area(vec2(0,0), vec2(1920, 1080)), Area(vec2(radialCenter.x - 125, windowHeight - radialCenter.y + 25), vec2(radialCenter.x - 75, windowHeight - radialCenter.y -25)), GL_NEAREST, GL_COLOR_BUFFER_BIT);
	//iconFbo->blitTo(radialFbo, Area(vec2(0, 0), vec2(1920, 1080)), Area(vec2(0,0), vec2(100,100)), GL_NEAREST, GL_COLOR_BUFFER_BIT);
	/*
	gl::color(1.0, 0.9, 0.5);
	gl::drawStrokedCircle(vec2(radialCenter.x - 100, radialCenter.y), 30.0f, 2.0f);

	gl::color(1.0,1.0,1.0);
	gl::drawSolidCircle(vec2(radialCenter.x - 100, radialCenter.y), 29.0f);
	*/
	//Draw Right IconimageTexture = gl::Texture::create(loadImage(loadAsset("AllColor.png")));

	imageTexture = gl::Texture::create(loadImage(loadAsset("ShapesIcon.png")));
	(*iconFbo).bindFramebuffer();
	gl::clear(ColorA(1.0, 1.0, 1.0, 1.0));
	gl::draw(imageTexture, Rectf(10, 10, 1910, 1070));
	(*iconFbo).unbindFramebuffer();
	(*radialFbo).bindFramebuffer();

	iconFbo->blitTo(radialFbo, Area(vec2(0, 0), vec2(1920, 1080)), Area(vec2(radialCenter.x + 75, windowHeight - radialCenter.y + 25), vec2(radialCenter.x + 125, windowHeight - radialCenter.y - 25)), GL_NEAREST, GL_COLOR_BUFFER_BIT);
	/*
	gl::color(1.0, 0.9, 0.5);
	gl::drawStrokedCircle(vec2(radialCenter.x + 100, radialCenter.y), 30.0f, 2.0f);

	gl::color(1.0, 1.0, 1.0);
	gl::drawSolidCircle(vec2(radialCenter.x + 100, radialCenter.y), 29.0f);
	*/

	//Draw lower Icon
	gl::color(1.0, 0.9, 0.5);
	gl::drawStrokedCircle(vec2(radialCenter.x, radialCenter.y - 100), 30.0f, 2.0f);

	gl::color(1.0, 1.0, 1.0);

	gl::drawSolidCircle(vec2(radialCenter.x , radialCenter.y - 100), 29.0f);
	//Draw Upper Icon
	gl::color(1.0, 0.9, 0.5);
	gl::drawStrokedCircle(vec2(radialCenter.x, radialCenter.y + 100), 30.0f, 2.0f);

	gl::color(1.0, 1.0, 1.0);
	gl::drawSolidCircle(vec2(radialCenter.x, radialCenter.y + 100), 29.0f);

	(*radialFbo).unbindFramebuffer();

	radialActive = true;

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

void TouchPointsApp::saveFboImage(std::shared_ptr<gl::Fbo> myFbo){

	writeImage(getHomeDirectory() / "cinder" / "Saved_Images" / (toString(imageNum) + "myPixels.png"), (myFbo->getColorTexture())->createSource());
}
/*Mode Change Functions*/

void TouchPointsApp::modeRectangle(){

	if (brush.getRandColor()){
		ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, currentAlpha);
		gl::color(newColor);
		//gl::lineWidth(10);
		//gl::drawSolidCircle(vec2(0,0), 10);
		//gl::drawSolidCircle(vec2(1920,1080), 100);
		if (!brush.getFilledShapes())
			gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		else gl::drawSolidRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95));
		//(*uiFbo).unbindFramebuffer();
	}

	else{
		ColorA newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2], currentAlpha);
		gl::color(newColor);
		//gl::lineWidth(10);
		//gl::drawSolidCircle(vec2(0,0), 10);
		//gl::drawSolidCircle(vec2(1920,1080), 100);
		if (!brush.getFilledShapes())
			gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		else gl::drawSolidRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95));
		//(*uiFbo).unbindFramebuffer();
	}
}

void TouchPointsApp::modeCircle(){

	if (brush.getRandColor()){
		ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, currentAlpha);
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!brush.getFilledShapes())
			gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, lineSize*2.0f);
		else
			gl::drawSolidCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05);

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		//(*uiFbo).unbindFramebuffer();
	}
	else{

		ColorA newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2], currentAlpha);
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!brush.getFilledShapes())
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
	if (brush.getRandColor()){
		ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, currentAlpha);
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!brush.getFilledShapes()){
			/*"Better" Triagnles
			//gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, lineSize*2.0f);
			TouchPoint uiTouchPoint(vec2(windowWidth*.85, windowHeight*.95), newColor, lineSize);
			uiTouchPoint.addPoint(vec2(windowWidth*.95, windowHeight*.95));
			uiTouchPoint.addPoint(vec2(windowWidth*.9, windowHeight*.85));
			uiTouchPoint.addPoint(vec2(windowWidth*.85, windowHeight*.95));

			missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.95, uiTouchPoint);
			missedPoints(windowWidth*.95, windowHeight*.95, windowWidth*.9, windowHeight*.85, uiTouchPoint);
			missedPoints(windowWidth*.9, windowHeight*.85, windowWidth*.85, windowHeight*.95, uiTouchPoint);
			uiTouchPoint.draw();
			*/

			gl::lineWidth(lineSize);
			gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.95));
			gl::drawLine(vec2(windowWidth*.95, windowHeight*.95), vec2(windowWidth*.9, windowHeight*.85));
			gl::drawLine(vec2(windowWidth*.9, windowHeight*.85), vec2(windowWidth*.85, windowHeight*.95));

		}
		else
			gl::drawSolidTriangle(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight *.95), vec2(windowWidth*.9, windowHeight*.85));

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		//(*uiFbo).unbindFramebuffer();
	}
	else{
		ColorA newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2], currentAlpha);
		gl::color(newColor);
		//gl::lineWidth(10);
		if (!brush.getFilledShapes()){
			/*"Better triangles maybe later"
			//gl::drawStrokedCircle(vec2(windowWidth*.9, windowHeight*.9), windowHeight * .05, lineSize*2.0f);
			TouchPoint uiTouchPoint(vec2(windowWidth*.85, windowHeight*.95), newColor, lineSize);
			uiTouchPoint.addPoint(vec2(windowWidth*.95, windowHeight*.95));
			uiTouchPoint.addPoint(vec2(windowWidth*.9, windowHeight*.85));
			uiTouchPoint.addPoint(vec2(windowWidth*.85, windowHeight*.95));

			missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.95, uiTouchPoint);
			missedPoints(windowWidth*.95, windowHeight*.95, windowWidth*.9, windowHeight*.85, uiTouchPoint);
			missedPoints(windowWidth*.9, windowHeight*.85, windowWidth*.85, windowHeight*.95, uiTouchPoint);
			uiTouchPoint.draw();
			*/

			gl::lineWidth(lineSize);
			gl::drawLine(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight*.95));
			gl::drawLine(vec2(windowWidth*.95, windowHeight*.95), vec2(windowWidth*.9, windowHeight*.85));
			gl::drawLine(vec2(windowWidth*.9, windowHeight*.85), vec2(windowWidth*.85, windowHeight*.95));

		}
		else
			gl::drawSolidTriangle(vec2(windowWidth*.85, windowHeight*.95), vec2(windowWidth*.95, windowHeight *.95), vec2(windowWidth*.9, windowHeight*.85));

		//gl::drawSolidCircle(vec2(1920,1080), 100);
		//gl::drawStrokedRect(Rectf(windowWidth*.85, windowHeight*.85, windowWidth*.95, windowHeight*.95), lineSize);
		//(*uiFbo).unbindFramebuffer();
	}
}

void TouchPointsApp::modeLine(){

	if (brush.getRandColor()){
		//Performance Issues with drawing these 3 lines every frame!
		ColorA newColor1(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, currentAlpha);
		ColorA newColor2(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, currentAlpha);
		ColorA newColor3(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, currentAlpha);
		//gl::color(newColor1);
		TouchPoint newTouchPoints1(vec2(windowWidth*.85, windowHeight*.95), newColor1, lineSize);
		illustrator.missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.85, newTouchPoints1);
		TouchPoint newTouchPoints2(vec2(windowWidth*.86, windowHeight*.90), newColor2, lineSize);
		illustrator.missedPoints(windowWidth*.86, windowHeight*.90, windowWidth*.92, windowHeight*.84, newTouchPoints2);
		TouchPoint newTouchPoints3(vec2(windowWidth*.88, windowHeight*.97), newColor3, lineSize);
		illustrator.missedPoints(windowWidth*.88, windowHeight*.97, windowWidth*.94, windowHeight*.90, newTouchPoints3);
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

		ColorA newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2], currentAlpha);
		gl::color(newColor);
		TouchPoint newTouchPoints1(vec2(windowWidth*.85, windowHeight*.95), newColor, lineSize);
		illustrator.missedPoints(windowWidth*.85, windowHeight*.95, windowWidth*.95, windowHeight*.85, newTouchPoints1);
		TouchPoint newTouchPoints2(vec2(windowWidth*.86, windowHeight*.90), newColor, lineSize);
		illustrator.missedPoints(windowWidth*.86, windowHeight*.90, windowWidth*.92, windowHeight*.84, newTouchPoints2);
		TouchPoint newTouchPoints3(vec2(windowWidth*.88, windowHeight*.97), newColor, lineSize);
		illustrator.missedPoints(windowWidth*.88, windowHeight*.97, windowWidth*.94, windowHeight*.90, newTouchPoints3);
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

/**/
void TouchPointsApp::increaseAlpha(){
	if (currentAlpha != 1.0){
		currentAlpha += 0.1;
	}
	else {}
	modeChangeFlag = true;
}

void TouchPointsApp::decreaseAlpha(){
	if (currentAlpha > 0.0){
		currentAlpha -= 0.1;
	}
	else {}
	modeChangeFlag = true;
}

void TouchPointsApp::keyDown(KeyEvent event)
{
	if (event.getChar() == 'z') {
		// Toggle full screen when the user presses the 'f' key.
		if (lineSize != 1.0f)
			lineSize--;
		
		brush.decreaseLineSize();
		ui.setModeChangeFlag();
		
	}
	else if (event.getChar() == 'x') {
		// Clear the list of points when the user presses the space bar.
		if (lineSize != 15.0f)
			lineSize++;
		
		brush.increaseLineSize();
		ui.setModeChangeFlag();

	}
	else if (event.getCode() == KeyEvent::KEY_ESCAPE) {
		//Clears EyeX context then quits the program.
#ifdef EYEX
		txDisableConnection(hContext);
		txReleaseObject(&g_hGlobalInteractorSnapshot);
		success = txShutdownContext(hContext, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
		success &= txReleaseContext(&hContext) == TX_RESULT_OK;
		success &= txUninitializeEyeX() == TX_RESULT_OK;
#endif
		quit();
	}
	else if (event.getChar() == 'r')	//Turns on random color mode
	{
		//Turns on random color mode
		brush.changeRandColor(!brush.getRandColor());

		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'q')	//Cycles through colors
	{
		if (currColor != 0)
			currColor--;
		else currColor = COLOR_AMOUNT - 1;
		

		
		brush.decrementColor();
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'w')	//Cycles through colors
	{
		if (currColor != COLOR_AMOUNT - 1)
			currColor++;
		else currColor = 0;
		brush.incrementColor();

		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 't')	//Increase transparency / Decrease alpha
	{
		brush.decreaseAlpha();
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'y')	//Decrease transparency/ increase alpha
	{
		brush.increaseAlpha();
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'e')	//Eraser mode
	{

		brush.changeEraserMode(!(brush.getEraserMode()));

		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'c')	//Clear Screen (Broken with framebuffers).
	{
		/*
		(*firstFbo).bindFramebuffer();
		glClearColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*firstFbo).unbindFramebuffer();

		(*secondFbo).bindFramebuffer();
		glClearColor(backgroundArray[currBackground]`[0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*secondFbo).unbindFramebuffer();

		(*thirdFbo).bindFramebuffer();
		glClearColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*thirdFbo).unbindFramebuffer();

		(*uiFbo).bindFramebuffer();
		glClearColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*uiFbo).unbindFramebuffer();

		ui.setModeChangeFlag();
		*/
		ui.clearLayers();

	}
	else if (event.getChar() == 'b')
	{
		/*
		if (currBackground != BACKGROUND_COLORS - 1)
			currBackground = currBackground + 1;
		else currBackground = 0;
		*/
		ui.changeBackgroundColor(Color(1.0, 1.0, 1.0));
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'u')
	{
		brush.changeShape(Shape::Shape::Line);
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'i')
	{
		brush.changeShape(Shape::Shape::Circle);
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'f')
	{
		brush.changeFilledShapes(!brush.getFilledShapes());
		//filledShapes = !filledShapes;
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'o')
	{
		brush.changeShape(Shape::Shape::Rectangle);
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'p')
	{
		brush.changeShape(Shape::Shape::Triangle);
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'n'){
		saveImage(".png");
		//saveFboImage(firstFbo);
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
	else if (event.getChar() == 'v'){
		leapDrawFlag = !leapDrawFlag;
	}
	else if (event.getChar() == 's'){
		mySymmetry.toggleSymmetry();
	}
	else if (event.getChar() == 'a'){

		drawRadial();
	}
	else if (event.getChar() == 'g'){
		leapDrawFlag = true;
	}
#ifdef EYEX
	else if (event.getChar() == ' ')
	{

		if (gazePositionX <= resolutionX / 2 && gazePositionY <= resolutionY / 2)
		{
			brush.changeShape(Shape::Shape::Line);
		}
		else if (gazePositionX >= resolutionX / 2 && gazePositionY <= resolutionY / 2)
		{
			brush.changeShape(Shape::Shape::Circle);
		}
		else if (gazePositionX <= resolutionX / 2 && gazePositionY >= resolutionY / 2)
		{
			brush.changeShape(Shape::Shape::Rectangle);
		}
		else if (gazePositionX >= resolutionX / 2 && gazePositionY >= resolutionY / 2)
		{
			brush.changeShape(Shape::Shape::Triangle);
		}

	}
#endif
	else if (event.getChar() == 'l')
	{

		std::shared_ptr<gl::Fbo> temp1 = layerList[0];
		std::shared_ptr<gl::Fbo> temp2 = layerList[1];
		std::shared_ptr<gl::Fbo> temp3 = layerList[2];
		layerList[2] = temp2;
		layerList[1] = temp1;
		layerList[0] = temp3;


		/*
		if (currLayer == 0){
		currLayer = 1;
		}
		else if (currLayer == 1)
		{
		currLayer = 0;
		}
		*/
	}

}

bool TouchPointsApp::inInteractiveUi(int x, int y)
{
	if (radialActive){

		if ((((radialCenter.x - 100) - 30) < x && x < ((radialCenter.x - 100) + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30))){
			leapColorChange();
			return true;
		}

		if ((((radialCenter.x + 100) - 30) < x && x < ((radialCenter.x + 100) + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30))){
			leapShapeChange();
			return true;
		}

		if ((((radialCenter.x) - 30) < x && x < ((radialCenter.x) + 30)) && ((radialCenter.y - 100) - 30) < y && y < (radialCenter.y - 100) + 30){
			mySymmetry.toggleSymmetry();
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
				brush.changeFilledShapes(!brush.getFilledShapes());
				modeChangeFlag = true;
				return true;
			}
			else if (x < 200 && y < 50){
				brush.increaseLineSize();
				//else lineSize = 1;
				modeChangeFlag = true;
				return true;
			}
			else if (x < 250 && y < 50){
				brush.decreaseLineSize();
				//else lineSize = 15;
				modeChangeFlag = true;
				return true;
			}
			else if (x < 300 && y < 50){
				brush.decreaseAlpha();
				return true;
			}
			else if (x < 350 && y < 50){
				brush.increaseAlpha();
				return true;
			}
			else if (x < 400 && y < 50){
				mySymmetry.toggleSymmetry();
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
		for (int i = 0; i < COLOR_AMOUNT; i++){
			if (x < 50 && y < (50 * i + 100))
			{
				//currColor = i;
				brush.changeStaticColor(static_cast<ourColors::ourColors>(i));
				colorButtons = false;
				modeChangeFlag = true;
				return true;
			}
		}
	}
	if (layerVisualization){
		int yDist = layerList.size();
		yDist = layerList.size() * 200 + 50;
		int size = 0;
		for (auto frame : layerList){
			//gl::color(0.9, 0.85, 0.65);
			//gl::drawStrokedRect(Rectf(400, y - 200, 600, y), 10);

			if (x > 400 && x < 600)
			{
				if (y > yDist - 200 && y < yDist)
				{
					std::shared_ptr<gl::Fbo> temp = layerList[2];
					layerList[2] = layerList[size];
					layerList[size] = temp;
					return true;

				}
			}
			yDist = yDist - 200;
			size = size + 1;
		}
	}
	if (shapeButtons){
		if (x > 50 && x < 100 && y < 100){
			brush.changeShape(Shape::Shape::Line);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 150){
			brush.changeShape(Shape::Shape::Circle);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 200){
			brush.changeShape(Shape::Shape::Rectangle);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 250){
			brush.changeShape(Shape::Shape::Triangle);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
	}

	return false;
}

double lastTouch = 0;
float lastX = 0;
float lastY = 0;



bool TouchPointsApp::findMultiTouchGestures(TouchEvent::Touch previousPoint, TouchEvent::Touch currentPoint){

	//Checks for double tap
	//TouchEvent::Touch currentPoint = pointList[pointList.size() - 1];
	//TouchEvent::Touch previousPoint = pointList[pointList.size() - 2];
	//Detects double tap.
	if (0.25 >   currentPoint.getTime() - previousPoint.getTime())
	{
		if (previousPoint.getX() < currentPoint.getX() + 20 && previousPoint.getX() > currentPoint.getX() - 20)
		{
			if (previousPoint.getY() < currentPoint.getY() + 20 && previousPoint.getY() > currentPoint.getY() - 20){


				//(*firstFbo).bindFramebuffer();
				//gl::color(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
				//gl::drawSolidCircle(vec2(500, 500), 30, 10);
				//(*firstFbo).unbindFramebuffer();
				//uiFboFlag = !uiFboFlag;
				radialCenter = currentPoint.getPos();

				drawRadial();
				radialActive = true;
				bufferTouches.erase(previousPoint.getId());
				bufferTouches.erase(currentPoint.getId());
				return true;

			}
		}
	}
	else return false;

}


void TouchPointsApp::touchesBegan(TouchEvent event)
{
	for (const auto &touch : event.getTouches()) {
		
		if (radialActive){
			int x = touch.getX();
			int y = touch.getY();
			if ((((radialCenter.x - 100) - 30) < x && x < ((radialCenter.x - 100) + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30))){
				//brush.changeStaticColor();
				leapColorChange();
				continue;
			}

			if ((((radialCenter.x + 100) - 30) < x && x < ((radialCenter.x + 100) + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30))){
				//brush.cycleShape();
				leapShapeChange();
				continue;
			}

			if ((((radialCenter.x) - 30) < x && x < ((radialCenter.x) + 30)) && ((radialCenter.y - 100) - 30) < y && y < (radialCenter.y - 100) + 30){
				mySymmetry.toggleSymmetry();
				continue;
			}

			if ((((radialCenter.x) - 30) < x && x < ((radialCenter.x) + 30)) && ((radialCenter.y + 100) - 30) < y && y < (radialCenter.y + 100) + 30){
				uiFboFlag = !uiFboFlag;
				continue;
			}

			if ((radialCenter.x - 30) < x && x < ((radialCenter.x + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30))){
				radialActive = false;
				continue;
			}
		}
		if (ui.inInteractiveUi(touch.getX(), touch.getY())){

		}
		else {
			list<TouchEvent::Touch> tempList;
			tempList.push_back(touch);
			bufferTouches.emplace(touch.getId(), tempList);

			if (!findMultiTouchGestures(previousTouch, touch)) {
				//If we didn't find a gesture this time, save the point incase.
				previousTouch = touch;
			}
			else {
				//Clears the previous touch if we found a multitouch gesture.
				previousTouch.setPos(vec2(-100, -100));
			}
			//Tests for double tap....
			/*
			if (0.5 > touch.getTime() - lastTouch )
			{
			if (lastX < touch.getX() + 10 && lastX > touch.getX() - 10)
			{
			if (lastY < touch.getY() + 10 && lastY > touch.getY() - 10){


			(*firstFbo).bindFramebuffer();
			gl::color(CM_HSV, Rand::randFloat(), 0.5f, 1.0f);
			gl::drawSolidCircle(vec2(500, 500), 30, 10);
			(*firstFbo).unbindFramebuffer();
			continue;
			}
			}
			}

			lastTouch = touch.getTime();
			lastX = touch.getX();
			lastY = touch.getY();
			*/
			//beginTouchShapes(touch);
		}
		//if (inInteractiveUi(touch.getX(), touch.getY())){

		//}
		//else beginTouchShapes(touch.getId(), touch.getPos());


	}
}

void TouchPointsApp::touchesMoved(TouchEvent event)
{

	for (const auto &touch : event.getTouches()) {
		if (bufferTouches.find(touch.getId()) != bufferTouches.end() && touch.getPos() != touch.getPrevPos())
		{
			//bufferTouches[touch.getId()].push_back(touch);
			//list<TouchEvent::Touch> touchPoints = bufferTouches[touch.getId()];
			//if (touchPoints.back().getTime() - touchPoints.front().getTime() > .25){
			//beginTouchShapes(touchPoints.front().getId(), touchPoints.front().getPos());
			//for (auto points : touchPoints){
			//movingTouchShapes(points.getId(), points.getPos(), points.getPos());
			//}
			//bufferTouches.erase(touch.getId());
			//}
			//continue;
			illustrator.beginTouchShapes(touch.getId(), bufferTouches[touch.getId()].front().getPos());
			illustrator.movingTouchShapes(touch.getId(), touch.getPos(), touch.getPrevPos());
			bufferTouches.erase(touch.getId());

		}
		else illustrator.movingTouchShapes(touch.getId(), touch.getPos(), touch.getPrevPos());
	}
}

void TouchPointsApp::touchesEnded(TouchEvent event)
{
	for (const auto &touch : event.getTouches()) {

		illustrator.endTouchShapes(touch.getId());
	}
}

void TouchPointsApp::mouseDown(MouseEvent event)
{
}

void TouchPointsApp::update(){


	//bool testvar2 = System::hasMultiTouch();
	//auto testvar3 = System::getMaxMultiTouchPoints();

	if (deviceHandler.deviceConnection()){
		ui.setModeChangeFlag();
	}

	if (eyeXRunning){
		if (gazePositionX < 400 && gazePositionY < 100){
			modeButtons = true;
		}
		else modeButtons = false;
	}
}

void TouchPointsApp::draw()
{

	gl::enableAlphaBlending();

	//Add a vector instead of the 3 ref to arrays.
	//gl::clear(ColorA(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 0.0));
	//gl::clear(ColorA(ui.getBackgroundColor(), 0.0));
	//gl::color(1.0, 1.0, 1.0);
	//gl::drawStrokedCircle(vec2(windowWidth *.5, windowHeight*.5), 200.0f, 50.0f);
	Color myBG = ui.getBackgroundColor();
	glClearColor(myBG.r, myBG.g, myBG.b, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	gl::color(1.0, 1.0, 1.0, 1.0);
	for (auto frames : layerList){
		gl::draw(frames->getColorTexture());
	}
	currentFrame = getLeapFrame(leapContr);

	if (leapDrawFlag){
		leapDraw(currentFrame);
	}

	if (!lockCurrentFrame){
		//Calls specified action from gesture recgonized
		gestRecognition(currentFrame, leapContr);
	}

	lockCurrentFrame = false;

#ifdef EYEX
	gl::color(1.0, 1.0, 1.0, .4);
	vec2 gaze1(gazePositionX - 10, gazePositionY);
	vec2 gaze2(gazePositionX + 10, gazePositionY);

	gl::drawStrokedCircle(gaze1, 10.0f, 10.0f);
	gl::drawStrokedCircle(gaze2, 10.0f, 10.0f);

#endif


	//gl::draw(layerList[0]->getColorTexture());

	/*Draws the framebuffers for layer one and layer two*/
	/*
	if (currLayer == 0)
	{
	//Must always draw framebuffers as a set color!
	gl::color(1.0, 1.0, 1.0, 1.0);
	//Layer 1 is on top, second is below
	gl::draw(secondFbo->getColorTexture());
	gl::draw(firstFbo->getColorTexture());

	//gl::draw(uiFbo->getColorTexture());
	}
	else if (currLayer == 1)
	{
	gl::color(1.0, 1.0, 1.0,1.0);
	//Layer 2 is on top.
	gl::draw(firstFbo->getColorTexture());
	gl::draw(secondFbo->getColorTexture());
	//gl::draw(uiFbo->getColorTexture());

	}
	*/
	/*Draws image that provides feedback */
	if (imageFlag){
		//Area center = calcCenter(imageTexture);
		drawImageTexture();
		//gl::draw(imageFbo->getColorTexture(), center);
		gl::color(ColorA(0.0, 0.0, 0.0, 1.0));
		imageFbo->blitToScreen(Area(0, 0, 1920, 1080), Area(windowWidth / 2 - 100, windowHeight / 2 + 100, windowWidth / 2 + 55, windowHeight / 2 - 55), GL_NEAREST, GL_COLOR_BUFFER_BIT);
	}
	
	//drawUi();
	ui.drawUi();
	if (radialActive){
		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(radialFbo->getColorTexture());
	}


	gl::color(1.0, 1.0, 1.0, 1.0);

	/*Draws the frame buffer for UI*/
	if (eyeXRunning){


		if (gazePositionX > windowWidth*.8 && gazePositionY > windowHeight*.8)
		{
			gl::draw(uiFbo->getColorTexture());
		}
	}
	else if (ui.getUiFboFlag()) gl::draw(uiFbo->getColorTexture());

	gl::color(1.0, 1.0, 1.0, 1.0);
	(*activeFbo).bindFramebuffer();
	glClearColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//gl::clear(ui.getBackgroundColor(), 0.0);
	//Draws all active Shapes (non-permanent);
	illustrator.drawActiveShapes();

	(*activeFbo).unbindFramebuffer();

	gl::color(1.0, 1.0, 1.0, 1.0);
	gl::draw(activeFbo->getColorTexture());


}

CINDER_APP(TouchPointsApp, RendererGl, prepareSettings)
