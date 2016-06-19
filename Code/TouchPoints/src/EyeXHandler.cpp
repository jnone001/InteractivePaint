#include "EyeXHandler.h"
#include <assert.h>

namespace touchpoints { namespace devices
{
	//static functions
	void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param);
	void TX_CALLCONVENTION OnEngineStateChanged(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam);
	void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam);
	void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam);

	TX_HANDLE EyeXHandler::g_hGlobalInteractorSnapshot = nullptr;
	TX_CONTEXTHANDLE EyeXHandler::hContext = nullptr;

	EyeXHandler::EyeXHandler() {};
	EyeXHandler::EyeXHandler(float gazePositionX, float gazePositionY, int resolutionX, int resolutionY, 
		TX_CONNECTIONSTATE isEyeXConnected)
		: gazePositionX(gazePositionX), gazePositionY(gazePositionY), resolutionX(resolutionX),
		  resolutionY(resolutionY), isEyeXConnected(isEyeXConnected), hGazeTrackingStateChangedTicket(TX_INVALID_TICKET),
		hConnectionStateChangedTicket(TX_INVALID_TICKET), hEventHandlerTicket(TX_INVALID_TICKET) {};

	bool EyeXHandler::InitEyeX()
	{
		bool success;
		success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
		success &= txCreateContext(&hContext, TX_FALSE) == TX_RESULT_OK;
		success &= this->InitializeGlobalInteractorSnapshot(hContext);
		success &= txRegisterConnectionStateChangedHandler(hContext, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, this) == TX_RESULT_OK;
		success &= txRegisterEventHandler(hContext, &hEventHandlerTicket, HandleEvent, this) == TX_RESULT_OK;
		success &= txEnableConnection(hContext) == TX_RESULT_OK;
		success &= txRegisterStateChangedHandler(hContext, &hGazeTrackingStateChangedTicket, TX_STATEPATH_GAZETRACKING, OnEngineStateChanged, this) == TX_RESULT_OK;

		return success;
	}

	bool EyeXHandler::EyeXTearDown()
	{
		bool success;
		txDisableConnection(hContext);
		txReleaseObject(&g_hGlobalInteractorSnapshot);
		success = txShutdownContext(hContext, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
		success &= txReleaseContext(&hContext) == TX_RESULT_OK;
		success &= txUninitializeEyeX() == TX_RESULT_OK;

		return success;
	}

	bool EyeXHandler::InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext)
	{
		TX_HANDLE hInteractor = TX_EMPTY_HANDLE;
		TX_GAZEPOINTDATAPARAMS params = {TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED};
		bool success;

		success = txCreateGlobalInteractorSnapshot(
		                                           hContext,
		                                           "Sparkly Unicorns",
		                                           &g_hGlobalInteractorSnapshot,
		                                           &hInteractor) == TX_RESULT_OK;
		success &= txCreateGazePointDataBehavior(hInteractor, &params) == TX_RESULT_OK;

		txReleaseObject(&hInteractor);

		return success;
	}

	void EyeXHandler::OnGazeDataEvent(TX_HANDLE hGazeDataBehavior)
	{
		TX_GAZEPOINTDATAEVENTPARAMS eventParams;
		if (txGetGazePointDataEventParams(hGazeDataBehavior, &eventParams) == TX_RESULT_OK)
		{
			this->gazePositionX = eventParams.X;
			this->gazePositionY = eventParams.Y;
		}
	}

	void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param)
	{
		// check the result code using an assertion.
		// this will catch validation errors and runtime errors in debug builds. in release builds it won't do anything.
		TX_RESULT result = TX_RESULT_UNKNOWN;
		txGetAsyncDataResultCode(hAsyncData, &result);
		assert(result == TX_RESULT_OK || result == TX_RESULT_CANCELLED);
	}

	void TX_CALLCONVENTION OnEngineStateChanged(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
	{
		TX_RESULT result = TX_RESULT_UNKNOWN;
		TX_HANDLE hStateBag = TX_EMPTY_HANDLE;

		if (txGetAsyncDataResultCode(hAsyncData, &result) == TX_RESULT_OK &&
			txGetAsyncDataContent(hAsyncData, &hStateBag) == TX_RESULT_OK)
		{
			txReleaseObject(&hStateBag);
		}
	}

	void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam)
	{
		switch (connectionState)
		{
			case TX_CONNECTIONSTATE_CONNECTED:
				{
					BOOL success;
					success = txCommitSnapshotAsync(EyeXHandler::g_hGlobalInteractorSnapshot, OnSnapshotCommitted, NULL) == TX_RESULT_OK;
					txGetStateAsync(EyeXHandler::hContext, TX_STATEPATH_EYETRACKING, OnEngineStateChanged, NULL);
				}
				break;
		}
	}

	void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
	{
		TX_HANDLE hEvent = TX_EMPTY_HANDLE;
		TX_HANDLE hBehavior = TX_EMPTY_HANDLE;

		txGetAsyncDataContent(hAsyncData, &hEvent);

		// NOTE. Uncomment the following line of code to view the event object. The same function can be used with any interaction object.
		//OutputDebugStringA(txDebugObject(hEvent));
		if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK)
		{
			EyeXHandler* handler = static_cast<EyeXHandler*>(userParam);
			handler->OnGazeDataEvent(hBehavior);
			txReleaseObject(&hBehavior);
		}

		// NOTE since this is a very simple application with a single interactor and a single data stream,
		// our event handling code can be very simple too. A more complex application would typically have to
		// check for multiple behaviors and route events based on interactor IDs.
		txReleaseObject(&hEvent);
	}
}}
