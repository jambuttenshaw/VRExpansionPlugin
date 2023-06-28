#pragma once

#include "CoreMinimal.h"

#include "OpenXRExpansionTypes.h"

#include "OpenXRHandGestures.generated.h"




UENUM(BlueprintType)
enum class EOpenXRGestureFingerState : uint8
{
	OXR_GESTURE_FINGER_NONE = 0			UMETA(Hidden),
	OXR_GESTURE_FINGER_IGNORED			UMETA(DisplayName = "Ignored"), 
	OXR_GESTURE_FINGER_CLOSED			UMETA(DisplayName = "Closed"),
	OXR_GESTURE_FINGER_EXTENDED			UMETA(DisplayName = "Extended"),
	OXR_GESTURE_FINGER_PINCHED			UMETA(DisplayName = "Pinched")
};


UENUM(BlueprintType)
enum class EOpenXRGestureHand : uint8
{
	OXR_GESTURE_HAND_INVALID = 0		UMETA(Hidden),
	OXR_GESTURE_HAND_LEFT    = 0b01		UMETA(DisplayName = "Left Hand"),
	OXR_GESTURE_HAND_RIGHT   = 0b10		UMETA(DisplayName = "Right Hand"),
	OXR_GESTURE_HAND_ANY     = 0b11		UMETA(DisplayName = "Any Hand")
};
// Due to the nature of the enum, this is handy
// e.g, instead of checking for either HAND_LEFT or HAND_ANY, can simply & HAND_LEFT
bool operator& (EOpenXRGestureHand A, EOpenXRGestureHand B);


USTRUCT(BlueprintType, Category = "VRGestures")
struct OPENXREXPANSIONPLUGIN_API FOpenXRGestureFinger
{
	GENERATED_BODY()
public:

	// The finger index, not editable
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "VRGesture")
		EXRHandJointType IndexType;

	// The state of the finger
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "VRGesture")
		EOpenXRGestureFingerState FingerState;

	FOpenXRGestureFinger(EOpenXRGestureFingerState State, EXRHandJointType Type)
	{
		IndexType = Type;
		FingerState = State;
	}
	FOpenXRGestureFinger()
	{
		IndexType = EXRHandJointType::OXR_HAND_JOINT_INDEX_TIP_EXT;
		FingerState = EOpenXRGestureFingerState::OXR_GESTURE_FINGER_NONE;
	}
};


USTRUCT(BlueprintType, Category = "VRGestures")
struct OPENXREXPANSIONPLUGIN_API FOpenXRGesture
{
	GENERATED_BODY()
public:

	// Name of this gesture
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "VRGesture")
		FName Name;

	// Display Name of this gesture
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "VRGesture")
		FString DisplayName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "VRGesture")
		EOpenXRGestureHand Hand;

	// States of each finger in this gesture
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, EditFixedSize, Category = "VRGesture")
		TArray<FOpenXRGestureFinger> FingerValues;

	FOpenXRGesture()
	{
		Name = NAME_None;
		DisplayName = TEXT("None");
		Hand = EOpenXRGestureHand::OXR_GESTURE_HAND_ANY;

		InitPoseValues();
	}

private:
	void InitPoseValues()
	{
		FingerValues.Empty();
		FingerValues.Add({ EOpenXRGestureFingerState::OXR_GESTURE_FINGER_IGNORED, EXRHandJointType::OXR_HAND_JOINT_THUMB_TIP_EXT });
		FingerValues.Add({ EOpenXRGestureFingerState::OXR_GESTURE_FINGER_IGNORED, EXRHandJointType::OXR_HAND_JOINT_INDEX_TIP_EXT });
		FingerValues.Add({ EOpenXRGestureFingerState::OXR_GESTURE_FINGER_IGNORED, EXRHandJointType::OXR_HAND_JOINT_MIDDLE_TIP_EXT });
		FingerValues.Add({ EOpenXRGestureFingerState::OXR_GESTURE_FINGER_IGNORED, EXRHandJointType::OXR_HAND_JOINT_RING_TIP_EXT });
		FingerValues.Add({ EOpenXRGestureFingerState::OXR_GESTURE_FINGER_IGNORED, EXRHandJointType::OXR_HAND_JOINT_LITTLE_TIP_EXT });
	}
};


/**
* Items Database DataAsset, here we can save all of our game items
*/
UCLASS(BlueprintType, Category = "VRGestures")
class OPENXREXPANSIONPLUGIN_API UOpenXRGestureDatabase : public UDataAsset
{
	GENERATED_BODY()
public:

	// Gestures in this database
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "VRGestures")
		TArray <FOpenXRGesture> Gestures;

	UOpenXRGestureDatabase()
	{
	}

#if WITH_EDITOR
	EDataValidationResult IsDataValid(class FDataValidationContext& Context) override;
#endif

};
