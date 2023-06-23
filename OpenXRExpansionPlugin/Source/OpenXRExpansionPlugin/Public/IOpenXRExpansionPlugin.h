// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Modules/ModuleManager.h"
#include "IInputDeviceModule.h"
#include "IInputDevice.h"


class OPENXREXPANSIONPLUGIN_API IOpenXRExpansionPluginModule : public IInputDeviceModule
{
public:
	static inline IOpenXRExpansionPluginModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IOpenXRExpansionPluginModule>("OpenXRExpansionPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("OpenXRExpansionPlugin");
	}

	virtual TSharedPtr<IInputDevice> GetInputDevice() const = 0;
};
