// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "HapticPlatformController.h"
#include "IPAddress.h"

UHapticPlatformController::UHapticPlatformController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHapticPlatformController::BeginPlay()
{
	Super::BeginPlay();
	
	// Auto-initialize if config is set
	if (!Config.ControllerIPAddress.IsEmpty())
	{
		InitializePlatform(Config);
	}
}

void UHapticPlatformController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShutdownUDPConnection();
	Super::EndPlay(EndPlayReason);
}

void UHapticPlatformController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsInitialized)
	{
		return;
	}

	// Receive data from hardware (bidirectional IO) - handled by base class
	// Base class TickComponent already calls ProcessIncomingUDPData()

	// Update HOTAS input if enabled
	if (bHOTASConnected)
	{
		UpdateHOTASInput();
	}

	// Update motion interpolation if we're in motion
	if (MotionTimeRemaining > 0.0f)
	{
		UpdateMotionInterpolation(DeltaTime);
	}
}

bool UHapticPlatformController::InitializePlatform(const FHapticPlatformConfig& InConfig)
{
	Config = InConfig;

	// NOOP: TODO - Establish network connection to hardware controller
	// For now, just validate config
	if (Config.ControllerIPAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("HapticPlatformController: Cannot initialize - no controller IP address specified"));
		return false;
	}

	// Initialize actuators based on platform type
	if (Config.Actuators.Num() == 0)
	{
		switch (Config.PlatformType)
		{
		case ELBEASTPlatformType::MovingPlatform_SinglePlayer:
			// Standard 4-actuator configuration
			Config.Actuators.Add(FHydraulicActuator{ FName("FrontLeft"), 0.5f, FVector(-50, -50, 0), 30.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("FrontRight"), 0.5f, FVector(50, -50, 0), 30.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("RearLeft"), 0.5f, FVector(-50, 50, 0), 30.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("RearRight"), 0.5f, FVector(50, 50, 0), 30.0f });
			break;

		case ELBEASTPlatformType::Gunship_FourPlayer:
			// Larger 6-actuator configuration for multi-player platform
			Config.Actuators.Add(FHydraulicActuator{ FName("FrontLeft"), 0.5f, FVector(-100, -100, 0), 40.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("FrontCenter"), 0.5f, FVector(0, -100, 0), 40.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("FrontRight"), 0.5f, FVector(100, -100, 0), 40.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("RearLeft"), 0.5f, FVector(-100, 100, 0), 40.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("RearCenter"), 0.5f, FVector(0, 100, 0), 40.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("RearRight"), 0.5f, FVector(100, 100, 0), 40.0f });
			break;

		case ELBEASTPlatformType::CarSim_SinglePlayer:
			// Standard 4-actuator configuration for car/racing simulator (same as moving platform)
			Config.Actuators.Add(FHydraulicActuator{ FName("FrontLeft"), 0.5f, FVector(-50, -50, 0), 30.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("FrontRight"), 0.5f, FVector(50, -50, 0), 30.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("RearLeft"), 0.5f, FVector(-50, 50, 0), 30.0f });
			Config.Actuators.Add(FHydraulicActuator{ FName("RearRight"), 0.5f, FVector(50, 50, 0), 30.0f });
			break;

		case ELBEASTPlatformType::FlightSim_2DOF:
			// 2DOF gyroscope system - no hydraulic actuators needed
			// All control is via gyroscope rotation on two axes
			UE_LOG(LogTemp, Log, TEXT("HapticPlatformController: 2DOF Flight Sim - using gyroscope control"));
			break;

		default:
			break;
		}
	}

	// Initialize UDP connection to hardware controller (uses base class)
	if (!InitializeUDPConnection(Config.ControllerIPAddress, Config.ControllerPort, TEXT("LBEAST_HapticPlatform")))
	{
		UE_LOG(LogTemp, Error, TEXT("HapticPlatformController: Failed to initialize UDP connection"));
		return false;
	}

	// Initialize HOTAS if this is a flight sim
	if (Config.PlatformType == ELBEASTPlatformType::FlightSim_2DOF && Config.GyroscopeConfig.HOTASType != ELBEASTHOTASType::None)
	{
		if (!InitializeHOTAS())
		{
			UE_LOG(LogTemp, Warning, TEXT("HapticPlatformController: HOTAS initialization failed, continuing without HOTAS"));
		}
	}

	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("HapticPlatformController: Initialized successfully with %d actuators"), Config.Actuators.Num());
	return true;
}

void UHapticPlatformController::SendMotionCommand(const FPlatformMotionCommand& Command, bool bUseStructPacket)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("HapticPlatformController: Cannot send command - not initialized"));
		return;
	}

	TargetState = Command;

	// Handle rotation based on platform type
	if (Config.PlatformType == ELBEASTPlatformType::FlightSim_2DOF && Command.bUseContinuousRotation)
	{
		// 2DOF Flight Sim: Allow unlimited rotation for gyroscope control
		// No clamping - allow values beyond 360 degrees for continuous rotation
		TargetState.Pitch = Command.Pitch;
		TargetState.Roll = Command.Roll;
		// Translation not applicable for gyroscope system
		TargetState.TranslationY = 0.0f;
		TargetState.TranslationZ = 0.0f;
	}
	else
	{
		// 4DOF platforms: Clamp to configured limits
		TargetState.Pitch = FMath::Clamp(Command.Pitch, -Config.MaxPitchDegrees, Config.MaxPitchDegrees);
		TargetState.Roll = FMath::Clamp(Command.Roll, -Config.MaxRollDegrees, Config.MaxRollDegrees);
		TargetState.TranslationY = FMath::Clamp(Command.TranslationY, -Config.MaxTranslationY, Config.MaxTranslationY);
		TargetState.TranslationZ = FMath::Clamp(Command.TranslationZ, -Config.MaxTranslationZ, Config.MaxTranslationZ);
	}

	MotionTimeRemaining = Command.Duration;
	MotionTotalDuration = Command.Duration;

	SendCommandToHardware(TargetState, bUseStructPacket);
}

void UHapticPlatformController::SendNormalizedMotion(float TiltX, float TiltY, float ForwardOffset, float VerticalOffset, float Duration)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("HapticPlatformController: Cannot send normalized motion - not initialized"));
		return;
	}

	// Clamp inputs to valid range
	TiltX = FMath::Clamp(TiltX, -1.0f, 1.0f);
	TiltY = FMath::Clamp(TiltY, -1.0f, 1.0f);
	ForwardOffset = FMath::Clamp(ForwardOffset, -1.0f, 1.0f);
	VerticalOffset = FMath::Clamp(VerticalOffset, -1.0f, 1.0f);

	// Map normalized inputs to hardware capabilities
	FPlatformMotionCommand Command;
	
	// X axis = Roll (left/right tilt)
	// -1.0 = full left tilt (negative roll), +1.0 = full right tilt (positive roll)
	Command.Roll = TiltX * Config.MaxRollDegrees;
	
	// Y axis = Pitch (forward/backward tilt)
	// -1.0 = full backward tilt (negative pitch), +1.0 = full forward tilt (positive pitch)
	Command.Pitch = TiltY * Config.MaxPitchDegrees;
	
	// Scissor lift translations
	Command.TranslationY = ForwardOffset * Config.MaxTranslationY;  // Forward/reverse (scissor lift)
	Command.TranslationZ = VerticalOffset * Config.MaxTranslationZ;  // Up/down (scissor lift)
	
	// Set duration
	Command.Duration = FMath::Max(Duration, 0.01f); // Minimum 10ms to prevent instant snapping
	
	// Handle continuous rotation for flight sim
	Command.bUseContinuousRotation = (Config.PlatformType == ELBEASTPlatformType::FlightSim_2DOF);
	
	// Send the command through the standard pipeline
	SendMotionCommand(Command);
	
	UE_LOG(LogTemp, Verbose, TEXT("HapticPlatformController: Normalized motion sent - TiltX: %.2f (Roll: %.2f°), TiltY: %.2f (Pitch: %.2f°), Forward: %.2f, Vertical: %.2f"), 
		TiltX, Command.Roll, TiltY, Command.Pitch, ForwardOffset, VerticalOffset);
}

void UHapticPlatformController::SetActuatorExtension(FName ActuatorID, float Extension)
{
	if (!bIsInitialized)
	{
		return;
	}

	Extension = FMath::Clamp(Extension, 0.0f, 1.0f);

	for (FHydraulicActuator& Actuator : Config.Actuators)
	{
		if (Actuator.ActuatorID == ActuatorID)
		{
			Actuator.Extension = Extension;
			// NOOP: TODO - Send individual actuator command to hardware
			break;
		}
	}
}

void UHapticPlatformController::EmergencyStop()
{
	if (!bIsInitialized)
	{
		return;
	}

	// Immediately stop all motion
	MotionTimeRemaining = 0.0f;
	TargetState = CurrentState;

	// NOOP: TODO - Send emergency stop command to hardware
	UE_LOG(LogTemp, Warning, TEXT("HapticPlatformController: EMERGENCY STOP"));
}

void UHapticPlatformController::ReturnToNeutral(float Duration)
{
	FPlatformMotionCommand NeutralCommand;
	NeutralCommand.Pitch = 0.0f;
	NeutralCommand.Roll = 0.0f;
	NeutralCommand.TranslationY = 0.0f;
	NeutralCommand.TranslationZ = 0.0f;
	NeutralCommand.Duration = Duration;

	SendMotionCommand(NeutralCommand);
}

FTransform UHapticPlatformController::GetCurrentPlatformTransform() const
{
	FTransform Transform = FTransform::Identity;

	// Apply rotation
	FRotator Rotation(CurrentState.Pitch, 0.0f, CurrentState.Roll);
	Transform.SetRotation(Rotation.Quaternion());

	// Apply translation
	FVector Translation(0.0f, CurrentState.TranslationY, CurrentState.TranslationZ);
	Transform.SetTranslation(Translation);

	return Transform;
}

void UHapticPlatformController::SendCommandToHardware(const FPlatformMotionCommand& Command, bool bUseStructPacket)
{
	if (!bIsInitialized || !IsHardwareConnected())
	{
		return;
	}

	if (bUseStructPacket)
	{
		// Send as single struct packet (Channel 200 for full motion command structs)
		// More efficient: 1 UDP packet instead of 5
		SendStruct<FPlatformMotionCommand>(200, Command);
		UE_LOG(LogTemp, Verbose, TEXT("HapticPlatformController: Sent command as struct - Pitch: %.2f, Roll: %.2f, Y: %.2f, Z: %.2f, Duration: %.2f"),
			Command.Pitch, Command.Roll, Command.TranslationY, Command.TranslationZ, Command.Duration);
	}
	else
	{
		// Map motion command to channels (experience-specific)
		// GunshipExperience uses: Ch0=Pitch, Ch1=Roll, Ch2=TranslationY, Ch3=TranslationZ, Ch4=Duration
		// Other experiences can override this mapping by calling SendFloat directly
		
		// For GunshipExperience (4DOF platform):
		SendFloat(0, Command.Pitch);           // Channel 0: Pitch (degrees)
		SendFloat(1, Command.Roll);            // Channel 1: Roll (degrees)
		SendFloat(2, Command.TranslationY);   // Channel 2: Forward/Reverse (cm)
		SendFloat(3, Command.TranslationZ);   // Channel 3: Up/Down (cm)
		SendFloat(4, Command.Duration);        // Channel 4: Duration (seconds)

		UE_LOG(LogTemp, Verbose, TEXT("HapticPlatformController: Sent command as channels - Pitch: %.2f, Roll: %.2f, Y: %.2f, Z: %.2f, Duration: %.2f"),
			Command.Pitch, Command.Roll, Command.TranslationY, Command.TranslationZ, Command.Duration);
	}
}


void UHapticPlatformController::UpdateMotionInterpolation(float DeltaTime)
{
	MotionTimeRemaining -= DeltaTime;

	if (MotionTimeRemaining <= 0.0f)
	{
		// Motion complete
		CurrentState = TargetState;
		MotionTimeRemaining = 0.0f;
		return;
	}

	// Calculate interpolation alpha
	float Alpha = 1.0f - (MotionTimeRemaining / MotionTotalDuration);
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

	// Smooth interpolation using ease in-out
	Alpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);

	// Interpolate all motion parameters
	CurrentState.Pitch = FMath::Lerp(CurrentState.Pitch, TargetState.Pitch, Alpha);
	CurrentState.Roll = FMath::Lerp(CurrentState.Roll, TargetState.Roll, Alpha);
	CurrentState.TranslationY = FMath::Lerp(CurrentState.TranslationY, TargetState.TranslationY, Alpha);
	CurrentState.TranslationZ = FMath::Lerp(CurrentState.TranslationZ, TargetState.TranslationZ, Alpha);
}

FVector2D UHapticPlatformController::GetHOTASJoystickInput() const
{
	if (!bHOTASConnected || !Config.GyroscopeConfig.bEnableJoystick)
	{
		return FVector2D::ZeroVector;
	}
	return HOTASJoystickInput * Config.GyroscopeConfig.JoystickSensitivity;
}

float UHapticPlatformController::GetHOTASThrottleInput() const
{
	if (!bHOTASConnected || !Config.GyroscopeConfig.bEnableThrottle)
	{
		return 0.0f;
	}
	return HOTASThrottleInput * Config.GyroscopeConfig.ThrottleSensitivity;
}

float UHapticPlatformController::GetHOTASPedalInput() const
{
	if (!bHOTASConnected || !Config.GyroscopeConfig.bEnablePedals)
	{
		return 0.0f;
	}
	return HOTASPedalInput;
}

bool UHapticPlatformController::IsHOTASConnected() const
{
	return bHOTASConnected;
}

ELBEASTHOTASType UHapticPlatformController::GetHOTASType() const
{
	return Config.GyroscopeConfig.HOTASType;
}

bool UHapticPlatformController::InitializeHOTAS()
{
	if (Config.GyroscopeConfig.HOTASType == ELBEASTHOTASType::None)
	{
		return false;
	}

	// NOOP: TODO - Initialize HOTAS SDK connection based on type
	switch (Config.GyroscopeConfig.HOTASType)
	{
	case ELBEASTHOTASType::LogitechX56:
		UE_LOG(LogTemp, Log, TEXT("HapticPlatformController: Initializing Logitech G X56 HOTAS..."));
		// NOOP: TODO - Initialize Logitech G X56 SDK
		break;

	case ELBEASTHOTASType::ThrustmasterTFlight:
		UE_LOG(LogTemp, Log, TEXT("HapticPlatformController: Initializing Thrustmaster T.Flight HOTAS..."));
		// NOOP: TODO - Initialize Thrustmaster T.Flight SDK
		break;

	case ELBEASTHOTASType::Custom:
		UE_LOG(LogTemp, Log, TEXT("HapticPlatformController: Initializing Custom HOTAS..."));
		// NOOP: TODO - Initialize custom HOTAS interface
		break;

	default:
		break;
	}

	// For now, simulate successful connection
	// In a real implementation, this would check if the SDK initialized successfully
	bHOTASConnected = true;
	UE_LOG(LogTemp, Log, TEXT("HapticPlatformController: HOTAS initialized successfully"));
	return true;
}

void UHapticPlatformController::UpdateHOTASInput()
{
	if (!bHOTASConnected)
	{
		return;
	}

	// NOOP: TODO - Read actual input from HOTAS SDK
	// For now, we can read from Unreal's generic input system or use the specific SDK

	// Placeholder: Read from Unreal's input system (which can map to HOTAS devices)
	// In a real implementation, this would query the Logitech/Thrustmaster SDK directly
	
	// Example of how it might work:
	// HOTASJoystickInput.X = ReadJoystickAxis("HOTAS_Roll");
	// HOTASJoystickInput.Y = ReadJoystickAxis("HOTAS_Pitch");
	// HOTASThrottleInput = ReadThrottleAxis("HOTAS_Throttle");
	// HOTASPedalInput = ReadPedalAxis("HOTAS_Pedals");

	// Apply axis inversions if configured
	if (Config.GyroscopeConfig.bInvertPitchAxis)
	{
		HOTASJoystickInput.Y *= -1.0f;
	}
	if (Config.GyroscopeConfig.bInvertRollAxis)
	{
		HOTASJoystickInput.X *= -1.0f;
	}
}

// Channel-Based IO API (SendFloat, SendBool, SendInt32, SendBytes, SendStruct, GetReceivedFloat, GetReceivedBool, GetReceivedInt32)
// are inherited from ULBEASTUDPTransport base class - no duplicate implementation needed

bool UHapticPlatformController::IsHardwareConnected() const
{
	return IsUDPConnected();
}

// UDP socket and protocol management now handled by base class (ULBEASTUDPTransport)


