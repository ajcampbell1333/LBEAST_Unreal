// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "HapticPlatformController.h"
#include "Models/TiltState.h"
#include "Models/ScissorLiftState.h"
#include "4DOFPlatformController.generated.h"

/**
 * 4DOF Platform Controller
 * 
 * Specialized controller for 4DOF motion platforms that use:
 * - Tilt (pitch and roll) via hydraulic actuators
 * - Scissor lift (Y and Z translations) via scissor lift mechanism
 * 
 * Used by Experience Genre Templates:
 * - GunshipExperience (4-player seated gunship)
 * - MovingPlatformExperience (single-player standing platform)
 * - CarSimExperience (single-player seated racing/driving simulator)
 * 
 * This subclass provides struct-based transmission methods for efficient
 * UDP communication with hardware ECUs that support these data models.
 */
UCLASS(ClassGroup=(LBEAST), meta=(BlueprintSpawnableComponent))
class LARGEHAPTICS_API U4DOFPlatformController : public UHapticPlatformController
{
	GENERATED_BODY()

public:
	U4DOFPlatformController();

	/**
	 * Send tilt state (pitch and roll only) as a struct packet
	 * @param TiltState - The tilt state to send
	 * @param Channel - Channel number for the struct packet (default: 100 for tilt structs)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|4DOF")
	void SendTiltStruct(const FTiltState& TiltState, int32 Channel = 100);

	/**
	 * Send scissor lift state (Y and Z translations only) as a struct packet
	 * @param LiftState - The scissor lift state to send
	 * @param Channel - Channel number for the struct packet (default: 101 for scissor lift structs)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|4DOF")
	void SendScissorLiftStruct(const FScissorLiftState& LiftState, int32 Channel = 101);

	/**
	 * Get current tilt state from hardware feedback (bidirectional IO)
	 * @param OutTiltState - Output tilt state received from hardware
	 * @return True if valid tilt state was received
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Haptics|4DOF|Feedback")
	bool GetTiltStateFeedback(FTiltState& OutTiltState) const;

	/**
	 * Get current scissor lift state from hardware feedback (bidirectional IO)
	 * @param OutLiftState - Output scissor lift state received from hardware
	 * @return True if valid lift state was received
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Haptics|4DOF|Feedback")
	bool GetScissorLiftStateFeedback(FScissorLiftState& OutLiftState) const;
};

