// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OpenXRGlobalSettings.generated.h"


UCLASS(config = Engine, defaultconfig)
class OPENXREXPANSIONPLUGIN_API UOpenXRGlobalSettings : public UObject
{
	GENERATED_BODY()
public:
	UOpenXRGlobalSettings(const FObjectInitializer& ObjectInitializer);


	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|Filtering")
		float SimpleLowpassCutoffFrequency;

	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|Filtering")
		bool bUseOneEuroLowpassFilter;

	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|Filtering")
		float MinimumCutoff;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|Filtering")
		float DeltaCutoff;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|Filtering")
		float CutoffSlope;

	// Gesture Detection Thresholds
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Extended")
		float ExtendedThresholdThumb;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Extended")
		float ExtendedThresholdIndex;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Extended")
		float ExtendedThresholdMiddle;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Extended")
		float ExtendedThresholdRing;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Extended")
		float ExtendedThresholdLittle;

	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Closed")
		float ClosedThresholdThumb;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Closed")
		float ClosedThresholdIndex;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Closed")
		float ClosedThresholdMiddle;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Closed")
		float ClosedThresholdRing;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Closed")
		float ClosedThresholdLittle;

	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Pinched")
		float PinchedThresholdIndex;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Pinched")
		float PinchedThresholdMiddle;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Pinched")
		float PinchedThresholdRing;
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures|FingerStateThresholds|Pinched")
		float PinchedThresholdLittle;


	UFUNCTION(BlueprintPure, Category="OpenXRHandGestures")
		static bool GetExtendedThreshold(int FingerIndex, float& Threshold);
	UFUNCTION(BlueprintPure, Category = "OpenXRHandGestures")
		static bool GetClosedThreshold(int FingerIndex, float& Threshold);
	UFUNCTION(BlueprintPure, Category = "OpenXRHandGestures")
		static bool GetPinchedThreshold(int FingerIndex, float& Threshold);

};
