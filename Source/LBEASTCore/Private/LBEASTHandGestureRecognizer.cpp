// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "LBEASTHandGestureRecognizer.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "IXRTrackingSystem.h"
#include "IHandTracker.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "HeadMountedDisplayTypes.h"
#include "Features/IModularFeatures.h"

ULBEASTHandGestureRecognizer::ULBEASTHandGestureRecognizer()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	XRSystem = nullptr;
	HandTracker = nullptr;
	CachedPlayerController = nullptr;
	UpdateTimer = 0.0f;
	bOnlyProcessLocalPlayer = true;  // Default: Only process local player (multiplayer safety)
	FistDetectionThreshold = 2.0f;
	MinFingersClosedForFist = 4;
	UpdateRate = 60.0f;
	LeftHandGesture = ELBEASTHandGesture::None;
	RightHandGesture = ELBEASTHandGesture::None;
}

void ULBEASTHandGestureRecognizer::BeginPlay()
{
	Super::BeginPlay();
	// Auto-initialize if we have a player controller
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		InitializeRecognizer(PC);
	}
}

void ULBEASTHandGestureRecognizer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTimer += DeltaTime;
	float UpdateInterval = 1.0f / UpdateRate;

	if (UpdateTimer >= UpdateInterval)
	{
		UpdateGestureRecognition(UpdateTimer);
		UpdateTimer = 0.0f;
	}
}

bool ULBEASTHandGestureRecognizer::InitializeRecognizer(APlayerController* InPlayerController)
{
	CachedPlayerController = InPlayerController;

	// Get XR system
	XRSystem = GetXRSystem();
	if (!XRSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTHandGestureRecognizer: XR system not available"));
		return false;
	}

	// Get hand tracker
	HandTracker = GetHandTracker();
	if (!HandTracker)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTHandGestureRecognizer: Hand tracker not available"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("LBEASTHandGestureRecognizer: Initialized"));
	return true;
}

bool ULBEASTHandGestureRecognizer::IsHandFistClosed(bool bLeftHand) const
{
	EControllerHand ControllerHand = bLeftHand ? EControllerHand::Left : EControllerHand::Right;
	IHandTracker* Tracker = GetHandTracker();
	if (!Tracker)
	{
		return false;
	}

	// Get hand center (middle knuckle/MCP)
	FTransform HandCenterTransform = GetHandNodeTransform(bLeftHand, EHandKeypoint::MiddleMetacarpal);
	if (HandCenterTransform.Equals(FTransform::Identity))
	{
		return false; // Hand not tracking
	}

	FVector HandCenter = HandCenterTransform.GetLocation();

	// Get all fingertip positions
	TArray<EHandKeypoint> FingertipKeypoints = {
		EHandKeypoint::ThumbTip,
		EHandKeypoint::IndexTip,
		EHandKeypoint::MiddleTip,
		EHandKeypoint::RingTip,
		EHandKeypoint::LittleTip
	};

	int32 FingersClosed = 0;
	for (EHandKeypoint Keypoint : FingertipKeypoints)
	{
		FTransform TipTransform = GetHandNodeTransform(bLeftHand, Keypoint);
		if (TipTransform.Equals(FTransform::Identity))
		{
			continue; // Tip not tracking
		}

		FVector TipPos = TipTransform.GetLocation();
		float DistanceToCenter = FVector::Dist(TipPos, HandCenter);
		
		if (DistanceToCenter < FistDetectionThreshold)
		{
			FingersClosed++;
		}
	}

	return FingersClosed >= MinFingersClosedForFist;
}

FVector ULBEASTHandGestureRecognizer::GetWristPosition(bool bLeftHand) const
{
	FTransform WristTransform = GetHandNodeTransform(bLeftHand, EHandKeypoint::Wrist);
	return WristTransform.GetLocation();
}

FVector ULBEASTHandGestureRecognizer::GetHandCenterPosition(bool bLeftHand) const
{
	FTransform HandCenterTransform = GetHandNodeTransform(bLeftHand, EHandKeypoint::MiddleMetacarpal);
	return HandCenterTransform.GetLocation();
}

void ULBEASTHandGestureRecognizer::GetFingertipPositions(bool bLeftHand, TArray<FVector>& OutPositions) const
{
	OutPositions.Empty(5);
	
	TArray<EHandKeypoint> FingertipKeypoints = {
		EHandKeypoint::ThumbTip,
		EHandKeypoint::IndexTip,
		EHandKeypoint::MiddleTip,
		EHandKeypoint::RingTip,
		EHandKeypoint::LittleTip
	};

	for (EHandKeypoint Keypoint : FingertipKeypoints)
	{
		FTransform TipTransform = GetHandNodeTransform(bLeftHand, Keypoint);
		OutPositions.Add(TipTransform.GetLocation());
	}
}

ELBEASTHandGesture ULBEASTHandGestureRecognizer::GetCurrentGesture(bool bLeftHand) const
{
	return bLeftHand ? LeftHandGesture : RightHandGesture;
}

bool ULBEASTHandGestureRecognizer::IsHandTrackingActive() const
{
	return GetHandTracker() != nullptr;
}

bool ULBEASTHandGestureRecognizer::IsProcessingForLocalPlayer() const
{
	return ShouldProcessGestures();
}

IXRTrackingSystem* ULBEASTHandGestureRecognizer::GetXRSystem() const
{
	if (!XRSystem && GEngine && GEngine->XRSystem.IsValid())
	{
		XRSystem = GEngine->XRSystem.Get();
	}
	return XRSystem;
}

IHandTracker* ULBEASTHandGestureRecognizer::GetHandTracker() const
{
	if (!HandTracker)
	{
		// Access IHandTracker via IModularFeatures (not via IXRTrackingSystem)
		IModularFeatures& ModularFeatures = IModularFeatures::Get();
		FName HandTrackerFeatureName = IHandTracker::GetModularFeatureName();
		if (ModularFeatures.IsModularFeatureAvailable(HandTrackerFeatureName))
		{
			HandTracker = static_cast<IHandTracker*>(ModularFeatures.GetModularFeatureImplementation(HandTrackerFeatureName, 0));
		}
	}
	return HandTracker;
}

FTransform ULBEASTHandGestureRecognizer::GetHandNodeTransform(bool bLeftHand, EHandKeypoint Keypoint) const
{
	// TODO: When VR Player Transport replication is implemented, check for replicated hand node transforms first
	// if (bOnlyProcessLocalPlayer == false && HasReplicatedHandData())
	// {
	//     return GetReplicatedHandNodeTransform(bLeftHand, Keypoint);
	// }
	
	// Current implementation: Use OpenXR APIs (local player only)
	IHandTracker* Tracker = GetHandTracker();
	if (!Tracker)
	{
		return FTransform::Identity;
	}

	EControllerHand ControllerHand = bLeftHand ? EControllerHand::Left : EControllerHand::Right;
	FTransform OutTransform;
	float OutRadius;
	
	if (Tracker->GetKeypointState(ControllerHand, Keypoint, OutTransform, OutRadius))
	{
		return OutTransform;
	}

	return FTransform::Identity;
}

void ULBEASTHandGestureRecognizer::UpdateGestureRecognition(float DeltaTime)
{
	// Only process gestures for locally controlled pawns (multiplayer safety)
	if (!ShouldProcessGestures())
	{
		return;
	}

	if (!IsHandTrackingActive())
	{
		return;
	}

	// Detect gestures for both hands
	ELBEASTHandGesture NewLeftGesture = DetectGesture(true);
	ELBEASTHandGesture NewRightGesture = DetectGesture(false);

	// Fire delegates if gestures changed
	if (NewLeftGesture != LeftHandGesture)
	{
		OnHandGestureDetected.Broadcast(true, NewLeftGesture, 1.0f);
		LeftHandGesture = NewLeftGesture;
	}

	if (NewRightGesture != RightHandGesture)
	{
		OnHandGestureDetected.Broadcast(false, NewRightGesture, 1.0f);
		RightHandGesture = NewRightGesture;
	}
}

ELBEASTHandGesture ULBEASTHandGestureRecognizer::DetectGesture(bool bLeftHand) const
{
	// For now, just detect fist vs open hand
	// Future: Add more gesture recognition (pointing, thumbs up, peace sign, etc.)
	
	if (IsHandFistClosed(bLeftHand))
	{
		return ELBEASTHandGesture::FistClosed;
	}
	
	return ELBEASTHandGesture::HandOpen;
}

bool ULBEASTHandGestureRecognizer::ShouldProcessGestures() const
{
	// If configured to process all players, skip the local-only check
	if (!bOnlyProcessLocalPlayer)
	{
		return true;
	}

	// Get the owner pawn
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	// Check if owner is a pawn
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!OwnerPawn)
	{
		// If not a pawn, check if we have a player controller reference
		if (CachedPlayerController)
		{
			return CachedPlayerController->IsLocalController();
		}
		// If no pawn and no controller, assume single-player (process gestures)
		return true;
	}

	// In multiplayer, only process gestures for locally controlled pawns
	// IsLocallyControlled() returns true only for the local player's pawn
	// This prevents remote players' gesture recognizers from firing events
	return OwnerPawn->IsLocallyControlled();
}

