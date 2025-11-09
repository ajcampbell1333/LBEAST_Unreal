// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IVOIPAudioVisitor.h"
#include "AIFacemaskASRManager.generated.h"

/**
 * Delegate for ASR transcription events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnASRTranscriptionComplete, int32, PlayerId, const FString&, TranscribedText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnASRTranscriptionStarted, int32, PlayerId);

/**
 * Configuration for ASR (Automatic Speech Recognition)
 */
USTRUCT(BlueprintType)
struct FAIFacemaskASRConfig
{
	GENERATED_BODY()

	/** Whether ASR is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE ASR")
	bool bEnableASR = true;

	/** 
	 * Local ASR endpoint URL
	 * Supports multiple backends:
	 * - NVIDIA Riva ASR (gRPC): "localhost:50051"
	 * - Other ASR services: HTTP REST API endpoints
	 * - Format depends on service (gRPC for Riva, HTTP for others)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE ASR")
	FString LocalASREndpointURL;

	/** Whether to use local ASR or cloud ASR */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE ASR")
	bool bUseLocalASR = true;

	/** Language code for ASR (e.g., "en-US", "en-GB") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE ASR")
	FString LanguageCode = TEXT("en-US");

	/** Minimum audio duration to trigger ASR (seconds) - filters out brief noises */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE ASR", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float MinAudioDuration = 0.5f;

	/** Maximum audio duration to process (seconds) - prevents processing very long audio */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE ASR", meta = (ClampMin = "1.0", ClampMax = "30.0"))
	float MaxAudioDuration = 10.0f;

	/** Whether to automatically trigger improv after transcription */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE ASR")
	bool bAutoTriggerImprov = true;

	FAIFacemaskASRConfig()
		: bEnableASR(true)
		, LocalASREndpointURL(TEXT("localhost:50051"))  // Riva ASR default gRPC port
		, bUseLocalASR(true)
		, LanguageCode(TEXT("en-US"))
		, MinAudioDuration(0.5f)
		, MaxAudioDuration(10.0f)
		, bAutoTriggerImprov(true)
	{}
};

/**
 * NVIDIA ACE ASR Manager Component
 * 
 * Handles Automatic Speech Recognition (ASR) for player voice input.
 * Converts player speech (received via Mumble) to text for improv responses.
 * 
 * ARCHITECTURE:
 * - Runs on dedicated server (receives audio from Mumble)
 * - Receives audio streams from players via Mumble/VOIP
 * - Converts speech to text using local ASR (NVIDIA Riva) or cloud ASR
 * - Triggers improv manager with transcribed text
 * 
 * WORKFLOW:
 * 1. Player speaks into HMD microphone
 * 2. Audio captured by VOIPManager → Sent to Mumble server
 * 3. Server receives audio via Mumble → ASR Manager processes it
 * 4. ASR converts audio → text
 * 5. Text sent to ACEImprovManager → Generates improvised response
 * 6. Response converted to facial animation → Streamed to live actor's HMD
 * 
 * INTEGRATION:
 * - Subscribes to VOIPManager audio events (OnRemotePlayerAudioReceived)
 * - Buffers audio until speech ends (voice activity detection)
 * - Sends buffered audio to ASR service
 * - Forwards transcribed text to ACEImprovManager
 */
UCLASS(ClassGroup=(LBEAST), meta=(BlueprintSpawnableComponent))
class AIFACEMASK_API UAIFacemaskASRManager : public UActorComponent, public IVOIPAudioVisitor
{
	GENERATED_BODY()

public:
	UAIFacemaskASRManager();

	/** Configuration for ASR */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE ASR")
	FAIFacemaskASRConfig ASRConfig;

	/** Event fired when transcription completes */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|ACE ASR")
	FOnASRTranscriptionComplete OnTranscriptionComplete;

	/** Event fired when transcription starts */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|ACE ASR")
	FOnASRTranscriptionStarted OnTranscriptionStarted;

	/**
	 * Initialize the ASR manager
	 * @return true if initialization was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE ASR")
	bool InitializeASRManager();

	/**
	 * Process audio data from a player (called by VOIPManager when audio is received)
	 * @param PlayerId - Player ID who spoke
	 * @param AudioData - PCM audio data (from Mumble, decoded from Opus)
	 * @param SampleRate - Audio sample rate (typically 48000 for Mumble)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE ASR")
	void ProcessPlayerAudio(int32 PlayerId, const TArray<float>& AudioData, int32 SampleRate);

	// IVOIPAudioVisitor implementation
	virtual void OnPlayerAudioReceived(int32 PlayerId, const TArray<float>& AudioData, int32 SampleRate, const FVector& Position) override;

	/**
	 * Manually trigger transcription for a player (if audio buffering is enabled)
	 * @param PlayerId - Player ID to transcribe
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE ASR")
	void TriggerTranscriptionForPlayer(int32 PlayerId);

	/**
	 * Check if a player is currently being transcribed
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|ACE ASR")
	bool IsPlayerBeingTranscribed(int32 PlayerId) const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Whether the ASR manager is initialized */
	bool bIsInitialized = false;

	/** Audio buffers per player (for voice activity detection and buffering) */
	TMap<int32, TArray<float>> PlayerAudioBuffers;

	/** Timestamps for when audio started per player */
	TMap<int32, float> PlayerAudioStartTimes;

	/** Whether each player is currently speaking (voice activity detection) */
	TMap<int32, bool> PlayerSpeakingStates;

	/** Whether transcription is in progress per player */
	TMap<int32, bool> PlayerTranscribingStates;

	/** Timer for voice activity detection (silence detection) */
	float VoiceActivityTimer = 0.0f;

	/** Silence duration threshold (seconds) - if exceeded, trigger transcription */
	UPROPERTY(EditAnywhere, Category = "LBEAST|ACE ASR")
	float SilenceThreshold = 1.0f;

	/**
	 * Request ASR transcription from local ASR service
	 * NOOP: TODO - Implement gRPC/HTTP request to local ASR service
	 */
	void RequestASRTranscription(int32 PlayerId, const TArray<float>& AudioData, int32 SampleRate);

	/**
	 * Handle transcription result and trigger improv
	 */
	void HandleTranscriptionResult(int32 PlayerId, const FString& TranscribedText);

	/**
	 * Detect voice activity in audio buffer (simple energy-based VAD)
	 */
	bool DetectVoiceActivity(const TArray<float>& AudioData) const;

	/**
	 * Clear audio buffer for a player
	 */
	void ClearPlayerAudioBuffer(int32 PlayerId);
};

