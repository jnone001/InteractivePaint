#include "LeapMotionHandler.h"
#include "Enums.h"
#include "cinder/gl/gl.h"

namespace touchpoints { namespace devices
{
	LeapMotionHandler::LeapMotionHandler() {};

	LeapMotionHandler::LeapMotionHandler(int windowWidth, int windowHeight)
		: windowWidth(windowWidth), windowHeight(windowHeight) {};

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
		int& currShape, bool& imageFlag, drawing::Brush& brush, ui::UserInterface& gui,
		drawing::ImageHandler& imageHandler, gl::Fbo* proxFbo)
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

	void LeapMotionHandler::leapSave(ui::UserInterface& gui, drawing::ImageHandler& imageHandler)
	{
		if (gui.isBackgroundTransparent())
		{
			imageHandler.saveCanvas(vec2(windowWidth, windowHeight), ColorA(gui.getBackgroundColor(), 0.0));
		}
		else imageHandler.saveCanvas(vec2(windowWidth, windowHeight), ColorA(gui.getBackgroundColor(), 1.0));
	}

	void LeapMotionHandler::leapDraw(bool& lockCurrentFrame, bool proxActive, drawing::Illustrator& illustrator)
	{
		proxActive = false;
		if (proxActive)
		{
			return;
		}

		//Get all pointables from current leap frame
		Leap::PointableList pointables = this->currentFrame.pointables();
		//Gets a virtual rectangular prism which is within the field of view of Leap
		Leap::InteractionBox iBox = this->currentFrame.interactionBox();

		auto fingerLocationCircles = vector<drawing::TouchCircle>();
		auto drawEventsToSendToIllustrator = vector<drawing::DrawEvent>();

		//Traverse all pointables
		for (auto& point : pointables)
		{
			//Normalize points from iBox
			Leap::Vector normalizedPosition = iBox.normalizePoint(point.stabilizedTipPosition());
			//Get x and y coordinate value form normalized value within given window
			float leapXCoordinate = normalizedPosition.x * windowWidth;
			float leapYCoordinate = windowHeight - normalizedPosition.y * windowHeight;

			float touchDistance = point.touchDistance();

			auto currentPoint = vec2(leapXCoordinate, leapYCoordinate);
			auto pointId = point.id();
			auto guid = getGuid(pointId);
			int eventMaxLifeSpan = 1;

			if (touchDistance > 0 && point.touchZone() != Leap::Pointable::Zone::ZONE_NONE)
			{
				auto color = distanceToColor(touchDistance);
				fingerLocationCircles.push_back(drawing::TouchCircle(currentPoint, 40.0f, color, 50.0f, true, 1));

				auto finalizableDrawEventIterator = finalizeableDrawEvents.find(guid);
				bool finilizableDrawEventWasFound = finalizableDrawEventIterator != finalizeableDrawEvents.end();
				if (finilizableDrawEventWasFound)
				{
					auto finalizableEvent = finalizableDrawEventIterator->second;
					//finalize that sumbitch send it to the illustrator
					finalizableEvent.SetEndPoint(currentPoint);
					drawEventsToSendToIllustrator.push_back(finalizableEvent);
					//delete it from finalizeableDrawEvents
					finalizeableDrawEvents.erase(finalizableDrawEventIterator);
					//remove guid associated with this event
					pointIdToGuidMap.erase(pointId);
				}
			}
			if(touchDistance < 0)
			{
				lockCurrentFrame = true;

				auto finalizableDrawEventIterator = finalizeableDrawEvents.find(guid);
				bool finalizableDrawEventWasFound = finalizableDrawEventIterator != finalizeableDrawEvents.end();
				
				if(finalizableDrawEventWasFound) //continuation of series of draw events
				{
					auto finalizableEvent = finalizableDrawEventIterator->second;

					//reset life of finilizable draw event, so it doesnt die
					finalizableEvent.ResetCurrentAge();

					//check if we are continuing any temp draw events
					auto tempDrawEventIterator = temporaryDrawEvents.find(guid);
					bool tempDrawEventWasFound = tempDrawEventIterator != temporaryDrawEvents.end();
					
					if (tempDrawEventWasFound) //continuation of previous temp draw event
					{
						auto tempDrawEvent = tempDrawEventIterator->second;
						//finish temp draw event
						tempDrawEvent.SetEndPoint(currentPoint);
						//send it off
						drawEventsToSendToIllustrator.push_back(tempDrawEvent);
						//remove from temporaryDrawEvents
						temporaryDrawEvents.erase(tempDrawEventIterator);
					}
					else
					{
						auto parentStartLocation = finalizableEvent.GetStartPoint();
						//create new temp draw event
						auto newTempDrawEvent = drawing::DrawEvent(currentPoint, parentStartLocation, guid, false, eventMaxLifeSpan);
						temporaryDrawEvents.insert_or_assign(guid, newTempDrawEvent);
					}
				}
				else //should be a new series of drawing events
				{
					//create new finilizable draw event
					auto newFinalizableDrawEvent = drawing::DrawEvent(currentPoint, guid, true, eventMaxLifeSpan);
					createPointIdToGuidMapping(pointId, guid);
					finalizeableDrawEvents.insert_or_assign(guid, newFinalizableDrawEvent);
				}
			}
		}

		if (fingerLocationCircles.size() > 0)
		{
			illustrator.addToTemporaryCircles(fingerLocationCircles);
		}

		if (drawEventsToSendToIllustrator.size() > 0)
		{
			illustrator.addDrawEventsToQueue(drawEventsToSendToIllustrator);
		}

		//increment all draw events lifespan
		//remove all those that exceed max life span
		for (auto it = begin(temporaryDrawEvents); it != end(temporaryDrawEvents);)
		{
			if (!it->second.ShouldBeAlive())
			{
				it = temporaryDrawEvents.erase(it);
			}
			else
				++it;
		}
		for (auto it = begin(finalizeableDrawEvents); it != end(finalizeableDrawEvents);)
		{
			if (!it->second.ShouldBeAlive())
			{
				it = finalizeableDrawEvents.erase(it);
			}
			else
				++it;
		}
	}

	void LeapMotionHandler::leapShapeChange(int& currShape, bool& imageFlag, drawing::Brush& brush,
	                                        ui::UserInterface& gui, drawing::ImageHandler& imageHandler)
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

	void LeapMotionHandler::leapColorChange(drawing::Brush& brush, ui::UserInterface& gui,
	                                        drawing::ImageHandler& imageHandler)
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

	ColorA LeapMotionHandler::distanceToColor(float distance) 
	{
		auto green = ColorA(0.0f, 1.0f, 0.0f, 1);
		auto limeGreen = ColorA(0.33f, 1.0f, 0.0f, 1);
		auto yellowGreen = ColorA(0.68f, 1.0f, 0.0f, 1);
		auto pickle = ColorA(0.78f, 1.0f, 0.0f, 1);
		auto yellow = ColorA(1.0f, 0.93f, 0.0f, 1);
		auto sunset = ColorA(1.0f, 0.61f, 0.0f, 1);
		auto orange = ColorA(1.0f, 0.38f, 0.0f, 1);
		auto crimson = ColorA(1.0f, 0.18f, 0.0f, 1);
		auto red = ColorA(1.0f, 0.0f, 0.0f, 1);

		if(distance <= 0.1)
		{
			return green;
		}
		if (distance <= 0.2)
		{
			return limeGreen;
		}
		if (distance <= 0.3)
		{
			return yellowGreen;
		}
		if (distance <= 0.4)
		{
			return pickle;
		}
		if (distance <= 0.5)
		{
			return yellow;
		}
		if (distance <= 0.6)
		{
			return sunset;
		}
		if (distance <= 0.7)
		{
			return orange;
		}
		if (distance <= 0.8)
		{
			return crimson;
		}
		return red;
	}

	Guid LeapMotionHandler::getGuid(int pointId)
	{
		auto guidIterator = pointIdToGuidMap.find(pointId);
		bool guidWasFound = guidIterator != pointIdToGuidMap.end();

		return guidWasFound ? guidIterator->second : guidGenerator.newGuid();
	}

	void LeapMotionHandler::createPointIdToGuidMapping(int pointId, Guid guid)
	{
		pointIdToGuidMap.insert_or_assign(pointId, guid);
	}
}}
