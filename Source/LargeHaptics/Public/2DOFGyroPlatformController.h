// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "HapticPlatformController.h"
#include "Models/GyroState.h"
#include "2DOFGyroPlatformController.generated.h"

/**
 * 2DOF Gyroscope Platform Controller
 * 
 * Specialized controller for 2DOF continuous rotation gyroscopes that use:
 * - Continuous pitch rotation (servo motor, no limit switches)
 * - Continuous roll rotation (servo motor, no limit switches)
 * 
 * Used by Experience Genre Templates:
 * - FlightSimExperience (single-player flight simulator with HOTAS)
 * 
 * This subclass provides struct-based transmission methods for efficient
 * UDP communication with hardware ECUs that support continuous rotation.
 */
UCLASS(ClassGroup=(LBEAST), meta=(BlueprintSpawnableComponent))
class LARGEHAPTICS_API U2DOFGyroPlatformController : public UHapticPlatformController
{
	GENERATED_BODY()

public:
	U2DOFGyroPlatformController();

	/**
	 * Send gyroscope state (pitch and roll with continuous rotation) as a struct packet
	 * @param GyroState - The gyroscope state to send (unlimited degrees)
	 * @param Channel - Channel number for the struct packet (default: 102 for gyro structs)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|2DOF Gyro")
	void SendGyroStruct(const FGyroState& GyroState, int32 Channel = 102);

	/**
	 * Send continuous rotation from normalized joystick input
	 * @param NormalizedPitch - Joystick Y axis (-1.0 = full backward rotation, +1.0 = full forward rotation)
	 * @param NormalizedRoll - Joystick X axis (-1.0 = full left rotation, +1.0 = full right rotation)
	 * @param DeltaTime - Time since last update (seconds)
	 * @param CurrentPitch - Current pitch angle (for cumulative rotation)
	 * @param CurrentRoll - Current roll angle (for cumulative rotation)
	 * @param Channel - Channel number for the struct packet (default: 102)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|2DOF Gyro")
	void SendGyroFromNormalized(float NormalizedPitch, float NormalizedRoll, float DeltaTime, float CurrentPitch = 0.0f, float CurrentRoll = 0.0f, int32 Channel = 102);
};

