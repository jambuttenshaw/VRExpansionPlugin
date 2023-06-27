// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IOpenXRExpansionPlugin.h"


class FOpenXRExpansionPluginModule : public IOpenXRExpansionPluginModule
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** IInputDeviceModule implementation */
	virtual TSharedPtr<IInputDevice> CreateInputDevice(const TSharedRef<class FGenericApplicationMessageHandler>& InMessageHandler) override;

	/** IOpenXRExpansionPluginModule implementation */
	virtual TSharedPtr<IInputDevice> GetInputDevice() const override;


	void RegisterSettings();
	void UnregisterSettings();

private:
	TWeakPtr<class FOpenXRHandGestureDevice> HandGestureDevice;
};