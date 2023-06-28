#include "OpenXRHandGestureInputState.h"

#include "OpenXRHandPoseComponent.h"
#include "OpenXRHandGestures.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"

#include "GameFramework/Pawn.h"
#include "Engine/LocalPlayer.h"

#include "OpenXRGlobalSettings.h"


FOpenXRHandGestureSkeletalDataState::FOpenXRHandGestureSkeletalDataState()
{
	FilteredTipLocations.AddUninitialized(5);
	CurrentFingerStates.AddDefaulted(5);

	const UOpenXRGlobalSettings& XRSettings = *GetDefault<UOpenXRGlobalSettings>();
	LocationFilteringCutoff = XRSettings.LowpassCutoffFrequency;
}


FOpenXRHandGestureInputState::FOpenXRHandGestureInputState(UOpenXRHandPoseComponent* InHandPoseComponent)
{
	HandPoseComponent = InHandPoseComponent;

	// We should always construct with a valid HandPoseComponent
	// The HandPoseComponent is checked in the device registration, so if it gets this far it must be valid
	check(HandPoseComponent.IsValid());
	UOpenXRHandPoseComponent* HPC = HandPoseComponent.Get();

	// Create an object to hold the state of each Skeletal Data structure in the component
	for (int i = 0; i < HPC->HandSkeletalActions.Num(); i++)
		SkeletalDataStates.Add(FOpenXRHandGestureSkeletalDataState());

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

void FOpenXRHandGestureInputState::UpdateCurrentState(float DeltaTime)
{
	check(IsValid());


	int SkeletalActionIndex = -1;
	for (const FBPOpenXRActionSkeletalData& SkeletalAction : HandPoseComponent->HandSkeletalActions)
	{
		SkeletalActionIndex++;
		FOpenXRHandGestureSkeletalDataState& CurrentState = SkeletalDataStates[SkeletalActionIndex];

		if (SkeletalAction.SkeletalTransforms.Num() < EHandKeypointCount)
			continue;

		int32 FingerMap[5] =
		{
			(int32)EXRHandJointType::OXR_HAND_JOINT_THUMB_TIP_EXT,
			(int32)EXRHandJointType::OXR_HAND_JOINT_INDEX_TIP_EXT,
			(int32)EXRHandJointType::OXR_HAND_JOINT_MIDDLE_TIP_EXT,
			(int32)EXRHandJointType::OXR_HAND_JOINT_RING_TIP_EXT,
			(int32)EXRHandJointType::OXR_HAND_JOINT_LITTLE_TIP_EXT
		};

		FVector WristLoc = SkeletalAction.SkeletalTransforms[(int32)EXRHandJointType::OXR_HAND_JOINT_WRIST_EXT].GetLocation();

		// Get all finger tip locations
		for (int i = 0; i < 5; ++i)
			CurrentState.UpdateTipLocation(i, SkeletalAction.SkeletalTransforms[FingerMap[i]].GetLocation() - WristLoc, DeltaTime);


		for (int i = 0; i < 5; ++i)
		{
			FVector TipLocation = CurrentState.GetTipLocation(i);

			// Don't check if the thumb is pinched, check other fingers
			if (i > 0)
			{
				float PinchedThreshold;
				UOpenXRGlobalSettings::GetPinchedThreshold(i, PinchedThreshold);
				if (FVector::DistSquared(CurrentState.GetTipLocation(0), TipLocation) < PinchedThreshold * PinchedThreshold)
				{
					CurrentState.SetFingerState(0, EOpenXRGestureFingerState::OXR_GESTURE_FINGER_PINCHED);
					CurrentState.SetFingerState(i, EOpenXRGestureFingerState::OXR_GESTURE_FINGER_PINCHED);
					// A finger cannot be considered as extended or closed if it is pinched
					continue;
				}
			}

			// TODO: Investigate other ways of doing this, potentially rotation of base joint of fingers?
			float ExtendedThreshold, ClosedThreshold;
			UOpenXRGlobalSettings::GetExtendedThreshold(i, ExtendedThreshold);
			UOpenXRGlobalSettings::GetClosedThreshold(i, ClosedThreshold);

			if (TipLocation.SquaredLength() > ExtendedThreshold * ExtendedThreshold)
				CurrentState.SetFingerState(i, EOpenXRGestureFingerState::OXR_GESTURE_FINGER_EXTENDED);
			else if (TipLocation.SquaredLength() < ClosedThreshold * ClosedThreshold)
				CurrentState.SetFingerState(i, EOpenXRGestureFingerState::OXR_GESTURE_FINGER_CLOSED);
			else
				CurrentState.SetFingerState(i, EOpenXRGestureFingerState::OXR_GESTURE_FINGER_NONE);
		}
	}
}
