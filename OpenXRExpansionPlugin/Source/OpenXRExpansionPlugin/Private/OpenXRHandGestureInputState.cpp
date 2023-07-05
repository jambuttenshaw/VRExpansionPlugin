#include "OpenXRHandGestureInputState.h"

#include "OpenXRHandPoseComponent.h"
#include "OpenXRHandGestures.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"

#include "GameFramework/Pawn.h"
#include "Engine/LocalPlayer.h"

#include "OpenXRGlobalSettings.h"


FOpenXRHandGestureSkeletalDataState::FOpenXRHandGestureSkeletalDataState()
{
	const UOpenXRGlobalSettings& XRSettings = *GetDefault<UOpenXRGlobalSettings>();

	// Create filters for finger positions
	for (int i = 0; i < 5; i++)
	{
		if (XRSettings.bUseOneEuroLowpassFilter)
		{
			FilteredTipLocations.Add(MakeShared<FOneEuroLowpassFilter>(
				XRSettings.MinimumCutoff,
				XRSettings.DeltaCutoff,
				XRSettings.CutoffSlope
			));
		}
		else
		{
			FilteredTipLocations.Add(MakeShared<FSimpleLowpassFilter>(XRSettings.SimpleLowpassCutoffFrequency));
		}
	}

	CurrentFingerStates.AddDefaulted(5);
}

void FOpenXRHandGestureSkeletalDataState::ResetFilters()
{
	for (const TSharedPtr<ILowpassFilter>& Filter : FilteredTipLocations)
	{
		Filter->Reset();
	}
}


FOpenXRHandGestureInputState::FOpenXRHandGestureInputState(UOpenXRHandPoseComponent* InHandPoseComponent)
{
	UE_LOG(LogHandGesture, Log, TEXT("Device registration: Creating state for new hand pose component."));

	HandPoseComponent = InHandPoseComponent;

	// We should always construct with a valid HandPoseComponent
	// The HandPoseComponent is checked in the device registration, so if it gets this far it must be valid
	check(HandPoseComponent.IsValid());

	// Create an object to hold the state of each Skeletal Data structure in the component
	for (int i = 0; i < InHandPoseComponent->HandSkeletalActions.Num(); i++)
		SkeletalDataStates.Add(FOpenXRHandGestureSkeletalDataState());

	// work out the platform user id from the HandPoseComponent
	AActor* Owner = InHandPoseComponent->GetOwner();
	
	if (Owner)
	{
		// Traverse heirarchy to find top level owner
		for (; Owner->GetOwner(); Owner = Owner->GetOwner())
		{
		}

		// it SHOULD be a player controller
		APlayerController* PlayerController = Cast<APlayerController>(Owner);
		if (PlayerController)
		{
			ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
			if (LocalPlayer)
				PlatformUserId = LocalPlayer->GetPlatformUserId();
			else
			{
				UE_LOG(LogHandGesture, Warning, TEXT("Device registration: Local player not found."));
			}
		}
		else
		{
			FString ClassName = Owner->GetClass()->GetName();
			UE_LOG(LogHandGesture, Warning, TEXT("Device registration: Top of owner hierarchy was not a player controller. Owner class name: %s"), *ClassName);
		}
	}
	else
	{
		UE_LOG(LogHandGesture, Warning, TEXT("Device registration: component has no owner!"));
	}
	// PlatformUserId will be default (invalid) if for some reason the device failed to get the local player

	// Get input device id for user
	InputDeviceId = IPlatformInputDeviceMapper::Get().GetPrimaryInputDeviceForUser(PlatformUserId);


	GestureButtonStates.Empty();

	// Check GestureDB is valid
	if (InHandPoseComponent->GesturesDB)
	{
		// Create Gesture Button States
		for (const FOpenXRGesture& Gesture : InHandPoseComponent->GesturesDB->Gestures)
			GestureButtonStates.Add(Gesture.Name, false);
	}

	UE_LOG(LogHandGesture, Log, TEXT("Device registration: PlatformUserId valid: %d  Internal: %d"), PlatformUserId.IsValid(), PlatformUserId.GetInternalId());
	UE_LOG(LogHandGesture, Log, TEXT("Device registration: InputDeviceId valid: %d"), InputDeviceId.IsValid());
	UE_LOG(LogHandGesture, Log, TEXT("Device registration: Created %d gesture button states."), GestureButtonStates.Num());
	
	UE_LOG(LogHandGesture, Log, TEXT("Device registration complete."));
}

bool FOpenXRHandGestureInputState::IsValid() const
{
	return HandPoseComponent.IsValid()	&& 
		   PlatformUserId.IsValid()		&& 
		   InputDeviceId.IsValid();
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

void FOpenXRHandGestureInputState::UpdateCurrentState(float DeltaTime, bool bResetFilters)
{
	// Registered component should be validated prior to attempting to update its state
	check(IsValid());

	int SkeletalActionIndex = -1;
	for (const FBPOpenXRActionSkeletalData& SkeletalAction : HandPoseComponent->HandSkeletalActions)
	{
		SkeletalActionIndex++;
		FOpenXRHandGestureSkeletalDataState& CurrentState = SkeletalDataStates[SkeletalActionIndex];

		if (bResetFilters)
			CurrentState.ResetFilters();

		if (!SkeletalAction.bHasValidData)
		{
			continue;
		}
		if (SkeletalAction.SkeletalTransforms.Num() < EHandKeypointCount)
		{
			UE_LOG(LogHandGesture, Warning, TEXT("Invalid skeletal transform count!"));
			continue;
		}
		

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
		{
			FVector NewTipLocation = SkeletalAction.SkeletalTransforms[FingerMap[i]].GetLocation() - WristLoc;
			CurrentState.UpdateTipLocation(i, NewTipLocation, DeltaTime);
		}


		for (int i = 0; i < 5; ++i)
		{
			FVector TipLocation = CurrentState.GetTipLocation(i);

			// Don't check if the thumb is pinched, check other fingers
			if (i > 0)
			{
				float PinchedThreshold;
				verify(UOpenXRGlobalSettings::GetPinchedThreshold(i, PinchedThreshold));
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
			verify(UOpenXRGlobalSettings::GetExtendedThreshold(i, ExtendedThreshold));
			verify(UOpenXRGlobalSettings::GetClosedThreshold(i, ClosedThreshold));

			if (TipLocation.SquaredLength() > ExtendedThreshold * ExtendedThreshold)
				CurrentState.SetFingerState(i, EOpenXRGestureFingerState::OXR_GESTURE_FINGER_EXTENDED);
			else if (TipLocation.SquaredLength() < ClosedThreshold * ClosedThreshold)
				CurrentState.SetFingerState(i, EOpenXRGestureFingerState::OXR_GESTURE_FINGER_CLOSED);
			else
				CurrentState.SetFingerState(i, EOpenXRGestureFingerState::OXR_GESTURE_FINGER_NONE);
		}
	}
}
