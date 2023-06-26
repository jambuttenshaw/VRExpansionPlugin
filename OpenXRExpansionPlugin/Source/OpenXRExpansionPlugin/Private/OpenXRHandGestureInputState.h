#pragma once

#include "CoreMinimal.h"


class FOpenXRHandGestureInputState
{
public:
	FOpenXRHandGestureInputState(class UOpenXRHandPoseComponent* InHandPoseComponent);

	inline FPlatformUserId GetPlatformUserId() const { return PlatformUserId; }
	inline FInputDeviceId GetInputDeviceId() const { return InputDeviceId; }
	inline class UOpenXRHandPoseComponent* GetHandPoseComponent() const
	{
		if (HandPoseComponent.IsValid())
			return HandPoseComponent.Get();
		return nullptr;
	}

	bool IsValid() const;

	bool GetGestureButtonState(const FName& GestureName);
	void SetGestureButtonState(const FName& GestureName, bool State);

private:
	FPlatformUserId PlatformUserId;
	FInputDeviceId InputDeviceId;

	TWeakObjectPtr<class UOpenXRHandPoseComponent> HandPoseComponent;
	
	TMap<FName, bool> GestureButtonStates;
};
