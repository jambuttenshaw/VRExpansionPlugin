#include "OpenXRHandGestureDevice.h"

#include "OpenXRHandGestures.h"
#include "OpenXRHandPoseComponent.h"

#include "Modules/ModuleManager.h"
#include "AssetRegistry/AssetRegistryModule.h"

DEFINE_LOG_CATEGORY(LogHandGesture)

#define LOCTEXT_NAMESPACE "OpenXRExpansionPluginModule"


FOpenXRHandGestureDevice::FOpenXRHandGestureDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
	: MessageHandler(InMessageHandler)
{
	UE_LOG(LogHandGesture, Log, TEXT("XR Hand Gesture Device Created"));

	InitKeynames();
}

FOpenXRHandGestureDevice::~FOpenXRHandGestureDevice()
{
	UE_LOG(LogHandGesture, Log, TEXT("XR Hand Gesture Device Destroyed"));
}

void FOpenXRHandGestureDevice::Tick(float DeltaTime)
{
	CleanupInvalidComponents();
}

void FOpenXRHandGestureDevice::SendControllerEvents()
{
	// Check for gestures
	for (FOpenXRHandGestureInputState& RegisteredComponentState : RegisteredComponents)
	{
		if (RegisteredComponentState.IsValid())
		{
			CheckForGestures(RegisteredComponentState);
		}
		else
		{
			// No problem: component will be cleaned up on the next tick
		}
	}
}

void FOpenXRHandGestureDevice::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

bool FOpenXRHandGestureDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FOpenXRHandGestureDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
}

void FOpenXRHandGestureDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values)
{
}

void FOpenXRHandGestureDevice::RegisterComponent(UOpenXRHandPoseComponent* HandPoseComponent)
{
	if (!HandPoseComponent)
		return;

	// TODO: Add check to make sure that this component is not registered already

	RegisteredComponents.Add(FOpenXRHandGestureInputState(HandPoseComponent));
}


void FOpenXRHandGestureDevice::InitKeynames()
{
	// Load key names for gestures

	// Create category
	EKeys::AddMenuCategoryDisplayInfo("HandGestures", LOCTEXT("HandGestureCategory", "Hand Gestures"), TEXT("GraphEditor.PadEvent_16x"));

	// Get all Gesture DB assets
	if (FModuleManager::Get().IsModuleLoaded("AssetRegistry"))
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(UOpenXRGestureDatabase::StaticClass()->GetFName(), AssetData);

		UE_LOG(LogHandGesture, Log, TEXT("Found %d Gesture Databases"), AssetData.Num());

		// Iterate over all found Gesture Databases
		for (int i = 0; i < AssetData.Num(); i++)
		{
			UOpenXRGestureDatabase* GestureDB = Cast<UOpenXRGestureDatabase>(AssetData[i].GetAsset());
			UE_LOG(LogHandGesture, Log, TEXT("Gesture Database %d contains %d gestures"), i, GestureDB->Gestures.Num());

			// Process Database for keynames
			for (const FOpenXRGesture& Gesture : GestureDB->Gestures)
			{
				// Create key(s) for this gesture
				// Format for key name:
				//		HandGesture_Hand_GestureName
				// Format for display name:
				//		[GestureDisplayName] [Hand]

				// First, check if a left hand key should be created
				if (Gesture.Hand & EOpenXRGestureHand::OXR_GESTURE_HAND_LEFT)
				{
					if (!LeftKeyMappings.Contains(Gesture.Name))
					{
						FString GestureName = FString::Printf(TEXT("HandGesture_Left_%s"), *Gesture.Name.ToString());
						FString GestureDisplayName = Gesture.DisplayName.Len() > 0
							? FString::Printf(TEXT("%s Left Hand"), *Gesture.DisplayName) : GestureName;

						// Create the key
						FKey GestureKey(*GestureName);
						EKeys::AddKey(FKeyDetails(GestureKey, FText::FromString(GestureDisplayName), FKeyDetails::GamepadKey, "HandGestures"));

						// Insert into key mappings
						LeftKeyMappings.Add(Gesture.Name, GestureKey);
					}
				}

				// Then check if a right hand key should be created
				if (Gesture.Hand & EOpenXRGestureHand::OXR_GESTURE_HAND_RIGHT)
				{
					if (!RightKeyMappings.Contains(Gesture.Name))
					{
						FString GestureName = FString::Printf(TEXT("HandGesture_Right_%s"), *Gesture.Name.ToString());
						FString GestureDisplayName = Gesture.DisplayName.Len() > 0
							? FString::Printf(TEXT("%s Right Hand"), *Gesture.DisplayName) : GestureName;

						// Create the key
						FKey GestureKey(*GestureName);
						EKeys::AddKey(FKeyDetails(GestureKey, FText::FromString(GestureDisplayName), FKeyDetails::GamepadKey, "HandGestures"));

						// Insert into key mappings
						RightKeyMappings.Add(Gesture.Name, GestureKey);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogHandGesture, Error, TEXT("Asset Registry Module not available!"));
	}
}


void FOpenXRHandGestureDevice::CleanupInvalidComponents()
{
	for (int i = 0; i < RegisteredComponents.Num();)
	{
		if (RegisteredComponents[i].IsValid())
			i++;
		else
			RegisteredComponents.RemoveAt(i);
	}
}

void FOpenXRHandGestureDevice::CheckForGestures(FOpenXRHandGestureInputState& RegisteredComponentState)
{
	UOpenXRHandPoseComponent* HandPoseComponent = RegisteredComponentState.GetHandPoseComponent();
	// Should already have been validated before calling this function
	check(HandPoseComponent);

	// Check we definitely want to detect gestures on this component, and that the gesture db is valid
	if (!HandPoseComponent->bDetectGestures)
		return;
	if (!(HandPoseComponent->GesturesDB))
		return;

	for (const FBPOpenXRActionSkeletalData& SkeletalAction : HandPoseComponent->HandSkeletalActions)
	{
		if (SkeletalAction.SkeletalTransforms.Num() < EHandKeypointCount)
			return;

		// Get Current State
		int32 FingerMap[5] =
		{
			(int32)EXRHandJointType::OXR_HAND_JOINT_THUMB_TIP_EXT,
			(int32)EXRHandJointType::OXR_HAND_JOINT_INDEX_TIP_EXT,
			(int32)EXRHandJointType::OXR_HAND_JOINT_MIDDLE_TIP_EXT,
			(int32)EXRHandJointType::OXR_HAND_JOINT_RING_TIP_EXT,
			(int32)EXRHandJointType::OXR_HAND_JOINT_LITTLE_TIP_EXT
		};

		FVector WristLoc = SkeletalAction.SkeletalTransforms[(int32)EXRHandJointType::OXR_HAND_JOINT_WRIST_EXT].GetLocation();

		// Early fill in an array to keep from performing math for each gesture

		// Get all finger tip locations
		TArray<FVector> CurrentTipLocations;
		CurrentTipLocations.AddUninitialized(5);
		for (int i = 0; i < 5; ++i)
			CurrentTipLocations[i] = SkeletalAction.SkeletalTransforms[FingerMap[i]].GetLocation() - WristLoc;

		// TODO: These should be calculated / per finger
		const float PinchThreshold = 1.0f;
		const float ExtendedThreshold = 12.0f;

		TArray<EOpenXRGestureFingerState> CurrentFingerStates;
		CurrentFingerStates.AddUninitialized(5);

		for (int i = 0; i < 5; ++i)
		{
			// Don't check if the thumb is pinched, check other fingers
			if (i > 0)
			{
				if (FVector::DistSquared(CurrentTipLocations[0], CurrentTipLocations[i]) < PinchThreshold * PinchThreshold)
				{
					CurrentFingerStates[0] = EOpenXRGestureFingerState::OXR_GESTURE_FINGER_PINCHED;
					CurrentFingerStates[i] = EOpenXRGestureFingerState::OXR_GESTURE_FINGER_PINCHED;
					// A finger cannot be considered as extended or closed if it is pinched
					continue;
				}
			}
			 
			// TODO: Investigate other ways of doing this, potentially rotation of base joint of fingers?
			bool Extended = CurrentTipLocations[i].SquaredLength() > ExtendedThreshold * ExtendedThreshold;
			CurrentFingerStates[i] = Extended ? EOpenXRGestureFingerState::OXR_GESTURE_FINGER_EXTENDED : EOpenXRGestureFingerState::OXR_GESTURE_FINGER_CLOSED;
		}

		
		// Check all gestures in the DB
		for (const FOpenXRGesture& Gesture : HandPoseComponent->GesturesDB->Gestures)
		{
			// Find the key for this gesture
			FKey GestureKey;
			bool FoundKey = GetGestureKey(Gesture, SkeletalAction.TargetHand, GestureKey);
			if (!FoundKey)
				return;

			// Check gesture for match
			bool Match = true;

			for (int i = 0; i < 5; ++i)
			{
				if (Gesture.FingerValues[i].FingerState == EOpenXRGestureFingerState::OXR_GESTURE_FINGER_IGNORED)
					continue;

				Match &= Gesture.FingerValues[i].FingerState == CurrentFingerStates[i];
			}

			// Get button state
			if (Match)
			{
				if (!RegisteredComponentState.GetGestureButtonState(Gesture.Name))
				{
					RegisteredComponentState.SetGestureButtonState(Gesture.Name, true);
					bool result = MessageHandler->OnControllerButtonPressed(GestureKey.GetFName(), RegisteredComponentState.GetPlatformUserId(), RegisteredComponentState.GetInputDeviceId(), false);
				}
			}
			else
			{
				if (RegisteredComponentState.GetGestureButtonState(Gesture.Name))
				{
					RegisteredComponentState.SetGestureButtonState(Gesture.Name, false);
					bool result = MessageHandler->OnControllerButtonReleased(GestureKey.GetFName(), RegisteredComponentState.GetPlatformUserId(), RegisteredComponentState.GetInputDeviceId(), false);
				}
			}
		}
	}

}

bool FOpenXRHandGestureDevice::DoesGestureApplyToHand(const EOpenXRGestureHand& GestureHand, const EVRSkeletalHandIndex& Hand) const
{
	switch (GestureHand)
	{
	case EOpenXRGestureHand::OXR_GESTURE_HAND_INVALID:	return false;
	case EOpenXRGestureHand::OXR_GESTURE_HAND_LEFT:		return Hand == EVRSkeletalHandIndex::EActionHandIndex_Left;
	case EOpenXRGestureHand::OXR_GESTURE_HAND_RIGHT:	return Hand == EVRSkeletalHandIndex::EActionHandIndex_Right;
	case EOpenXRGestureHand::OXR_GESTURE_HAND_ANY:		return true;
	default:											return false;
	}
	return false;
}

bool FOpenXRHandGestureDevice::GetGestureKey(const FOpenXRGesture& Gesture, const EVRSkeletalHandIndex& TargetHand, FKey& GestureKey)
{
	// Check if this gesture is applicable for this hand
	if (!DoesGestureApplyToHand(Gesture.Hand, TargetHand))
		return false;

	// Find the key that this hand should trigger
	// ie, a left hand should only trigger keys for left events, even when the gesture can be triggered by either hand

	if (TargetHand == EVRSkeletalHandIndex::EActionHandIndex_Left)
	{
		if (!LeftKeyMappings.Contains(Gesture.Name))
			return false;
		GestureKey = LeftKeyMappings[Gesture.Name];
		return true;
	}
	if (TargetHand == EVRSkeletalHandIndex::EActionHandIndex_Right)
	{
		if (!RightKeyMappings.Contains(Gesture.Name))
			return false;
		GestureKey = RightKeyMappings[Gesture.Name];
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE 
