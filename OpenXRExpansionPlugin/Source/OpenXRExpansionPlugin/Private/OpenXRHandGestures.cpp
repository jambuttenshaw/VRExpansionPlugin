#include "OpenXRHandGestures.h"

#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "OpenXRExpansionPluginModule"

bool operator&(EOpenXRGestureHand A, EOpenXRGestureHand B)
{
	return static_cast<int>(A) & static_cast<int>(B);
}


#if WITH_EDITOR

EDataValidationResult UOpenXRGestureDatabase::IsDataValid(FDataValidationContext& Context)
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	for (const FOpenXRGesture& Gesture : Gestures)
	{
		if (Gesture.Name == NAME_None)
		{
			Context.AddError(LOCTEXT("InvalidName", "Gesture name cannot be none."));
			Result = EDataValidationResult::Invalid;
		}

		if (Gesture.DisplayName.Len() < 1)
		{
			Context.AddWarning(LOCTEXT("EmptyDisplayName", "Display name is empty."));
			Result = EDataValidationResult::Invalid;
		}

		if (Gesture.Hand == EOpenXRGestureHand::OXR_GESTURE_HAND_INVALID)
		{
			Context.AddError(LOCTEXT("InvalidHand", "Gesture is set to an invalid hand."));
			Result = EDataValidationResult::Invalid;
		}

		bool AllIgnored = true;

		for (const FOpenXRGestureFinger& Finger : Gesture.FingerValues)
		{
			AllIgnored &= Finger.FingerState == EOpenXRGestureFingerState::OXR_GESTURE_FINGER_IGNORED;
			
			if (Finger.FingerState == EOpenXRGestureFingerState::OXR_GESTURE_FINGER_NONE)
			{
				Context.AddWarning(LOCTEXT("InvalidFinger", "Finger is set to an invalid state."));
				Result = EDataValidationResult::Invalid;
			}
		}
		if (AllIgnored)
		{
			Context.AddWarning(LOCTEXT("AllFingersIgnored", "All fingers are ignored: gesture is invalid."));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}

#endif


#undef LOCTEXT_NAMESPACE
