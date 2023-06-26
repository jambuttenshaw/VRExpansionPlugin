#include "OpenXRHandGestureInputState.h"

#include "OpenXRHandPoseComponent.h"
#include "OpenXRHandGestures.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"

#include "GameFramework/Pawn.h"
#include "Engine/LocalPlayer.h"


FOpenXRHandGestureInputState::FOpenXRHandGestureInputState(UOpenXRHandPoseComponent* InHandPoseComponent)
{
	HandPoseComponent = InHandPoseComponent;

	// We should always construct with a valid HandPoseComponent
	// The HandPoseComponent is checked in the device registration, so if it gets this far it must be valid
	check(HandPoseComponent.IsValid());
	UOpenXRHandPoseComponent* HPC = HandPoseComponent.Get();

	// work out the platform user id from the HandPoseComponent
	APawn* OwnerPawn = Cast<APawn>(HPC->GetOwner());
	if (OwnerPawn)
	{
		APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
		if (PlayerController)
		{
			ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
			if (LocalPlayer)
				PlatformUserId = LocalPlayer->GetPlatformUserId();
		}
	}
	// PlatformUserId will be default (invalid) if for some reason the device failed to get the local player

	InputDeviceId = IPlatformInputDeviceMapper::Get().GetDefaultInputDevice();

	GestureButtonStates.Empty();

	// Check GestureDB is valid
	if (HPC->GesturesDB)
	{
		// Create Gesture Button States
		for (const FOpenXRGesture& Gesture : HPC->GesturesDB->Gestures)
			GestureButtonStates.Add(Gesture.Name, false);
	}

}

bool FOpenXRHandGestureInputState::IsValid() const
{
	return HandPoseComponent.IsValid();
}

bool FOpenXRHandGestureInputState::GetGestureButtonState(const FName& GestureName)
{
	check(GestureButtonStates.Contains(GestureName));
	return GestureButtonStates[GestureName];
}

void FOpenXRHandGestureInputState::SetGestureButtonState(const FName& GestureName, bool State)
{
	check(GestureButtonStates.Contains(GestureName));
	GestureButtonStates[GestureName] = State;
}