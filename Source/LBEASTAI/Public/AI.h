// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AIAPI.h"  // For LBEASTAI_API macro

/**
 * LBEASTAI Module
 * 
 * Low-level AI API for all generative AI capabilities in LBEAST.
 * This module provides:
 * - LLM providers (Ollama, OpenAI-compatible, NVIDIA NIM)
 * - ASR providers (NVIDIA Riva, Parakeet, Canary, Whisper)
 * - TTS providers (NVIDIA Riva, etc.)
 * - Audio2Face integration (generic, not mask-specific)
 * - Container management for Docker-based AI services
 * - HTTP/gRPC clients for AI service communication
 * 
 * Future capabilities:
 * - Text-to-Image (TTI)
 * - Text-to-Video (TTV)
 * - Other generative AI models
 */
class LBEASTAI_API FLBEASTAIModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};




