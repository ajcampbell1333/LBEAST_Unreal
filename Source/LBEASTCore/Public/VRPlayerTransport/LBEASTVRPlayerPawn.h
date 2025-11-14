// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LBEASTVRPlayerPawn.generated.h"

// Forward declarations
class ULBEASTVRPlayerReplicationComponent;
class ULBEASTHandGestureRecognizer;

/**
 * ALBEASTVRPlayerPawn
 * 
 * Base pawn class for VR players in LBEAST experiences.
 * Automatically includes VR replication component for multiplayer hand/HMD tracking.
 * 
 * This is an optional convenience class - you can also add ULBEASTVRPlayerReplicationComponent
 * to any existing pawn class if you prefer.
 * 
 * Usage:
 * 1. Create a Blueprint child of this pawn class
 * 2. Add your VR player mesh/representation
 * 3. The replication component is automatically included
 * 4. Add ULBEASTHandGestureRecognizer if you need gesture recognition
 * 
 * The pawn will automatically replicate HMD and hand tracking data to all clients.
 */
UCLASS(Blueprintable, ClassGroup=(LBEAST))
class LBEASTCORE_API ALBEASTVRPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	ALBEASTVRPlayerPawn(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	/**
	 * Get the VR replication component
	 * @return The VR replication component (nullptr if not found)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|VRPlayer")
	ULBEASTVRPlayerReplicationComponent* GetVRReplicationComponent() const { return VRReplicationComponent; }

	/**
	 * Get the hand gesture recognizer component (if added)
	 * @return The hand gesture recognizer component (nullptr if not found)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|VRPlayer")
	ULBEASTHandGestureRecognizer* GetHandGestureRecognizer() const;

protected:
	/** VR replication component - automatically replicates HMD and hand tracking data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|VRPlayer|Components")
	TObjectPtr<ULBEASTVRPlayerReplicationComponent> VRReplicationComponent;

	/** Whether to automatically create a hand gesture recognizer component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|VRPlayer|Config")
	bool bAutoCreateHandGestureRecognizer = false;
};

