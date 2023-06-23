// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "OpenXRExpansionPlugin.h"
#include "OpenXRExpansionFunctionLibrary.h"

#include "OpenXRHandGestureDevice.h"

#define LOCTEXT_NAMESPACE "FXRExpansionPluginModule"


void FOpenXRExpansionPluginModule::StartupModule()
{
	IInputDeviceModule::StartupModule();
}

void FOpenXRExpansionPluginModule::ShutdownModule()
{
	IInputDeviceModule::ShutdownModule();
}

TSharedPtr<IInputDevice> FOpenXRExpansionPluginModule::CreateInputDevice(const TSharedRef<class FGenericApplicationMessageHandler>& InMessageHandler)
{
	TSharedPtr<FOpenXRHandGestureDevice> InputDevice(new FOpenXRHandGestureDevice(InMessageHandler));
	HandGestureDevice = InputDevice;
	return InputDevice;
}

TSharedPtr<IInputDevice> FOpenXRExpansionPluginModule::GetInputDevice() const
{
	if (HandGestureDevice.IsValid())
	{
		return HandGestureDevice.Pin();
	}
	return nullptr;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenXRExpansionPluginModule, OpenXRExpansionPlugin)