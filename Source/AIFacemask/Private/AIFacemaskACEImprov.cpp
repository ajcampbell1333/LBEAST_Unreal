// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "AIFacemaskACEImprov.h"
#include "AIFacemaskACEScript.h"

UAIFacemaskACEImprovManager::UAIFacemaskACEImprovManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	bIsInitialized = false;
	bIsGeneratingResponse = false;
	MaxConversationHistory = 10;
}

void UAIFacemaskACEImprovManager::BeginPlay()
{
	Super::BeginPlay();
}

void UAIFacemaskACEImprovManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// NOOP: TODO - Monitor async response generation status
	// Check if LLM/TTS/Audio2Face requests have completed
}

bool UAIFacemaskACEImprovManager::InitializeImprovManager()
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEImprovManager: Already initialized"));
		return true;
	}

	if (!ImprovConfig.bEnableImprov)
	{
		UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEImprovManager: Improv is disabled in config"));
		return false;
	}

	bIsInitialized = true;

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEImprovManager: Initialized with local LLM: %s, Local TTS: %s, Local Audio2Face: %s"), 
		*ImprovConfig.LocalLLMEndpointURL, 
		ImprovConfig.bUseLocalTTS ? *ImprovConfig.LocalTTSEndpointURL : TEXT("Cloud"),
		ImprovConfig.bUseLocalAudio2Face ? *ImprovConfig.LocalAudio2FaceEndpointURL : TEXT("Cloud"));
	
	return true;
}

FString UAIFacemaskACEImprovManager::GenerateImprovResponse(const FString& PlayerInput)
{
	if (!bIsInitialized || !ImprovConfig.bEnableImprov)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEImprovManager: Cannot generate response - not initialized or disabled"));
		return FString();
	}

	if (bIsGeneratingResponse)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEImprovManager: Already generating a response, ignoring new request"));
		return FString();
	}

	bIsGeneratingResponse = true;
	CurrentPlayerInput = PlayerInput;

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEImprovManager: Generating improvised response to: '%s'"), *PlayerInput);

	// Build conversation context
	FString ConversationContext = BuildConversationContext(PlayerInput);

	// Request LLM to generate response
	FString AIResponse = RequestLLMResponse(PlayerInput, ImprovConfig.CharacterSystemPrompt, ConversationHistory);

	if (!AIResponse.IsEmpty())
	{
		CurrentAIResponse = AIResponse;
		
		// Add to conversation history
		ConversationHistory.Add(FString::Printf(TEXT("Player: %s"), *PlayerInput));
		ConversationHistory.Add(FString::Printf(TEXT("AI: %s"), *AIResponse));
		
		// Trim conversation history if needed
		if (ConversationHistory.Num() > MaxConversationHistory * 2)  // *2 because we store both player and AI messages
		{
			ConversationHistory.RemoveAt(0, ConversationHistory.Num() - MaxConversationHistory * 2);
		}

		// Broadcast response generated event
		OnImprovResponseGenerated.Broadcast(PlayerInput, AIResponse);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIFacemaskACEImprovManager: Failed to generate LLM response"));
	}

	bIsGeneratingResponse = false;
	return AIResponse;
}

void UAIFacemaskACEImprovManager::GenerateAndPlayImprovResponse(const FString& PlayerInput, bool bAsync)
{
	if (!bIsInitialized || !ImprovConfig.bEnableImprov)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEImprovManager: Cannot generate and play response - not initialized or disabled"));
		return;
	}

	// Generate text response
	FString AIResponse = GenerateImprovResponse(PlayerInput);
	
	if (AIResponse.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("AIFacemaskACEImprovManager: Failed to generate text response, cannot play"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEImprovManager: Playing improvised response: '%s'"), *AIResponse);

	// Broadcast response started event
	OnImprovResponseStarted.Broadcast(AIResponse);

	// Convert text to speech (local TTS)
	if (ImprovConfig.bUseLocalTTS)
	{
		RequestTTSConversion(AIResponse, ImprovConfig.VoiceType);
	}
	else
	{
		// NOOP: TODO - Use cloud TTS
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEImprovManager: Cloud TTS not yet implemented"));
	}

	// NOOP: TODO - After TTS completes, automatically trigger Audio2Face conversion
	// This should happen in the TTS completion callback
}

void UAIFacemaskACEImprovManager::ClearConversationHistory()
{
	ConversationHistory.Empty();
	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEImprovManager: Conversation history cleared"));
}

void UAIFacemaskACEImprovManager::StopCurrentResponse()
{
	if (!bIsGeneratingResponse)
	{
		return;
	}

	bIsGeneratingResponse = false;
	CurrentPlayerInput.Empty();
	CurrentAIResponse.Empty();

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEImprovManager: Stopped current response generation"));
}

FString UAIFacemaskACEImprovManager::RequestLLMResponse(const FString& PlayerInput, const FString& SystemPrompt, const TArray<FString>& ConversationHistory)
{
	// NOOP: TODO - Implement HTTP request to local LLM endpoint
	// 
	// BACKEND-AGNOSTIC IMPLEMENTATION:
	// This function should detect the endpoint type and use appropriate API format.
	// Supports multiple backends (all available as options):
	// 
	// 1. Ollama API (http://localhost:11434/api/generate)
	//    - POST /api/generate
	//    - Body: {"model": "llama3.2:3b", "prompt": "...", "stream": false}
	//    - Supports custom LoRA models via model name
	// 
	// 2. vLLM API (http://localhost:8000/v1/chat/completions)
	//    - POST /v1/chat/completions
	//    - OpenAI-compatible format
	//    - Supports LoRA via model name or API parameters
	// 
	// 3. NVIDIA NIM API (http://localhost:8000/v1/chat/completions)
	//    - POST /v1/chat/completions
	//    - OpenAI-compatible format
	//    - NVIDIA-optimized inference
	// 
	// 4. Any OpenAI-compatible API endpoint
	//    - Standard OpenAI API format
	//    - Works with any compatible service
	//
	// Request format (Ollama example):
	// POST /api/generate
	// {
	//   "model": "llama3.2:3b",
	//   "prompt": "<system prompt> + <conversation history> + Player: <input>",
	//   "stream": false,
	//   "options": {
	//     "temperature": 0.7,
	//     "num_predict": 150
	//   }
	// }
	//
	// Response format:
	// {
	//   "response": "AI generated text response..."
	// }

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEImprovManager: Requesting LLM response from %s (model: %s)"), 
		*ImprovConfig.LocalLLMEndpointURL, *ImprovConfig.LLMModelName);

	// For now, return placeholder
	return FString::Printf(TEXT("[Improv Response to: %s]"), *PlayerInput);
}

void UAIFacemaskACEImprovManager::RequestTTSConversion(const FString& Text, const ELBEASTACEVoiceType& VoiceType)
{
	// NOOP: TODO - Implement gRPC/HTTP request to local TTS service
	// 
	// BACKEND-AGNOSTIC IMPLEMENTATION:
	// Supports multiple backends (all available as options):
	// 
	// 1. NVIDIA Riva TTS (gRPC on port 50051)
	//    - Service: nvidia.riva.tts.RivaSpeechSynthesis
	//    - Method: Synthesize
	//    - Input: text, voice_name, sample_rate
	//    - Output: audio_data (PCM/WAV)
	// 
	// 2. Other local TTS services (HTTP REST API)
	//    - Standard HTTP POST request
	//    - Format depends on service (e.g., Coqui TTS, Piper TTS, etc.)
	//    - Should accept text and voice parameters
	//
	// Riva TTS example (gRPC):
	// - Service: nvidia.riva.tts.RivaSpeechSynthesis
	// - Method: Synthesize
	// - Input: text, voice_name, sample_rate, etc.
	// - Output: audio_data (PCM/WAV)
	//
	// After TTS completes, save audio to temp file and trigger Audio2Face conversion

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEImprovManager: Requesting TTS conversion from %s (voice: %d)"), 
		*ImprovConfig.LocalTTSEndpointURL, (int32)VoiceType);
}

void UAIFacemaskACEImprovManager::RequestAudio2FaceConversion(const FString& AudioFilePath)
{
	// NOOP: TODO - Implement HTTP/gRPC request to local Audio2Face microservice
	// 
	// BACKEND-AGNOSTIC IMPLEMENTATION:
	// Supports multiple backends (all available as options):
	// 
	// 1. NVIDIA NIM Audio2Face (HTTP REST API on port 8000)
	//    - POST /api/audio2face
	//    - Body: {"audio_file": "<base64>", "format": "wav", "stream": true}
	//    - Response: Stream of facial animation data (blend shapes + textures)
	// 
	// 2. Audio2Face-3D plugin (if integrated directly)
	//    - Direct function calls to plugin API
	//    - No HTTP/gRPC needed
	// 
	// 3. Other Audio2Face services
	//    - HTTP/gRPC endpoints as configured
	//    - Should accept audio and return facial animation data
	//
	// Request format (NIM Audio2Face example):
	// POST /api/audio2face
	// {
	//   "audio_file": "<base64 encoded audio>",
	//   "format": "wav",
	//   "stream": true  // Stream facial animation frames in real-time
	// }
	//
	// Response: Stream of facial animation data (blend shapes + textures)
	// This should be forwarded to AIFaceController.ReceiveFacialAnimationData()

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEImprovManager: Requesting Audio2Face conversion from %s (audio: %s)"), 
		*ImprovConfig.LocalAudio2FaceEndpointURL, *AudioFilePath);
}

FString UAIFacemaskACEImprovManager::BuildConversationContext(const FString& PlayerInput) const
{
	FString Context = ImprovConfig.CharacterSystemPrompt + TEXT("\n\n");
	
	// Add conversation history
	for (const FString& HistoryEntry : ConversationHistory)
	{
		Context += HistoryEntry + TEXT("\n");
	}
	
	// Add current player input
	Context += FString::Printf(TEXT("Player: %s\nAI:"), *PlayerInput);
	
	return Context;
}

