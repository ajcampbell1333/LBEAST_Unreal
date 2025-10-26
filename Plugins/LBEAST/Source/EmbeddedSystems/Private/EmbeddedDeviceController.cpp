// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "EmbeddedDeviceController.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

UEmbeddedDeviceController::UEmbeddedDeviceController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEmbeddedDeviceController::BeginPlay()
{
	Super::BeginPlay();
	
	// Auto-initialize if config is set
	if (!Config.DeviceAddress.IsEmpty())
	{
		InitializeDevice(Config);
	}
}

void UEmbeddedDeviceController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DisconnectDevice();
	Super::EndPlay(EndPlayReason);
}

void UEmbeddedDeviceController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsConnected)
	{
		return;
	}

	// Process any incoming data
	ProcessIncomingData();

	// Check connection health
	CheckConnectionHealth();
}

bool UEmbeddedDeviceController::InitializeDevice(const FEmbeddedDeviceConfig& InConfig)
{
	Config = InConfig;

	// TODO: Establish connection based on protocol type
	switch (Config.Protocol)
	{
	case ELBEASTCommProtocol::Serial:
		// TODO: Open serial port connection
		UE_LOG(LogTemp, Log, TEXT("EmbeddedDeviceController: Initializing Serial connection to %s"), *Config.DeviceAddress);
		break;

	case ELBEASTCommProtocol::WiFi:
		// TODO: Open UDP/TCP socket connection
		UE_LOG(LogTemp, Log, TEXT("EmbeddedDeviceController: Initializing WiFi connection to %s:%d"), *Config.DeviceAddress, Config.Port);
		break;

	case ELBEASTCommProtocol::Bluetooth:
		// TODO: Open Bluetooth connection
		UE_LOG(LogTemp, Log, TEXT("EmbeddedDeviceController: Initializing Bluetooth connection to %s"), *Config.DeviceAddress);
		break;

	case ELBEASTCommProtocol::Ethernet:
		// TODO: Open Ethernet socket connection
		UE_LOG(LogTemp, Log, TEXT("EmbeddedDeviceController: Initializing Ethernet connection to %s:%d"), *Config.DeviceAddress, Config.Port);
		break;
	}

	// Initialize input cache
	InputValueCache.Empty();
	for (int32 i = 0; i < Config.InputChannelCount; i++)
	{
		InputValueCache.Add(i, 0.0f);
	}

	bIsConnected = true;
	LastCommTimestamp = GetWorld()->GetTimeSeconds();

	UE_LOG(LogTemp, Log, TEXT("EmbeddedDeviceController: Initialized successfully"));
	return true;
}

void UEmbeddedDeviceController::SendOutputCommand(const FEmbeddedOutputCommand& Command)
{
	if (!bIsConnected)
	{
		UE_LOG(LogTemp, Warning, TEXT("EmbeddedDeviceController: Cannot send command - not connected"));
		return;
	}

	// TODO: Format and send command to device
	// Command format would depend on the firmware protocol
	UE_LOG(LogTemp, Log, TEXT("EmbeddedDeviceController: Sending output - Channel: %d, Value: %.2f"), 
		Command.Channel, Command.Value);
}

void UEmbeddedDeviceController::TriggerHapticPulse(int32 Channel, float Intensity, float Duration)
{
	FEmbeddedOutputCommand Command;
	Command.Channel = Channel;
	Command.OutputType = ELBEASTOutputType::Discrete;
	Command.Value = FMath::Clamp(Intensity, 0.0f, 1.0f);
	Command.Duration = Duration;

	SendOutputCommand(Command);
}

void UEmbeddedDeviceController::SetContinuousOutput(int32 Channel, float Value)
{
	FEmbeddedOutputCommand Command;
	Command.Channel = Channel;
	Command.OutputType = ELBEASTOutputType::Continuous;
	Command.Value = FMath::Clamp(Value, 0.0f, 1.0f);
	Command.Duration = 0.0f; // Continuous

	SendOutputCommand(Command);
}

float UEmbeddedDeviceController::GetInputValue(int32 Channel) const
{
	const float* Value = InputValueCache.Find(Channel);
	return Value ? *Value : 0.0f;
}

bool UEmbeddedDeviceController::IsDeviceConnected() const
{
	return bIsConnected;
}

void UEmbeddedDeviceController::DisconnectDevice()
{
	if (!bIsConnected)
	{
		return;
	}

	// TODO: Close connection based on protocol type
	bIsConnected = false;
	InputValueCache.Empty();

	UE_LOG(LogTemp, Log, TEXT("EmbeddedDeviceController: Disconnected"));
}

void UEmbeddedDeviceController::ProcessIncomingData()
{
	// TODO: Read data from connection and parse input messages
	// This would depend on the firmware protocol
	// When input is received, update cache and fire event

	// Example of firing event (placeholder):
	// FEmbeddedInputData InputData;
	// InputData.Channel = 0;
	// InputData.Value = 1.0f;
	// InputData.Timestamp = GetWorld()->GetTimeSeconds();
	// InputValueCache.Add(InputData.Channel, InputData.Value);
	// OnInputReceived.Broadcast(InputData);
}

void UEmbeddedDeviceController::SendDataToDevice(const TArray<uint8>& Data)
{
	if (!bIsConnected)
	{
		return;
	}

	// TODO: Send raw data based on protocol type
	LastCommTimestamp = GetWorld()->GetTimeSeconds();
}

void UEmbeddedDeviceController::CheckConnectionHealth()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float TimeSinceLastComm = CurrentTime - LastCommTimestamp;

	// If no communication for 5 seconds, consider connection lost
	if (TimeSinceLastComm > 5.0f && bIsConnected)
	{
		UE_LOG(LogTemp, Warning, TEXT("EmbeddedDeviceController: Connection timeout - no data received for %.1f seconds"), TimeSinceLastComm);
		bIsConnected = false;
	}
}



