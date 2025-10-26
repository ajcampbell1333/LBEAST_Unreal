// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "LBEASTExperienceBase.h"
#include "AIFacemaskExperience.generated.h"

// Forward declarations
class UAIFaceController;
class UEmbeddedDeviceController;

/**
 * AI Facemask Experience Template
 * 
 * Pre-configured experience for LAN multiplayer VR with immersive theater actors.
 * Combines:
 * - AI facial animation on actor avatars
 * - Embedded systems for costume controls
 * - LAN multiplayer support
 * - Narrative state machine integration
 * 
 * Perfect for experiences where one or more players are professional actors
 * driving avatars with AI-generated faces and interactive costume elements.
 */
UCLASS(Blueprintable, ClassGroup=(LBEAST))
class LBEASTCORE_API AAIFacemaskExperience : public ALBEASTExperienceBase
{
	GENERATED_BODY()
	
public:
	AAIFacemaskExperience();

	/** Reference to the actor's skeletal mesh for facial animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Facemask")
	TObjectPtr<USkeletalMeshComponent> ActorMesh;

	/** AI Face controller component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|AI Facemask")
	TObjectPtr<UAIFaceController> FaceController;

	/** Embedded systems controller for costume controls */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|AI Facemask")
	TObjectPtr<UEmbeddedDeviceController> CostumeController;

	/** Enable passthrough for actors to help players */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Facemask")
	bool bEnableActorPassthrough = true;

	/** Number of actor roles in this experience */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Facemask", meta = (ClampMin = "1", ClampMax = "4"))
	int32 NumberOfActors = 1;

	/** Number of player roles in this experience */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Facemask", meta = (ClampMin = "1", ClampMax = "8"))
	int32 NumberOfPlayers = 1;

	/**
	 * Set the facial expression blend shape weights
	 * @param BlendShapes - Map of blend shape names to weights (0-1)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Facemask")
	void SetFacialExpression(const TMap<FName, float>& BlendShapes);

	/**
	 * Trigger a costume haptic effect
	 * @param Channel - Embedded device channel
	 * @param Intensity - Haptic intensity (0-1)
	 * @param Duration - Duration in seconds
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Facemask")
	void TriggerCostumeHaptic(int32 Channel, float Intensity, float Duration);

	virtual int32 GetMaxPlayers() const override { return NumberOfActors + NumberOfPlayers; }

protected:
	virtual bool InitializeExperienceImpl() override;
	virtual void ShutdownExperienceImpl() override;
};

