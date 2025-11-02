// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LBEASTEmbeddedDeviceInterface.generated.h"

/**
 * Interface for embedded device controllers - allows LBEASTCore to reference embedded systems without dependency on EmbeddedSystems module
 */
UINTERFACE(MinimalAPI, BlueprintType, NotBlueprintable)
class ULBEASTEmbeddedDeviceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that embedded device controllers must implement
 * This breaks the circular dependency between LBEASTCore and EmbeddedSystems
 */
class LBEASTCOMMON_API ILBEASTEmbeddedDeviceInterface
{
	GENERATED_BODY()

public:
	/** Check if the device is currently connected */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	virtual bool IsDeviceConnected() const { return false; }

	/** Get the digital input value for a button channel */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	virtual bool GetDigitalInput(int32 Channel) const { return false; }

	/** Get the analog input value for an axis channel */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Embedded")
	virtual float GetAnalogInput(int32 Channel) const { return 0.0f; }
};

