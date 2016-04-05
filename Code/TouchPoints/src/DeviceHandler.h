#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H

#include "libusb.h"
#include "UserInterface.h"
#include <stdio.h>

struct DeviceHandler{

	//Creates a Device Handlers
	DeviceHandler(){
		leapConnected = 0;
		multiTouchConnected = 0;
		eyeXConnected = 0;
		realSenseConnected = 0;
		leapConnectedFlag = 0;
		multiTouchConnectedFlag = 0;
		eyeXConnectedFlag = 0;
		stateCounter = 0;
		vendorList[0] = 61826;
		vendorList[1] = 8746;
		vendorList[2] = 8452;
		vendorList[3] = 1054;
		overrideLeap = false;
		overrideMultiTouch = false;
		overrideEyeX = false;
		leapDrawEnabled = true;
		leapGestureEnabled = true;
	}


	int deviceConnection();
	int getDefaultMode();
	int leapStatus();
	int multiTouchStatus();
	int eyeXStatus();
	int realSenseStatus();

	void toggleLeap();
	void toggleMultiTouch();
	void toggleEyeX();
	void toggleRealSense();
	void toggleLeapDraw();
	void toggleLeapGesture();

	bool leapDraw();
	bool leapGesture();

	int setIdealMode();
	int setNoMultitouchMode();
	int setNoLeapMode();
	int setNoEyeX();
	int setOnlyLeapMode();
	int setOnlyEyeXMode();
	int setOnlyMultiTouchMode();
	


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
	//int vendorList[3];
	
	//Mode Flags
	int leapConnected;
	int multiTouchConnected;
	int eyeXConnected;
	int realSenseConnected;
	bool leapDrawEnabled;
	bool leapGestureEnabled;
	bool eyeXEnabled;
	bool overrideLeap;
	bool overrideMultiTouch;
	bool overrideEyeX;

};

bool DeviceHandler::leapDraw(){
	return leapDrawEnabled;
}
bool DeviceHandler::leapGesture(){
	return leapGestureEnabled;
}

void DeviceHandler::toggleLeap(){

	overrideLeap = true;
	leapConnected = !leapConnected;
}
void DeviceHandler::toggleLeapDraw(){

	leapDrawEnabled = !leapDrawEnabled;
}
void DeviceHandler::toggleLeapGesture(){

	leapGestureEnabled = !leapGestureEnabled;

}
/*
void DeviceHandler::toggleRealSense(){}
*/
void DeviceHandler::toggleMultiTouch(){
	overrideMultiTouch = true;
	multiTouchConnected = !multiTouchConnected;

}
void DeviceHandler::toggleEyeX(){
	overrideEyeX = true;

	eyeXConnected = !eyeXConnected;

	
}

int DeviceHandler::deviceConnection(){

	libusb_device **devs;			//pointer to pointer of device, used to retrieve a list of devices
	libusb_context *contex = NULL;		//a libusb session

	int r;			//return values
	ssize_t listCount;	//Number of devices in list

	//Intialize Library Session
	r = libusb_init(&contex);
	if (r < 0){
		//Intialization Error
		return -1;
	}

	//Get the list of devices
	listCount = libusb_get_device_list(contex, &devs);
	if (listCount < 0) {
		return (int)listCount;
	}

	libusb_device *dev;

	int i = 0, j = 0;
	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			//Error
			return -1;
		}
		uint16_t vendorid = desc.idVendor;

		if (vendorid == vendorList[0]){
			leapConnectedFlag = 1;
		}
		if (vendorid == vendorList[1]){
			multiTouchConnectedFlag = 1;
		}
		if (vendorid == vendorList[2]){
			eyeXConnectedFlag = 1;
		}
		if (vendorid == vendorList[3])
		{
			realSenseConnectedFlag = 1;
		}
	}

	stateCounter = 0;

	stateCounter = stateCounter + (setLeapState());

	stateCounter = stateCounter + (setMultiTouchState());

	stateCounter = stateCounter + (setEyeXState());

	stateCounter = stateCounter + (setRealSenseState());

	resetFlags();

	libusb_free_device_list(devs, 1);


	libusb_exit(NULL);

	return stateCounter;
}

int DeviceHandler::getDefaultMode(){

	//All three devices connected
	if (multiTouchConnected && leapConnected & eyeXConnected){ //&& eyeXConnectedFlag){
		return setIdealMode();
	}
	else if (leapConnected && eyeXConnected){
		return setNoMultitouchMode();
	}
	else if (multiTouchConnected && eyeXConnected){
		return setNoLeapMode();
	}
	else if (multiTouchConnected && leapConnected){
		return setNoEyeX();
	}
	else if (leapConnected){
		return setOnlyLeapMode();
	}
	else if (multiTouchConnected){
		return setOnlyMultiTouchMode();
	}
	else{
		return setOnlyEyeXMode();
	}
	
}

int DeviceHandler::leapStatus(){
	return leapConnected;
}

int DeviceHandler::multiTouchStatus(){
	return multiTouchConnected;
}

int DeviceHandler::eyeXStatus(){
	return eyeXConnected;
}

int DeviceHandler::realSenseStatus(){
	return realSenseConnected;
}

int DeviceHandler::setLeapState(){

	if (leapConnectedFlag == 1){
		if (leapConnected == 0){
			if (overrideLeap){
				return 0;
			}
			leapConnected = 1;
			return 1;
		}
		else{
			return 0;
		}
	}
	else{
		if (leapConnected == 1){
			leapConnected = 0;
			return 1;
		}
		else{
			return 0;
		}
	}
}

int DeviceHandler::setMultiTouchState(){

	if( multiTouchConnectedFlag == 1){
		if (multiTouchConnected == 0){
			if (overrideMultiTouch){
				return 0;
			}
			multiTouchConnected = 1;
			return 1;
		}
		else{
			return 0;
		}
	}
	else{
		if (multiTouchConnected == 1){
			multiTouchConnected = 0;
			return 1;
		}
		else{
			return 0;
		}
	}
}

int DeviceHandler::setEyeXState(){

	if (eyeXConnectedFlag == 1){
		if (eyeXConnected == 0){
			if (overrideEyeX){
				return 0;
			}
			eyeXConnected = 1;
			return 1;
		}
		else{
			return 0;
		}
	}
	else{
		if (eyeXConnected == 1){
			eyeXConnected = 0;
			return 1;
		}
		else{
			return 0;
		}
	}
}

int DeviceHandler::setRealSenseState(){

	if (realSenseConnectedFlag == 1){
		if (realSenseConnected == 0){
			if (overrideEyeX){
				return 0;
			}
			realSenseConnected = 1;
			return 1;
		}
		else{
			return 0;
		}
	}
	else{
		if (realSenseConnected == 1){
			realSenseConnected = 0;
			return 1;
		}
		else{
			return 0;
		}
	}
}

void DeviceHandler::resetFlags(){
	leapConnectedFlag = 0;
	multiTouchConnectedFlag = 0;
	eyeXConnectedFlag = 0;
	realSenseConnectedFlag = 0;
}

int DeviceHandler::setIdealMode(){

	//Turn off leapDraw
	toggleLeapDraw();
	return 1;

}
int DeviceHandler::setNoMultitouchMode(){
	return 2;
}
int DeviceHandler::setNoLeapMode(){
	toggleLeapDraw();
	toggleLeapGesture();
	return 3;
}
int DeviceHandler::setNoEyeX(){

	toggleLeapDraw();
	return 4;
}
int DeviceHandler::setOnlyLeapMode(){
	return 5;
}
int DeviceHandler::setOnlyEyeXMode(){
	toggleLeapDraw();
	toggleLeapGesture();
	return 6;
}
int DeviceHandler::setOnlyMultiTouchMode(){

	toggleLeapDraw();
	toggleLeapGesture();
	return 7;
}


#endif
