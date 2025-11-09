// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "AIFacemaskACEScriptManager.h"

UAIFacemaskACEScriptManager::UAIFacemaskACEScriptManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	bIsInitialized = false;
	bIsPlayingScript = false;
	CurrentScriptLineIndex = -1;
	ScriptPlaybackTimer = 0.0f;
	CurrentScriptLineStartTime = 0.0f;
	bWaitingForStartDelay = false;
	StartDelayTimer = 0.0f;
}

void UAIFacemaskACEScriptManager::BeginPlay()
{
	Super::BeginPlay();
}

void UAIFacemaskACEScriptManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsInitialized || !bIsPlayingScript)
	{
		return;
	}

	// Handle start delay
	if (bWaitingForStartDelay)
	{
		StartDelayTimer += DeltaTime;
		if (StartDelayTimer >= CurrentScript.StartDelay)
		{
			bWaitingForStartDelay = false;
			StartDelayTimer = 0.0f;
			// Start playing first script line
			if (CurrentScript.ScriptLines.Num() > 0)
			{
				StartScriptLine(0);
			}
		}
		return;
	}

	// Handle script line playback
	if (CurrentScriptLineIndex >= 0 && CurrentScriptLineIndex < CurrentScript.ScriptLines.Num())
	{
		const FAIFacemaskACEScriptLine& CurrentLine = CurrentScript.ScriptLines[CurrentScriptLineIndex];
		
		ScriptPlaybackTimer += DeltaTime;
		
		// Check if current line has finished (if we have duration estimate)
		if (CurrentLine.EstimatedDuration > 0.0f)
		{
			float ElapsedTime = ScriptPlaybackTimer - CurrentScriptLineStartTime;
			if (ElapsedTime >= CurrentLine.EstimatedDuration)
			{
				// Advance to next line or finish script
				AdvanceToNextScriptLine();
			}
		}
		// If no duration estimate, we rely on ACE server to signal completion
		// (This would require additional integration with ACE server callbacks)
	}
}

bool UAIFacemaskACEScriptManager::InitializeScriptManager(const FString& InACEServerBaseURL)
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEScriptManager: Already initialized"));
		return true;
	}

	ACEServerBaseURL = InACEServerBaseURL;
	bIsInitialized = true;

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Initialized with ACE server URL: %s"), *ACEServerBaseURL);
	
	return true;
}

bool UAIFacemaskACEScriptManager::TriggerScriptForState(FName StateName)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEScriptManager: Cannot trigger script - not initialized"));
		return false;
	}

	// Stop any currently playing script
	if (bIsPlayingScript)
	{
		StopCurrentScript();
	}

	// Find script for this state
	FAIFacemaskACEScript* Script = ScriptCollection.GetScriptForState(StateName);
	if (!Script)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEScriptManager: No script found for state '%s'"), *StateName.ToString());
		return false;
	}

	// Check if script is pre-baked
	if (!Script->bIsFullyPreBaked)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEScriptManager: Script for state '%s' is not pre-baked. Pre-baking now..."), *StateName.ToString());
		// Attempt to pre-bake synchronously (this may take time)
		PreBakeScriptForState(StateName, false);
	}

	// Start playing the script
	CurrentScript = *Script;
	CurrentScriptLineIndex = -1;
	ScriptPlaybackTimer = 0.0f;
	bIsPlayingScript = true;
	bWaitingForStartDelay = (CurrentScript.StartDelay > 0.0f);
	StartDelayTimer = 0.0f;

	// Broadcast script started event
	OnScriptStarted.Broadcast(StateName, CurrentScript);

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Started script for state '%s' (%d lines)"), 
		*StateName.ToString(), CurrentScript.ScriptLines.Num());

	// Request script playback from ACE server
	RequestScriptPlaybackFromACE(CurrentScript, 0);

	return true;
}

void UAIFacemaskACEScriptManager::StopCurrentScript()
{
	if (!bIsPlayingScript)
	{
		return;
	}

	FName CurrentStateName = CurrentScript.AssociatedStateName;
	
	bIsPlayingScript = false;
	CurrentScriptLineIndex = -1;
	ScriptPlaybackTimer = 0.0f;
	CurrentScriptLineStartTime = 0.0f;
	bWaitingForStartDelay = false;
	StartDelayTimer = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Stopped script for state '%s'"), *CurrentStateName.ToString());
}

void UAIFacemaskACEScriptManager::PreBakeAllScripts(bool bAsync)
{
	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Pre-baking all scripts (async: %s)"), bAsync ? TEXT("true") : TEXT("false"));
	
	// NOOP: TODO - Implement async pre-baking
	// For now, pre-bake synchronously
	for (auto& ScriptPair : ScriptCollection.ScriptsByState)
	{
		PreBakeScriptForState(ScriptPair.Key, false);
	}
}

void UAIFacemaskACEScriptManager::PreBakeScriptForState(FName StateName, bool bAsync)
{
	FAIFacemaskACEScript* Script = ScriptCollection.GetScriptForState(StateName);
	if (!Script)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFacemaskACEScriptManager: Cannot pre-bake - no script found for state '%s'"), *StateName.ToString());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Pre-baking script for state '%s' (async: %s)"), 
		*StateName.ToString(), bAsync ? TEXT("true") : TEXT("false"));

	// NOOP: TODO - Implement async pre-baking
	// For now, request pre-bake synchronously
	RequestScriptPreBakeFromACE(*Script);
}

bool UAIFacemaskACEScriptManager::HasScriptForState(FName StateName) const
{
	return ScriptCollection.HasScriptForState(StateName);
}

FAIFacemaskACEScript UAIFacemaskACEScriptManager::GetScriptForState(FName StateName) const
{
	FAIFacemaskACEScript* Script = ScriptCollection.GetScriptForState(StateName);
	if (Script)
	{
		return *Script;
	}
	return FAIFacemaskACEScript();
}

void UAIFacemaskACEScriptManager::HandleNarrativeStateChanged(FName OldState, FName NewState, int32 NewStateIndex)
{
	if (!bAutoTriggerOnStateChange)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Narrative state changed to '%s', triggering script..."), *NewState.ToString());
	
	TriggerScriptForState(NewState);
}

void UAIFacemaskACEScriptManager::StartScriptLine(int32 LineIndex)
{
	if (!CurrentScript.ScriptLines.IsValidIndex(LineIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("AIFacemaskACEScriptManager: Invalid script line index %d"), LineIndex);
		return;
	}

	CurrentScriptLineIndex = LineIndex;
	CurrentScriptLineStartTime = ScriptPlaybackTimer;
	
	const FAIFacemaskACEScriptLine& ScriptLine = CurrentScript.ScriptLines[LineIndex];

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Started script line %d: '%s'"), 
		LineIndex, *ScriptLine.TextPrompt);

	// Broadcast script line started event
	OnScriptLineStarted.Broadcast(CurrentScript.AssociatedStateName, LineIndex, ScriptLine);

	// Request this specific line from ACE server
	RequestScriptPlaybackFromACE(CurrentScript, LineIndex);
}

void UAIFacemaskACEScriptManager::AdvanceToNextScriptLine()
{
	if (CurrentScriptLineIndex < 0 || !CurrentScript.ScriptLines.IsValidIndex(CurrentScriptLineIndex))
	{
		return;
	}

	int32 NextLineIndex = CurrentScriptLineIndex + 1;

	// Check if there are more lines
	if (NextLineIndex < CurrentScript.ScriptLines.Num())
	{
		// Advance to next line
		StartScriptLine(NextLineIndex);
	}
	else
	{
		// Check if script should loop
		if (CurrentScript.bLoopScript)
		{
			// Loop back to first line
			StartScriptLine(0);
		}
		else
		{
			// Finish script
			FinishCurrentScript();
		}
	}
}

void UAIFacemaskACEScriptManager::FinishCurrentScript()
{
	FName StateName = CurrentScript.AssociatedStateName;
	
	bIsPlayingScript = false;
	CurrentScriptLineIndex = -1;
	ScriptPlaybackTimer = 0.0f;
	CurrentScriptLineStartTime = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Finished script for state '%s'"), *StateName.ToString());

	// Broadcast script finished event
	OnScriptFinished.Broadcast(StateName, CurrentScript);
}

void UAIFacemaskACEScriptManager::RequestScriptPlaybackFromACE(const FAIFacemaskACEScript& Script, int32 StartLineIndex)
{
	// NOOP: TODO - Implement HTTP request to NVIDIA ACE server
	// This should:
	// 1. Send POST request to ACE server endpoint (e.g., /api/playback/start)
	// 2. Include script ID, start line index, and any playback parameters
	// 3. ACE server will stream facial animation data back to AIFaceController
	
	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Requesting script playback from ACE server (State: %s, StartLine: %d)"), 
		*Script.AssociatedStateName.ToString(), StartLineIndex);
}

void UAIFacemaskACEScriptManager::RequestScriptPreBakeFromACE(const FAIFacemaskACEScript& Script)
{
	// NOOP: TODO - Implement HTTP request to NVIDIA ACE server for pre-baking
	// This should:
	// 1. For each script line:
	//    a. Send text prompt to Text-to-Speech service → Get audio file path
	//    b. Send audio file to Audio-to-Face service → Get facial animation data
	//    c. Cache both on ACE server with script line ID
	// 2. Update script line PreBakedAudioPath and bIsPreBaked flags
	// 3. Calculate EstimatedDuration for each line
	// 4. Mark script as fully pre-baked when all lines are done
	
	UE_LOG(LogTemp, Log, TEXT("AIFacemaskACEScriptManager: Requesting script pre-bake from ACE server (State: %s, %d lines)"), 
		*Script.AssociatedStateName.ToString(), Script.ScriptLines.Num());
}

