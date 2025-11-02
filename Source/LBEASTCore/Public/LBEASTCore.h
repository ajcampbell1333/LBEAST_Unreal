// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * LBEAST Core Module
 * 
 * Provides foundational systems for the Location-Based Entertainment Activation Standard.
 * This module contains shared functionality used by all LBEAST subsystems including:
 * - HMD abstraction layer (OpenXR, SteamVR, Meta)
 * - 6DOF tracking abstraction layer
 * - Networking configuration (LAN multiplayer)
 * - Core data structures and utilities
 */
class FLBEASTCoreModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};



