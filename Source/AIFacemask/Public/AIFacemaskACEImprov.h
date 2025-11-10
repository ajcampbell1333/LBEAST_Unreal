// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AIFacemaskACEScript.h"
#include "AIFacemaskACEImprov.generated.h"

/**
 * Configuration for real-time improvised responses
 */
USTRUCT(BlueprintType)
struct FAIFacemaskACEImprovConfig
{
	GENERATED_BODY()

	/** Whether improvised responses are enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv")
	bool bEnableImprov = true;

	/** 
	 * Local LLM endpoint URL
	 * Supports multiple backends:
	 * - Ollama: "http://localhost:11434"
	 * - vLLM: "http://localhost:8000"
	 * - NVIDIA NIM: "http://localhost:8000"
	 * - Any OpenAI-compatible API endpoint
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv")
	FString LocalLLMEndpointURL;

	/** 
	 * LLM model name/ID
	 * Examples:
	 * - Ollama: "llama3.2:3b", "mistral:7b", or custom LoRA model name
	 * - vLLM/NIM: Model name as configured in your deployment
	 * - Custom LoRA: Specify the LoRA model identifier
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv")
	FString LLMModelName;

	/** System prompt/character context for the AI actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv", meta = (MultiLine = true))
	FString CharacterSystemPrompt;

	/** Maximum response length in tokens */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv", meta = (ClampMin = "10", ClampMax = "500"))
	int32 MaxResponseTokens = 150;

	/** Temperature for LLM generation (0.0 = deterministic, 1.0+ = creative) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float LLMTemperature = 0.7f;

	/** Whether to use local TTS or cloud TTS */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv")
	bool bUseLocalTTS = true;

	/** 
	 * Local TTS endpoint URL
	 * Supports multiple backends:
	 * - NVIDIA Riva TTS (gRPC): "localhost:50051"
	 * - Other TTS services: HTTP REST API endpoints
	 * - Format depends on service (gRPC for Riva, HTTP for others)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv")
	FString LocalTTSEndpointURL;

	/** Voice type for improvised responses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv")
	ELBEASTACEVoiceType VoiceType = ELBEASTACEVoiceType::Default;

	/** Whether to use local Audio2Face or cloud Audio2Face */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv")
	bool bUseLocalAudio2Face = true;

	/** 
	 * Local Audio2Face endpoint URL
	 * Supports multiple backends:
	 * - NVIDIA NIM Audio2Face: "http://localhost:8000"
	 * - Audio2Face-3D plugin: Direct integration (if available)
	 * - Other Audio2Face services: HTTP/gRPC endpoints
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv")
	FString LocalAudio2FaceEndpointURL;

	FAIFacemaskACEImprovConfig()
		: bEnableImprov(true)
		, LocalLLMEndpointURL(TEXT("http://localhost:11434"))
		, LLMModelName(TEXT("llama3.2:3b"))
		, CharacterSystemPrompt(TEXT("You are a helpful AI character in a VR experience."))
		, MaxResponseTokens(150)
		, LLMTemperature(0.7f)
		, bUseLocalTTS(true)
		, LocalTTSEndpointURL(TEXT("http://localhost:50051"))  // Riva TTS default gRPC port
		, VoiceType(ELBEASTACEVoiceType::Default)
		, bUseLocalAudio2Face(true)
		, LocalAudio2FaceEndpointURL(TEXT("http://localhost:8000"))  // NIM Audio2Face default
	{}
};

/**
 * Delegate for improvised response events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImprovResponseGenerated, const FString&, PlayerInput, const FString&, AIResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImprovResponseStarted, const FString&, AIResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImprovResponseFinished, const FString&, AIResponse);

/**
 * NVIDIA ACE Real-Time Improv Manager Component
 * 
 * Handles real-time improvised responses for AI-facemasked actors.
 * Enables player-to-AI conversations where:
 * 1. Player provides text input
 * 2. Local LLM (with optional LoRA) generates improvised response
 * 3. Local TTS (NVIDIA Riva) converts text → audio
 * 4. Local Audio2Face (NVIDIA NIM) converts audio → facial animation
 * 5. Facial animation streamed to AIFaceController in real-time
 * 
 * ALL PROCESSING IS LOCAL - No internet connection required for improv responses.
 * 
 * ARCHITECTURE:
 * - Local LLM: Supports Ollama, vLLM, NVIDIA NIM, or any OpenAI-compatible API (with custom LoRA support)
 * - Local TTS: Supports NVIDIA Riva (gRPC), or any HTTP REST TTS service
 * - Local Audio2Face: Supports NVIDIA NIM Audio2Face, Audio2Face-3D plugin, or other Audio2Face services
 * - All components run on the same dedicated server PC as the Unreal Engine server
 * - Developers can mix and match backends based on their needs, hardware, and preferences
 * 
 * FLEXIBLE BACKEND SUPPORT:
 * The system is backend-agnostic - configure endpoint URLs to point to any compatible service.
 * Example configurations:
 * - Option 1: NVIDIA NIM (all NVIDIA services) - Recommended for best integration
 * - Option 2: Ollama + Riva + Audio2Face - Good for open-source stack
 * - Option 3: vLLM + Riva + Audio2Face - Good for high-performance LLM inference
 * - Option 4: Mix and match - Use best tool for each component
 */
UCLASS(ClassGroup=(LBEAST), meta=(BlueprintSpawnableComponent))
class AIFACEMASK_API UAIFacemaskACEImprovManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIFacemaskACEImprovManager();

	/** Configuration for improvised responses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv")
	FAIFacemaskACEImprovConfig ImprovConfig;

	/** Conversation history (for context-aware responses) */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|ACE Improv")
	TArray<FString> ConversationHistory;

	/** Maximum conversation history entries to keep */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Improv", meta = (ClampMin = "1", ClampMax = "50"))
	int32 MaxConversationHistory = 10;

	/** Event fired when an improvised response is generated (text only) */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|ACE Improv")
	FOnImprovResponseGenerated OnImprovResponseGenerated;

	/** Event fired when improvised response playback starts (audio + facial animation) */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|ACE Improv")
	FOnImprovResponseStarted OnImprovResponseStarted;

	/** Event fired when improvised response playback finishes */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|ACE Improv")
	FOnImprovResponseFinished OnImprovResponseFinished;

	/**
	 * Initialize the improv manager
	 * @return true if initialization was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Improv")
	bool InitializeImprovManager();

	/**
	 * Generate an improvised response to player input
	 * @param PlayerInput - Player's text input/question
	 * @return Generated AI response text (empty if generation failed)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Improv")
	FString GenerateImprovResponse(const FString& PlayerInput);

	/**
	 * Generate and play an improvised response (text → LLM → TTS → Audio2Face → Facial animation)
	 * This is the main function to call when a player interacts with the AI actor
	 * @param PlayerInput - Player's text input/question
	 * @param bAsync - If true, generation happens asynchronously
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Improv")
	void GenerateAndPlayImprovResponse(const FString& PlayerInput, bool bAsync = true);

	/**
	 * Clear conversation history
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Improv")
	void ClearConversationHistory();

	/**
	 * Check if improv is currently generating/playing a response
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|ACE Improv")
	bool IsGeneratingResponse() const { return bIsGeneratingResponse; }

	/**
	 * Stop current improv response generation/playback
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Improv")
	void StopCurrentResponse();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Whether the improv manager is initialized */
	bool bIsInitialized = false;

	/** Whether we're currently generating a response */
	bool bIsGeneratingResponse = false;

	/** Current player input being processed */
	FString CurrentPlayerInput;

	/** Current AI response being generated/played */
	FString CurrentAIResponse;

	/**
	 * Request LLM to generate response (local LLM endpoint)
	 * NOOP: TODO - Implement HTTP request to local LLM (Ollama, vLLM, or NVIDIA NIM)
	 */
	FString RequestLLMResponse(const FString& PlayerInput, const FString& SystemPrompt, const TArray<FString>& InConversationHistory);

	/**
	 * Request TTS conversion (local TTS endpoint - NVIDIA Riva or other)
	 * NOOP: TODO - Implement gRPC/HTTP request to local TTS service
	 */
	void RequestTTSConversion(const FString& Text, const ELBEASTACEVoiceType& VoiceType);

	/**
	 * Request Audio2Face conversion (local Audio2Face endpoint - NVIDIA NIM)
	 * NOOP: TODO - Implement HTTP/gRPC request to local Audio2Face microservice
	 */
	void RequestAudio2FaceConversion(const FString& AudioFilePath);

	/**
	 * Build conversation context for LLM (includes system prompt + history)
	 */
	FString BuildConversationContext(const FString& PlayerInput) const;
};

