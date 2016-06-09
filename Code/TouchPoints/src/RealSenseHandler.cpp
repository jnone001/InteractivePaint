#include "RealSenseHandler.h"
#include <cinder/gl/scoped.h>
#include <cinder/gl/draw.h>

namespace touchpoints { namespace devices
{
	RealSenseHandler::RealSenseHandler() { }

	RealSenseHandler::RealSenseHandler(drawing::Illustrator* illustrator)
	{
		kissGestureFlag = false;
		browGestureFlag = false;
		cheekGestureFlag = false;
		smileGestureFlag = false;
		tongueGestureFlag = false;

		browLFlag = false;
		browRFlag = false;
		cheekLFlag = false;
		cheekRFlag = false;
		smileFlag = false;
		tongueFlag = false;

		hoverZoneFlag = false;
		realDrawFlag = false;
		realSenseDrawEnabled = true;

		myIllustrator = illustrator;

		//Real sense constructor not complete until realSenseSetup called
	}

	void RealSenseHandler::realSenseSetup()
	{
		pxcStatus sts;

		senseManager = PXCSenseManager::CreateInstance();
		senseManager->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480);

		sts = senseManager->EnableFace();
		if (sts < PXC_STATUS_NO_ERROR)
		{
			wprintf_s(L"Unable to enable Hand Tracking\n");
		}

		sts = senseManager->EnableHandCursor();
		if (sts < PXC_STATUS_NO_ERROR)
		{
			wprintf_s(L"Unable to enable Hand Tracking\n");
		}

		faceAnalyzer = senseManager->QueryFace();

		cursorAnalyzer = senseManager->QueryHandCursor();
		if (!senseManager)
		{
			wprintf_s(L"Unable to retrieve hand results\n");
		}

		senseManager->Init();

		/*Face Output Data*/
		faceOutputData = faceAnalyzer->CreateOutput();
		faceConfig = faceAnalyzer->CreateActiveConfiguration();
		/*Cursor Output Data*/
		cursorOutputData = cursorAnalyzer->CreateOutput();
		cursorConfig = cursorAnalyzer->CreateActiveConfiguration();

		/*Initialziation of Face and Cursor Configurations*/
		intializeFaceSensing();
		intializeCursorSensing();
	}

	void RealSenseHandler::intializeFaceSensing()
	{
		faceConfig->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH);
		faceConfig->QueryExpressions()->Enable();
		faceConfig->QueryExpressions()->EnableAllExpressions();
		faceConfig->QueryExpressions()->properties.maxTrackedFaces = MAX_FACES;
		faceConfig->ApplyChanges();
	}

	void RealSenseHandler::intializeCursorSensing()
	{
		cursorConfig->ApplyChanges();
	}

	void RealSenseHandler::streamData()
	{
		if (senseManager->AcquireFrame(true) >= PXC_STATUS_NO_ERROR)
		{
			/*Update the Output Data to current Frame*/
			faceOutputData->Update();
			cursorOutputData->Update();

			/* Face Detection Struct */
			PXCFaceData::ExpressionsData* expressionData = nullptr;

			/* Face Data Analysis*/
			for (pxcU16 i = 0; i <= faceOutputData->QueryNumberOfDetectedFaces(); i++)
			{
				// Get face data by index
				PXCFaceData::Face* trackedFace = faceOutputData->QueryFaceByIndex(i);

				if (trackedFace != NULL)
				{
					/* Query Expression Data */
					expressionData = trackedFace->QueryExpressions();
				}

				if (expressionData != NULL)
				{
					kissGestureFlag = kissDetection(expressionData);
					browGestureFlag = eyebrowDetection(expressionData);
					cheekGestureFlag = cheekDetection(expressionData);
					tongueGestureFlag = tongueDetection(expressionData);
					smileGestureFlag = smileDetection(expressionData);
				}
			}

			senseManager->ReleaseFrame();
		}
	}

	void RealSenseHandler::streamCursorData()
	{
		if (senseManager->AcquireFrame(true) >= PXC_STATUS_NO_ERROR)
		{
			/*Update the Output Data to current Frame*/
			cursorOutputData->Update();

			/* Cursor Data Analysis */
			PXCCursorData::ICursor* cursor;
			for (int i = 0; i < cursorOutputData->QueryNumberOfCursors(); ++i)
			{
				cursorOutputData->QueryCursorData(PXCCursorData::ACCESS_ORDER_BY_TIME, i, cursor);
				std::string handSide = "Unknown Hand";
				handSide = cursor->QueryBodySide() == PXCHandData::BODY_SIDE_LEFT ? "Left Hand Cursor" : "Right Hand Cursor";

				xPosition = 1920 - (((cursor->QueryCursorImagePoint().x) / 640) * 1920);
				yPosition = ((cursor->QueryCursorImagePoint().y) / 480) * 1080;
				zPosition = cursor->QueryCursorImagePoint().z;

				currentId = cursor->QueryUniqueId();

				if (zPosition >= .35 && zPosition <= .50)
				{
					hoverZoneFlag = true;
					realDrawFlag = false;
				}
				else if (zPosition >= 0 && zPosition < .35)
				{
					hoverZoneFlag = false;
					realDrawFlag = true;
				}
				else
				{
					hoverZoneFlag = false;
					realDrawFlag = false;
				}

				std::printf("%s\n==============\n", handSide.c_str());
				std::printf("Cursor Image Point: X: %f, Y: %f Z: %f \n", xPosition, yPosition, zPosition);
				std::printf("Cursor World Point: X: %f, Y: %f, Z: %f \n", cursor->QueryCursorWorldPoint().x, cursor->QueryCursorWorldPoint().y, cursor->QueryCursorWorldPoint().z);
			}

			senseManager->ReleaseFrame();
		}
	}

	pxcBool RealSenseHandler::kissDetection(PXCFaceData::ExpressionsData* expressionData)
	{
		PXCFaceData::ExpressionsData::FaceExpressionResult expressionResultKiss;
		pxcBool kissResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_KISS, &expressionResultKiss);

		if (kissResult)
		{
			pxcI32 value = expressionResultKiss.intensity;
			if (value == 100)
			{
				kissCount++;

				if (kissCount > 15)
				{
					kissCount = 0;
					return true;
				}
			}
			else
			{
				kissCount = 0;
			}
		}
		return false;
	}

	pxcBool RealSenseHandler::eyebrowDetection(PXCFaceData::ExpressionsData* expressionData)
	{
		PXCFaceData::ExpressionsData::FaceExpressionResult expressionBrowL;
		pxcBool browLResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_LEFT, &expressionBrowL);
		if (browLResult)
		{
			pxcI32 value = expressionBrowL.intensity;
			if (value == 100)
			{
				browLCount++;
				if (browLCount > 15)
				{
					browLFlag = true;
					browLCount = 0;
				}
			}
			else
			{
				browLCount = 0;
			}
		}

		PXCFaceData::ExpressionsData::FaceExpressionResult expressionBrowR;
		pxcBool browRResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_LEFT, &expressionBrowR);
		if (browRResult)
		{
			pxcI32 value = expressionBrowR.intensity;
			if (value == 100)
			{
				browRCount++;
				if (browRCount > 15)
				{
					browRFlag = true;
					browRCount = 0;
				}
			}
			else
			{
				browRCount = 0;
			}
		}

		if (browRFlag && browLFlag)
		{
			browRFlag = false;
			browLFlag = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	pxcBool RealSenseHandler::cheekDetection(PXCFaceData::ExpressionsData* expressionData)
	{
		PXCFaceData::ExpressionsData::FaceExpressionResult expressionCheekL;
		pxcBool cheekLResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_PUFF_LEFT, &expressionCheekL);
		if (cheekLResult)
		{
			pxcI32 value = expressionCheekL.intensity;
			if (value == 100)
			{
				cheekLCount++;
				if (cheekLCount > 15)
				{
					browLCount = 0;
					cheekLFlag = true;
				}
			}
			else
			{
				cheekLCount = 0;
			}
		}

		PXCFaceData::ExpressionsData::FaceExpressionResult expressionCheekR;
		pxcBool cheekRResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_PUFF_RIGHT, &expressionCheekR);
		if (cheekRResult)
		{
			pxcI32 value = expressionCheekR.intensity;
			if (value == 100)
			{
				cheekRCount++;
				if (cheekRCount > 15)
				{
					cheekRFlag = true;
					cheekRCount = 0;
				}
			}
			else
			{
				cheekRCount = 0;
			}
		}

		if (cheekLFlag && cheekRFlag)
		{
			cheekLFlag = false;
			cheekRFlag = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	pxcBool RealSenseHandler::tongueDetection(PXCFaceData::ExpressionsData* expressionData)
	{
		PXCFaceData::ExpressionsData::FaceExpressionResult expressionResultTongue;
		pxcBool tongueResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_TONGUE_OUT, &expressionResultTongue);

		if (tongueResult)
		{
			pxcI32 value = expressionResultTongue.intensity;
			if (value == 100)
			{
				tongueCount++;

				if (tongueCount > 15)
				{
					tongueCount = 0;
					return true;
				}
			}
			else
			{
				tongueCount = 0;
			}
		}
		return false;
	}

	pxcBool RealSenseHandler::smileDetection(PXCFaceData::ExpressionsData* expressionData)
	{
		PXCFaceData::ExpressionsData::FaceExpressionResult expressionResultSmile;
		pxcBool smileResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_SMILE, &expressionResultSmile);

		if (smileResult)
		{
			pxcI32 value = expressionResultSmile.intensity;
			if (value > 25)
			{
				smileCount++;

				if (smileCount > 15)
				{
					smileCount = 0;
					return true;
				}
			}
			else
			{
				smileCount = 0;
			}
		}
		return false;
	}

	bool RealSenseHandler::getKissGestureFlag()
	{
		return kissGestureFlag;
	}

	bool RealSenseHandler::getBrowGestureFlag()
	{
		return browGestureFlag;
	}

	bool RealSenseHandler::getCheekGestureFlag()
	{
		return cheekGestureFlag;
	}

	bool RealSenseHandler::getTongueGestureFlag()
	{
		return tongueGestureFlag;
	}

	bool RealSenseHandler::getSmileGestureFlag()
	{
		return smileGestureFlag;
	}

	void RealSenseHandler::resetBrowGestureFlag()
	{
		browGestureFlag = false;
	}

	void RealSenseHandler::resetKissGestureFlag()
	{
		kissGestureFlag = false;
	}

	void RealSenseHandler::resetGesturesFlag()
	{
		browGestureFlag = false;
		kissGestureFlag = false;
		cheekGestureFlag = false;
		tongueGestureFlag = false;
		smileGestureFlag = false;
	}

	void RealSenseHandler::realSenseDraw(std::shared_ptr<gl::Fbo>& fingerLocation)
	{
		if (hoverZoneFlag)
		{
			fingerLocation->bindFramebuffer();
			glClearColor(0.0, 0.0, 0.0, 0.0);
			glClear(GL_COLOR_BUFFER_BIT);

			gl::color(1, 0.75, 0, 1);
			gl::drawSolidCircle(vec2(xPosition, yPosition), 40);
			gl::color(1.0, 0.9, 0.5, 1);
			gl::drawStrokedCircle(vec2(xPosition, yPosition), 40.0f, 10.0f);
			fingerLocation->unbindFramebuffer();
		}
		else if (realDrawFlag)
		{
			auto result = realPointsMap.find(currentId);
			//New cursor is has started to draw
			if (result == realPointsMap.end())
			{
				myIllustrator->beginTouchShapes(currentId, vec2(xPosition, yPosition));

				realPointsMap.emplace(currentId, vec2(xPosition, yPosition));
				realActivePointsMap.emplace(currentId, true);
			}
			else
			{
				//Already drawing with current cursor
				myIllustrator->movingTouchShapes(currentId, vec2(xPosition, yPosition), realPointsMap[currentId]);
				realActivePointsMap[currentId] = true;
				realPointsMap[currentId] = vec2(xPosition, yPosition);
			}
		}

		std::vector<uint32_t> list;
		for (auto& points : realActivePointsMap)
		{
			if (points.second)
			{
				points.second = false;
			}
			else
			{
				myIllustrator->endTouchShapes(points.first);
				list.emplace_back(points.first);
				realPointsMap.erase(points.first);
			}
		}
		for (auto ids : list)
		{
			realActivePointsMap.erase(ids);
		}
	}

	bool RealSenseHandler::getHoverFlag()
	{
		return hoverZoneFlag;
	}

	bool RealSenseHandler::realDrawStatus()
	{
		return realDrawFlag;
	}

	bool RealSenseHandler::getRealSenseDrawEnabled()
	{
		return realSenseDrawEnabled;
	}
}}
