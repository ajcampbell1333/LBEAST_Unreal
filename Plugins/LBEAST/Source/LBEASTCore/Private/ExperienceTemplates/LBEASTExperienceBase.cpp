// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "ExperienceTemplates/LBEASTExperienceBase.h"

ALBEASTExperienceBase::ALBEASTExperienceBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Default HMD configuration
	HMDConfig.HMDType = ELBEASTHMDType::OpenXR;
	HMDConfig.bEnablePassthrough = false;
	HMDConfig.PassthroughAlpha = 0.0f;

	// Default tracking configuration
	TrackingConfig.TrackingSystem = ELBEASTTrackingSystem::SteamVRTrackers;
	TrackingConfig.ExpectedDeviceCount = 0;
}

void ALBEASTExperienceBase::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoInitialize)
	{
		InitializeExperience();
	}
}

void ALBEASTExperienceBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShutdownExperience();
	Super::EndPlay(EndPlayReason);
}

bool ALBEASTExperienceBase::InitializeExperience()
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTExperience: Already initialized"));
		return true;
	}

	UE_LOG(LogTemp, Log, TEXT("LBEASTExperience: Initializing experience..."));

	// Call derived class implementation
	if (!InitializeExperienceImpl())
	{
		UE_LOG(LogTemp, Error, TEXT("LBEASTExperience: Failed to initialize experience"));
		return false;
	}

	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("LBEASTExperience: Initialization complete"));
	return true;
}

void ALBEASTExperienceBase::ShutdownExperience()
{
	if (!bIsInitialized)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("LBEASTExperience: Shutting down experience..."));

	ShutdownExperienceImpl();

	bIsInitialized = false;
	UE_LOG(LogTemp, Log, TEXT("LBEASTExperience: Shutdown complete"));
}

bool ALBEASTExperienceBase::InitializeExperienceImpl()
{
	// Base implementation - override in derived classes
	return true;
}

void ALBEASTExperienceBase::ShutdownExperienceImpl()
{
	// Base implementation - override in derived classes
}


