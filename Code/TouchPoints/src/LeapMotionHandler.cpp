#include "LeapMotionHandler.h"
#include "Enums.h"
#include "cinder/gl/gl.h"

namespace touchpoints { namespace devices
{
	LeapMotionHandler::LeapMotionHandler() {};

	LeapMotionHandler::LeapMotionHandler(/*drawing::ImageHandler imageHandler,*/ int windowWidth, int windowHeight/*,
	                                     ui::UserInterface& gui, drawing::Illustrator& illustrator, drawing::Brush& brush*/)
		: /*imageHandler(imageHandler),*/ windowWidth(windowWidth), windowHeight(windowHeight)/*,
		  gui(gui), illustrator(illustrator), brush(brush)*/ { };

	void LeapMotionHandler::InitLeapMotion()
	{
		enableGest(leapController);
		leapController.config().setFloat("Gesture.Swipe.MinLength", 150.0);
		leapController.config().setFloat("Gesture.Swipe.MinVelocity", 500.0);
		leapController.config().save();
		leapController.addListener(leapListener);
	}

	void LeapMotionHandler::enableGest(Leap::Controller controller)
	{
		controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
		controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
		controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
		controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
	}

	void LeapMotionHandler::gestRecognition(bool isDrawing, bool& processing, bool& proxActive,
	                                        int& currShape, bool& imageFlag, drawing::Brush brush,
											ui::UserInterface gui, drawing::ImageHandler imageHandler, 
											gl::Fbo* proxFbo)
	{
		if (!isDrawing)
		{
			//List of all gestures
			gestureList = this->currentFrame.gestures();
			//Process gestures...
			if (!imageHandler.getIconFlag())
			{
				for (Leap::Gesture gesture : gestureList)
				{
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
										leapColorChange(brush, gui, imageHandler);
									}

									if (position.x < 0 && position.y < 150)
									{
										processing = true;
										leapShapeChange(currShape, imageFlag, brush, gui, imageHandler);
									}

									if (position.x > 0 && position.y > 250)
									{
										processing = true;
										leapSave(gui, imageHandler);
									}

									if (position.x > 0 && position.y < 150)
									{
										proxActive = false;
									}
								}
								else
								{
									//counterclockwise circle
									drawProx(proxActive, proxFbo);
								}

								// Calculate angle swept since last frame
								float sweptAngle = 0;
								if (circle.state() != Leap::Gesture::STATE_START)
								{
									Leap::CircleGesture previousUpdate = Leap::CircleGesture(this->leapController.frame(1).gesture(circle.id()));
									sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * M_PI;
								}
								break;
							}
						case Leap::Gesture::TYPE_SWIPE:
							{
								processing = true;
								leapColorChange(brush, gui, imageHandler);
								break;
							}
						case Leap::Gesture::TYPE_KEY_TAP:
							{
								processing = true;
								leapSave(gui, imageHandler);
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

	void LeapMotionHandler::leapSave(ui::UserInterface gui, drawing::ImageHandler imageHandler)
	{
		if (gui.isBackgroundTransparent())
		{
			imageHandler.saveCanvas(vec2(windowWidth, windowHeight), ColorA(gui.getBackgroundColor(), 0.0));
		}
		else imageHandler.saveCanvas(vec2(windowWidth, windowHeight), ColorA(gui.getBackgroundColor(), 1.0));
	}

	void LeapMotionHandler::leapDraw(bool& lockCurrentFrame, bool& proxActive, drawing::Illustrator illustrator)
	{
		//Get all pointables from current leap frame
		Leap::PointableList pointables = this->currentFrame.pointables();
		//Gets a virtual rectangular prism which is within the field of view of Leap
		Leap::InteractionBox iBox = this->currentFrame.interactionBox();

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

	void LeapMotionHandler::leapShapeChange(int& currShape, bool& imageFlag, drawing::Brush brush, 
		ui::UserInterface gui, drawing::ImageHandler imageHandler)
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
		gui.setModeChangeFlag();
		switch (brush.getCurrentShape())
		{
			case 0:
				{
					imageHandler.loadIcon(SHAPE_LINE);

					break;
				}
			case 1:
				{
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
					break;
				}
		}
	}

	void LeapMotionHandler::leapColorChange(drawing::Brush brush, ui::UserInterface gui,
		drawing::ImageHandler imageHandler)
	{
		brush.incrementColor();
		gui.setModeChangeFlag();
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

	void LeapMotionHandler::drawProx(bool& proxActive, gl::Fbo* proxFbo)
	{
		proxFbo->bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		proxFbo->unbindFramebuffer();

		//Draw to radial menu buffer
		proxFbo->bindFramebuffer();
		//Make background transparent
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		gl::lineWidth(10);
		gl::color(0.0, 0.0, 0.0, 1.0);
		gl::drawLine(vec2(windowWidth * .5, windowHeight), vec2(windowWidth * .5, 0));

		gl::drawLine(vec2(0, windowHeight * .5), vec2(windowWidth, windowHeight * .5));

		proxFbo->unbindFramebuffer();

		proxActive = true;
	}
}}
