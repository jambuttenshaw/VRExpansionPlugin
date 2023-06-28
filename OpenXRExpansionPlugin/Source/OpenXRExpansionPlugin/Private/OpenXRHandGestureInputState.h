#pragma once

#include "CoreMinimal.h"
#include "OpenXRHandGestures.h"


// A lowpass filter to remove high frequency movement from the hand data, to make detecting gestures more stable
// Thanks to Jimmy Van den Berg for the implementation (https://github.com/jimmyberg/LowPassFilter)
template <typename T>
class TOpenXRHandGestureLowPassFilter
{
public:
	TOpenXRHandGestureLowPassFilter(float Cutoff, float DeltaTime)
	{
		Factor = 1.0f - FMath::Exp(-DeltaTime * 2.0f * PI * Cutoff);
	}
	
	T Update(const T& Input)
	{
		return Output += (Input - Output) * Factor;
	}
	T Update(const T& Input, float DeltaTime, float Cutoff)
	{
		Factor = 1.0f - FMath::Exp(-DeltaTime * 2.0f * PI * Cutoff);
		return Output += (Input - Output) * Factor;
	}
	
	inline const T& GetOutput() const { return Output; }
	inline void ResetOutput(const T& Value) const
	{
		Output = Value;
	}

private:
	T Output;
	float Factor = 0.0f;
};


// Current values of a SkeletalAction
class FOpenXRHandGestureSkeletalDataState
{
	friend class FOpenXRHandGestureInputState;
public:
	FOpenXRHandGestureSkeletalDataState();

	EOpenXRGestureFingerState GetFingerState(int FingerIndex) const
	{
		check(FingerIndex >= 0 && FingerIndex < 5);
		return CurrentFingerStates[FingerIndex];
	}

private:
	const FVector& GetTipLocation(int FingerIndex) const
	{
		check(FingerIndex >= 0 && FingerIndex < 5);
		return FilteredTipLocations[FingerIndex].GetOutput();
	}
	void UpdateTipLocation(int FingerIndex, const FVector& InTipLocation, float DeltaTime)
	{
		check(FingerIndex >= 0 && FingerIndex < 5);
		FilteredTipLocations[FingerIndex].Update(InTipLocation, DeltaTime, LocationFilteringCutoff);
	}
	void SetFingerState(int FingerIndex, EOpenXRGestureFingerState State)
	{
		check(FingerIndex >= 0 && FingerIndex < 5);
		CurrentFingerStates[FingerIndex] = State;
	}

private:
	float LocationFilteringCutoff = 3.0f; // 3Hz

	TArray<TOpenXRHandGestureLowPassFilter<FVector>> FilteredTipLocations;
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

	void UpdateCurrentState(float DeltaTime);

private:
	FPlatformUserId PlatformUserId;
	FInputDeviceId InputDeviceId;

	TWeakObjectPtr<class UOpenXRHandPoseComponent> HandPoseComponent;

	// Current hand state
	TArray<FOpenXRHandGestureSkeletalDataState> SkeletalDataStates;
	
	TMap<FName, bool> GestureButtonStates;
};
