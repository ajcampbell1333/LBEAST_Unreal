// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "ExperienceTemplates/LBEASTExperienceBase.h"
#include "MovingPlatformExperience.generated.h"

// Forward declarations
class UHapticPlatformController;

/**
 * 5DOF Moving Platform Experience Template
 * 
 * Pre-configured single-player standing VR experience on hydraulic platform.
 * Combines:
 * - 5DOF hydraulic platform (pitch, roll, Y/Z translation)
 * - Safety harness integration
 * - Standing player position
 * - Configurable motion profiles
 * 
 * Perfect for experiences requiring unstable ground simulation, earthquakes,
 * ship decks, moving vehicles, or any standing VR experience with motion.
 */
UCLASS(Blueprintable, ClassGroup=(LBEAST))
class LBEASTCORE_API AMovingPlatformExperience : public ALBEASTExperienceBase
{
	GENERATED_BODY()
	
public:
	AMovingPlatformExperience();

	/** Haptic platform controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|Moving Platform")
	TObjectPtr<UHapticPlatformController> PlatformController;

	/** Maximum pitch angle in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Moving Platform", meta = (ClampMin = "1.0", ClampMax = "15.0"))
	float MaxPitch = 10.0f;

	/** Maximum roll angle in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Moving Platform", meta = (ClampMin = "1.0", ClampMax = "15.0"))
	float MaxRoll = 10.0f;

	/** Maximum vertical translation in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Moving Platform", meta = (ClampMin = "10.0", ClampMax = "200.0"))
	float MaxVerticalTranslation = 100.0f;

	/**
	 * Send motion command to platform
	 * @param Pitch - Target pitch angle in degrees
	 * @param Roll - Target roll angle in degrees
	 * @param VerticalOffset - Vertical translation in cm
	 * @param Duration - Time to reach target (seconds)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Moving Platform")
	void SendPlatformMotion(float Pitch, float Roll, float VerticalOffset, float Duration = 1.0f);

	/**
	 * Return platform to neutral position
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Moving Platform")
	void ReturnToNeutral(float Duration = 2.0f);

	/**
	 * Emergency stop platform motion
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Moving Platform")
	void EmergencyStop();

protected:
	virtual bool InitializeExperienceImpl() override;
	virtual void ShutdownExperienceImpl() override;
};


