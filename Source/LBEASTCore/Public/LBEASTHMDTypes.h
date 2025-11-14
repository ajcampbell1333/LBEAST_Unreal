// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "LBEASTHMDTypes.generated.h"

/**
 * Enumeration of supported HMD systems in LBEAST
 * 
 * Note: LBEAST now uses Unreal's native XR APIs directly (IXRTrackingSystem, IHandTracker).
 * This enum is kept for configuration purposes (e.g., passthrough settings) but doesn't
 * require wrapper components - Unreal's OpenXR implementation handles all HMD types.
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
 * 
 * Note: This config is primarily for passthrough settings and documentation purposes.
 * HMD tracking is handled directly via Unreal's native XR APIs (IXRTrackingSystem).
 */
USTRUCT(BlueprintType)
struct LBEASTCORE_API FLBEASTHMDConfig
{
	GENERATED_BODY()

	/** Which HMD system to use (informational - Unreal's XR system handles this automatically) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|HMD")
	ELBEASTHMDType HMDType = ELBEASTHMDType::OpenXR;

	/** Enable passthrough for mixed reality experiences */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|HMD")
	bool bEnablePassthrough = false;

	/** Passthrough alpha blend value (0.0 = full VR, 1.0 = full passthrough) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|HMD", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PassthroughAlpha = 0.0f;
};

