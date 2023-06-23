// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInputDevice.h"


DECLARE_LOG_CATEGORY_EXTERN(LogHandGesture, Log, All);


/**
 * 
 */
class OPENXREXPANSIONPLUGIN_API FOpenXRHandGestureDevice : public IInputDevice
{
public:
	FOpenXRHandGestureDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
	virtual ~FOpenXRHandGestureDevice();

	virtual void Tick(float DeltaTime) override;
	virtual void SendControllerEvents() override;

	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;

	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values) override;

private:
	TSharedPtr<FGenericApplicationMessageHandler> MessageHandler;
};
