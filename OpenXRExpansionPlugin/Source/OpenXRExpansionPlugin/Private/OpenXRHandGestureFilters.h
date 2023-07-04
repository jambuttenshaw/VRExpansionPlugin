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


// Simple exponential lowpass filter implementation
class FBaseLowpassFilter
{
public:
	FBaseLowpassFilter();

	FVector Filter(const FVector& InValue, const FVector& Alpha);

	inline void Reset() { bFirstTime = true; }

	// the previous filtered value
	FVector Previous;
	// the previous unfiltered value
	FVector PreviousRaw;

	bool bFirstTime;
};


// Simple exponential lowpass filter implementation, that takes a cutoff parameter given in Hz
class FSimpleLowpassFilter : public ILowpassFilter
{
public:
	FSimpleLowpassFilter();
	FSimpleLowpassFilter(float InCutoff);

	FVector Filter(const FVector& InValue, float InDeltaTime) override;

	inline FVector GetOutput() const override { return BaseFilter.Previous; }
	inline void Reset() override { BaseFilter.Reset(); }


	// Cutoff frequency in Hertz
	float Cutoff;

private:
	FBaseLowpassFilter BaseFilter;
};


// One Euro Filter implementation, based upon implementation found here: https://gery.casiez.net/1euro/1efilter.cc
class FOneEuroLowpassFilter : public ILowpassFilter
{
public:
	FOneEuroLowpassFilter();
	FOneEuroLowpassFilter(const float InMinCutoff, const float InCutoffSlope, const float InDeltaCutoff);

	FVector Filter(const FVector& InValue, float InDeltaTime) override;

	inline FVector GetOutput() const override { return RawFilter.Previous; }
	inline void Reset()
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
	FVector CalculateCutoff(const FVector& InValue);
	FVector CalculateAlphaTau(const FVector& InCutoff, float InDeltaTime);

private:
	FBaseLowpassFilter RawFilter;
	FBaseLowpassFilter DeltaFilter;
};
