#include "OpenXRHandGestureDevice.h"


DEFINE_LOG_CATEGORY(LogHandGesture)


FOpenXRHandGestureDevice::FOpenXRHandGestureDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
	: MessageHandler(InMessageHandler)
{
	UE_LOG(LogHandGesture, Display, TEXT("XR Hand Gesture Device Created"));
}

FOpenXRHandGestureDevice::~FOpenXRHandGestureDevice()
{
	UE_LOG(LogHandGesture, Display, TEXT("XR Hand Gesture Device Destroyed"));
}

void FOpenXRHandGestureDevice::Tick(float DeltaTime)
{
}

void FOpenXRHandGestureDevice::SendControllerEvents()
{
}

void FOpenXRHandGestureDevice::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

bool FOpenXRHandGestureDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FOpenXRHandGestureDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
}

void FOpenXRHandGestureDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values)
{
}

