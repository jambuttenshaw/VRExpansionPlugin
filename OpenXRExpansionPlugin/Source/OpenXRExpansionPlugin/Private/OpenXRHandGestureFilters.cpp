// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenXRHandGestureFilters.h"

FBaseLowpassFilter::FBaseLowpassFilter()
	: Previous(FVector::ZeroVector)
	, bFirstTime(true)
{
}

FVector FBaseLowpassFilter::Filter(const FVector& InValue, const FVector& Alpha)
{
	FVector Result;

	if (bFirstTime)
	{
		Result = InValue;
		bFirstTime = false;
	}
	else
	{
		Result = Alpha * InValue + (FVector::OneVector - Alpha) * Previous;
	}

	Previous = Result;
	PreviousRaw = InValue;
	return Result;
}



FSimpleLowpassFilter::FSimpleLowpassFilter()
	: Cutoff(1.0f)
{
}
FSimpleLowpassFilter::FSimpleLowpassFilter(float InCutoff)
	: Cutoff(InCutoff)
{
}

FVector FSimpleLowpassFilter::Filter(const FVector& InValue, float InDeltaTime)
{
	float Alpha = 1.0f - FMath::Exp(-InDeltaTime * TWO_PI * Cutoff);
	return BaseFilter.Filter(InValue, FVector(Alpha));
}



FOneEuroLowpassFilter::FOneEuroLowpassFilter()
	: MinCutoff(0.9f)
	, DeltaCutoff(1.0f)
	, CutoffSlope(0.007f)
{
}

FOneEuroLowpassFilter::FOneEuroLowpassFilter(const float InMinCutoff, const float InCutoffSlope, const float InDeltaCutoff)
	: MinCutoff(InMinCutoff)
	, DeltaCutoff(InDeltaCutoff)
	, CutoffSlope(InCutoffSlope)
{
}



FVector FOneEuroLowpassFilter::Filter(const FVector& InValue, float InDeltaTime)
{
	if (InDeltaTime <= 0.0f)
	{
		return InValue;
	}

	FVector Delta = RawFilter.bFirstTime ? FVector::ZeroVector : (InValue - RawFilter.PreviousRaw) * 1.0f / InDeltaTime;

	FVector Estimated = DeltaFilter.Filter(Delta, CalculateAlphaTau(FVector(DeltaCutoff), InDeltaTime));
	FVector Cutoff = CalculateCutoff(Estimated);

	return RawFilter.Filter(InValue, CalculateAlphaTau(Cutoff, InDeltaTime));
}

FVector FOneEuroLowpassFilter::CalculateCutoff(const FVector& InValue)
{
	return FVector(MinCutoff) + CutoffSlope * InValue.GetAbs();
}

FVector FOneEuroLowpassFilter::CalculateAlphaTau(const FVector& InCutoff, float InDeltaTime)
{
	FVector tau = FVector::OneVector / (2.0f * PI * InCutoff);
	return FVector::OneVector / (FVector::OneVector + tau / InDeltaTime);
}

