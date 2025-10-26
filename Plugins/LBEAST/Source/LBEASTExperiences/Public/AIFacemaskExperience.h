// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "LBEASTExperienceBase.h"
#include "AIFacemaskExperience.generated.h"

// Forward declarations
class UAIFaceController;
class UEmbeddedDeviceController;
class UExperienceStateMachine;

/**
 * AI Facemask Experience Template
 * 
 * Pre-configured experience for LAN multiplayer VR with immersive theater live actors.
 * 
 * Architecture:
 * - AI facial animation operates AUTONOMOUSLY (driven by NVIDIA Audio2Face)
 * - Live actors wear wrist-mounted button controls (4 buttons: 2 left, 2 right)
 * - Buttons control the Experience Loop state machine (not the AI face)
 * 
 * Button Layout:
 * - Left Wrist:  Button 0 (Forward), Button 1 (Backward)
 * - Right Wrist: Button 2 (Forward), Button 3 (Backward)
 * 
 * The live actor directs the experience flow, while the AI face handles
 * natural conversation and emotional responses autonomously.
 * 
 * Perfect for interactive theater, escape rooms, and narrative-driven LBE experiences
 * requiring professional performers to guide players through story beats.
 */
UCLASS(Blueprintable, ClassGroup=(LBEAST))
class LBEASTCORE_API AAIFacemaskExperience : public ALBEASTExperienceBase
{
	GENERATED_BODY()
	
public:
	AAIFacemaskExperience();

	/** Reference to the live actor's skeletal mesh for facial animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Facemask")
	TObjectPtr<USkeletalMeshComponent> LiveActorMesh;

	/** AI Face controller component (autonomous, driven by Neural Face) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|AI Facemask|Components")
	TObjectPtr<UAIFaceController> FaceController;

	/** Embedded systems controller for wrist-mounted buttons */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|AI Facemask|Components")
	TObjectPtr<UEmbeddedDeviceController> CostumeController;

	/** Experience Loop state machine */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|AI Facemask|Components")
	TObjectPtr<UExperienceStateMachine> ExperienceLoop;

	/** Enable passthrough for live actors to help players */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Facemask|Config")
	bool bEnableLiveActorPassthrough = true;

	/** Number of live actor roles in this experience */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Facemask|Config", meta = (ClampMin = "1", ClampMax = "4"))
	int32 NumberOfLiveActors = 1;

	/** Number of player roles in this experience */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Facemask|Config", meta = (ClampMin = "1", ClampMax = "8"))
	int32 NumberOfPlayers = 1;

	/**
	 * Get the current experience state
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|AI Facemask|Experience Loop")
	FName GetCurrentExperienceState() const;

	/**
	 * Manually advance the experience to the next state (usually triggered by buttons)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Facemask|Experience Loop")
	bool AdvanceExperience();

	/**
	 * Manually retreat the experience to the previous state (usually triggered by buttons)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Facemask|Experience Loop")
	bool RetreatExperience();

	virtual int32 GetMaxPlayers() const override { return NumberOfLiveActors + NumberOfPlayers; }

protected:
	virtual bool InitializeExperienceImpl() override;
	virtual void ShutdownExperienceImpl() override;
	virtual void Tick(float DeltaTime) override;

private:
	/** Handle button input from wrist-mounted controls */
	void ProcessButtonInput();

	/** Handle state change events */
	UFUNCTION()
	void OnExperienceStateChanged(FName OldState, FName NewState, int32 NewStateIndex);

	/** Previous button states for edge detection */
	bool PreviousButtonStates[4] = {false, false, false, false};
};

