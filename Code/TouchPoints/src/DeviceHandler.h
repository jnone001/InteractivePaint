#pragma once
#include "Enums.h"
#include  "IDeviceHandler.h"

namespace touchpoints { namespace devices
{
	struct DeviceHandler 
	{
	public:
		DeviceHandler();
		int deviceConnection();
		Mode::DefaultModes getDefaultMode();
		int leapStatus();
		int multiTouchStatus();
		int eyeXStatus();
		int realSenseStatus();

		void toggleMultiTouch();
		void toggleEyeX();
		//Sense Device and Functionality
		void toggleRealSense();
		void toggleRealSenseExpressions();
		void toggleRealSenseDraw();

		bool realSenseExpressions();
		bool realSenseDraw();
		//Leap Device and Functionality
		void toggleLeap();
		void toggleLeapDraw();
		void toggleLeapGesture();

		bool leapDraw();
		bool leapGesture();

		Mode::DefaultModes set_MLER_Mode();
		Mode::DefaultModes set_MLE_Mode();
		Mode::DefaultModes set_MLR_Mode();
		Mode::DefaultModes set_MER_Mode();
		Mode::DefaultModes set_LER_Mode();
		Mode::DefaultModes set_ML_Mode();
		Mode::DefaultModes set_ME_Mode();
		Mode::DefaultModes set_MR_Mode();
		Mode::DefaultModes set_LE_Mode();
		Mode::DefaultModes set_LR_Mode();
		Mode::DefaultModes set_ER_Mode();
		Mode::DefaultModes set_L_Mode();
		Mode::DefaultModes set_E_Mode();
		Mode::DefaultModes set_M_Mode();
		Mode::DefaultModes set_R_Mode();

		bool getUpdateDefaultFlag();
		void updateDefaultMode();
		void setUpdateStatus();

		inline bool GetEyeXConnected() const { return this->eyeXConnected; };
	private:
		int setLeapState();
		int setMultiTouchState();
		int setEyeXState();
		int setRealSenseState();
		void resetFlags();

		int leapConnectedFlag;
		int multiTouchConnectedFlag;
		int eyeXConnectedFlag;
		int realSenseConnectedFlag;
		int stateCounter;
		int vendorList[4];

		//Mode Flags
		int leapConnected;
		int multiTouchConnected;
		bool eyeXConnected;
		int realSenseConnected;
		bool realSenseExpressionsEnabled;
		bool realSenseDrawEnabled;
		bool leapDrawEnabled;
		bool leapGestureEnabled;
		bool eyeXEnabled;
		bool overrideLeap;
		bool overrideMultiTouch;
		bool overrideEyeX;
		bool overrideRealSense;

		bool updateDefaultFlag;
	};
}}
