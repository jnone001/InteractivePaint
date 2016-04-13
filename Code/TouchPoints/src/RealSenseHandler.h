#ifndef REALSENSE_H
#define REALSENSE_H

#include "pxcsession.h"
#include "pxcfacedata.h"
#include "pxchandconfiguration.h"
#include "pxchanddata.h"
#include "pxchandmodule.h"
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
#define NUM_HANDS 2

struct RealSenseHandler{

	RealSenseHandler(){
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

		int kissCount = 0; 
		int browLCount = 0;
		int browRCount = 0;
		int cheekLCount = 0;
		int cheekRCount = 0;
		int smileCount = 0;
		int tongueCount = 0;

		pxcStatus sts;

		senseManager = PXCSenseManager::CreateInstance();
		senseManager->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480); 
		//senseManager->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480);
		//senseManager->EnableFace();
		
		sts = senseManager->EnableFace();
		if (sts < PXC_STATUS_NO_ERROR) {
			wprintf_s(L"Unable to enable Hand Tracking\n");
		}
		
		sts = senseManager->EnableHand();
		if (sts < PXC_STATUS_NO_ERROR) {
			wprintf_s(L"Unable to enable Hand Tracking\n");
		}

		faceAnalyzer = senseManager->QueryFace();

		handAnalyzer = senseManager->QueryHand();
		if (!senseManager){
			wprintf_s(L"Unable to retrieve hand results\n");
		}


		senseManager->Init();
		faceOutputData = faceAnalyzer->CreateOutput();
		handOutputData = handAnalyzer->CreateOutput();
		handConfig = handAnalyzer->CreateActiveConfiguration();
		faceConfig = faceAnalyzer->CreateActiveConfiguration();
		
	}

	void intializeFaceSensing();
	void intializeHandSensing();
	void streamData();

	pxcBool kissDetection(PXCFaceData::ExpressionsData *expressionData);
	pxcBool eyebrowDetection(PXCFaceData::ExpressionsData *expressionData);
	pxcBool cheekDetection(PXCFaceData::ExpressionsData *expressionData);
	pxcBool tongueDetection(PXCFaceData::ExpressionsData *expressionData);
	pxcBool smileDetection(PXCFaceData::ExpressionsData *expressionData);

	bool getKissGestureFlag();
	bool getBrowGestureFlag();
	bool getCheekGestureFlag();
	bool getTongueGestureFlag();
	bool getSmileGestureFlag();

	void resetGesturesFlag();
	void resetBrowGestureFlag();
	void resetKissGestureFlag();

private:

	PXCSenseManager *senseManager = 0;
	PXCFaceModule *faceAnalyzer;
	PXCHandModule* handAnalyzer = 0;
	PXCFaceData *faceOutputData;
	PXCHandData *handOutputData;
	PXCFaceConfiguration *faceConfig;
	PXCHandConfiguration *handConfig;

	bool kissFlag;
	bool browLFlag;
	bool browRFlag;
	bool tongueFlag;
	bool cheekLFlag;
	bool cheekRFlag;
	bool smileFlag;
	bool kissGestureFlag;
	bool browGestureFlag;
	bool cheekGestureFlag;
	bool tongueGestureFlag;
	bool smileGestureFlag;

	int kissCount;
	int browLCount;
	int browRCount;
	int cheekLCount;
	int cheekRCount;
	int smileCount;
	int tongueCount;
};

void RealSenseHandler::intializeFaceSensing(){

	faceConfig->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH);
	faceConfig->QueryExpressions()->Enable();
	faceConfig->QueryExpressions()->EnableAllExpressions();
	faceConfig->QueryExpressions()->properties.maxTrackedFaces = MAX_FACES;
	faceConfig->ApplyChanges();

}

void RealSenseHandler::intializeHandSensing(){
	handConfig->SetTrackingMode(PXCHandData::TRACKING_MODE_FULL_HAND);
	handConfig->ApplyChanges();
}

void RealSenseHandler::streamData(){

	//while (senseManager->AcquireFrame(true) >= PXC_STATUS_NO_ERROR){
	if (senseManager->AcquireFrame(true) >= PXC_STATUS_NO_ERROR){


		faceOutputData->Update();
		handOutputData->Update();

		/* Face Detection Struct */
		PXCFaceData::ExpressionsData *expressionData = nullptr;
		/* Hand Detection Struct */
		PXCHandData::JointData nodes[NUM_HANDS][PXCHandData::NUMBER_OF_JOINTS] = {};

		pxcU16 numOfFaces = faceOutputData->QueryNumberOfDetectedFaces();
		pxcU16 numOfHands = handOutputData->QueryNumberOfHands();
		pxcU16 handID;

		/* Face Data Analysis*/
		for (pxcU16 i = 0; i <= numOfFaces; i++)
		{

			// get face data by index
			PXCFaceData::Face *trackedFace = faceOutputData->QueryFaceByIndex(i);
			if (trackedFace != NULL)
			{

				/* Query Expression Data */
				expressionData = trackedFace->QueryExpressions();
				if (expressionData != NULL)
				{

					kissGestureFlag = kissDetection(expressionData);
					browGestureFlag = eyebrowDetection(expressionData);
					cheekGestureFlag = cheekDetection(expressionData);
					tongueGestureFlag = tongueDetection(expressionData);
					smileGestureFlag = smileDetection(expressionData);

				}
			}
		}

		/* Hand Data Analysis */
		for (pxcU16 i = 0; i <= numOfHands; i++){
			//Retrieve first hand that has appeared 
			PXCHandData::IHand *handData;
			if (handOutputData->QueryHandData(PXCHandData::ACCESS_ORDER_BY_TIME, i, handData) == PXC_STATUS_NO_ERROR){
				for (pxcU16 j = 0; j < PXCHandData::NUMBER_OF_JOINTS; j++){
					handData->QueryTrackedJoint((PXCHandData::JointType)j, nodes[i][j]);
				}
			}
		}

		senseManager->ReleaseFrame();
	}
}

pxcBool RealSenseHandler::kissDetection(PXCFaceData::ExpressionsData *expressionData){

	PXCFaceData::ExpressionsData::FaceExpressionResult expressionResultKiss;
	pxcBool kissResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_KISS, &expressionResultKiss);

	if (kissResult){
		pxcI32 value = expressionResultKiss.intensity;
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
pxcBool RealSenseHandler::cheekDetection(PXCFaceData::ExpressionsData *expressionData){

	PXCFaceData::ExpressionsData::FaceExpressionResult expressionCheekL;
	pxcBool cheekLResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_PUFF_LEFT, &expressionCheekL);
	if (cheekLResult){
		pxcI32 value = expressionCheekL.intensity;
		if (value == 100){
			cheekLCount++;
			if (cheekLCount > 15)
			{
				browLCount = 0;
				cheekLFlag = true;
				
			}
		}
		else{
			cheekLCount = 0;
		}
	}

	PXCFaceData::ExpressionsData::FaceExpressionResult expressionCheekR;
	pxcBool cheekRResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_PUFF_RIGHT, &expressionCheekR);
	if (cheekRResult){
		pxcI32 value = expressionCheekR.intensity;
		if (value == 100){
			cheekRCount++;
			if (cheekRCount > 15)
			{
				cheekRFlag = true;
				cheekRCount = 0;
			}
		}
		else{
			cheekRCount = 0;
		}
	}

	if (cheekLFlag && cheekRFlag){
		cheekLFlag = false;
		cheekRFlag = false;
		return true;
	}
	else{
		return false;
	}
}
pxcBool RealSenseHandler::tongueDetection(PXCFaceData::ExpressionsData *expressionData){

	PXCFaceData::ExpressionsData::FaceExpressionResult expressionResultTongue;
	pxcBool tongueResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_TONGUE_OUT, &expressionResultTongue);

	if (tongueResult){
		pxcI32 value = expressionResultTongue.intensity;
		if (value == 100){

			tongueCount++;

			if (tongueCount > 15)
			{
				tongueCount = 0;
				return true;
			}

		}
		else{
			tongueCount = 0;
		}
	}
	return false;
}
pxcBool RealSenseHandler::smileDetection(PXCFaceData::ExpressionsData *expressionData){
	PXCFaceData::ExpressionsData::FaceExpressionResult expressionResultSmile;
	pxcBool smileResult = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_SMILE, &expressionResultSmile);

	if (smileResult){
		pxcI32 value = expressionResultSmile.intensity;
		if (value > 25){

			smileCount++;

			if (smileCount > 15)
			{
				smileCount = 0;
				return true;
			}

		}
		else{
			smileCount = 0;
		}
	}
	return false;
}

bool RealSenseHandler::getKissGestureFlag(){
	return kissGestureFlag;
}
bool RealSenseHandler::getBrowGestureFlag(){
	return browGestureFlag;
}
bool RealSenseHandler::getCheekGestureFlag(){
	return cheekGestureFlag; 
}
bool RealSenseHandler::getTongueGestureFlag(){
	return tongueGestureFlag;
}
bool RealSenseHandler::getSmileGestureFlag(){
	return smileGestureFlag;
}

void RealSenseHandler::resetBrowGestureFlag(){
	browGestureFlag = false;
	
}
void RealSenseHandler::resetKissGestureFlag(){
	kissGestureFlag = false;
}
void RealSenseHandler::resetGesturesFlag(){
	browGestureFlag = false;
	kissGestureFlag = false;
	cheekGestureFlag = false;
	tongueGestureFlag = false;
	smileGestureFlag = false;
}


#endif