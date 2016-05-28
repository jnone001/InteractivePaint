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
#include <chrono>

//Standard Library Includes
#include <vector>
#include <map>
#include <list>

//Our own includes
#include "SymmetryLine.h"
#include "Illustrator.h"
#include "Enums.h"
#include "Brush.h"
#include "UserInterface.h"
#include "ImageHandler.h"
#include "RealSenseHandler.h"
#include "libusb.h"
#include <stdio.h>
#include "DeviceHandler.h"

//Leap Includes
#include "Leap.h"
#include "LeapMath.h"

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
#define BACKGROUND_COLORS 8
#define TOTAL_SYMBOLS 7
#define TOTAL_SHAPES 7
#define SWIPE_GESTURE 8
#define windowWidth  getWindowSize().x
#define windowHeight getWindowSize().y
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

/*Code for Proximity Menu*/
bool proxActive = false;

bool modeChangeFlag = true;

//Layers
int currLayer = 0;

float lineSize = 1.0f;
float currentAlpha = 1.0;

float r, g, b = 256.0f;

int currColor = 0;
int currShape = 0;
int currBackground = 0;

string symbolArray[TOTAL_SYMBOLS];

//Global EyeX Handler
TX_CONTEXTHANDLE hContext = TX_EMPTY_HANDLE;

class LeapListener : public Leap::Listener
{
public:
	virtual void onConnect(const Leap::Controller&);
	//virtual void onFrame(const Leap::Controller&);
	virtual void onDisconnect(const Leap::Controller&);
};

//TouchPointsApp, our libcinder app!
class TouchPointsApp : public App
{
public:
	void setup() override;

	void mouseDown(MouseEvent event) override;

	void touchesBegan(TouchEvent event) override;
	void touchesMoved(TouchEvent event) override;
	void touchesEnded(TouchEvent event) override;
	void keyDown(KeyEvent event) override;

	//Drawing Functions for drawUi
	void modeRectangle();
	void modeCircle();
	void modeTriangle();
	void modeLine();

	//Functions to alter our alpha variable.
	void increaseAlpha();
	void decreaseAlpha();

	//Ui Functions
	void drawUi();
	bool inInteractiveUi(int x, int y, uint32_t id);
	void drawRadial();

	//Proxyfucntions
	void drawProx();

	/*Leap related functions*/
	void enableGest(Leap::Controller controller);
	void leapSave();
	void leapColorChange();
	void leapShapeChange();
	void leapDraw(Leap::Frame frame);
	void gestRecognition(Leap::Frame frame, Leap::Controller controller);
	void setDefaultMode(Mode::DefaultModes mode);

	//List of drawUI Flags
	bool modeButtons = true;
	bool colorButtons = false;
	bool shapeButtons = false;
	bool uiFboFlag = false;
	bool layerVisualization = false;
	bool startUpFlag = true;

	//Variables to keep track of performance. It is cleared once every second.
	int frames = 0;
	int fps = 0;

	void modeChangeFlagTrue();

	void update() override;
	void draw() override;

private:
	//Setup
	bool setupComplete = false;
	//Needs organizing
	Brush brush;
	Illustrator illustrator;
	UserInterface ui;
	DeviceHandler deviceHandler;
	ImageHandler imageHandler;
	RealSenseHandler realSenseHandler;
	//UndoLine undoLine;

	//Keeps time for the last time we checked for connected or disconnected devices
	std::chrono::milliseconds lastDeviceCheck;

	//Leap Motion Controller
	Leap::Controller leapContr;
	Leap::Frame currentFrame;
	Leap::GestureList gestList;
	LeapListener myLeapListener;

	//EyeX
	vec2 radialCenter = vec2(windowWidth * .5, windowHeight * .5);

	TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
	TX_TICKET hEventHandlerTicket = TX_INVALID_TICKET;
	TX_TICKET hGazeTrackingStateChangedTicket = TX_INVALID_TICKET;
	TX_EYEXAVAILABILITY availability;
	BOOL success;
	TX_HANDLE hStateBag = TX_EMPTY_HANDLE;

	//Symmetry lines
	SymmetryLine mySymmetry;

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

	list<TouchShape*> myShapes;
	int activeDrawings;

	map<uint32_t, TouchPoint> myActivePoints;
	list<TouchPoint> myPoints;
	map<uint32_t, TouchPoint> myActivePointsEraser;
	list<TouchPoint> myPointsEraser;
	map<uint32_t, TouchCircle> myActiveCirclesEraser;
	list<TouchCircle> myCirclesEraser;
	map<uint32_t, TouchCircle> myActiveCircles;
	list<TouchCircle> myCircles;
	map<uint32_t, TouchRectangle> myActiveRectangles;
	list<TouchRectangle> myRectangles;
	map<uint32_t, TouchTriangle> myActiveTriangles;
	list<TouchTriangle> myTriangles;

	//Fbo's for Layering.
	//We may want to set a vector of framebuffers?
	//This way we can dynamically have multiple layers....????
	std::shared_ptr<gl::Fbo> firstFbo;
	std::shared_ptr<gl::Fbo> secondFbo;
	std::shared_ptr<gl::Fbo> thirdFbo;
	std::shared_ptr<gl::Fbo> activeFbo;
	std::shared_ptr<gl::Fbo> iconFbo;
	std::shared_ptr<gl::Fbo> saveImageFbo;

	std::shared_ptr<gl::Fbo> backgroundFbo;
	std::vector<std::shared_ptr<gl::Fbo>> layerList;
	std::vector<float> layerAlpha;

	//Fbo for UserInterface
	std::shared_ptr<gl::Fbo> uiFbo;
	std::shared_ptr<gl::Fbo> imageFbo;
	std::shared_ptr<gl::Fbo> radialFbo;
	//Proxy Menu Fbo
	std::shared_ptr<gl::Fbo> proxFbo;
	std::shared_ptr<gl::Fbo> fingerLocationFbo;
};

/*Retrieves frames from leap motion Service*/
Leap::Frame getLeapFrame(Leap::Controller controller)
{
	return controller.frame();
}

void TouchPointsApp::modeChangeFlagTrue()
{
	modeChangeFlag = true;
}

//Leapmotion listeners
void LeapListener::onConnect(const Leap::Controller& controller)
{
	leapRunning = true;
	modeChangeFlag = true;
}

void LeapListener::onDisconnect(const Leap::Controller& controller)
{
	leapRunning = false;
	modeChangeFlag = true;
}

void prepareSettings(TouchPointsApp::Settings* settings)
{
	settings->setFullScreen(true);

	settings->setTitle("InteractivePaint");
	//setFullScreen(1);

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
	if (success)
	{
		switch (eyeTrackingState)
		{
			case TX_EYETRACKINGDEVICESTATUS_TRACKING:
				eyeXRunning = true;
				modeChangeFlag = true;
				break;
			case TX_EYETRACKINGDEVICESTATUS_DEVICENOTCONNECTED:
				eyeXRunning = false;
				modeChangeFlag = true;
				break;
		}
	}
}

void TX_CALLCONVENTION OnEngineStateChanged(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
{
	TX_RESULT result = TX_RESULT_UNKNOWN;
	TX_HANDLE hStateBag = TX_EMPTY_HANDLE;

	if (txGetAsyncDataResultCode(hAsyncData, &result) == TX_RESULT_OK &&
		txGetAsyncDataContent(hAsyncData, &hStateBag) == TX_RESULT_OK)
	{
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
	TX_GAZEPOINTDATAPARAMS params = {TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED};
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
	switch (connectionState)
	{
		case TX_CONNECTIONSTATE_CONNECTED:
			{
				BOOL success;
				success = txCommitSnapshotAsync(g_hGlobalInteractorSnapshot, OnSnapshotCommitted, NULL) == TX_RESULT_OK;
				txGetStateAsync(hContext, TX_STATEPATH_EYETRACKING, OnEngineStateChanged, NULL);
			}
			break;
		case TX_CONNECTIONSTATE_DISCONNECTED:
			eyeXRunning = false;
			break;
	}
}

/*
* Handles an event from the Gaze Point data stream.
*/
void OnGazeDataEvent(TX_HANDLE hGazeDataBehavior)
{
	TX_GAZEPOINTDATAEVENTPARAMS eventParams;
	if (txGetGazePointDataEventParams(hGazeDataBehavior, &eventParams) == TX_RESULT_OK)
	{
		gazePositionX = eventParams.X;
		gazePositionY = eventParams.Y;
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
	if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK)
	{
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
	auto testvar1 = System::hasMultiTouch();
	int32_t testvar2 = System::getMaxMultiTouchPoints();

	CI_LOG_I("MT: " << System::hasMultiTouch() << " Max points: " << System::getMaxMultiTouchPoints());
	glEnable(GL_LINE_SMOOTH);

	//Sets window size and initializes framebuffers (layers).
	setWindowSize(windowWidth, windowHeight);
	gl::Fbo::Format format;
	firstFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	secondFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	thirdFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	activeFbo = gl::Fbo::create(windowWidth, windowHeight, format);

	iconFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	saveImageFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	//Set up UI
	uiFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	//Set up image feedback fbo
	imageFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	//Set up fbo for proxy menu
	radialFbo = gl::Fbo::create(windowWidth, windowHeight, format);
	//Set up fbo for proxy menu
	proxFbo = gl::Fbo::create(windowWidth, windowHeight, format);

	//Background FBO Testing
	backgroundFbo = gl::Fbo::create(windowWidth, windowHeight, format);

	//Enable all Leap Gestures
	TouchPointsApp::enableGest(leapContr);

	//Set up fingerlocation FBo
	fingerLocationFbo = gl::Fbo::create(windowWidth, windowHeight, format);

	leapContr.config().setFloat("Gesture.Swipe.MinLength", 150.0);
	leapContr.config().setFloat("Gesture.Swipe.MinVelocity", 500.0);
	leapContr.config().save();

	leapContr.addListener(myLeapListener);

	setFrameRate(FRAME_RATE);

	mySymmetry = SymmetryLine(windowWidth / 2, true);

	//Sets up layers
	layerList.emplace_back(firstFbo);
	layerList.emplace_back(secondFbo);
	layerList.emplace_back(thirdFbo);

	//Set up Brush
	bool tempFalse = false;
	bool tempTrue = true;
	ColorA newColor = ColorA(0.0f, 0.0f, 0.0f, 1.0f);
	float tempFloat = 1.0f;
	int tempInt = 1;
	Shape::Shape myShape = Shape::Shape::Line;
	brush = Brush(myShape, newColor, tempFloat, tempInt, tempFalse, tempFalse, tempFalse, &mySymmetry);
	illustrator = Illustrator(&brush, &layerList);
	deviceHandler = DeviceHandler();
	cinder::getHomeDirectory();
	imageHandler = ImageHandler(&layerList, &layerAlpha);

	//RealSense Setup
	realSenseHandler = RealSenseHandler(&illustrator);

	//Set up UI
	deviceHandler.deviceConnection();
	lastDeviceCheck = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	ui = UserInterface(windowWidth, windowHeight, leapRunning, eyeXRunning, &brush, &illustrator, &deviceHandler, uiFbo, &layerList, &layerAlpha);

	deviceHandler.deviceConnection();
	Mode::DefaultModes resultMode = deviceHandler.getDefaultMode();
	setDefaultMode(resultMode);

	//Sets up eyeX context
#ifdef EYEX
	//Get desktop resolution
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	resolutionX = desktop.right;
	resolutionY = desktop.bottom;

	// initialize and enable the context that is our link to the EyeX Engine.
	success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
	success &= txCreateContext(&hContext, TX_FALSE) == TX_RESULT_OK;
	success &= InitializeGlobalInteractorSnapshot(hContext);
	success &= txRegisterConnectionStateChangedHandler(hContext, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
	success &= txRegisterEventHandler(hContext, &hEventHandlerTicket, HandleEvent, NULL) == TX_RESULT_OK;
	success &= txEnableConnection(hContext) == TX_RESULT_OK;
	success &= txRegisterStateChangedHandler(hContext, &hGazeTrackingStateChangedTicket, TX_STATEPATH_GAZETRACKING, OnEngineStateChanged, NULL) == TX_RESULT_OK;
#endif
}

static Area calcCenter(gl::TextureRef imageTexture)
{
	Area image = imageTexture->getBounds();
	Area window = getWindowBounds();
	Area center = Area::proportionalFit(window, image, true, false);
	//Area center(vec2(800, 800), vec2(1100, 1100));
	return center;
}

void TouchPointsApp::enableGest(Leap::Controller controller)
{
	controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
	controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
	controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
}

void TouchPointsApp::gestRecognition(Leap::Frame frame, Leap::Controller controller)
{
	if (!drawing)
	{
		//List of all gestures
		gestList = frame.gestures();
		//Process gestures...
		if (!imageHandler.getIconFlag())
		{
			//for (int g = 0; g < gestures.count(); ++g)
			for (Leap::Gesture gesture : gestList)
			{
				//Gesture gesture = gestList[g];

				switch (gesture.type())
				{
					case Leap::Gesture::TYPE_CIRCLE:
						{
							Leap::CircleGesture circle = gesture;

							if (circle.pointable().direction().angleTo(circle.normal()) <= M_PI / 2)
							{
								//clockwise circle

								Leap::Vector position = circle.pointable().tipPosition();

								if (position.x < 0 && position.y > 250)
								{
									processing = true;
									leapColorChange();
								}

								if (position.x < 0 && position.y < 150)
								{
									processing = true;
									leapShapeChange();
								}

								if (position.x > 0 && position.y > 250)
								{
									processing = true;
									leapSave();
								}

								if (position.x > 0 && position.y < 150)
								{
									proxActive = false;
								}
							}
							else
							{
								//counterclockwise circle
								drawProx();
							}

							// Calculate angle swept since last frame

							float sweptAngle = 0;
							if (circle.state() != Leap::Gesture::STATE_START)
							{
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

void TouchPointsApp::leapSave()
{
	if (ui.isBackgroundTransparent())
	{
		imageHandler.saveCanvas(vec2(windowWidth, windowHeight), ColorA(ui.getBackgroundColor(), 0.0));
	}
	else imageHandler.saveCanvas(vec2(windowWidth, windowHeight), ColorA(ui.getBackgroundColor(), 1.0));
}

void TouchPointsApp::leapDraw(Leap::Frame frame)
{
	//Get all pointables from current leap frame
	Leap::PointableList pointables = frame.pointables();
	//Gets a virtual rectangular prism which is within the field of vie of Leap
	Leap::InteractionBox iBox = frame.interactionBox();

	//Traverse all pointables
	for (auto& points : pointables)
	{
		//Normalize points from iBox
		Leap::Vector normalizedPosition = iBox.normalizePoint(points.stabilizedTipPosition());
		//Get x and y coordinate value form normalized value within given window
		float leapXCoordinate = normalizedPosition.x * windowWidth;
		float leapYCoordinate = windowHeight - normalizedPosition.y * windowHeight;

		if (points.touchDistance() > 0 && points.touchZone() != Leap::Pointable::Zone::ZONE_NONE)
		{
			gl::color(0, 1, 0, 1 - points.touchDistance());
			gl::drawSolidCircle(vec2(leapXCoordinate, leapYCoordinate), 40);
			gl::color(1.0, 0.9, 0.5, 1 - points.touchDistance());
			gl::drawStrokedCircle(vec2(leapXCoordinate, leapYCoordinate), 40.0f, 10.0f);
			/*LEAP DRAW ALL SHAPES CODE. NOT READY FOR IMPLEMENTATION*/
			if (pointsMap.find(points.id()) != pointsMap.end())
			{
				pointsMap.erase(points.id());

				activePointsMap.erase(points.id());
				illustrator.endTouchShapes(points.id());
			}
		}
		else if (points.touchDistance() <= 0 && !proxActive)
		{
			lockCurrentFrame = true;

			//Check to see if id for pointable object is present
			auto result = pointsMap.find(points.id());
			//Checks to see if new pointable is drawing
			if (result == pointsMap.end())
			{
				/*LEAP DRAW ALL SHAPES CODE. NOT READY FOR IMPLEMENTATION*/
				illustrator.beginTouchShapes(points.id(), vec2(leapXCoordinate, leapYCoordinate));

				pointsMap.emplace(points.id(), vec2(leapXCoordinate, leapYCoordinate));
				activePointsMap.emplace(points.id(), true);
			}
			else
			{
				/*LEAP DRAW ALL SHAPES CODE. NOT READY FOR IMPLEMENTATION*/
				illustrator.movingTouchShapes(points.id(), vec2(leapXCoordinate, leapYCoordinate), pointsMap[points.id()]);
				activePointsMap[points.id()] = true;
				pointsMap[points.id()] = vec2(leapXCoordinate, leapYCoordinate);
			}
		}
	}
	std::vector<uint32_t> list;
	for (auto& points : activePointsMap)
	{
		if (points.second)
		{
			points.second = false;
		}
		else
		{
			illustrator.endTouchShapes(points.first);
			list.emplace_back(points.first);
			pointsMap.erase(points.first);
		}
	}
	for (auto ids : list)
	{
		activePointsMap.erase(ids);
	}
}

void TouchPointsApp::leapShapeChange()
{
	if (currShape != TOTAL_SYMBOLS - 1)
	{
		currShape++;
	}
	else
	{
		currShape = 0;
	}
	brush.incrementShape();
	ui.setModeChangeFlag();
	switch (brush.getCurrentShape())
	{
		case 0:
			{
				imageHandler.loadIcon(SHAPE_LINE);

				break;
			}
		case 1:
			{
				bool tempBool = false;

				if (!brush.getFilledShapes()) imageHandler.loadIcon(SHAPE_Circle);
				else imageHandler.loadIcon(SHAPE_Filled_Circle);
				break;
			}
		case 2:
			{
				if (!brush.getFilledShapes()) imageHandler.loadIcon(SHAPE_Rectangle);
				else imageHandler.loadIcon(SHAPE_Filled_Rectangle);
				imageFlag = true;
				break;
			}
		case 3:
			{
				if (!brush.getFilledShapes()) imageHandler.loadIcon(SHAPE_Triangle);
				else imageHandler.loadIcon(SHAPE_Filled_Triangle);
				imageFlag = true;
				break;
			}
		default:
			{
				//std::cout << std::string(2, ' ') << "Unknown gesture type." << std::endl;
				break;
			}
	}
}

void TouchPointsApp::leapColorChange()
{
	brush.incrementColor();
	ui.setModeChangeFlag();
	//Provides correct image to provide feedback
	switch (brush.getCurrentColor())
	{
		case 0:
			{
				imageHandler.loadIcon(COLOR_ZERO);

				break;
			}
		case 1:
			{
				imageHandler.loadIcon(COLOR_ONE);

				break;
			}
		case 2:
			{
				imageHandler.loadIcon(COLOR_TWO);

				break;
			}
		case 3:
			{
				imageHandler.loadIcon(COLOR_THREE);

				break;
			}
		case 4:
			{
				imageHandler.loadIcon(COLOR_FOUR);
				break;
			}
		case 5:
			{
				imageHandler.loadIcon(COLOR_FIVE);

				break;
			}
		case 6:
			{
				imageHandler.loadIcon(COLOR_SIX);
				break;
			}
		case 7:
			{
				imageHandler.loadIcon(COLOR_SEVEN);
				break;
			}
		default:
			{
				break;
			}
	}
}

void TouchPointsApp::drawRadial()
{
	gl::color(1.0, 1.0, 1.0, 1.0);
	(*radialFbo).bindFramebuffer();
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	(*radialFbo).unbindFramebuffer();

	//Draw to radial menu buffer
	(*radialFbo).bindFramebuffer();
	//Make background transparent
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//Center Circle
	gl::color(1.0, 0.9, 0.5);
	gl::drawStrokedCircle(radialCenter, 30.0f, 2.0f);

	gl::color(.24, .24, .24, 0.8);

	gl::drawSolidCircle(radialCenter, 29.0f);

	//Draws outer ring
	gl::color(.24, .24, .24, 0.8);
	gl::drawStrokedCircle(radialCenter, 100.0f, 60.0f);

	gl::color(0.0, 0.0, 0.0, 0.5);
	gl::drawStrokedCircle(radialCenter, 131.0f, 2.0f, 300);

	gl::color(0.0, 0.0, 0.0, 0.8);
	gl::drawStrokedCircle(radialCenter, 69.0f, 2.0f, 300);

	//Draw left icon
	gl::color(1.0, 1.0, 1.0, 1.0);

	(*radialFbo).bindFramebuffer();

	//New way to draw icons to FBO.
	gl::TextureRef texture = gl::Texture::create(loadImage(loadAsset("Colors.png")));

	gl::color(1.0, 1.0, 1.0, 1.0);

	gl::draw(texture, Rectf(radialCenter.x - 125, radialCenter.y - 25, radialCenter.x - 75, radialCenter.y + 25));

	texture = gl::Texture::create(loadImage(loadAsset("Shapes.png")));

	gl::color(1.0, 1.0, 1.0, 1.0);

	gl::draw(texture, Rectf(radialCenter.x + 75, radialCenter.y - 25, radialCenter.x + 125, radialCenter.y + 25));

	//Draw Upper Icon
	texture = gl::Texture::create(loadImage(loadAsset("Undo.png")));

	gl::color(1.0, 1.0, 1.0, 1.0);

	gl::draw(texture, Rectf(radialCenter.x - 25, radialCenter.y - 130, radialCenter.x + 25, radialCenter.y - 70));

	//Draw Lower Icon
	gl::color(1.0, 0.9, 0.5);
	gl::drawStrokedCircle(vec2(radialCenter.x, radialCenter.y + 100), 30.0f, 2.0f);

	gl::color(1.0, 1.0, 1.0);
	gl::drawSolidCircle(vec2(radialCenter.x, radialCenter.y + 100), 29.0f);

	(*radialFbo).unbindFramebuffer();

	radialActive = true;
}

void TouchPointsApp::drawProx()
{
	(*proxFbo).bindFramebuffer();
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	(*proxFbo).unbindFramebuffer();

	//Draw to radial menu buffer
	(*proxFbo).bindFramebuffer();
	//Make background transparent
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	gl::lineWidth(10);
	gl::color(0.0, 0.0, 0.0, 1.0);
	gl::drawLine(vec2(windowWidth * .5, windowHeight), vec2(windowWidth * .5, 0));

	gl::drawLine(vec2(0, windowHeight * .5), vec2(windowWidth, windowHeight * .5));

	(*proxFbo).unbindFramebuffer();

	proxActive = true;
}

/*Mode Change Functions*/
void TouchPointsApp::keyDown(KeyEvent event)
{
	if (event.getChar() == 'z')
	{
		// Toggle full screen when the user presses the 'f' key.
		if (lineSize != 1.0f) lineSize--;

		brush.decreaseLineSize();
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'x')
	{
		// Clear the list of points when the user presses the space bar.
		if (lineSize != 15.0f) lineSize++;

		brush.increaseLineSize();
		ui.setModeChangeFlag();
	}
	else if (event.getCode() == KeyEvent::KEY_ESCAPE)
	{
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
	else if (event.getChar() == 'r') //Turns on random color mode
	{
		//Turns on random color mode
		brush.changeRandColor(!brush.getRandColor());

		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'q') //Cycles through colors
	{
		brush.decrementColor();
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'w') //Cycles through colors
	{
		brush.incrementColor();

		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 't') //Increase transparency / Decrease alpha
	{
		brush.decreaseAlpha();
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'y') //Decrease transparency/ increase alpha
	{
		brush.increaseAlpha();
		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'e') //Eraser mode
	{
		brush.changeEraserMode(!(brush.getEraserMode()));

		ui.setModeChangeFlag();
	}
	else if (event.getChar() == 'c') //Clear Screen (Broken with framebuffers).
	{
		ui.clearLayers();
	}
	else if (event.getChar() == 'b')
	{
		ui.incrementBackground();
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
	else if (event.getChar() == 'n')
	{
		leapSave();
	}
	else if (event.getChar() == 'j')
	{
		imageHandler.changeImageType("jpeg");
	}
	else if (event.getChar() == 't')
	{
		imageHandler.changeImageType("tif");
	}
	else if (event.getChar() == 'v')
	{
		leapDrawFlag = !leapDrawFlag;
	}
	else if (event.getChar() == 's')
	{
		mySymmetry.toggleSymmetry();
	}
	else if (event.getChar() == 'a')
	{
		drawRadial();
	}
	else if (event.getChar() == 'g')
	{
		ui.toggleUiFlag();
	}
#ifdef EYEX
	else if (event.getChar() == ' ')
	{
		if (deviceHandler.eyeXStatus())
		{
			if (gazePositionX <= resolutionX / 2 && gazePositionY <= resolutionY / 2)
			{
				brush.changeShape(Shape::Shape::Line);
				ui.setModeChangeFlag();
				imageHandler.loadIcon(SHAPE_LINE);
			}
			else if (gazePositionX >= resolutionX / 2 && gazePositionY <= resolutionY / 2)
			{
				brush.changeShape(Shape::Shape::Circle);
				ui.setModeChangeFlag();
				imageHandler.loadIcon(SHAPE_Circle);
			}
			else if (gazePositionX <= resolutionX / 2 && gazePositionY >= resolutionY / 2)
			{
				brush.changeShape(Shape::Shape::Rectangle);
				ui.setModeChangeFlag();
				imageHandler.loadIcon(SHAPE_Rectangle);
			}
			else if (gazePositionX >= resolutionX / 2 && gazePositionY >= resolutionY / 2)
			{
				brush.changeShape(Shape::Shape::Triangle);
				ui.setModeChangeFlag();
				imageHandler.loadIcon(SHAPE_Triangle);
			}
		}
	}
#endif
	else if (event.getChar() == 'l')
	{
		leapShapeChange();
	}
	else if (event.getChar() == 'k')
	{
		leapColorChange();
	}
	else if (event.getChar() == '1')
	{
		leapShapeChange();
	}
	else if (event.getChar() == '2')
	{
		illustrator.undoDraw(ui.getBackgroundColor());
	}
	else if (event.getChar() == '3')
	{
		leapColorChange();
	}
	else if (event.getChar() == '4')
	{
		mySymmetry.toggleSymmetry();
	}
	else if (event.getChar() == '5')
	{
		ui.toggleUiFlag();
	}
}

bool TouchPointsApp::findMultiTouchGestures(TouchEvent::Touch previousPoint, TouchEvent::Touch currentPoint)
{
	//Checks for double tap
	//TouchEvent::Touch currentPoint = pointList[pointList.size() - 1];
	//TouchEvent::Touch previousPoint = pointList[pointList.size() - 2];
	//Detects double tap.
	if (0.25 > currentPoint.getTime() - previousPoint.getTime())
	{
		if (previousPoint.getX() < currentPoint.getX() + 20 && previousPoint.getX() > currentPoint.getX() - 20)
		{
			if (previousPoint.getY() < currentPoint.getY() + 20 && previousPoint.getY() > currentPoint.getY() - 20)
			{
				radialCenter = currentPoint.getPos();

				drawRadial();
				radialActive = true;
				bufferTouches.erase(previousPoint.getId());
				bufferTouches.erase(currentPoint.getId());
				return true;
			}
		}
	}
	//'Two finger tap' gesture
	if (0.05 > currentPoint.getTime() - previousPoint.getTime())
	{
		if (((previousPoint.getX() > currentPoint.getX() + 20) && previousPoint.getX() < currentPoint.getX() + 75) || (previousPoint.getX() < currentPoint.getX() - 20 && previousPoint.getX() > currentPoint.getX() - 75))
		{
			if ((previousPoint.getY() < currentPoint.getY() + 120) && (previousPoint.getY() > currentPoint.getY() - 120))
			{
				leapColorChange();
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
	for (const auto& touch : event.getTouches())
	{
		if (!deviceHandler.multiTouchStatus())
		{
			ui.inInteractiveUi(touch.getX(), touch.getY(), touch.getId());
			return;
		}

		if (radialActive)
		{
			int x = touch.getX();
			int y = touch.getY();
			if ((((radialCenter.x - 100) - 30) < x && x < ((radialCenter.x - 100) + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30)))
			{
				//brush.changeStaticColor();
				leapColorChange();
				return;
			}

			if ((((radialCenter.x + 100) - 30) < x && x < ((radialCenter.x + 100) + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30)))
			{
				//brush.cycleShape();
				leapShapeChange();
				return;
			}

			if ((((radialCenter.x) - 30) < x && x < ((radialCenter.x) + 30)) && ((radialCenter.y - 100) - 30) < y && y < (radialCenter.y - 100) + 30)
			{
				//mySymmetry.toggleSymmetry();
				illustrator.undoDraw(ui.getBackgroundColor());
				return;
			}

			if ((((radialCenter.x) - 30) < x && x < ((radialCenter.x) + 30)) && ((radialCenter.y + 100) - 30) < y && y < (radialCenter.y + 100) + 30)
			{
				ui.toggleUiFlag();
				return;
			}

			if ((radialCenter.x - 30) < x && x < ((radialCenter.x + 30)) && ((radialCenter.y - 30) < y && y < (radialCenter.y + 30)))
			{
				radialActive = false;
				return;
			}
		}
		if (ui.inInteractiveUi(touch.getX(), touch.getY(), touch.getId()))
		{
			return;
		}
		else
		{
			list<TouchEvent::Touch> tempList;
			tempList.push_back(touch);
			bufferTouches.emplace(touch.getId(), tempList);

			if (!findMultiTouchGestures(previousTouch, touch))
			{
				//If we didn't find a gesture this time, save the point incase.
				previousTouch = touch;
			}
			else
			{
				//Clears the previous touch if we found a multitouch gesture.
				previousTouch.setPos(vec2(-100, -100));
			}
		}
	}
}

void TouchPointsApp::touchesMoved(TouchEvent event)
{
	if (!deviceHandler.multiTouchStatus()) return;

	for (const auto& touch : event.getTouches())
	{
		ui.slideButtons(touch);
		if (bufferTouches.find(touch.getId()) == bufferTouches.end())
		{
			illustrator.movingTouchShapes(touch.getId(), touch.getPos(), touch.getPrevPos());
		}
		else
		{
			TouchEvent::Touch myTouch = bufferTouches[touch.getId()].front();

			//'Extended Touch' Gesture
			if (touch.getPos() == touch.getPrevPos() && touch.getTime() > myTouch.getTime() + .75)
			{
				leapShapeChange();
				bufferTouches[touch.getId()].clear();
				bufferTouches[touch.getId()].emplace_front(touch);
			}
			else
			//On finger movement, we know it is no longer a gesture. 
			//So we remove it from 'buffer' touches and begin drawing
			if ((touch.getPos().x > myTouch.getX() + 25 || touch.getX() < myTouch.getX() - 25) || (touch.getY() > myTouch.getY() + 25 || touch.getY() < myTouch.getY() - 25))
			{
				illustrator.beginTouchShapes(touch.getId(), bufferTouches[touch.getId()].front().getPos());
				for (auto touch : bufferTouches[touch.getId()])
				{
					illustrator.movingTouchShapes(touch.getId(), touch.getPos(), touch.getPrevPos());
				}
				illustrator.movingTouchShapes(touch.getId(), touch.getPos(), touch.getPrevPos());
				bufferTouches.erase(touch.getId());
			}
			else
			{
				bufferTouches[touch.getId()].emplace_back(touch);
			}
		}
	}
}

void TouchPointsApp::touchesEnded(TouchEvent event)
{
	if (!deviceHandler.multiTouchStatus()) return;
	for (const auto& touch : event.getTouches())
	{
		bufferTouches.erase(touch.getId());
		illustrator.endTouchShapes(touch.getId());
		ui.endButtonPress(touch);
	}
}

void TouchPointsApp::setDefaultMode(Mode::DefaultModes mode)
{
	bool temp = false;
	bool temp2 = true;

	switch (mode)
	{
		case Mode::DefaultModes::MLE:
			ui.changeModeButtons(temp2);
			break;
		case Mode::DefaultModes::MLR:
			ui.changeModeButtons(temp2);
			break;
		case Mode::DefaultModes::MER:
			ui.changeModeButtons(temp2);
			break;
		case Mode::DefaultModes::LER:
			ui.changeModeButtons(temp);
			break;
		case Mode::DefaultModes::ML:
			ui.changeModeButtons(temp2);
			break;
		case Mode::DefaultModes::ME:
			ui.changeModeButtons(temp2);
			break;
		case Mode::DefaultModes::MR:
			ui.changeModeButtons(temp2);
			break;
		case Mode::DefaultModes::LE:
			ui.changeModeButtons(temp);
			break;
		case Mode::DefaultModes::LR:
			ui.changeModeButtons(temp);
			break;
		case Mode::DefaultModes::ER:
			ui.changeModeButtons(temp);
			break;
		case Mode::DefaultModes::M:
			ui.changeModeButtons(temp2);
			break;
		case Mode::DefaultModes::L:
			ui.changeModeButtons(temp);
			break;
		case Mode::DefaultModes::E:
			ui.changeModeButtons(temp);
			break;
		case Mode::DefaultModes::R:
			ui.changeModeButtons(temp);
			break;
	}
}

void TouchPointsApp::mouseDown(MouseEvent event) {}

void TouchPointsApp::update()
{
	if (!setupComplete)
	{
		ui.uiSetup();
		realSenseHandler.realSenseSetup();
		setupComplete = true;
	}
	//Increment the frame counter
	frames++;

	//Gets current clock time in milliseconds
	std::chrono::milliseconds currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	//This is how often we will check our device connection in milliseconds.
	std::chrono::milliseconds checkTime{2000};
	if (currentTime - lastDeviceCheck >= checkTime)
	{
		//Updates FPS - Ensure frames is being divided by how many seconds it takes to get into this loop.
		fps = frames / 2;
		frames = 0;

		//Resets our lastDeviceCheck time.
		lastDeviceCheck = currentTime;

		//Checks if any device statuses have changed.
		if (deviceHandler.deviceConnection())
		{
			ui.drawDeviceButtonsFbo();
			ui.setModeChangeFlag();
		}
		//Checks to see if default mode needs to be reset
		if (deviceHandler.getUpdateDefaultFlag())
		{
			deviceHandler.updateDefaultMode();
			Mode::DefaultModes resultMode = deviceHandler.getDefaultMode();
			setDefaultMode(resultMode);
		}
	}

	//Checks the real sense device and updates anything associated with it.
	if (deviceHandler.realSenseStatus())
	{
		//Checks if expressions is turned on.
		if (deviceHandler.realSenseExpressions())
		{
			if (illustrator.getActiveDrawings() == 0)
			{
				realSenseHandler.streamData();

				if (realSenseHandler.getBrowGestureFlag())
				{
					illustrator.undoDraw(ui.getBackgroundColor());
				}
				if (realSenseHandler.getKissGestureFlag())
				{
					ui.toggleUiFlag();
				}
				if (realSenseHandler.getTongueGestureFlag())
				{
					mySymmetry.toggleSymmetry();
				}
				if (realSenseHandler.getCheekGestureFlag())
				{
					leapColorChange();
				}
				if (realSenseHandler.getSmileGestureFlag())
				{
					leapShapeChange();
				}
				realSenseHandler.resetGesturesFlag();
			}
		}

		//Real Sense Draw
		if (deviceHandler.realSenseDraw())
		{
			//Draw functions for real sense.
			realSenseHandler.streamCursorData();
			realSenseHandler.realSenseDraw(fingerLocationFbo);
		}
	}

	//Updates the active shapes being drawn by the user
	gl::color(1.0, 1.0, 1.0, 1.0);
	(*activeFbo).bindFramebuffer();
	//Clears Previous Active Shapes
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//Draws all active Shapes (non-permanent);
	illustrator.drawActiveShapes();
	(*activeFbo).unbindFramebuffer();
}

void TouchPointsApp::draw()
{
	//Clears the Application Context to the background color with a 0 Alpha Value (transparent)
	Color myBG = ui.getBackgroundColor();
	glClearColor(myBG.r, myBG.g, myBG.b, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	/* Small script that checks FPS
	gl::color(1.0, 1.0, 1.0);
	gl::drawSolidCircle(vec2(960, 540), (float)fps * 2);

	gl::color(1.0, 0.0, 0.0);
	gl::drawStrokedCircle(vec2(960, 540), 15.0f * 2);
	gl::drawStrokedCircle(vec2(960, 540), 30.0f * 2);
	gl::drawStrokedCircle(vec2(960, 540), 45.0f * 2);
	gl::drawStrokedCircle(vec2(960, 540), 60.0f * 2);
	*/

	//Currently leapDraw before drawing layers to prevent flickering. 
	//However, this makes it impossible to see green 'hands' on top of images.
	if (deviceHandler.leapStatus())
	{
		currentFrame = getLeapFrame(leapContr);
		if (deviceHandler.leapDraw())
		{
			if (leapDrawFlag)
			{
				leapDraw(currentFrame);
			}
		}
		if (deviceHandler.leapGesture())
		{
			if (!lockCurrentFrame)
			{
				//Calls specified action from gesture recgonized
				gestRecognition(currentFrame, leapContr);
			}
		}
		lockCurrentFrame = false;
	}

	//Draw Checkerboard pattern if background is transparent
	if (ui.isBackgroundTransparent())
	{
		/*
		backgroundFbo->bindFramebuffer();
		//gl::clear(ColorA(1.0f, 0.0f, 0.0f, 0.0f));
		//gl::color(Color(1.0, 1.0, 1.0));
		cinder::gl::TextureRef tempText = gl::Texture::create(loadImage(loadAsset("TransparentBackground.png")));
		gl::draw(tempText, Rectf(0, 0, windowWidth, windowHeight));
		backgroundFbo->unbindFramebuffer();
		gl::color(1.0, 1.0, 1.0, 0.5);
		*/
		gl::color(.5, .5, .5, 1.0);
		gl::draw(ui.getTransparentBackground()->getColorTexture());
	}
	//Loop Which Draws our Layers
	int x = 0;
	for (auto frames : layerList)
	{
		gl::color(1.0, 1.0, 1.0, ui.getLayerAlpha(x));
		gl::draw(frames->getColorTexture());
		x++;
	}

	if (imageHandler.getStartUpFlag())
	{
		imageHandler.loadStartIcon(START_LOGO);
	}
	/*Draws icons that provides feedback */
	imageHandler.displayIcon();
	imageHandler.displayStartIcon();

	//Draws all the UI elements (Currently only updated the uiFbo which stores data for the mode box), drawing is done below.
	ui.drawUi();

	//Draws radial menu
	if (radialActive)
	{
		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(radialFbo->getColorTexture());
	}

	//Draws proximity menu
	if (proxActive)
	{
		gl::draw(proxFbo->getColorTexture());
	}

	gl::color(1.0, 1.0, 1.0, 1.0);

	/*Draws the frame buffer for UI*/
	if (deviceHandler.eyeXStatus())
	{
		gl::color(0.0, 0.0, 0.0, 0.4);

		vec2 gaze1(gazePositionX - 10, gazePositionY);
		vec2 gaze2(gazePositionX + 10, gazePositionY);

		gl::drawStrokedCircle(gaze1, 10.0f, 10.0f);
		gl::drawStrokedCircle(gaze2, 10.0f, 10.0f);

		gl::color(1.0, 1.0, 1.0, 1.0);
		if (gazePositionX < 500 && gazePositionY < 100)
		{
			bool tempBool = true;
			ui.changeModeButtons(tempBool);
		}
		else
		{
			bool tempBool = false;
			ui.changeModeButtons(tempBool);
		}

		if (gazePositionX > windowWidth * .75 && gazePositionY > windowHeight * .6)
		{
			gl::draw(uiFbo->getColorTexture());
		}
	}
	else if (ui.getUiFboFlag()) gl::draw(uiFbo->getColorTexture());

	//Draws all the active shapes being drawn by the user
	gl::color(1.0, 1.0, 1.0, 1.0);
	gl::draw(activeFbo->getColorTexture());

	if (ui.getFps())
	{
		auto mFont = Font("Quicksand Book Regular", 36.0f);
		gl::drawString("Framerate: " + toString((int)getAverageFps()), vec2(windowWidth * .90, windowHeight * .01), ColorA(0.0, 0.0, 0.0, 1.0), mFont);
	}

	if (realSenseHandler.getHoverFlag())
	{
		//Draws finger location
		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw(fingerLocationFbo->getColorTexture());
	}
}

CINDER_APP(TouchPointsApp, RendererGl, prepareSettings);
