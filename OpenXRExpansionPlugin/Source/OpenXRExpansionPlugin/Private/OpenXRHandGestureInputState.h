#pragma once

#include "CoreMinimal.h"
#include "OpenXRHandGestures.h"
#include "OpenXRHandGestureFilters.h"


// Current values of a SkeletalAction
class FOpenXRHandGestureSkeletalDataState
{
	friend class FOpenXRHandGestureInputState;
public:
	FOpenXRHandGestureSkeletalDataState();

	void ResetFilters();

	inline EOpenXRGestureFingerState GetFingerState(int FingerIndex) const
	{
		check(FingerIndex >= 0 && FingerIndex < 5);
		return CurrentFingerStates[FingerIndex];
	}
	inline FVector GetTipLocation(int FingerIndex) const
	{
		check(FingerIndex >= 0 && FingerIndex < 5);
		return FilteredTipLocations[FingerIndex]->GetOutput();
	}

private:
	inline void SetFingerState(int FingerIndex, EOpenXRGestureFingerState State)
	{
		check(FingerIndex >= 0 && FingerIndex < 5);
		CurrentFingerStates[FingerIndex] = State;
	}
	inline void UpdateTipLocation(int FingerIndex, const FVector& InTipLocation, float DeltaTime)
	{
		check(FingerIndex >= 0 && FingerIndex < 5);
		FilteredTipLocations[FingerIndex]->Filter(InTipLocation, DeltaTime);
	}

private:
	TArray<TSharedPtr<ILowpassFilter>> FilteredTipLocations;
	TArray<EOpenXRGestureFingerState> CurrentFingerStates;
};


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
	inline FOpenXRHandGestureSkeletalDataState& GetSkeletalDataState(int Index)
	{
		check(SkeletalDataStates.IsValidIndex(Index));
		return SkeletalDataStates[Index];
	}

	bool GetGestureButtonState(const FName& GestureName);
	void SetGestureButtonState(const FName& GestureName, bool State);

	bool IsValid() const;

	void UpdateCurrentState(float DeltaTime, bool bResetFilters = false);

private:
	FPlatformUserId PlatformUserId;
	FInputDeviceId InputDeviceId;

	TWeakObjectPtr<class UOpenXRHandPoseComponent> HandPoseComponent;

	// Current hand state
	TArray<FOpenXRHandGestureSkeletalDataState> SkeletalDataStates;
	
	// The states of all gesture buttons
	TMap<FName, bool> GestureButtonStates;
};
