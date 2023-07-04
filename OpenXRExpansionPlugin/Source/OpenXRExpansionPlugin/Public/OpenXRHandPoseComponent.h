// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/Texture.h"
#include "Engine/EngineTypes.h"
#include "HeadMountedDisplayTypes.h"
//#include "Runtime/Launch/Resources/Version.h"

#include "Animation/AnimInstanceProxy.h"
#include "OpenXRExpansionTypes.h"
#include "OpenXRHandGestures.h"
#include "Engine/DataAsset.h"

#include "OpenXRHandPoseComponent.generated.h"

USTRUCT(BlueprintType, Category = "VRExpansionFunctions|OpenXR|HandSkeleton")
struct OPENXREXPANSIONPLUGIN_API FBPXRSkeletalRepContainer
{
	GENERATED_BODY()
public:

	UPROPERTY(Transient, NotReplicated)
		EVRSkeletalHandIndex TargetHand;

	UPROPERTY(Transient, NotReplicated)
		bool bAllowDeformingMesh;

	// If true we will skip sending the 4 metacarpal bones that ue4 doesn't need, (STEAMVR skeletons need this disabled!)
	UPROPERTY(Transient, NotReplicated)
		bool bEnableUE4HandRepSavings;

	UPROPERTY(Transient, NotReplicated)
		TArray<FTransform> SkeletalTransforms;

	UPROPERTY(Transient, NotReplicated)
		uint8 BoneCount;


	FBPXRSkeletalRepContainer()
	{
		TargetHand = EVRSkeletalHandIndex::EActionHandIndex_Left;
		bAllowDeformingMesh = false;
		bEnableUE4HandRepSavings = true;
		BoneCount = 0;
	}

	bool bHasValidData()
	{
		return SkeletalTransforms.Num() > 0;
	}

	void CopyForReplication(FBPOpenXRActionSkeletalData& Other);
	static void CopyReplicatedTo(const FBPXRSkeletalRepContainer& Container, FBPOpenXRActionSkeletalData& Other);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits< FBPXRSkeletalRepContainer > : public TStructOpsTypeTraitsBase2<FBPXRSkeletalRepContainer>
{
	enum
	{
		WithNetSerializer = true
	};
};


UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class OPENXREXPANSIONPLUGIN_API UOpenXRHandPoseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOpenXRHandPoseComponent(const FObjectInitializer& ObjectInitializer);

	// Says whether we should run gesture detection
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VRGestures")
		bool bDetectGestures;

	UFUNCTION(BlueprintCallable, Category = "VRGestures")
		void SetDetectGestures(bool bNewDetectGestures)
	{
		bDetectGestures = bNewDetectGestures;
	}

	// Known sequences
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VRGestures")
		UOpenXRGestureDatabase *GesturesDB;
	 
	// Need this as I can't think of another way for an actor component to make sure it isn't on the server
	inline bool IsLocallyControlled() const
	{
#if ENGINE_MAJOR_VERSION > 4 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 22)
		const AActor* MyOwner = GetOwner();
		return MyOwner->HasLocalNetOwner();
#else
		// I like epics new authority check more than mine
		const AActor* MyOwner = GetOwner();
		const APawn* MyPawn = Cast<APawn>(MyOwner);

		return MyPawn ? MyPawn->IsLocallyControlled() : (MyOwner && MyOwner->GetLocalRole() == ENetRole::ROLE_Authority);
#endif
	}

	// Using tick and not timers because skeletal components tick anyway, kind of a waste to make another tick by adding a timer over that
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;


	//virtual void OnUnregister() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkeletalData|Actions")
		bool bGetMockUpPoseForDebugging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkeletalData|Actions")
		TArray<FBPOpenXRActionSkeletalData> HandSkeletalActions;

	UPROPERTY(Replicated, Transient, ReplicatedUsing = OnRep_SkeletalTransformLeft)
		FBPXRSkeletalRepContainer LeftHandRep;

	UPROPERTY(Replicated, Transient, ReplicatedUsing = OnRep_SkeletalTransformRight)
		FBPXRSkeletalRepContainer RightHandRep;

	UFUNCTION(Unreliable, Server, WithValidation)
		void Server_SendSkeletalTransforms(const FBPXRSkeletalRepContainer& SkeletalInfo);

	bool bLerpingPositionLeft;
	bool bReppedOnceLeft;

	bool bLerpingPositionRight;
	bool bReppedOnceRight;

	struct FTransformLerpManager
	{
		bool bReplicatedOnce;
		bool bLerping;
		float UpdateCount;
		float UpdateRate;
		TArray<FTransform> NewTransforms;

		FTransformLerpManager();
		void NotifyNewData(FBPOpenXRActionSkeletalData& ActionInfo, int NetUpdateRate);

		FORCEINLINE void BlendBone(uint8 BoneToBlend, FBPOpenXRActionSkeletalData& ActionInfo, float & LerpVal)
		{
			ActionInfo.SkeletalTransforms[BoneToBlend].Blend(ActionInfo.OldSkeletalTransforms[BoneToBlend], NewTransforms[BoneToBlend], LerpVal);
		}

		void UpdateManager(float DeltaTime, FBPOpenXRActionSkeletalData& ActionInfo);

	}; 
	
	FTransformLerpManager LeftHandRepManager;
	FTransformLerpManager RightHandRepManager;

	UFUNCTION()
	virtual void OnRep_SkeletalTransformLeft()
	{
		for (int i = 0; i < HandSkeletalActions.Num(); i++)
		{
			if (HandSkeletalActions[i].TargetHand == LeftHandRep.TargetHand)
			{
				HandSkeletalActions[i].OldSkeletalTransforms = HandSkeletalActions[i].SkeletalTransforms;

				FBPXRSkeletalRepContainer::CopyReplicatedTo(LeftHandRep, HandSkeletalActions[i]);
				
				if(bSmoothReplicatedSkeletalData)
					LeftHandRepManager.NotifyNewData(HandSkeletalActions[i], ReplicationRateForSkeletalAnimations);
				break;
			}
		}
	}

	UFUNCTION()
	virtual void OnRep_SkeletalTransformRight()
	{
		for (int i = 0; i < HandSkeletalActions.Num(); i++)
		{
			if (HandSkeletalActions[i].TargetHand == RightHandRep.TargetHand)
			{
				HandSkeletalActions[i].OldSkeletalTransforms = HandSkeletalActions[i].SkeletalTransforms;

				FBPXRSkeletalRepContainer::CopyReplicatedTo(RightHandRep, HandSkeletalActions[i]);
				
				if (bSmoothReplicatedSkeletalData)
					RightHandRepManager.NotifyNewData(HandSkeletalActions[i], ReplicationRateForSkeletalAnimations);
				break;
			}
		}
	}

	// If we should replicate the skeletal transform data
	UPROPERTY(EditAnywhere, Category = SkeletalData)
		bool bReplicateSkeletalData;

	// If true we will lerp between updates of the skeletal mesh transforms and smooth the result
	UPROPERTY(EditAnywhere, Category = SkeletalData)
		bool bSmoothReplicatedSkeletalData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletalData)
		float ReplicationRateForSkeletalAnimations;

	// Used in Tick() to accumulate before sending updates, didn't want to use a timer in this case, also used for remotes to lerp position
	float SkeletalNetUpdateCount;
	// Used in Tick() to accumulate before sending updates, didn't want to use a timer in this case, also used for remotes to lerp position
	float SkeletalUpdateCount;

private:
	bool bRegisteredWithHandGestureDevice = false;
};	

USTRUCT()
struct OPENXREXPANSIONPLUGIN_API FOpenXRAnimInstanceProxy : public FAnimInstanceProxy
{
public:
	GENERATED_BODY()

		FOpenXRAnimInstanceProxy() {}
		FOpenXRAnimInstanceProxy(UAnimInstance* InAnimInstance);

		/** Called before update so we can copy any data we need */
		virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;

public:

	EVRSkeletalHandIndex TargetHand;
	TArray<FBPOpenXRActionSkeletalData> HandSkeletalActionData;

};

UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class OPENXREXPANSIONPLUGIN_API UOpenXRAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(transient)
		UOpenXRHandPoseComponent * OwningPoseComp;

	FOpenXRAnimInstanceProxy AnimInstanceProxy;

	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override
	{
		return new FOpenXRAnimInstanceProxy(this);
		//return &AnimInstanceProxy;
	}

	virtual void NativeBeginPlay() override;

	//virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = "BoneMappings")
	void InitializeCustomBoneMapping(UPARAM(ref) FBPOpenXRSkeletalMappingData & SkeletalMappingData);


};
