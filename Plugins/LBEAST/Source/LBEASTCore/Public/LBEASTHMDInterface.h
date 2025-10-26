// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LBEASTHMDInterface.generated.h"

/**
 * Enumeration of supported HMD systems in LBEAST
 */
UENUM(BlueprintType)
enum class ELBEASTHMDType : uint8
{
	OpenXR UMETA(DisplayName = "OpenXR"),
	SteamVR UMETA(DisplayName = "SteamVR"),
	Meta UMETA(DisplayName = "Meta Quest"),
	None UMETA(DisplayName = "None")
};

/**
 * Configuration settings for LBEAST HMD system
 */
USTRUCT(BlueprintType)
struct FLBEASTHMDConfig
{
	GENERATED_BODY()

	/** Which HMD system to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|HMD")
	ELBEASTHMDType HMDType = ELBEASTHMDType::OpenXR;

	/** Enable passthrough for mixed reality experiences */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|HMD")
	bool bEnablePassthrough = false;

	/** Passthrough alpha blend value (0.0 = full VR, 1.0 = full passthrough) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|HMD", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PassthroughAlpha = 0.0f;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class ULBEASTHMDInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for LBEAST HMD abstraction
 * 
 * This interface provides a unified API for working with different HMD systems.
 * Implementations handle the specifics of OpenXR, SteamVR, and Meta platforms.
 */
class LBEASTCORE_API ILBEASTHMDInterface
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the HMD system
	 * @param Config - Configuration settings for the HMD
	 * @return true if initialization was successful
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LBEAST|HMD")
	bool InitializeHMD(const FLBEASTHMDConfig& Config);

	/**
	 * Get the current HMD transform in world space
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LBEAST|HMD")
	FTransform GetHMDTransform() const;

	/**
	 * Set passthrough alpha blend value
	 * @param Alpha - Blend value (0.0 = full VR, 1.0 = full passthrough)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LBEAST|HMD")
	void SetPassthroughAlpha(float Alpha);

	/**
	 * Get whether passthrough is currently supported
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LBEAST|HMD")
	bool IsPassthroughSupported() const;

	/**
	 * Enable or disable passthrough
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LBEAST|HMD")
	void SetPassthroughEnabled(bool bEnabled);
};



