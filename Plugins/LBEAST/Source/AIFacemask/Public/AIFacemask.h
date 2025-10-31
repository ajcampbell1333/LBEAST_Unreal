// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * AI Facemask Module
 * 
 * Provides AI-driven facial animation for immersive theater actors in VR experiences.
 * This module enables developers to:
 * - Deploy real-time AI-generated facial expressions on actor avatars
 * - Integrate with LAN multiplayer systems for multi-actor experiences
 * - Configure facial animation playback on any transform node (typically HMD)
 * - Support narrative state machine integration via embedded systems
 */
class FAIFacemaskModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};




