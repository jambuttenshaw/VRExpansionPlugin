// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInputDevice.h"

#include "OpenXRExpansionTypes.h"
#include "OpenXRHandGestures.h"

#include "../Private/OpenXRHandGestureInputState.h"


DECLARE_LOG_CATEGORY_EXTERN(LogHandGesture, Log, All);


/**
 * 
 */
class OPENXREXPANSIONPLUGIN_API FOpenXRHandGestureDevice : public IInputDevice
{
public:
	FOpenXRHandGestureDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
	virtual ~FOpenXRHandGestureDevice();

	// IInputDevice implementation

	virtual void Tick(float DeltaTime) override;
	virtual void SendControllerEvents() override;

	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;

	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values) override;
	virtual bool SupportsForceFeedback(int32 ControllerId) override { return false; }

	// End IInputDevice implementation

	// Register a new component with the device
	bool RegisterComponent(class UOpenXRHandPoseComponent* HandPoseComponent);

	void DebugDraw();
private:

	// Searches for all gesture databases and constructs key names from them
	void InitKeynames();

	// Searches all registered components, and unregisters any that have become invalid
	// Using WeakPtr's means that the device can safely check when a registered component has been deleted
	void CleanupInvalidComponents();

	// Identifies any gestures on a component
	void CheckForGestures(FOpenXRHandGestureInputState& RegisteredComponentState);

	bool DoesGestureApplyToHand(const EOpenXRGestureHand& GestureHand, const EVRSkeletalHandIndex& Hand) const;
	bool GetGestureKey(const FOpenXRGesture& Gesture, const EVRSkeletalHandIndex& TargetHand, FKey& GestureKey);


private:
	TSharedPtr<FGenericApplicationMessageHandler> MessageHandler;

	// Only update this device if currently hand tracking
	bool bIsCurrentlyHandTracking = false;

	float HandTrackingCheckInterval = 2.0f;
	float HandTrackingCheckTimer = 0.0f;

	// All components currently registered with the device to process hand gestures
	TArray<FOpenXRHandGestureInputState> RegisteredComponents;

	// Left Hand Gestures mapped to the keys they will trigger
	TMap<FName, FKey> LeftKeyMappings;
	// Right Hand Gestures mapped to the keys they will trigger
	TMap<FName, FKey> RightKeyMappings;
};
