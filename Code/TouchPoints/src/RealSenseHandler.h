#pragma once
#include "pxcfacedata.h"
#include "pxchanddata.h"
#include "pxccursorconfiguration.h"
#include "pxccursordata.h"
#include "pxchandcursormodule.h"
#include "pxcfaceconfiguration.h"
#include "pxcdefs.h"
#include "pxcsensemanager.h"
#include "pxcfacemodule.h"
#include "pxcstatus.h"
#include "pxccapture.h"
#include "Illustrator.h"
#include <vector>
#include <map> 

using namespace std;

#define MAX_FACES 1
#define NUM_HANDS 2

namespace devices {
	struct RealSenseHandler
	{
	public:
		RealSenseHandler();
		RealSenseHandler(drawing::Illustrator* illustrator);
		void realSenseSetup();
		void intializeFaceSensing();
		void intializeCursorSensing();
		void streamData();
		void streamCursorData();

		pxcBool kissDetection(PXCFaceData::ExpressionsData* expressionData);
		pxcBool eyebrowDetection(PXCFaceData::ExpressionsData* expressionData);
		pxcBool cheekDetection(PXCFaceData::ExpressionsData* expressionData);
		pxcBool tongueDetection(PXCFaceData::ExpressionsData* expressionData);
		pxcBool smileDetection(PXCFaceData::ExpressionsData* expressionData);

		bool getKissGestureFlag();
		bool getBrowGestureFlag();
		bool getCheekGestureFlag();
		bool getTongueGestureFlag();
		bool getSmileGestureFlag();

		bool getHoverFlag();
		bool realDrawStatus();

		void resetGesturesFlag();
		void resetBrowGestureFlag();
		void resetKissGestureFlag();

		void drawFingerLocation(std::shared_ptr<gl::Fbo>& fingerLocation);
		void realSenseDraw(std::shared_ptr<gl::Fbo>& fingerLocation);
		bool getRealSenseDrawEnabled();
	private:
		PXCSenseManager* senseManager = 0;
		PXCFaceModule* faceAnalyzer;
		PXCHandCursorModule* cursorAnalyzer = 0;
		PXCFaceData* faceOutputData;
		PXCCursorData* cursorOutputData;
		PXCFaceConfiguration* faceConfig;
		PXCCursorConfiguration* cursorConfig;

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

		bool hoverZoneFlag;
		bool realDrawFlag;
		bool realSenseDrawEnabled;

		int kissCount;
		int browLCount;
		int browRCount;
		int cheekLCount;
		int cheekRCount;
		int smileCount;
		int tongueCount;

		float xPosition;
		float yPosition;
		float zPosition;

		float currentId;

		//RealSense Draw maps
		map<uint32_t, vec2> realPointsMap;
		map<uint32_t, bool> realActivePointsMap;

		drawing::Illustrator* myIllustrator;
	};
}