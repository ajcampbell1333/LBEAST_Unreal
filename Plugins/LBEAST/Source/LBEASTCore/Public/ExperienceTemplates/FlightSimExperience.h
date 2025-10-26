// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "ExperienceTemplates/LBEASTExperienceBase.h"
#include "FlightSimExperience.generated.h"

// Forward declarations
class UHapticPlatformController;

/**
 * 2DOF Full 360 Flight Sim Experience Template
 * 
 * Pre-configured single-player flight simulator with gyroscope and HOTAS.
 * Combines:
 * - 2DOF gyroscope system (continuous pitch and roll beyond 360°)
 * - HOTAS controller integration (Logitech X56 or Thrustmaster T.Flight)
 * - Secured cockpit with safety harness
 * - No hydraulics - pure gyroscopic rotation
 * 
 * Perfect for realistic flight arcade games, space combat simulators,
 * and any experience requiring full 360° continuous rotation.
 */
UCLASS(Blueprintable, ClassGroup=(LBEAST))
class LBEASTCORE_API AFlightSimExperience : public ALBEASTExperienceBase
{
	GENERATED_BODY()
	
public:
	AFlightSimExperience();

	/** Gyroscope platform controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|Flight Sim")
	TObjectPtr<UHapticPlatformController> GyroscopeController;

	/** HOTAS controller type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Flight Sim|HOTAS")
	ELBEASTHOTASType HOTASType = ELBEASTHOTASType::LogitechX56;

	/** Enable HOTAS joystick */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Flight Sim|HOTAS")
	bool bEnableJoystick = true;

	/** Enable HOTAS throttle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Flight Sim|HOTAS")
	bool bEnableThrottle = true;

	/** Enable HOTAS pedals */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Flight Sim|HOTAS")
	bool bEnablePedals = false;

	/** Joystick sensitivity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Flight Sim|HOTAS", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float JoystickSensitivity = 1.5f;

	/** Throttle sensitivity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Flight Sim|HOTAS", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float ThrottleSensitivity = 1.0f;

	/** Maximum rotation speed in degrees per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Flight Sim", meta = (ClampMin = "10.0", ClampMax = "180.0"))
	float MaxRotationSpeed = 90.0f;

	/**
	 * Send continuous rotation command
	 * @param Pitch - Target pitch (can exceed 360°)
	 * @param Roll - Target roll (can exceed 360°)
	 * @param Duration - Time to complete rotation
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Flight Sim")
	void SendContinuousRotation(float Pitch, float Roll, float Duration);

	/**
	 * Get current HOTAS joystick input
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Flight Sim|HOTAS")
	FVector2D GetJoystickInput() const;

	/**
	 * Get current HOTAS throttle input
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Flight Sim|HOTAS")
	float GetThrottleInput() const;

	/**
	 * Get current HOTAS pedal input
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Flight Sim|HOTAS")
	float GetPedalInput() const;

	/**
	 * Check if HOTAS is connected
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Flight Sim|HOTAS")
	bool IsHOTASConnected() const;

	/**
	 * Return gyroscope to neutral position
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Flight Sim")
	void ReturnToNeutral(float Duration = 3.0f);

	/**
	 * Emergency stop
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Flight Sim")
	void EmergencyStop();

protected:
	virtual bool InitializeExperienceImpl() override;
	virtual void ShutdownExperienceImpl() override;
};


