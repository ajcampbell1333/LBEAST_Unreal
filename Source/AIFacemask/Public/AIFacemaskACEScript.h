// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AIFacemaskACEScript.generated.h"

/**
 * Script execution mode
 */
UENUM(BlueprintType)
enum class ELBEASTACEScriptMode : uint8
{
	/** Pre-baked script (text → TTS → Audio-to-Face, all cached on server) */
	PreBaked UMETA(DisplayName = "Pre-Baked"),
	
	/** Real-time/improv mode (text → TTS → Audio-to-Face, generated on-the-fly) */
	RealTime UMETA(DisplayName = "Real-Time/Improv")
};

/**
 * Voice configuration for NVIDIA ACE Text-to-Speech
 */
UENUM(BlueprintType)
enum class ELBEASTACEVoiceType : uint8
{
	/** Default voice (NVIDIA ACE default) */
	Default UMETA(DisplayName = "Default"),
	
	/** Male voice */
	Male UMETA(DisplayName = "Male"),
	
	/** Female voice */
	Female UMETA(DisplayName = "Female"),
	
	/** Custom voice model ID */
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Emotion preset for NVIDIA ACE Audio-to-Face
 * Influences facial expression generation during audio-to-face conversion
 */
UENUM(BlueprintType)
enum class ELBEASTACEEmotionPreset : uint8
{
	/** Neutral emotion */
	Neutral UMETA(DisplayName = "Neutral"),
	
	/** Happy/excited */
	Happy UMETA(DisplayName = "Happy"),
	
	/** Sad/melancholic */
	Sad UMETA(DisplayName = "Sad"),
	
	/** Angry/intense */
	Angry UMETA(DisplayName = "Angry"),
	
	/** Surprised/shocked */
	Surprised UMETA(DisplayName = "Surprised"),
	
	/** Fearful/anxious */
	Fearful UMETA(DisplayName = "Fearful"),
	
	/** Disgusted */
	Disgusted UMETA(DisplayName = "Disgusted"),
	
	/** Custom emotion (specified in script) */
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Single script line/segment for NVIDIA ACE performance
 * 
 * Represents one piece of dialogue that will be:
 * 1. Converted from text-to-speech (TTS) → Audio file (pre-baked or real-time)
 * 2. Processed through audio-to-face → Facial textures + blend shapes
 * 3. Streamed to Unreal Engine for real-time application
 * 
 * Can operate in two modes:
 * - PreBaked: Text → TTS → Audio-to-Face (all cached on ACE server)
 * - RealTime: Text → TTS → Audio-to-Face (generated on-the-fly, supports improv)
 */
USTRUCT(BlueprintType)
struct FAIFacemaskACEScriptLine
{
	GENERATED_BODY()

	/** Execution mode for this script line */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	ELBEASTACEScriptMode ScriptMode = ELBEASTACEScriptMode::PreBaked;

	/** Text prompt/dialogue for this script line */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	FString TextPrompt;

	/** Voice type for text-to-speech conversion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	ELBEASTACEVoiceType VoiceType = ELBEASTACEVoiceType::Default;

	/** Custom voice model ID (if VoiceType is Custom) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	FString CustomVoiceModelID;

	/** Emotion preset for audio-to-face conversion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	ELBEASTACEEmotionPreset EmotionPreset = ELBEASTACEEmotionPreset::Neutral;

	/** Custom emotion parameters (if EmotionPreset is Custom) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	TMap<FString, float> CustomEmotionParams;

	/** Pre-baked audio file path (on ACE server) - set after TTS conversion */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|ACE Script")
	FString PreBakedAudioPath;

	/** Estimated duration in seconds (calculated after TTS conversion) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|ACE Script")
	float EstimatedDuration = 0.0f;

	/** Whether this script line has been pre-baked (TTS + Audio-to-Face processed) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|ACE Script")
	bool bIsPreBaked = false;

	/** Unique identifier for this script line (for ACE server caching) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|ACE Script")
	FString ScriptLineID;

	/** Whether this is an improvised line (generated dynamically, not from script) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|ACE Script")
	bool bIsImprovLine = false;

	FAIFacemaskACEScriptLine()
		: TextPrompt(TEXT(""))
		, VoiceType(ELBEASTACEVoiceType::Default)
		, EmotionPreset(ELBEASTACEEmotionPreset::Neutral)
		, EstimatedDuration(0.0f)
		, bIsPreBaked(false)
	{
		// Generate unique ID for this script line
		ScriptLineID = FGuid::NewGuid().ToString();
	}
};

/**
 * Complete script for a narrative state
 * 
 * Contains all script lines that will be performed when this narrative state is active.
 * Scripts are pre-baked on the NVIDIA ACE server:
 * - Text prompts → Text-to-Speech → Audio files (cached on ACE server)
 * - Audio files → Audio-to-Face → Facial animation data (cached on ACE server)
 * 
 * When narrative state changes, the corresponding script is triggered and played sequentially.
 */
USTRUCT(BlueprintType)
struct FAIFacemaskACEScript
{
	GENERATED_BODY()

	/** Narrative state name this script is associated with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	FName AssociatedStateName;

	/** Human-readable description of this script */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	FString Description;

	/** Script lines to perform (played sequentially) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	TArray<FAIFacemaskACEScriptLine> ScriptLines;

	/** Whether to loop this script (repeat when finished) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	bool bLoopScript = false;

	/** Delay before starting script playback (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script", meta = (ClampMin = "0.0"))
	float StartDelay = 0.0f;

	/** Total estimated duration (sum of all script lines) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|ACE Script")
	float TotalEstimatedDuration = 0.0f;

	/** Whether all script lines have been pre-baked */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|ACE Script")
	bool bIsFullyPreBaked = false;

	FAIFacemaskACEScript()
		: AssociatedStateName(NAME_None)
		, Description(TEXT(""))
		, bLoopScript(false)
		, StartDelay(0.0f)
		, TotalEstimatedDuration(0.0f)
		, bIsFullyPreBaked(false)
	{}
};

/**
 * Collection of pre-baked scripts for NVIDIA ACE facemask performances
 * 
 * Maps narrative states to scripts that will be automatically triggered when states change.
 * Scripts are pre-baked on the NVIDIA ACE server to ensure smooth, low-latency playback.
 * 
 * WORKFLOW:
 * 1. Define scripts in this collection (text prompts + voice/emotion settings)
 * 2. Pre-bake scripts on ACE server (Text-to-Speech → Audio, Audio-to-Face → Facial data)
 * 3. When narrative state changes, corresponding script is automatically triggered
 * 4. ACE server streams pre-baked facial animation data to Unreal Engine
 * 5. AIFaceController receives and applies facial animation in real-time
 */
USTRUCT(BlueprintType)
struct FAIFacemaskACEScriptCollection
{
	GENERATED_BODY()

	/** Collection name/identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	FString CollectionName;

	/** Scripts mapped by narrative state name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	TMap<FName, FAIFacemaskACEScript> ScriptsByState;

	/** Whether to auto-trigger scripts on narrative state changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	bool bAutoTriggerOnStateChange = true;

	/** Whether all scripts in this collection have been pre-baked */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|ACE Script")
	bool bIsFullyPreBaked = false;

	FAIFacemaskACEScriptCollection()
		: CollectionName(TEXT("Default"))
		, bAutoTriggerOnStateChange(true)
		, bIsFullyPreBaked(false)
	{}

	/**
	 * Get script for a specific narrative state
	 */
	FAIFacemaskACEScript* GetScriptForState(FName StateName)
	{
		return ScriptsByState.Find(StateName);
	}

	/**
	 * Check if a script exists for a state
	 */
	bool HasScriptForState(FName StateName) const
	{
		return ScriptsByState.Contains(StateName);
	}
};

