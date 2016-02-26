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

//Device connection status
bool eyeXRunning = false;
bool leapRunning = false;

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

#define SHAPE_LINE "Line.png"
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

bool imageFlag = false;
bool leapDrawFlag = true;
bool processing = false;
gl::TextureRef imageTexture;
int imageNum;
float fadeTime = 1;


bool modeChangeFlag = true;

//Layers
int currLayer = 0;

float lineSize = 1.0f;

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
	enum Shape { Line, Circle, Rectangle, Triangle };


	//Drawing Functions for drawUi
	void	modeRectangle();
	void	modeCircle();
	void	modeTriangle();
	void	modeLine();

	//Function to change Shapes we are drawing
	void	changeShape(Shape x);

	//Ui Functions
	void	drawUi();
	bool	inInteractiveUi(int x, int y);


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
	void 	gestRecognition(Leap::Frame frame , Leap::Controller controller);


	//List of MODES
	bool randColor = false;
	bool eraserMode = false;
	bool lineDraw = true;
	bool circleDraw = false;
	bool rectDraw = false;
	bool triangleDraw = false;
	bool filledShapes = false;


	//List of drawUI Flags
	bool modeButtons = true;
	bool colorButtons = false;
	bool shapeButtons = false;
	bool uiFboFlag = false;


	void modeChangeFlagTrue();



#ifdef EYEX


#endif

	void update() override;
	void draw() override;


private:

	//Leap Motion Controller
	Leap::Controller leapContr;
	Leap::Frame currentFrame;
	Leap::GestureList gestList;
	LeapListener	myLeapListener;

	//EyeX


	TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
	TX_TICKET hEventHandlerTicket = TX_INVALID_TICKET;
	TX_TICKET hGazeTrackingStateChangedTicket = TX_INVALID_TICKET;
	TX_EYEXAVAILABILITY availability;
	BOOL success;
	TX_HANDLE hStateBag = TX_EMPTY_HANDLE;









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


void TouchPointsApp::modeChangeFlagTrue(){
	modeChangeFlag = true;
}

//Leapmotion listeners
void LeapListener::onConnect(const Leap::Controller& controller){
	leapRunning = true;

}
void LeapListener::onDisconnect(const Leap::Controller& controller){
	leapRunning = false;
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
	auto testvar2 = System::getMaxMultiTouchPoints();
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

	//Enable all Leap Gestures
	TouchPointsApp::enableGest(leapContr);

	leapContr.config().setFloat("Gesture.Swipe.MinLength", 150.0);
	leapContr.config().setFloat("Gesture.Swipe.MinVelocity", 500.0);
	leapContr.config().save();

	leapContr.addListener(myLeapListener);

	setFrameRate(FRAME_RATE);

	activeDrawings = 0;

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
	//Area center = Area::proportionalFit(window, image, true, false);
	Area center(vec2(800, 800), vec2(1100, 1100));
	return center;
}

void TouchPointsApp::enableGest(Leap::Controller controller){

	controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
	controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
	controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);

}

void TouchPointsApp::gestRecognition(Leap::Frame frame , Leap::Controller controller){
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
				//return 5;
				std::cout << "Screen Tap Gesture Found" << std::endl;
				break;
			}
			default:
				std::cout << std::string(2, ' ') << "Unknown gesture type." << std::endl;
				break;
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

void TouchPointsApp::leapShapeChange(){

	if (currShape != TOTAL_SYMBOLS - 1){
		currShape++;
	}
	else{
		currShape = 0;
	}

	switch (currShape){
	case 0:{
		changeShape(Line);
		loadImages(SHAPE_LINE);
		imageFlag = true;
		//Might not need this since changeShape has it
		//modeChangeFlag = true;
		break;
	}
	case 1:{
		filledShapes = false;
		changeShape(Circle);
		loadImages(SHAPE_Circle);
		imageFlag = true;
		//modeChangeFlag = true;
		break;
	}
	case 2:{
		filledShapes = false;
		changeShape(Rectangle);
		loadImages(SHAPE_Rectangle);
		imageFlag = true;
		//modeChangeFlag = true;
		break;
	}
	case 3:{
		filledShapes = false;
		changeShape(Triangle);
		loadImages(SHAPE_Triangle);
		imageFlag = true;
		//modeChangeFlag = true;
		break;
	}case 4:{
		filledShapes = true;
		changeShape(Circle);
		loadImages(SHAPE_Filled_Circle);
		imageFlag = true;
		//modeChangeFlag = true;
		break;
	}
	case 5:{
		filledShapes = true;
		changeShape(Rectangle);
		loadImages(SHAPE_Filled_Rectangle);
		imageFlag = true;
		//modeChangeFlag = true;
		break;
	}
	case 6:{
		filledShapes = true;
		changeShape(Triangle);
		loadImages(SHAPE_Filled_Triangle);
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

		//Provides correct image to provide feedback
		switch (currColor){
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
				   loadImages(COLOR_SIX);
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

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	gl::color(1.0, 1.0, 1.0, fadeTime);

	gl::draw(imageTexture);

	if (fadeTime <= 0.1){
		imageFlag = false;
		processing = false;
		fadeTime = 1;
	}

	fadeTime -= 0.009;


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

void TouchPointsApp::saveFboImage(std::shared_ptr<gl::Fbo> myFbo){

	writeImage(getHomeDirectory() / "cinder" / "Saved_Images" / (toString(imageNum) + "myPixels.png"), (myFbo->getColorTexture())->createSource());
}
/*Mode Change Functions*/
void TouchPointsApp::modeRectangle(){

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

void TouchPointsApp::changeShape(Shape x){

	if (activeDrawings != 0)
		return;
	if (x == Line){
		lineDraw = true;
		circleDraw = false;
		rectDraw = false;
		triangleDraw = false;
		modeChangeFlag = true;
	}
	else if (x == Circle){
		lineDraw = false;
		circleDraw = true;
		rectDraw = false;
		triangleDraw = false;
		modeChangeFlag = true;
	}
	else if (x == Rectangle)
	{
		lineDraw = false;
		circleDraw = false;
		rectDraw = true;
		triangleDraw = false;
		modeChangeFlag = true;
	}
	else if (x == Triangle){
		lineDraw = false;
		circleDraw = false;
		rectDraw = false;
		triangleDraw = true;
		modeChangeFlag = true;
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
		if (activeDrawings != 0)
			return;
		eraserMode = !eraserMode;

		modeChangeFlag = true;
	}
	else if (event.getChar() == 'c')	//Clear Screen (Broken with framebuffers).
	{


		(*firstFbo).bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*firstFbo).unbindFramebuffer();

		(*secondFbo).bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*secondFbo).unbindFramebuffer();

		(*uiFbo).bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		(*uiFbo).unbindFramebuffer();

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
		changeShape(Line);
	}
	else if (event.getChar() == 'i')
	{
		changeShape(Circle);
	}
	else if (event.getChar() == 'f')
	{
		filledShapes = !filledShapes;
		modeChangeFlag = true;
	}
	else if (event.getChar() == 'o')
	{
		changeShape(Rectangle);
	}
	else if (event.getChar() == 'p')
	{
		changeShape(Triangle);
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
	else if (event.getChar() == 'g'){
		leapDrawFlag = true;
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
		}
		else if (currLayer == 1)
		{
			currLayer = 0;
		}
	}

}

bool TouchPointsApp::inInteractiveUi(int x, int y)
{
	//modeButtons UI
	if (modeButtons){
		//Color change button.
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
			filledShapes = !filledShapes;
			modeChangeFlag = true;
			return true;
		}
		else if (x < 200 && y < 50){
			if (lineSize != 15.0f)
				lineSize++;
			//else lineSize = 1;
			modeChangeFlag = true;
		}
		else if (x < 250 && y < 50){
			if (lineSize != 1.0f)
				lineSize--;
			//else lineSize = 15;
			modeChangeFlag = true;
		}
	}

	//Color buttons UI
	if (colorButtons){
		/*
		if (x < 50 && y < 100){
			currColor = 1;
			colorButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x < 50 && y < 150){
			currColor = 2;
			colorButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x < 50 && y < 200){
			currColor = 3;
			colorButtons = false;
			modeChangeFlag = true;
			return true;
		}
		*/
		for (int i = 0; i < COLOR_AMOUNT; i++){
			if (x < 50 && y < (50 * i + 100))
			{
				currColor = i;
				colorButtons = false;
				modeChangeFlag = true;
				return true;
			}
		}
	}
	if (shapeButtons){
		if (x > 50 && x < 100 && y < 100){
			changeShape(Line);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 150){
			changeShape(Circle);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 &&y < 200){
			changeShape(Rectangle);
			shapeButtons = false;
			modeChangeFlag = true;
			return true;
		}
		if (x > 50 && x < 100 && y < 250){
			changeShape(Triangle);
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

void TouchPointsApp::touchesBegan(TouchEvent event)
{
	for (const auto &touch : event.getTouches()) {
		/*Tests for double tap....
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

		if (inInteractiveUi(touch.getX(), touch.getY())){

		}

		else if (eraserMode){
			activeDrawings++;
			Color newColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2]);
			myActivePoints.insert(make_pair(touch.getId(), TouchPoint(touch.getPos(), newColor, lineSize * 2)));
			bool tempBool = false;
			myActiveCircles.insert(make_pair(touch.getId(), TouchCircle(touch.getPos(), lineSize*2,Color(1.0,1.0,1.0), 1, tempBool)));
		}
		else if (lineDraw){
			activeDrawings++;
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
			activeDrawings++;
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
			activeDrawings++;
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
			activeDrawings++;
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

	for (const auto &touch : event.getTouches()) {

		if (eraserMode){
			if (myActivePoints.find(touch.getId()) == myActivePoints.end())
				return;


			missedPoints(touch.getPrevX(), touch.getPrevY(), touch.getX(), touch.getY(), myActivePoints[touch.getId()]);
			myPoints.push_back(myActivePoints[touch.getId()]);
			myActivePoints[touch.getId()].clearPoints();

			myActiveCircles[touch.getId()].changePosition(touch.getPos());

			if (currLayer == 0){


				(*firstFbo).bindFramebuffer();

				for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
					oldPoints->draw();
					++oldPoints;
				}

				(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
				(*secondFbo).bindFramebuffer();

				for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
					oldPoints->draw();
					++oldPoints;
				}

				(*secondFbo).unbindFramebuffer();
			}

			myPoints.clear();

			}
		else if (lineDraw){
			if (myActivePoints.find(touch.getId()) == myActivePoints.end())
				return;


				missedPoints(touch.getPrevX(), touch.getPrevY(), touch.getX(), touch.getY(), myActivePoints[touch.getId()]);
				myPoints.push_back(myActivePoints[touch.getId()]);
				myActivePoints[touch.getId()].clearPoints();

				if (currLayer == 0){


					(*firstFbo).bindFramebuffer();

					for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
						oldPoints->draw();
						++oldPoints;
					}

					(*firstFbo).unbindFramebuffer();
				}
				else if (currLayer == 1){
					(*secondFbo).bindFramebuffer();

					for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
						oldPoints->draw();
						++oldPoints;
					}

					(*secondFbo).unbindFramebuffer();
				}

				myPoints.clear();



		}
		else if (circleDraw){

			if (myActiveCircles.find(touch.getId()) == myActiveCircles.end())
				return;
				float a;
				float b;
				float c;
				a = touch.getPos().x - myActiveCircles[touch.getId()].getCenterX();
				b = touch.getPos().y - myActiveCircles[touch.getId()].getCenterY();
				a = a*a;
				b = b*b;
				c = sqrt(a + b);

				myActiveCircles[touch.getId()].changeRadius(c);

		}
		else if (rectDraw)
		{
			if (myActiveRectangles.find(touch.getId()) == myActiveRectangles.end())
				return;
				myActiveRectangles[touch.getId()].changePoint(touch.getPos().x, touch.getPos().y);



		}
		else if (triangleDraw)
		{
			if (myActiveTriangles.find(touch.getId()) == myActiveTriangles.end())
				return;
				myActiveTriangles[touch.getId()].changeSize(touch.getPos().x, touch.getPos().y);



		}
	}
}

void TouchPointsApp::touchesEnded(TouchEvent event)
{
	for (const auto &touch : event.getTouches()) {

		if (eraserMode){

			if (myActivePoints.find(touch.getId()) == myActivePoints.end())
				return;

			activeDrawings--;
			myActivePoints.erase(touch.getId());
			myActiveCircles.erase(touch.getId());
		}
		else if (lineDraw){

			if (myActivePoints.find(touch.getId()) == myActivePoints.end())
				return;

			activeDrawings--;
			myActivePoints.erase(touch.getId());

		}
		else if (circleDraw){

				if (myActiveCircles.find(touch.getId()) == myActiveCircles.end())
					return;

				activeDrawings--;

				myCircles.push_back(myActiveCircles[touch.getId()]);
				myActiveCircles.erase(touch.getId());

				if (currLayer == 0){


					(*firstFbo).bindFramebuffer();
					for (auto oldPoints = myCircles.begin(); oldPoints != myCircles.end();) {
						oldPoints->draw();
						++oldPoints;
					}

					(*firstFbo).unbindFramebuffer();
				}
				else if (currLayer == 1){
					(*secondFbo).bindFramebuffer();

					for (auto oldPoints = myCircles.begin(); oldPoints != myCircles.end();) {
						oldPoints->draw();
						++oldPoints;
					}

					(*secondFbo).unbindFramebuffer();
				}

				myCircles.clear();

		}
		else if (rectDraw){

			if (myActiveRectangles.find(touch.getId()) == myActiveRectangles.end())
				return;

			activeDrawings--;

			myRectangles.push_back(myActiveRectangles[touch.getId()]);
			myActiveRectangles.erase(touch.getId());
			if (currLayer == 0){


				(*firstFbo).bindFramebuffer();
				for (auto oldPoints = myRectangles.begin(); oldPoints != myRectangles.end();) {
					oldPoints->draw();
					++oldPoints;
				}

				(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
				(*secondFbo).bindFramebuffer();

				for (auto oldPoints = myRectangles.begin(); oldPoints != myRectangles.end();) {
					oldPoints->draw();
					++oldPoints;
				}

				(*secondFbo).unbindFramebuffer();
			}

			myRectangles.clear();
		}
		else if (triangleDraw){

			if (myActiveTriangles.find(touch.getId()) == myActiveTriangles.end())
				return;

			activeDrawings--;

			myTriangles.push_back(myActiveTriangles[touch.getId()]);
			myActiveTriangles.erase(touch.getId());
			if (currLayer == 0){


				(*firstFbo).bindFramebuffer();

				for (auto oldPoints = myTriangles.begin(); oldPoints != myTriangles.end();) {
					oldPoints->draw();
					++oldPoints;
				}

				(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
				(*secondFbo).bindFramebuffer();

				for (auto oldPoints = myTriangles.begin(); oldPoints != myTriangles.end();) {
					oldPoints->draw();
					++oldPoints;
				}

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

	//if (gazePositionX < 75 && gazePositionY < 120){
		modeButtons = true;
	//}
	//else modeButtons = false;

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

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		gl::color(1.0, 0.9, 0.5);
		//gl::lineWidth(10);
		//gl::drawSolidCircle(vec2(0,0), 10);
		//gl::drawSolidCircle(vec2(1920,1080), 100);

		gl::drawStrokedRect(Rectf(windowWidth*.8, windowHeight*.8, windowWidth, windowHeight), 5);
		gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(windowWidth*.8, windowHeight*.8, windowWidth, windowHeight));
		if (eraserMode){
			bool tempBool = false;
			TouchCircle(vec2(windowWidth*.9, windowHeight *.9), lineSize * 2, Color(1.0, 1.0, 1.0), 1, tempBool).draw();
		}
		else if (rectDraw) modeRectangle();
		else if (circleDraw) modeCircle();
		else if (triangleDraw) modeTriangle();
		else if (lineDraw) modeLine();

		if (System::hasMultiTouch()){
			gl::color(0.0, 0.0, 1.0);
			gl::drawSolidRect(Rectf(windowWidth*.81, windowHeight * .81, windowWidth*.83, windowHeight *.83));
		}
		if (leapRunning){
			gl::color(0.0, 1.0, 0.0);
			gl::drawSolidRect(Rectf(windowWidth*.84, windowHeight * .81, windowWidth*.86, windowHeight *.83));
		}
		if (eyeXRunning){
			gl::color(1.0, 0.0, 0.0);
			gl::drawSolidRect(Rectf(windowWidth*.87, windowHeight * .81, windowWidth*.89, windowHeight *.83));
		}
		(*uiFbo).unbindFramebuffer();
		uiFboFlag = false;
	}
	if (modeButtons){
		//Color Button
		gl::color(0.9, 0.85, 0.65);
		gl::drawStrokedRect(Rectf(0, 2, 100, 50), 10);
		gl::color(1.0, 1.0, 0);
		gl::drawSolidRect(Rectf(0, 0, 20, 50));
		gl::color(0.0, 1.0, 0.0);
		gl::drawSolidRect(Rectf(20, 0, 35, 50));
		gl::color(1.0, 0.0, 1.0);
		gl::drawSolidRect(Rectf(35, 0, 50, 50));

		//Shapes button
		gl::color(0.9, 0.85, 0.65);
		gl::drawStrokedRect(Rectf(50, 2, 100, 50), 10);
		gl::color(0.0, 1.0, 1.0);
		gl::drawSolidRect(Rectf(50, 0, 100, 50));

		//FilledShapes button
		gl::color(0.9, 0.85, 0.65);
		gl::drawStrokedRect(Rectf(100, 2, 150, 50), 10);
		gl::color(1.0, 1.0, 1.0);
		gl::drawSolidRect(Rectf(100, 0, 150, 50));

		//Line Size
		gl::color(0.9, 0.85, 0.65);
		gl::drawStrokedRect(Rectf(150, 2, 200, 50), 10);
		gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(150, 0, 200, 50));

		gl::color(1.0, 1.0, 1.0);
		gl::lineWidth(5);
		gl::drawLine(vec2(160, 25), vec2(190, 25));
		gl::drawLine(vec2(175, 10), vec2(175, 40));

		gl::color(0.9, 0.85, 0.65);
		gl::drawStrokedRect(Rectf(200, 2, 250, 50), 10);
		gl::color(0.0, 0.0, 0.0);
		gl::drawSolidRect(Rectf(200, 0, 250, 50));

		gl::color(1.0, 1.0, 1.0);
		//gl::lineWidth(lineSize);
		gl::drawLine(vec2(210, 25), vec2(240, 25));
		//gl::drawLine(vec2(225, 10), vec2(225, 40));

	}
	if (colorButtons){
		/*
		gl::color(colorArray[1][0], colorArray[1][1], colorArray[1][2]);
		gl::drawSolidRect(Rectf(0, 50, 50, 100));
		gl::color(colorArray[2][0], colorArray[2][1], colorArray[2][2]);
		gl::drawSolidRect(Rectf(0, 100, 50, 150));
		gl::color(colorArray[3][0], colorArray[3][1], colorArray[3][2]);
		gl::drawSolidRect(Rectf(0, 150, 50, 200));
		gl::color(colorArray[4][0], colorArray[4][1], colorArray[4][2]);
		gl::drawSolidRect(Rectf(0, 150, 50, 200));
		gl::color(colorArray[5][0], colorArray[5][1], colorArray[5][2]);
		gl::drawSolidRect(Rectf(0, 150, 50, 200));
		gl::color(colorArray[6][0], colorArray[6][1], colorArray[6][2]);
		gl::drawSolidRect(Rectf(0, 150, 50, 200));
		*/

		for (int i = 0; i < COLOR_AMOUNT; i++)
		{
			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(0, 50*(i) + 50 , 50, 50 * i + 100), 10);
			gl::color(colorArray[i][0], colorArray[i][1], colorArray[i][2]);
			gl::drawSolidRect(Rectf(0, 50 * (i)+50, 50, 50 * (i)+100));
		}
	}
	if (shapeButtons){

		for (int i = 0; i < 4; i++){

			gl::color(0.9, 0.85, 0.65);
			gl::drawStrokedRect(Rectf(50, 50 * (i)+50, 100, 50 * i + 100), 10);
			gl::color(0.0,0.0,0.0);
			gl::drawSolidRect(Rectf(50, 50 * i + 50, 100, 50 * i + 100));
		}

		gl::color(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2]);
		gl::lineWidth(2);
		gl::drawLine(vec2(55, 95), vec2(95, 55));

		if (filledShapes)
			gl::drawSolidCircle(vec2(75, 125), 15);
		else gl::drawStrokedCircle(vec2(75, 125), 15);

		if (filledShapes)
			gl::drawSolidRect(Rectf(60, 188, 90, 160));
		else gl::drawStrokedRect(Rectf(60, 188, 90, 160));


		if (filledShapes)
			gl::drawSolidTriangle(vec2(55, 245), vec2(95, 245), vec2(73, 205));
		else {

			gl::drawLine(vec2(55, 245), vec2(95, 245));
			gl::drawLine(vec2(95, 245), vec2(73, 205));
			gl::drawLine(vec2(73, 205), vec2(55, 245));
		}
		/*
		gl::color(colorArray[1][0], colorArray[1][1], colorArray[1][2]);
		gl::drawSolidRect(Rectf(50, 50, 100, 100));
		gl::color(colorArray[2][0], colorArray[2][1], colorArray[2][2]);
		gl::drawSolidRect(Rectf(50, 100, 100, 150));
		gl::color(colorArray[3][0], colorArray[3][1], colorArray[3][2]);
		gl::drawSolidRect(Rectf(50, 150, 100, 200));
		gl::color(colorArray[4][0], colorArray[4][1], colorArray[5][2]);
		gl::drawSolidRect(Rectf(50, 200, 100, 250));
		*/
	}
}

void TouchPointsApp::draw()
{
	gl::enableAlphaBlending();
	//Add a vector instead of the 3 ref to arrays.
	gl::clear(Color(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2]));

	currentFrame = getLeapFrame(leapContr);

	if (leapDrawFlag){
		leapDraw(currentFrame);
	}

#ifdef EYEX
	gl::color(1.0, 1.0, 1.0, .4);
	vec2 gaze1(gazePositionX - 10, gazePositionY);
	vec2 gaze2(gazePositionX + 10, gazePositionY);

	gl::drawStrokedCircle(gaze1, 10.0f, 10.0f);
	gl::drawStrokedCircle(gaze2, 10.0f, 10.0f);

#endif

	/*Draws the framebuffers for layer one and layer two*/
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

	//Calls specified action from gesture recgonized
	gestRecognition(currentFrame, leapContr);

	/*Draws image that provides feedback */
	if (imageFlag){
		Area center = calcCenter(imageTexture);
		drawImageTexture();
		gl::draw(imageFbo->getColorTexture(), center);
	}

	drawUi();
	gl::color(1.0, 1.0, 1.0);

	/*Draws the frame buffer for UI*/
	//if (gazePositionX > 1920 && gazePositionY > 1080)
	{
		gl::draw(uiFbo->getColorTexture());
	}

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


}

CINDER_APP(TouchPointsApp, RendererGl, prepareSettings)
