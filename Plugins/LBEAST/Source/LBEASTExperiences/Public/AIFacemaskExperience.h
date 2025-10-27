// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "LBEASTExperienceBase.h"
#include "Networking/LBEASTServerBeacon.h"
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
 * NETWORK ARCHITECTURE (REQUIRED):
 * This experience REQUIRES a dedicated server setup:
 * - Separate local PC running headless dedicated server
 * - Same PC processes AI workflow: Speech Recognition → NLU → Emotion → Audio2Face
 * - Omniverse Audio2Face streams facial animation to HMDs over network
 * - Offloads AI processing from HMDs for optimal performance
 * - Supports parallelization for multiple live actors
 * 
 * ServerMode is ENFORCED to DedicatedServer - attempting to use Listen Server will fail.
 * 
 * LIVE ACTOR CONTROLS:
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
class LBEASTEXPERIENCES_API AAIFacemaskExperience : public ALBEASTExperienceBase
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

	/** Server beacon for automatic discovery/connection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|AI Facemask|Components")
	TObjectPtr<ULBEASTServerBeacon> ServerBeacon;

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
	 * Request to advance the experience (input-agnostic, works with any input source)
	 * Call this from any input source (EmbeddedSystems, VR controllers, keyboard, etc.)
	 * Automatically handles server RPC if called on client
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Facemask|Experience Loop")
	void RequestAdvanceExperience();

	/**
	 * Request to retreat the experience (input-agnostic, works with any input source)
	 * Call this from any input source (EmbeddedSystems, VR controllers, keyboard, etc.)
	 * Automatically handles server RPC if called on client
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Facemask|Experience Loop")
	void RequestRetreatExperience();

	/**
	 * Server RPC: Advance experience (called automatically by RequestAdvanceExperience)
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAdvanceExperience();

	/**
	 * Server RPC: Retreat experience (called automatically by RequestRetreatExperience)
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRetreatExperience();

	virtual int32 GetMaxPlayers() const override { return NumberOfLiveActors + NumberOfPlayers; }

protected:
	virtual bool InitializeExperienceImpl() override;
	virtual void ShutdownExperienceImpl() override;
	virtual void Tick(float DeltaTime) override;

	/**
	 * Process input from VR controllers (for listen server hosts or Blueprint extension)
	 * Override in Blueprint to add VR controller input support
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "LBEAST|AI Facemask|Input")
	void ProcessVRControllerInput();
	virtual void ProcessVRControllerInput_Implementation() {}

private:
	/**
	 * Process input from wrist-mounted embedded system buttons
	 * Only runs on authority (server or listen server host)
	 */
	void ProcessEmbeddedSystemInput();

	/**
	 * Internal: Advance experience on server authority
	 * Only called on server after authority check
	 */
	bool AdvanceExperienceInternal();

	/**
	 * Internal: Retreat experience on server authority
	 * Only called on server after authority check
	 */
	bool RetreatExperienceInternal();

	/** Handle state change events */
	UFUNCTION()
	void OnExperienceStateChanged(FName OldState, FName NewState, int32 NewStateIndex);

	/** Handle server discovery (auto-connect) */
	UFUNCTION()
	void OnServerDiscovered(const FLBEASTServerInfo& ServerInfo);

	/** Previous button states for edge detection (embedded systems) */
	bool PreviousEmbeddedButtonStates[4] = {false, false, false, false};
};

