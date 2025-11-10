// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "2DOFGyroPlatformController.h"

U2DOFGyroPlatformController::U2DOFGyroPlatformController()
{
}

void U2DOFGyroPlatformController::SendGyroStruct(const FGyroState& GyroState, int32 Channel)
{
	if (!bIsInitialized || !IsHardwareConnected())
	{
		UE_LOG(LogTemp, Warning, TEXT("2DOFGyroPlatformController: Cannot send gyro struct - not initialized or not connected"));
		return;
	}

	// Send gyro state as struct packet (default Channel 102)
	SendStruct<FGyroState>(Channel, GyroState);
	UE_LOG(LogTemp, Verbose, TEXT("2DOFGyroPlatformController: Sent gyro struct on Ch%d - Pitch: %.2f°, Roll: %.2f°"), 
		Channel, GyroState.Pitch, GyroState.Roll);
}

void U2DOFGyroPlatformController::SendGyroFromNormalized(float NormalizedPitch, float NormalizedRoll, float DeltaTime, float CurrentPitch, float CurrentRoll, int32 Channel)
{
	if (!bIsInitialized || !IsHardwareConnected())
	{
		UE_LOG(LogTemp, Warning, TEXT("2DOFGyroPlatformController: Cannot send gyro from normalized - not initialized or not connected"));
		return;
	}

	// Get max rotation speed from config
	float MaxRotationSpeed = 90.0f; // Default
	if (bIsInitialized)
	{
		MaxRotationSpeed = Config.GyroscopeConfig.MaxRotationSpeed;
	}

	// Create gyro state from normalized input
	FGyroState GyroState = FGyroState::FromNormalized(
		NormalizedPitch, 
		NormalizedRoll, 
		MaxRotationSpeed, 
		DeltaTime, 
		CurrentPitch, 
		CurrentRoll
	);

	// Send as struct
	SendGyroStruct(GyroState, Channel);
}

