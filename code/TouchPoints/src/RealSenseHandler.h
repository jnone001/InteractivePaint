#ifndef REALSENSE_H
#define REALSENSE_H

#include "pxcsession.h"
#include "pxcfacedata.h"
#include "pxcfaceconfiguration.h"
#include <windows.h>
#include "pxcdefs.h"
#include "service/pxcsessionservice.h"
#include "pxcsensemanager.h"
#include "pxccapturemanager.h"
#include "pxcfacemodule.h"
#include "pxcstatus.h"
#include <WindowsX.h>
#include "pxccapture.h"
#include "pxcprojection.h"
#include "pxcmetadata.h"
#include "pxcbase.h"
#include "pxcaddref.h"
#include "pxcversion.h"



using namespace std;

#define MAX_FACES 1

struct RealSenseHandler{

	RealSenseHandler(){

		kissGestureFlag = false;
		browGestureFlag = false;
		browLFlag = false;
		browRFlag = false;
		int kissCount = 0; 
		int browLCount = 0;
		int browRCount = 0;

		senseManager = PXCSenseManager::CreateInstance();
		senseManager->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480);
		senseManager->EnableFace();
		faceAnalyzer = senseManager->QueryFace();
		senseManager->Init();
		outputData = faceAnalyzer->CreateOutput();
		config = faceAnalyzer->CreateActiveConfiguration();
	}

	void intializeFaceSensing();
	void streamData();
	pxcBool kissDetection(PXCFaceData::ExpressionsData *expressionData);
	pxcBool eyebrowDetection(PXCFaceData::ExpressionsData *expressionData);
	bool getKissGestureFlag();
	bool getBrowGestureFlag();
	void resetBrowGestureFlag();
	void resetKissGestureFlag();

private:

	PXCSenseManager *senseManager;
	PXCFaceModule *faceAnalyzer;
	PXCFaceData *outputData;
	PXCFaceConfiguration *config;

	bool kissFlag;
	bool browLFlag;
	bool browRFlag; 
	bool kissGestureFlag;
	bool browGestureFlag;

	int kissCount; 
	int browLCount;
	int browRCount; 
};

void RealSenseHandler::intializeFaceSensing(){

	config->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH);
	config->QueryExpressions()->Enable();
	config->QueryExpressions()->EnableAllExpressions();
	config->QueryExpressions()->properties.maxTrackedFaces = MAX_FACES;
	config->ApplyChanges();

}

void RealSenseHandler::streamData(){

	//while (senseManager->AcquireFrame(true) >= PXC_STATUS_NO_ERROR){
	if (senseManager->AcquireFrame(true) >= PXC_STATUS_NO_ERROR){


		outputData->Update();

		/* Detection Structs */
		PXCFaceData::ExpressionsData *expressionData = nullptr;
		//PXCFaceData::ExpressionsData::FaceExpressionResult expressionResult;

		// iterate through hands
		pxcU16 numOfFaces = outputData->QueryNumberOfDetectedFaces();


		for (pxcU16 i = 0; i <= numOfFaces; i++)
		{

			// get face data by index
			PXCFaceData::Face *trackedFace = outputData->QueryFaceByIndex(i);
			if (trackedFace != NULL)
			{

				/* Query Expression Data */
				expressionData = trackedFace->QueryExpressions();
				if (expressionData != NULL)
				{

					kissGestureFlag = kissDetection(expressionData);

					browGestureFlag = eyebrowDetection(expressionData);

				}
			}
		}
		senseManager->ReleaseFrame();
	}
}

pxcBool RealSenseHandler::kissDetection(PXCFaceData::ExpressionsData *expressionData){

	PXCFaceData::ExpressionsData::FaceExpressionResult expressionResult;
	pxcBool kissResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_KISS, &expressionResult);

	if (kissResult){
		pxcI32 value = expressionResult.intensity;
		if (value == 100){

			kissCount++;

			if (kissCount > 15)
			{
				kissCount = 0;
				return true;
			}

		}
		else{
			kissCount = 0;
		}
	}
	return false;
}

pxcBool RealSenseHandler::eyebrowDetection(PXCFaceData::ExpressionsData *expressionData){

	PXCFaceData::ExpressionsData::FaceExpressionResult expressionBrowL;
	pxcBool browLResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_LEFT, &expressionBrowL);
	if (browLResult){
		pxcI32 value = expressionBrowL.intensity;
		if (value == 100){
			browLCount++;
			if (browLCount > 15)
			{
				browLFlag = true;
				browLCount = 0;
			}
		}
		else{
			browLCount = 0;
		}
	}

	PXCFaceData::ExpressionsData::FaceExpressionResult expressionBrowR;
	pxcBool browRResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_LEFT, &expressionBrowR);
	if (browRResult){
		pxcI32 value = expressionBrowR.intensity;
		if (value == 100){
			browRCount++;
			if (browRCount > 15)
			{
				browRFlag = true;
				browRCount = 0;
			}
		}
		else{
			browRCount = 0;
		}
	}

	if (browRFlag && browLFlag){
		browRFlag = false;
		browLFlag = false;
		return true;
	}
	else{
		return false;
	}
}

bool RealSenseHandler::getKissGestureFlag(){
	return kissGestureFlag;
}

bool RealSenseHandler::getBrowGestureFlag(){
	return browGestureFlag;
}

void RealSenseHandler::resetBrowGestureFlag(){
	browGestureFlag = false;
}

void RealSenseHandler::resetKissGestureFlag(){
	kissGestureFlag = false;
}


#endif