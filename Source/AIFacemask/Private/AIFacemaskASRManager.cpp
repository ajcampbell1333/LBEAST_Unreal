// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "AIFacemaskASRManager.h"
#include "VOIP/IVOIPAudioVisitor.h"

UAIFacemaskASRManager::UAIFacemaskASRManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	bIsInitialized = false;
	VoiceActivityTimer = 0.0f;
	SilenceThreshold = 1.0f;
}

void UAIFacemaskASRManager::BeginPlay()
{
	Super::BeginPlay();
}

void UAIFacemaskASRManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsInitialized)
	{
		return;
	}

	// Check for silence periods (voice activity detection)
	// If a player was speaking but has been silent for SilenceThreshold seconds, trigger transcription
	VoiceActivityTimer += DeltaTime;
	
	if (VoiceActivityTimer >= SilenceThreshold)
	{
		VoiceActivityTimer = 0.0f;
		
		for (auto& PlayerPair : PlayerSpeakingStates)
		{
			int32 PlayerId = PlayerPair.Key;
			bool bWasSpeaking = PlayerPair.Value;
			
			if (bWasSpeaking && !PlayerTranscribingStates.Contains(PlayerId))
			{
				// Player was speaking but has been silent - trigger transcription
				TriggerTranscriptionForPlayer(PlayerId);
			}
		}
	}
}

bool UAIFacemaskASRManager::InitializeASRManager()
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskASRManager: Already initialized"));
		return true;
	}

	if (!ASRConfig.bEnableASR)
	{
		UE_LOG(LogTemp, Log, TEXT("AIFacemaskASRManager: ASR is disabled in config"));
		return false;
	}

	bIsInitialized = true;

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskASRManager: Initialized with local ASR: %s (language: %s)"), 
		ASRConfig.bUseLocalASR ? *ASRConfig.LocalASREndpointURL : TEXT("Cloud"),
		*ASRConfig.LanguageCode);
	
	return true;
}

void UAIFacemaskASRManager::ProcessPlayerAudio(int32 PlayerId, const TArray<float>& AudioData, int32 SampleRate)
{
	if (!bIsInitialized || !ASRConfig.bEnableASR)
	{
		return;
	}

	// Detect voice activity
	bool bHasVoiceActivity = DetectVoiceActivity(AudioData);
	
	// Update speaking state
	PlayerSpeakingStates.Add(PlayerId, bHasVoiceActivity);
	
	if (bHasVoiceActivity)
	{
		// Reset silence timer
		VoiceActivityTimer = 0.0f;
		
		// Add audio to buffer
		if (!PlayerAudioBuffers.Contains(PlayerId))
		{
			PlayerAudioBuffers.Add(PlayerId, TArray<float>());
			PlayerAudioStartTimes.Add(PlayerId, GetWorld()->GetTimeSeconds());
		}
		
		PlayerAudioBuffers[PlayerId].Append(AudioData);
		
		// Check if buffer exceeds max duration
		float AudioDuration = PlayerAudioBuffers[PlayerId].Num() / (float)SampleRate;
		if (AudioDuration >= ASRConfig.MaxAudioDuration)
		{
			// Buffer is full, trigger transcription
			TriggerTranscriptionForPlayer(PlayerId);
		}
	}
}

void UAIFacemaskASRManager::TriggerTranscriptionForPlayer(int32 PlayerId)
{
	if (!PlayerAudioBuffers.Contains(PlayerId))
	{
		return;
	}

	TArray<float>& AudioBuffer = PlayerAudioBuffers[PlayerId];
	
	// Check minimum duration
	float AudioDuration = AudioBuffer.Num() / 48000.0f;  // Assume 48kHz (Mumble default)
	if (AudioDuration < ASRConfig.MinAudioDuration)
	{
		// Audio too short, clear buffer and skip
		ClearPlayerAudioBuffer(PlayerId);
		return;
	}

	// Mark as transcribing
	PlayerTranscribingStates.Add(PlayerId, true);
	PlayerSpeakingStates.Add(PlayerId, false);
	
	// Broadcast transcription started
	OnTranscriptionStarted.Broadcast(PlayerId);
	
	UE_LOG(LogTemp, Log, TEXT("AIFacemaskASRManager: Starting transcription for player %d (duration: %.2fs)"), 
		PlayerId, AudioDuration);
	
	// Request ASR transcription
	RequestASRTranscription(PlayerId, AudioBuffer, 48000);  // Mumble uses 48kHz
	
	// Clear buffer
	ClearPlayerAudioBuffer(PlayerId);
}

bool UAIFacemaskASRManager::IsPlayerBeingTranscribed(int32 PlayerId) const
{
	return PlayerTranscribingStates.Contains(PlayerId) && PlayerTranscribingStates[PlayerId];
}

void UAIFacemaskASRManager::RequestASRTranscription(int32 PlayerId, const TArray<float>& AudioData, int32 SampleRate)
{
	// NOOP: TODO - Implement gRPC/HTTP request to local ASR service
	// 
	// BACKEND-AGNOSTIC IMPLEMENTATION:
	// Supports multiple backends (all available as options):
	// 
	// 1. NVIDIA Riva ASR (gRPC on port 50051)
	//    - Service: nvidia.riva.asr.RivaSpeechRecognition
	//    - Method: Recognize
	//    - Input: audio_data (PCM), sample_rate, language_code
	//    - Output: transcript (text)
	// 
	// 2. Other local ASR services (HTTP REST API)
	//    - Standard HTTP POST request
	//    - Format depends on service (e.g., Whisper API, DeepSpeech, etc.)
	//    - Should accept audio and return transcript
	//
	// After transcription completes, call HandleTranscriptionResult()

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskASRManager: Requesting ASR transcription from %s (player: %d, samples: %d)"), 
		*ASRConfig.LocalASREndpointURL, PlayerId, AudioData.Num());
	
	// For now, simulate transcription result (remove this when implementing actual ASR)
	// HandleTranscriptionResult(PlayerId, FString::Printf(TEXT("[Transcribed speech from player %d]"), PlayerId));
}

void UAIFacemaskASRManager::HandleTranscriptionResult(int32 PlayerId, const FString& TranscribedText)
{
	// Clear transcribing state
	PlayerTranscribingStates.Remove(PlayerId);
	
	if (TranscribedText.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskASRManager: Transcription returned empty text for player %d"), PlayerId);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskASRManager: Transcription complete for player %d: '%s'"), 
		PlayerId, *TranscribedText);
	
	// Broadcast transcription complete
	OnTranscriptionComplete.Broadcast(PlayerId, TranscribedText);
	
	// Auto-trigger improv if enabled
	if (ASRConfig.bAutoTriggerImprov)
	{
		// NOOP: TODO - Get reference to ACEImprovManager and call GenerateAndPlayImprovResponse()
		// This should be done via the experience actor (AAIFacemaskExperience)
		// For now, log that we would trigger improv
		UE_LOG(LogTemp, Log, TEXT("AIFacemaskASRManager: Would trigger improv with text: '%s'"), *TranscribedText);
	}
}

bool UAIFacemaskASRManager::DetectVoiceActivity(const TArray<float>& AudioData) const
{
	// Simple energy-based voice activity detection
	// Calculate RMS (Root Mean Square) energy
	float Energy = 0.0f;
	for (float Sample : AudioData)
	{
		Energy += Sample * Sample;
	}
	Energy = FMath::Sqrt(Energy / AudioData.Num());
	
	// Threshold for voice activity (adjust based on testing)
	float VoiceThreshold = 0.01f;  // Normalized audio threshold
	
	return Energy > VoiceThreshold;
}

void UAIFacemaskASRManager::ClearPlayerAudioBuffer(int32 PlayerId)
{
	PlayerAudioBuffers.Remove(PlayerId);
	PlayerAudioStartTimes.Remove(PlayerId);
	PlayerSpeakingStates.Remove(PlayerId);
}

void UAIFacemaskASRManager::OnPlayerAudioReceived(int32 PlayerId, const TArray<float>& AudioData, int32 SampleRate, const FVector& Position)
{
	// This is called by VOIPManager via the visitor interface
	// Process the audio for ASR
	ProcessPlayerAudio(PlayerId, AudioData, SampleRate);
}

