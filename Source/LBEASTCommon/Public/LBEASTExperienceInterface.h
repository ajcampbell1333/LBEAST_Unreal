// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LBEASTExperienceInterface.generated.h"

// Forward declaration
class ULBEASTInputAdapter;

/**
 * Interface for LBEAST Experience - allows LBEASTCore to reference experiences without dependency on LBEASTExperiences module
 */
UINTERFACE(MinimalAPI, BlueprintType, NotBlueprintable)
class ULBEASTExperienceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that all LBEAST Experiences must implement
 * This breaks the circular dependency between LBEASTCore and LBEASTExperiences
 */
class LBEASTCOMMON_API ILBEASTExperienceInterface
{
	GENERATED_BODY()

public:
	/**
	 * Get the InputAdapter associated with this experience
	 * @return InputAdapter component, or nullptr if not available
	 */
	virtual ULBEASTInputAdapter* GetInputAdapter() const { return nullptr; }
};

