#pragma once

#include "CoreMinimal.h"

#include "OpenXRExpansionTypes.h"

#include "OpenXRHandGestures.generated.h"




// finger state
UENUM(BlueprintType)
enum class EOpenXRGestureFingerState : uint8
{
	OXR_GESTURE_FINGER_INVALID = 0,
	OXR_GESTURE_FINGER_IGNORED, 
	OXR_GESTURE_FINGER_CLOSED,
	OXR_GESTURE_FINGER_EXTENDED 
};


USTRUCT(BlueprintType, Category = "VRGestures")
struct OPENXREXPANSIONPLUGIN_API FOpenXRGestureFinger
{
	GENERATED_BODY()
public:

	// The finger index, not editable
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "VRGesture")
		EXRHandJointType IndexType;

	// The state of the finger
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VRGesture")
		EOpenXRGestureFingerState FingerState;

	FOpenXRGestureFinger(EOpenXRGestureFingerState State, EXRHandJointType Type)
	{
		IndexType = Type;
		FingerState = State;
	}
	FOpenXRGestureFinger()
	{
		IndexType = EXRHandJointType::OXR_HAND_JOINT_INDEX_TIP_EXT;
		FingerState = EOpenXRGestureFingerState::OXR_GESTURE_FINGER_INVALID;
	}
};


USTRUCT(BlueprintType, Category = "VRGestures")
struct OPENXREXPANSIONPLUGIN_API FOpenXRGesture
{
	GENERATED_BODY()
public:

	// Name of this gesture
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VRGesture")
		FName Name;

	// States of each finger in this gesture
	UPROPERTY(BlueprintReadWrite, EditAnywhere, EditFixedSize, Category = "VRGesture")
		TArray<FOpenXRGestureFinger> FingerValues;

	FOpenXRGesture()
	{
		Name = NAME_None;
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VRGestures")
		TArray <FOpenXRGesture> Gestures;

	UOpenXRGestureDatabase()
	{
	}
};
