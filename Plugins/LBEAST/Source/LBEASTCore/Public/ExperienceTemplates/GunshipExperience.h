// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "ExperienceTemplates/LBEASTExperienceBase.h"
#include "GunshipExperience.generated.h"

// Forward declarations
class UHapticPlatformController;

/**
 * 5DOF Gunship Experience Template
 * 
 * Pre-configured four-player seated VR experience on hydraulic platform.
 * Combines:
 * - 5DOF hydraulic platform (pitch, roll, Y/Z translation)
 * - Four player seated positions
 * - LAN multiplayer support
 * - Synchronized motion for all players
 * 
 * Perfect for gunship, helicopter, spaceship, or any multi-crew vehicle
 * experiences requiring shared motion simulation.
 */
UCLASS(Blueprintable, ClassGroup=(LBEAST))
class LBEASTCORE_API AGunshipExperience : public ALBEASTExperienceBase
{
	GENERATED_BODY()
	
public:
	AGunshipExperience();

	/** Haptic platform controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|Gunship")
	TObjectPtr<UHapticPlatformController> PlatformController;

	/** Player seat locations (4 seats) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Gunship")
	TArray<FVector> SeatLocations;

	/** Maximum pitch angle in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Gunship", meta = (ClampMin = "1.0", ClampMax = "15.0"))
	float MaxPitch = 10.0f;

	/** Maximum roll angle in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Gunship", meta = (ClampMin = "1.0", ClampMax = "15.0"))
	float MaxRoll = 10.0f;

	/**
	 * Send motion command to platform
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Gunship")
	void SendGunshipMotion(float Pitch, float Roll, float LateralOffset, float VerticalOffset, float Duration = 1.0f);

	/**
	 * Return platform to neutral
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Gunship")
	void ReturnToNeutral(float Duration = 2.0f);

	/**
	 * Emergency stop
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Gunship")
	void EmergencyStop();

	virtual int32 GetMaxPlayers() const override { return 4; }

protected:
	virtual bool InitializeExperienceImpl() override;
	virtual void ShutdownExperienceImpl() override;
};


