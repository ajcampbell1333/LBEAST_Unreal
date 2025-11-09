// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "GunshipExperience.h"
#include "HapticPlatformController.h"

AGunshipExperience::AGunshipExperience()
{
	PlatformController = CreateDefaultSubobject<UHapticPlatformController>(TEXT("PlatformController"));
	bMultiplayerEnabled = true;

	// Default 4-seat configuration
	SeatLocations.Add(FVector(-100, -100, 0));  // Front Left
	SeatLocations.Add(FVector(100, -100, 0));   // Front Right
	SeatLocations.Add(FVector(-100, 100, 0));   // Rear Left
	SeatLocations.Add(FVector(100, 100, 0));    // Rear Right
}

bool AGunshipExperience::InitializeExperienceImpl()
{
	if (!Super::InitializeExperienceImpl())
	{
		return false;
	}

	if (!PlatformController)
	{
		UE_LOG(LogTemp, Error, TEXT("GunshipExperience: Platform controller is null"));
		return false;
	}

	// Configure platform for 4-player gunship
	FHapticPlatformConfig Config;
	Config.PlatformType = ELBEASTPlatformType::Gunship_FourPlayer;
	Config.MaxPitchDegrees = MaxPitch;
	Config.MaxRollDegrees = MaxRoll;
	Config.MaxTranslationY = 100.0f;
	Config.MaxTranslationZ = 100.0f;
	Config.ControllerIPAddress = TEXT("192.168.1.100");
	Config.ControllerPort = 8080;

	if (!PlatformController->InitializePlatform(Config))
	{
		UE_LOG(LogTemp, Error, TEXT("GunshipExperience: Failed to initialize platform"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("GunshipExperience: Initialized for 4 players"));
	return true;
}

void AGunshipExperience::ShutdownExperienceImpl()
{
	if (PlatformController)
	{
		PlatformController->ReturnToNeutral(1.0f);
	}

	Super::ShutdownExperienceImpl();
}

void AGunshipExperience::SendGunshipTilt(float TiltX, float TiltY, float ForwardOffset, float VerticalOffset, float Duration)
{
	if (!PlatformController)
	{
		return;
	}

	// Use the normalized API - automatically scales to hardware capabilities
	// TiltX = roll, TiltY = pitch, ForwardOffset = scissor lift forward/reverse, VerticalOffset = scissor lift up/down
	PlatformController->SendNormalizedMotion(TiltX, TiltY, ForwardOffset, VerticalOffset, Duration);
}

void AGunshipExperience::SendGunshipMotion(float Pitch, float Roll, float ForwardOffset, float VerticalOffset, float Duration)
{
	if (!PlatformController)
	{
		return;
	}

	FPlatformMotionCommand Command;
	Command.Pitch = Pitch;
	Command.Roll = Roll;
	// TranslationY = forward/reverse (scissor lift), TranslationZ = up/down (scissor lift)
	Command.TranslationY = ForwardOffset;
	Command.TranslationZ = VerticalOffset;
	Command.Duration = Duration;

	PlatformController->SendMotionCommand(Command);
}

void AGunshipExperience::ReturnToNeutral(float Duration)
{
	if (PlatformController)
	{
		PlatformController->ReturnToNeutral(Duration);
	}
}

void AGunshipExperience::EmergencyStop()
{
	if (PlatformController)
	{
		PlatformController->EmergencyStop();
	}
}

