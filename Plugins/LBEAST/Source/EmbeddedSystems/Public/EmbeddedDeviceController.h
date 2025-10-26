// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmbeddedDeviceController.generated.h"

/**
 * Supported microcontroller types
 */
UENUM(BlueprintType)
enum class ELBEASTMicrocontrollerType : uint8
{
	Arduino UMETA(DisplayName = "Arduino"),
	ESP32 UMETA(DisplayName = "ESP32"),
	STM32 UMETA(DisplayName = "STM32"),
	RaspberryPi UMETA(DisplayName = "Raspberry Pi"),
	Jetson UMETA(DisplayName = "NVIDIA Jetson"),
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Communication protocol type
 */
UENUM(BlueprintType)
enum class ELBEASTCommProtocol : uint8
{
	Serial UMETA(DisplayName = "Serial (USB/UART)"),
	WiFi UMETA(DisplayName = "WiFi (UDP/TCP)"),
	Bluetooth UMETA(DisplayName = "Bluetooth"),
	Ethernet UMETA(DisplayName = "Ethernet")
};

/**
 * Input type from embedded device
 */
UENUM(BlueprintType)
enum class ELBEASTInputType : uint8
{
	Discrete UMETA(DisplayName = "Discrete (Button Press)"),
	Continuous UMETA(DisplayName = "Continuous (Analog)")
};

/**
 * Output type to embedded device
 */
UENUM(BlueprintType)
enum class ELBEASTOutputType : uint8
{
	Discrete UMETA(DisplayName = "Discrete (On/Off)"),
	Continuous UMETA(DisplayName = "Continuous (PWM/Analog)")
};

/**
 * Configuration for embedded device
 */
USTRUCT(BlueprintType)
struct FEmbeddedDeviceConfig
{
	GENERATED_BODY()

	/** Type of microcontroller */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	ELBEASTMicrocontrollerType DeviceType = ELBEASTMicrocontrollerType::ESP32;

	/** Communication protocol */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	ELBEASTCommProtocol Protocol = ELBEASTCommProtocol::WiFi;

	/** Device address (COM port, IP address, MAC address, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	FString DeviceAddress = TEXT("192.168.1.50");

	/** Port number (for network protocols) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	int32 Port = 8888;

	/** Baud rate (for serial communication) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	int32 BaudRate = 115200;

	/** Number of input pins/channels */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	int32 InputChannelCount = 8;

	/** Number of output pins/channels */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	int32 OutputChannelCount = 8;
};

/**
 * Input data from embedded device
 */
USTRUCT(BlueprintType)
struct FEmbeddedInputData
{
	GENERATED_BODY()

	/** Channel/pin number */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	int32 Channel = 0;

	/** Input type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	ELBEASTInputType InputType = ELBEASTInputType::Discrete;

	/** Value (0-1 for analog, 0 or 1 for digital) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	float Value = 0.0f;

	/** Timestamp */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	float Timestamp = 0.0f;
};

/**
 * Output command to embedded device
 */
USTRUCT(BlueprintType)
struct FEmbeddedOutputCommand
{
	GENERATED_BODY()

	/** Channel/pin number */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	int32 Channel = 0;

	/** Output type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	ELBEASTOutputType OutputType = ELBEASTOutputType::Discrete;

	/** Value (0-1 for PWM/analog, 0 or 1 for digital) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	float Value = 0.0f;

	/** Duration for timed outputs (0 = continuous) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	float Duration = 0.0f;
};

// Delegate for input events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEmbeddedInputReceived, FEmbeddedInputData, InputData);

/**
 * Embedded Device Controller Component
 * 
 * Manages communication with embedded microcontrollers for:
 * - Button/trigger input from costume-mounted or prop-mounted sensors
 * - Haptic output to vibrators/kickers in costumes or props
 * - Integration with narrative state machines
 * - Wireless and wired communication protocols
 */
UCLASS(ClassGroup=(LBEAST), meta=(BlueprintSpawnableComponent))
class EMBEDDEDSYSTEMS_API UEmbeddedDeviceController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEmbeddedDeviceController();

	/** Device configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	FEmbeddedDeviceConfig Config;

	/** Event fired when input is received from device */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|Embedded")
	FOnEmbeddedInputReceived OnInputReceived;

	/**
	 * Initialize connection to embedded device
	 * @param InConfig - Configuration settings
	 * @return true if initialization was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	bool InitializeDevice(const FEmbeddedDeviceConfig& InConfig);

	/**
	 * Send output command to device
	 * @param Command - The output command to send
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	void SendOutputCommand(const FEmbeddedOutputCommand& Command);

	/**
	 * Trigger a haptic pulse on a specific channel
	 * @param Channel - Output channel number
	 * @param Intensity - Intensity (0-1)
	 * @param Duration - Duration in seconds
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	void TriggerHapticPulse(int32 Channel, float Intensity, float Duration);

	/**
	 * Set continuous output on a channel
	 * @param Channel - Output channel number
	 * @param Value - Output value (0-1)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	void SetContinuousOutput(int32 Channel, float Value);

	/**
	 * Get the most recent input value for a channel
	 * @param Channel - Input channel number
	 * @return The most recent input value
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	float GetInputValue(int32 Channel) const;

	/**
	 * Check if device is connected and responding
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	bool IsDeviceConnected() const;

	/**
	 * Disconnect from device
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	void DisconnectDevice();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Whether device is initialized and connected */
	bool bIsConnected = false;

	/** Cache of most recent input values per channel */
	TMap<int32, float> InputValueCache;

	/** Timestamp of last successful communication */
	float LastCommTimestamp = 0.0f;

	/**
	 * Process incoming data from device
	 */
	void ProcessIncomingData();

	/**
	 * Send data to device
	 */
	void SendDataToDevice(const TArray<uint8>& Data);

	/**
	 * Check connection health
	 */
	void CheckConnectionHealth();
};



