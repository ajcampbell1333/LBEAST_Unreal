// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * LBEAST Experiences Module
 * 
 * Contains pre-configured experience templates that combine multiple LBEAST APIs
 * to create complete, drag-and-drop LBE solutions.
 * 
 * Experience Templates:
 * - AI Facemask Experience
 * - Moving Platform Experience
 * - Gunship Experience
 * - Car Sim Experience
 * - Flight Sim Experience
 * - GoKart Experience
 */

DECLARE_LOG_CATEGORY_EXTERN(LogGoKart, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSuperheroFlight, Log, All);

class FLBEASTExperiencesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};




