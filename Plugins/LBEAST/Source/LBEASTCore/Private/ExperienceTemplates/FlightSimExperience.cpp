// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "ExperienceTemplates/FlightSimExperience.h"
#include "HapticPlatformController.h"

AFlightSimExperience::AFlightSimExperience()
{
	GyroscopeController = CreateDefaultSubobject<UHapticPlatformController>(TEXT("GyroscopeController"));
}

bool AFlightSimExperience::InitializeExperienceImpl()
{
	if (!Super::InitializeExperienceImpl())
	{
		return false;
	}

	if (!GyroscopeController)
	{
		UE_LOG(LogTemp, Error, TEXT("FlightSimExperience: Gyroscope controller is null"));
		return false;
	}

	// Configure gyroscope system
	FHapticPlatformConfig Config;
	Config.PlatformType = ELBEASTPlatformType::FlightSim_2DOF;
	Config.ControllerIPAddress = TEXT("192.168.1.100");
	Config.ControllerPort = 8080;

	// Configure gyroscope settings
	Config.GyroscopeConfig.bEnableContinuousPitch = true;
	Config.GyroscopeConfig.bEnableContinuousRoll = true;
	Config.GyroscopeConfig.MaxRotationSpeed = MaxRotationSpeed;

	// Configure HOTAS
	Config.GyroscopeConfig.HOTASType = HOTASType;
	Config.GyroscopeConfig.bEnableJoystick = bEnableJoystick;
	Config.GyroscopeConfig.bEnableThrottle = bEnableThrottle;
	Config.GyroscopeConfig.bEnablePedals = bEnablePedals;
	Config.GyroscopeConfig.JoystickSensitivity = JoystickSensitivity;
	Config.GyroscopeConfig.ThrottleSensitivity = ThrottleSensitivity;

	if (!GyroscopeController->InitializePlatform(Config))
	{
		UE_LOG(LogTemp, Error, TEXT("FlightSimExperience: Failed to initialize gyroscope"));
		return false;
	}

	if (GyroscopeController->IsHOTASConnected())
	{
		UE_LOG(LogTemp, Log, TEXT("FlightSimExperience: HOTAS connected successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FlightSimExperience: HOTAS not connected, using standard VR controllers"));
	}

	UE_LOG(LogTemp, Log, TEXT("FlightSimExperience: Initialized successfully"));
	return true;
}

void AFlightSimExperience::ShutdownExperienceImpl()
{
	if (GyroscopeController)
	{
		GyroscopeController->ReturnToNeutral(2.0f);
	}

	Super::ShutdownExperienceImpl();
}

void AFlightSimExperience::SendContinuousRotation(float Pitch, float Roll, float Duration)
{
	if (!GyroscopeController)
	{
		return;
	}

	FPlatformMotionCommand Command;
	Command.Pitch = Pitch;
	Command.Roll = Roll;
	Command.TranslationY = 0.0f;  // No translation for gyroscope
	Command.TranslationZ = 0.0f;
	Command.Duration = Duration;
	Command.bUseContinuousRotation = true;  // Enable continuous rotation

	GyroscopeController->SendMotionCommand(Command);
}

FVector2D AFlightSimExperience::GetJoystickInput() const
{
	if (GyroscopeController)
	{
		return GyroscopeController->GetHOTASJoystickInput();
	}
	return FVector2D::ZeroVector;
}

float AFlightSimExperience::GetThrottleInput() const
{
	if (GyroscopeController)
	{
		return GyroscopeController->GetHOTASThrottleInput();
	}
	return 0.0f;
}

float AFlightSimExperience::GetPedalInput() const
{
	if (GyroscopeController)
	{
		return GyroscopeController->GetHOTASPedalInput();
	}
	return 0.0f;
}

bool AFlightSimExperience::IsHOTASConnected() const
{
	if (GyroscopeController)
	{
		return GyroscopeController->IsHOTASConnected();
	}
	return false;
}

void AFlightSimExperience::ReturnToNeutral(float Duration)
{
	if (GyroscopeController)
	{
		GyroscopeController->ReturnToNeutral(Duration);
	}
}

void AFlightSimExperience::EmergencyStop()
{
	if (GyroscopeController)
	{
		GyroscopeController->EmergencyStop();
	}
}


