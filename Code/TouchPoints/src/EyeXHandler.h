#pragma once
#include "eyex-cpp/EyeX.hpp"
#pragma comment (lib, "Tobii.EyeX.Client.lib")
#include <bemapiset.h>

namespace touchpoints { namespace devices
{
	class EyeXHandler
	{
	public:
		EyeXHandler();
		EyeXHandler(float gazePositionX, float gazePositionY, int resolutionX, int resolutionY, TX_CONNECTIONSTATE isEyeXConnected);

		bool InitEyeX(TX_CONNECTIONSTATE isEyeXConnected, TX_TICKET hGazeTrackingStateChangedTicket,
			TX_TICKET hConnectionStateChangedTicket, TX_TICKET hEventHandlerTicket);
		bool EyeXTearDown();
		void OnGazeDataEvent(TX_HANDLE hGazeDataBehavior);
		bool InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext);
		inline float GetGazePositionX() const { return this->gazePositionX; };
		inline float GetGazePositionY() const { return this->gazePositionY; };
		inline int GetResolutionX() const { return this->resolutionX; };
		inline int GetResolutionY() const { return this->resolutionY; };
		inline void SetGazePositionX(float value) { this->gazePositionX = value; };
		inline void SetGazePositionY(float value) { this->gazePositionY = value; };
		inline void SetResolutionX(int value) { this->resolutionX = value; };
		inline void SetResolutionY(int value) { this->resolutionY = value; };

		friend static void TX_CALLCONVENTION OnEngineStateChanged(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam);
		friend static void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param);
		friend static void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam);
		friend static void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam);
		
		static TX_CONTEXTHANDLE hContext;
		static TX_HANDLE g_hGlobalInteractorSnapshot;
	private:
		float gazePositionX;
		float gazePositionY;
		int resolutionX;
		int resolutionY;
		TX_TICKET hGazeTrackingStateChangedTicket;
		TX_TICKET hConnectionStateChangedTicket;
		TX_TICKET hEventHandlerTicket;

		
	};
}}
