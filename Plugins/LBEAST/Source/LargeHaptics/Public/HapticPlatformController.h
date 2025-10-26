// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HapticPlatformController.generated.h"

/**
 * Platform type enumeration
 */
UENUM(BlueprintType)
enum class ELBEASTPlatformType : uint8
{
	MovingPlatform_SinglePlayer UMETA(DisplayName = "5DOF Moving Platform (Single Player)"),
	Gunship_FourPlayer UMETA(DisplayName = "5DOF Gunship (Four Player)"),
	CarSim_SinglePlayer UMETA(DisplayName = "5DOF Car Sim (Single Player)"),
	FlightSim_2DOF UMETA(DisplayName = "2DOF Full 360 Flight Sim"),
	Custom UMETA(DisplayName = "Custom Configuration")
};

/**
 * Actuator configuration for a single hydraulic cylinder
 */
USTRUCT(BlueprintType)
struct FHydraulicActuator
{
	GENERATED_BODY()

	/** Unique identifier for this actuator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	FName ActuatorID;

	/** Current extension (0.0 = fully retracted, 1.0 = fully extended) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float Extension = 0.5f;

	/** Position of this actuator relative to platform center */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	FVector RelativePosition = FVector::ZeroVector;

	/** Maximum extension range in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float MaxExtensionCm = 30.0f;
};

/**
 * Supported HOTAS controller types
 */
UENUM(BlueprintType)
enum class ELBEASTHOTASType : uint8
{
	None UMETA(DisplayName = "None (Standard VR Controllers)"),
	LogitechX56 UMETA(DisplayName = "Logitech G X56"),
	ThrustmasterTFlight UMETA(DisplayName = "Thrustmaster T.Flight"),
	Custom UMETA(DisplayName = "Custom HOTAS")
};

/**
 * Gyroscope configuration for 2DOF flight simulators
 */
USTRUCT(BlueprintType)
struct FGyroscopeConfig
{
	GENERATED_BODY()

	/** Enable continuous rotation beyond 360 degrees on pitch axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	bool bEnableContinuousPitch = true;

	/** Enable continuous rotation beyond 360 degrees on roll axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	bool bEnableContinuousRoll = true;

	/** Maximum rotation speed in degrees per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float MaxRotationSpeed = 60.0f;

	/** Pitch axis invert */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	bool bInvertPitchAxis = false;

	/** Roll axis invert */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	bool bInvertRollAxis = false;

	/** HOTAS controller type to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics|HOTAS")
	ELBEASTHOTASType HOTASType = ELBEASTHOTASType::None;

	/** Enable HOTAS joystick input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics|HOTAS")
	bool bEnableJoystick = true;

	/** Enable HOTAS throttle input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics|HOTAS")
	bool bEnableThrottle = true;

	/** Enable pedal controls */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics|HOTAS")
	bool bEnablePedals = false;

	/** Joystick sensitivity multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics|HOTAS", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float JoystickSensitivity = 1.0f;

	/** Throttle sensitivity multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics|HOTAS", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float ThrottleSensitivity = 1.0f;
};

/**
 * Platform configuration
 */
USTRUCT(BlueprintType)
struct FHapticPlatformConfig
{
	GENERATED_BODY()

	/** Type of platform */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	ELBEASTPlatformType PlatformType = ELBEASTPlatformType::MovingPlatform_SinglePlayer;

	/** Array of hydraulic actuators */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	TArray<FHydraulicActuator> Actuators;

	/** Maximum pitch angle in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float MaxPitchDegrees = 10.0f;

	/** Maximum roll angle in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float MaxRollDegrees = 10.0f;

	/** Maximum Y translation in cm (scissor lift) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float MaxTranslationY = 100.0f;

	/** Maximum Z translation in cm (scissor lift) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float MaxTranslationZ = 100.0f;

	/** Gyroscope configuration (for 2DOF Flight Sim) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	FGyroscopeConfig GyroscopeConfig;

	/** Network address of the platform controller hardware */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	FString ControllerIPAddress = TEXT("192.168.1.100");

	/** Network port for platform controller */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	int32 ControllerPort = 8080;
};

/**
 * Motion command for platform
 */
USTRUCT(BlueprintType)
struct FPlatformMotionCommand
{
	GENERATED_BODY()

	/** Target pitch angle in degrees (for 2DOF gyroscope: unlimited; for 5DOF: clamped to MaxPitchDegrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float Pitch = 0.0f;

	/** Target roll angle in degrees (for 2DOF gyroscope: unlimited; for 5DOF: clamped to MaxRollDegrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float Roll = 0.0f;

	/** Target Y translation in cm (5DOF platforms only) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float TranslationY = 0.0f;

	/** Target Z translation in cm (5DOF platforms only) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float TranslationZ = 0.0f;

	/** Duration to reach target position (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	float Duration = 1.0f;

	/** Use continuous rotation (2DOF gyroscope only - allows rotation beyond 360 degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	bool bUseContinuousRotation = false;
};

/**
 * Haptic Platform Controller Component
 * 
 * Controls large-scale motion platforms including:
 * - 5DOF Moving Platform (single player standing)
 * - 5DOF Gunship (four player seated)
 * - 5DOF Car Sim (single player seated racing/driving simulator)
 * - 2DOF Full 360 Flight Sim (single player gyroscope with continuous rotation)
 * 
 * Provides both high-level motion commands and low-level actuator control.
 */
UCLASS(ClassGroup=(LBEAST), meta=(BlueprintSpawnableComponent))
class LARGEHAPTICS_API UHapticPlatformController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHapticPlatformController();

	/** Platform configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Haptics")
	FHapticPlatformConfig Config;

	/**
	 * Initialize the haptic platform system
	 * @param InConfig - Configuration settings
	 * @return true if initialization was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics")
	bool InitializePlatform(const FHapticPlatformConfig& InConfig);

	/**
	 * Send a motion command to the platform (advanced - uses absolute angles)
	 * @param Command - The motion command to execute
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|Advanced")
	void SendMotionCommand(const FPlatformMotionCommand& Command);

	/**
	 * Send normalized platform motion (recommended for game code)
	 * Uses joystick-style input that automatically scales to hardware capabilities
	 * @param TiltX - Left/Right tilt (-1.0 = full left, +1.0 = full right, 0.0 = level)
	 * @param TiltY - Forward/Backward tilt (-1.0 = full backward, +1.0 = full forward, 0.0 = level)
	 * @param VerticalOffset - Vertical translation (-1.0 to +1.0, normalized to max capability)
	 * @param Duration - Time to reach target position (seconds)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics")
	void SendNormalizedMotion(float TiltX, float TiltY, float VerticalOffset = 0.0f, float Duration = 1.0f);

	/**
	 * Set a specific actuator extension
	 * @param ActuatorID - ID of the actuator to control
	 * @param Extension - Extension value (0.0 - 1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics")
	void SetActuatorExtension(FName ActuatorID, float Extension);

	/**
	 * Emergency stop - immediately halt all platform motion
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics")
	void EmergencyStop();

	/**
	 * Return platform to neutral position
	 * @param Duration - Time to return to neutral (seconds)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics")
	void ReturnToNeutral(float Duration = 2.0f);

	/**
	 * Get current platform transform relative to neutral
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics")
	FTransform GetCurrentPlatformTransform() const;

	/**
	 * Get HOTAS joystick input (X, Y axes)
	 * @return Vector2D with X (roll) and Y (pitch) input from joystick (-1.0 to 1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|HOTAS")
	FVector2D GetHOTASJoystickInput() const;

	/**
	 * Get HOTAS throttle input
	 * @return Throttle value (0.0 to 1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|HOTAS")
	float GetHOTASThrottleInput() const;

	/**
	 * Get HOTAS pedal input (if enabled)
	 * @return Pedal value (-1.0 to 1.0, left to right)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|HOTAS")
	float GetHOTASPedalInput() const;

	/**
	 * Check if HOTAS is connected and responding
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|HOTAS")
	bool IsHOTASConnected() const;

	/**
	 * Get the currently configured HOTAS type
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Haptics|HOTAS")
	ELBEASTHOTASType GetHOTASType() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Whether the system is initialized and connected */
	bool bIsInitialized = false;

	/** Current platform state */
	FPlatformMotionCommand CurrentState;

	/** Target platform state */
	FPlatformMotionCommand TargetState;

	/** Time remaining for current motion */
	float MotionTimeRemaining = 0.0f;

	/** Total duration of current motion */
	float MotionTotalDuration = 0.0f;

	/** HOTAS joystick input cache */
	FVector2D HOTASJoystickInput = FVector2D::ZeroVector;

	/** HOTAS throttle input cache */
	float HOTASThrottleInput = 0.0f;

	/** HOTAS pedal input cache */
	float HOTASPedalInput = 0.0f;

	/** Whether HOTAS is connected */
	bool bHOTASConnected = false;

	/**
	 * Send command to hardware controller
	 */
	void SendCommandToHardware(const FPlatformMotionCommand& Command);

	/**
	 * Interpolate between current and target state
	 */
	void UpdateMotionInterpolation(float DeltaTime);

	/**
	 * Update HOTAS input state
	 */
	void UpdateHOTASInput();

	/**
	 * Initialize HOTAS controller connection
	 */
	bool InitializeHOTAS();
};


