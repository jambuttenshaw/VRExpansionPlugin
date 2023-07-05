// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenXRGlobalSettings.h"

UOpenXRGlobalSettings::UOpenXRGlobalSettings(const FObjectInitializer& ObjectInitializer)
{
	// Filtering
	SimpleLowpassCutoffFrequency = 2.0f;

	bUseOneEuroLowpassFilter = false;

	MinimumCutoff = 0.9f;
	DeltaCutoff = 1.0f;
	CutoffSlope = 0.1f;

	// Finger state thresholds
	ExtendedThresholdThumb		= 13.0f;
	ExtendedThresholdIndex		= 14.0f;
	ExtendedThresholdMiddle		= 14.0f;
	ExtendedThresholdRing		= 14.0f;
	ExtendedThresholdLittle		= 13.0f;

	ClosedThresholdThumb		= 11.5f;
	ClosedThresholdIndex		= 11.0f;
	ClosedThresholdMiddle		= 11.0f;
	ClosedThresholdRing			= 11.0f;
	ClosedThresholdLittle		= 11.0f;

	PinchedThresholdIndex		= 1.1f;
	PinchedThresholdMiddle		= 1.3f;
	PinchedThresholdRing		= 1.3f;
	PinchedThresholdLittle		= 1.3f;
}

bool UOpenXRGlobalSettings::GetExtendedThreshold(int FingerIndex, float& Threshold)
{
	if (FingerIndex < 0 || FingerIndex > 4)
		return false;

	const UOpenXRGlobalSettings& XRSettings = *GetDefault<UOpenXRGlobalSettings>();
	switch (FingerIndex)
	{
	case 0:		Threshold = XRSettings.ExtendedThresholdThumb; return true;
	case 1:		Threshold = XRSettings.ExtendedThresholdIndex; return true;
	case 2:		Threshold = XRSettings.ExtendedThresholdMiddle; return true;
	case 3:		Threshold = XRSettings.ExtendedThresholdRing; return true;
	case 4:		Threshold = XRSettings.ExtendedThresholdLittle; return true;
	default:	return false;
	}

	return false;
}

bool UOpenXRGlobalSettings::GetClosedThreshold(int FingerIndex, float& Threshold)
{
	if (FingerIndex < 0 || FingerIndex > 4)
		return false;

	const UOpenXRGlobalSettings& XRSettings = *GetDefault<UOpenXRGlobalSettings>();
	switch (FingerIndex)
	{
	case 0:		Threshold = XRSettings.ClosedThresholdThumb; return true;
	case 1:		Threshold = XRSettings.ClosedThresholdIndex; return true;
	case 2:		Threshold = XRSettings.ClosedThresholdMiddle; return true;
	case 3:		Threshold = XRSettings.ClosedThresholdRing; return true;
	case 4:		Threshold = XRSettings.ClosedThresholdLittle; return true;
	default:	return false;
	}

	return false;
}

bool UOpenXRGlobalSettings::GetPinchedThreshold(int FingerIndex, float& Threshold)
{
	// Thumb doesn't have a pinch threshold
	if (FingerIndex < 1 || FingerIndex > 4)
		return false;

	const UOpenXRGlobalSettings& XRSettings = *GetDefault<UOpenXRGlobalSettings>();
	switch (FingerIndex)
	{
	case 1:		Threshold = XRSettings.PinchedThresholdIndex; return true;
	case 2:		Threshold = XRSettings.PinchedThresholdMiddle; return true;
	case 3:		Threshold = XRSettings.PinchedThresholdRing; return true;
	case 4:		Threshold = XRSettings.PinchedThresholdLittle; return true;
	default:	return false;
	}

	return false;
}
