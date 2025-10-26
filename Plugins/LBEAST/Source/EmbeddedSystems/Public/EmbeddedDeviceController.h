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

	/** Enable debug mode (uses JSON instead of binary, easier to debug with Wireshark) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded")
	bool bDebugMode = false;

	/** Security level for packet encryption and authentication */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded|Security")
	ELBEASTSecurityLevel SecurityLevel = ELBEASTSecurityLevel::Encrypted;

	/** Shared secret key for HMAC/AES (must match device firmware) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded|Security", meta = (PasswordField = true))
	FString SharedSecret = TEXT("CHANGE_ME_IN_PRODUCTION_2025");

	/** AES encryption key (auto-derived from SharedSecret if empty) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded|Security|Advanced")
	FString AESKey128;

	/** HMAC key (auto-derived from SharedSecret if empty) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Embedded|Security|Advanced")
	FString HMACKey;
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

// Delegates for typed input events (binary protocol)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBoolReceived, int32, Channel, bool, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInt32Received, int32, Channel, int32, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFloatReceived, int32, Channel, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStringReceived, int32, Channel, FString, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBytesReceived, int32, Channel, TArray<uint8>, Value);

/**
 * Data type enum for binary protocol
 */
UENUM(BlueprintType)
enum class ELBEASTDataType : uint8
{
	Bool = 0 UMETA(DisplayName = "Boolean"),
	Int32 = 1 UMETA(DisplayName = "Integer"),
	Float = 2 UMETA(DisplayName = "Float"),
	String = 3 UMETA(DisplayName = "String"),
	Bytes = 4 UMETA(DisplayName = "Raw Bytes"),
	Struct = 5 UMETA(DisplayName = "Struct")
};

/**
 * Security level for embedded communication
 */
UENUM(BlueprintType)
enum class ELBEASTSecurityLevel : uint8
{
	None UMETA(DisplayName = "None (Development Only)"),
	HMAC UMETA(DisplayName = "HMAC Authentication"),
	Encrypted UMETA(DisplayName = "AES-128 + HMAC (Recommended)"),
	DTLS UMETA(DisplayName = "DTLS (Future)")
};

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

	/** Event fired when input is received from device (legacy) */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|Embedded")
	FOnEmbeddedInputReceived OnInputReceived;

	/** Event fired when bool value is received (binary protocol) */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|Embedded")
	FOnBoolReceived OnBoolReceived;

	/** Event fired when int32 value is received (binary protocol) */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|Embedded")
	FOnInt32Received OnInt32Received;

	/** Event fired when float value is received (binary protocol) */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|Embedded")
	FOnFloatReceived OnFloatReceived;

	/** Event fired when string value is received (binary protocol) */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|Embedded")
	FOnStringReceived OnStringReceived;

	/** Event fired when raw bytes are received (binary protocol) */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|Embedded")
	FOnBytesReceived OnBytesReceived;

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

	// =====================================
	// Binary Protocol - Primitive Send API
	// =====================================

	/**
	 * Send a boolean value to device
	 * @param Channel - Channel/pin number
	 * @param Value - Boolean value to send
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded|Send")
	void SendBool(int32 Channel, bool Value);

	/**
	 * Send an integer value to device
	 * @param Channel - Channel/pin number
	 * @param Value - Integer value to send
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded|Send")
	void SendInt32(int32 Channel, int32 Value);

	/**
	 * Send a float value to device
	 * @param Channel - Channel/pin number
	 * @param Value - Float value to send
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded|Send")
	void SendFloat(int32 Channel, float Value);

	/**
	 * Send a string value to device
	 * @param Channel - Channel/pin number
	 * @param Value - String value to send (max 255 bytes)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded|Send")
	void SendString(int32 Channel, const FString& Value);

	/**
	 * Send raw bytes to device
	 * @param Channel - Channel/pin number
	 * @param Data - Raw byte array
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded|Send")
	void SendBytes(int32 Channel, const TArray<uint8>& Data);

	/**
	 * Send a POD struct to device (template, C++ only)
	 * @param Channel - Channel/pin number
	 * @param Data - Struct data (must be POD type)
	 */
	template<typename T>
	void SendStruct(int32 Channel, const T& Data)
	{
		static_assert(TIsPODType<T>::Value, "SendStruct requires POD (Plain Old Data) type");
		TArray<uint8> Bytes;
		Bytes.SetNumUninitialized(sizeof(T));
		FMemory::Memcpy(Bytes.GetData(), &Data, sizeof(T));
		SendBytes(Channel, Bytes);
	}

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

	/** UDP/TCP Socket for WiFi/Ethernet communication */
	FSocket* Socket = nullptr;

	/** Remote address for UDP communication */
	TSharedPtr<FInternetAddr> RemoteAddr;

	/** Receive buffer for incoming packets */
	TArray<uint8> ReceiveBuffer;

	/** Protocol start marker */
	static constexpr uint8 PACKET_START_MARKER = 0xAA;

	/** Derived AES key (16 bytes for AES-128) */
	uint8 DerivedAESKey[16];

	/** Derived HMAC key (32 bytes for SHA-256) */
	uint8 DerivedHMACKey[32];

	/** Random number generator state */
	uint32 RandomState;

	/**
	 * Process incoming data from device
	 */
	void ProcessIncomingData();

	/**
	 * Send data to device (protocol-agnostic)
	 */
	void SendDataToDevice(const TArray<uint8>& Data);

	/**
	 * Check connection health
	 */
	void CheckConnectionHealth();

	/**
	 * Initialize WiFi/Ethernet connection (UDP)
	 */
	bool InitializeWiFiConnection();

	/**
	 * Initialize Serial connection (COM port)
	 */
	bool InitializeSerialConnection();

	/**
	 * Send data via UDP
	 */
	void SendWiFiData(const TArray<uint8>& Data);

	/**
	 * Receive data via UDP
	 */
	void ReceiveWiFiData();

	/**
	 * Build binary packet for transmission
	 */
	TArray<uint8> BuildBinaryPacket(ELBEASTDataType Type, int32 Channel, const TArray<uint8>& Payload);

	/**
	 * Build JSON packet for transmission (debug mode)
	 */
	TArray<uint8> BuildJSONPacket(ELBEASTDataType Type, int32 Channel, const FString& ValueString);

	/**
	 * Parse incoming binary packet
	 */
	void ParseBinaryPacket(const TArray<uint8>& Data, int32 Length);

	/**
	 * Parse incoming JSON packet (debug mode)
	 */
	void ParseJSONPacket(const TArray<uint8>& Data, int32 Length);

	/**
	 * Calculate CRC checksum (XOR-based)
	 */
	uint8 CalculateCRC(const TArray<uint8>& Data, int32 Length) const;

	/**
	 * Validate CRC checksum
	 */
	bool ValidateCRC(const TArray<uint8>& Data, int32 Length, uint8 ExpectedCRC) const;

	/**
	 * Derive encryption keys from shared secret
	 */
	void DeriveKeysFromSecret();

	/**
	 * Encrypt payload using AES-128-CTR
	 * @param Plaintext - Data to encrypt
	 * @param IV - 4-byte initialization vector (random per packet)
	 * @return Encrypted data (same length as plaintext)
	 */
	TArray<uint8> EncryptAES128(const TArray<uint8>& Plaintext, uint32 IV) const;

	/**
	 * Decrypt payload using AES-128-CTR
	 * @param Ciphertext - Encrypted data
	 * @param IV - 4-byte initialization vector from packet
	 * @return Decrypted data
	 */
	TArray<uint8> DecryptAES128(const TArray<uint8>& Ciphertext, uint32 IV) const;

	/**
	 * Calculate HMAC-SHA256 (truncated to 8 bytes)
	 * @param Data - Data to authenticate
	 * @return 8-byte HMAC tag
	 */
	TArray<uint8> CalculateHMAC(const TArray<uint8>& Data) const;

	/**
	 * Validate HMAC-SHA256
	 * @param Data - Data (without HMAC)
	 * @param ExpectedHMAC - 8-byte HMAC from packet
	 * @return true if HMAC matches
	 */
	bool ValidateHMAC(const TArray<uint8>& Data, const TArray<uint8>& ExpectedHMAC) const;

	/**
	 * Generate random 32-bit value for IV
	 */
	uint32 GenerateRandomIV();
};



