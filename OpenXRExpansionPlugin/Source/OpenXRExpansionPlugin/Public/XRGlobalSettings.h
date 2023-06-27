// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "XRGlobalSettings.generated.h"


UCLASS(config = Engine, defaultconfig)
class OPENXREXPANSIONPLUGIN_API UXRGlobalSettings : public UObject
{
	GENERATED_BODY()
public:
	UXRGlobalSettings(const FObjectInitializer& ObjectInitializer);


	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category = "HandGestures")
		float LowpassCutoffFrequency; // 5Hz

};
