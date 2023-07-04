// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


class ILowpassFilter
{
public:
	virtual ~ILowpassFilter() = default;

	virtual FVector Filter(const FVector& InValue, float InDeltaTime) = 0;
	virtual FVector GetOutput() const = 0;
	virtual void Reset() = 0;
};


class FBaseLowpassFilter
{
public:
	FBaseLowpassFilter()
		: Previous(FVector::ZeroVector)
		, bFirstTime(true)
	{
	}

	FVector Filter(const FVector& InValue, const FVector& Alpha)
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

	void Reset()
	{
		bFirstTime = true;
	}

	// the previous filtered value
	FVector Previous;
	// the previous unfiltered value
	FVector PreviousRaw;

	bool bFirstTime;
};



class FSimpleLowpassFilter : public ILowpassFilter
{
public:
	FSimpleLowpassFilter()
		: Cutoff(1.0f)
	{
	}
	FSimpleLowpassFilter(float InCutoff)
		: Cutoff(InCutoff)
	{
	}

	FVector Filter(const FVector& InValue, float InDeltaTime) override
	{
		float Alpha = 1.0f - FMath::Exp(-InDeltaTime * TWO_PI * Cutoff);
		return BaseFilter.Filter(InValue, FVector(Alpha));
	}

	FVector GetOutput() const override { return BaseFilter.Previous; }
	void Reset() override { BaseFilter.Reset(); }


	// Cutoff frequency in Hertz
	float Cutoff;

private:
	FBaseLowpassFilter BaseFilter;
};


class FOneEuroLowpassFilter : public ILowpassFilter
{
public:
	FOneEuroLowpassFilter()
		: MinCutoff(0.9f)
		, DeltaCutoff(1.0f)
		, CutoffSlope(0.007f)
	{
	}

	FOneEuroLowpassFilter(const float InMinCutoff, const float InCutoffSlope, const float InDeltaCutoff)
		: MinCutoff(InMinCutoff)
		, DeltaCutoff(InDeltaCutoff)
		, CutoffSlope(InCutoffSlope)
	{
	}


	FVector Filter(const FVector& InValue, float InDeltaTime) override
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

	FVector GetOutput() const override { return RawFilter.Previous; }
	void Reset()
	{
		RawFilter.Reset();
		DeltaFilter.Reset();
	}


	// The smaller the value the less jitter and the more lag with micro movements
	float MinCutoff;

	// If latency is too high with fast movements increase this value
	float DeltaCutoff;

	// This is the magnitude of adjustment
	float CutoffSlope;

private:
	FVector CalculateCutoff(const FVector& InValue)
	{
		return FVector(MinCutoff) + CutoffSlope * InValue.GetAbs();
	}

	FVector CalculateAlphaTau(const FVector& InCutoff, float InDeltaTime)
	{
		FVector tau = FVector::OneVector / (2.0f * PI * InCutoff);
		return FVector::OneVector / (FVector::OneVector + tau / InDeltaTime);
	}

private:

	FBaseLowpassFilter RawFilter;
	FBaseLowpassFilter DeltaFilter;
};
