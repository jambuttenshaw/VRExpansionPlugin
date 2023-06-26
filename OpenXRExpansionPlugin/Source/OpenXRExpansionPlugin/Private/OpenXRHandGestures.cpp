#include "OpenXRHandGestures.h"

bool operator&(EOpenXRGestureHand A, EOpenXRGestureHand B)
{
	return static_cast<int>(A) & static_cast<int>(B);
}
