// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "ExperienceTemplates/LBEASTExperienceBase.h"
#include "CarSimExperience.generated.h"

// Forward declarations
class UHapticPlatformController;

/**
 * 5DOF Car Sim Experience Template
 * 
 * Pre-configured single-player racing/driving simulator on hydraulic platform.
 * Combines:
 * - 5DOF hydraulic platform (pitch, roll, Y/Z translation)
 * - Cockpit seating position
 * - Racing wheel and pedal integration support
 * - Motion profiles optimized for driving
 * 
 * Perfect for arcade racing games, driving simulators, and car-themed experiences.
 */
UCLASS(Blueprintable, ClassGroup=(LBEAST))
class LBEASTCORE_API ACarSimExperience : public ALBEASTExperienceBase
{
	GENERATED_BODY()
	
public:
	ACarSimExperience();

	/** Haptic platform controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|Car Sim")
	TObjectPtr<UHapticPlatformController> PlatformController;

	/** Maximum pitch angle in degrees (for acceleration/braking) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Car Sim", meta = (ClampMin = "1.0", ClampMax = "15.0"))
	float MaxPitch = 10.0f;

	/** Maximum roll angle in degrees (for cornering) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Car Sim", meta = (ClampMin = "1.0", ClampMax = "15.0"))
	float MaxRoll = 10.0f;

	/**
	 * Simulate cornering motion (left/right lean)
	 * @param LeanAngle - Negative for left turn, positive for right turn
	 * @param Duration - Transition time
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Car Sim")
	void SimulateCorner(float LeanAngle, float Duration = 0.5f);

	/**
	 * Simulate acceleration/braking motion (pitch forward/back)
	 * @param PitchAngle - Positive for acceleration, negative for braking
	 * @param Duration - Transition time
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Car Sim")
	void SimulateAcceleration(float PitchAngle, float Duration = 0.5f);

	/**
	 * Simulate road bumps
	 * @param Intensity - Bump intensity (0-1)
	 * @param Duration - Duration of bump effect
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Car Sim")
	void SimulateBump(float Intensity, float Duration = 0.2f);

	/**
	 * Return to neutral position
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Car Sim")
	void ReturnToNeutral(float Duration = 1.0f);

	/**
	 * Emergency stop
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Car Sim")
	void EmergencyStop();

protected:
	virtual bool InitializeExperienceImpl() override;
	virtual void ShutdownExperienceImpl() override;
};


