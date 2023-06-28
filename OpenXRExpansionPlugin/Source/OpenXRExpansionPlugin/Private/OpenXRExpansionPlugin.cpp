// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "OpenXRExpansionPlugin.h"
#include "OpenXRExpansionFunctionLibrary.h"

#include "OpenXRHandGestureDevice.h"

#include "OpenXRGlobalSettings.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"

#define LOCTEXT_NAMESPACE "FXRExpansionPluginModule"


void FOpenXRExpansionPluginModule::StartupModule()
{
	IInputDeviceModule::StartupModule();

	RegisterSettings();
}

void FOpenXRExpansionPluginModule::ShutdownModule()
{
	IInputDeviceModule::ShutdownModule();

	UnregisterSettings();
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


void FOpenXRExpansionPluginModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Create the new category
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");


		SettingsModule->RegisterSettings("Project", "Plugins", "XRExpansionPlugin",
			LOCTEXT("XRExpansionSettingsName", "XRExpansion Settings"),
			LOCTEXT("XRExpansionSettingsDescription", "Configure global settings for the XRExpansionPlugin"),
			GetMutableDefault<UOpenXRGlobalSettings>());
	}
}

void FOpenXRExpansionPluginModule::UnregisterSettings()
{
	// Ensure to unregister all of your registered settings here, hot-reload would
	// otherwise yield unexpected results.
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "XRExpansionPlugin");
	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenXRExpansionPluginModule, OpenXRExpansionPlugin)