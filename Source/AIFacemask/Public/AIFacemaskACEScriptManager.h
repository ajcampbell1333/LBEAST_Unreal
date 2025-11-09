// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIFacemaskACEScript.h"
#include "AIFacemaskACEImprov.h"
#include "AIFacemaskACEScriptManager.generated.h"

/**
 * Delegate for script playback events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnACEScriptStarted, FName, StateName, const FAIFacemaskACEScript&, Script);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnACEScriptLineStarted, FName, StateName, int32, LineIndex, const FAIFacemaskACEScriptLine&, ScriptLine);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnACEScriptFinished, FName, StateName, const FAIFacemaskACEScript&, Script);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnACEScriptPreBakeComplete, FName, StateName);

/**
 * NVIDIA ACE Script Manager Component
 * 
 * Manages pre-baked script collections for NVIDIA ACE facemask performances.
 * Automatically triggers scripts when narrative states change.
 * 
 * WORKFLOW:
 * 1. Define script collection (text prompts + voice/emotion settings)
 * 2. Pre-bake scripts on ACE server (Text-to-Speech → Audio, Audio-to-Face → Facial data)
 * 3. When narrative state changes, corresponding script is automatically triggered
 * 4. ACE server streams pre-baked facial animation data to AIFaceController
 * 
 * INTEGRATION:
 * - Subscribes to narrative state machine OnStateChanged events
 * - Maps narrative states to pre-baked ACE scripts
 * - Communicates with NVIDIA ACE server for pre-baking and playback
 * - Supports advance/retreat through script lines
 */
UCLASS(ClassGroup=(LBEAST), meta=(BlueprintSpawnableComponent))
class AIFACEMASK_API UAIFacemaskACEScriptManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIFacemaskACEScriptManager();

	/** Script collection for this experience */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	FAIFacemaskACEScriptCollection ScriptCollection;

	/** NVIDIA ACE server base URL (e.g., "http://192.168.1.100:8000") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	FString ACEServerBaseURL;

	/** Whether to auto-trigger scripts on narrative state changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|ACE Script")
	bool bAutoTriggerOnStateChange = true;

	/** Currently playing script (if any) */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|ACE Script")
	FAIFacemaskACEScript CurrentScript;

	/** Current script line index being played */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|ACE Script")
	int32 CurrentScriptLineIndex = -1;

	/** Whether a script is currently playing */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|ACE Script")
	bool bIsPlayingScript = false;

	/** Event fired when a script starts playing */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|ACE Script")
	FOnACEScriptStarted OnScriptStarted;

	/** Event fired when a script line starts playing */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|ACE Script")
	FOnACEScriptLineStarted OnScriptLineStarted;

	/** Event fired when a script finishes playing */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|ACE Script")
	FOnACEScriptFinished OnScriptFinished;

	/** Event fired when script pre-baking completes */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|ACE Script")
	FOnACEScriptPreBakeComplete OnScriptPreBakeComplete;

	/**
	 * Initialize the script manager
	 * @param InACEServerBaseURL - Base URL for NVIDIA ACE server
	 * @return true if initialization was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Script")
	bool InitializeScriptManager(const FString& InACEServerBaseURL);

	/**
	 * Trigger a script for a specific narrative state
	 * @param StateName - Narrative state name to trigger script for
	 * @return true if script was found and triggered
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Script")
	bool TriggerScriptForState(FName StateName);

	/**
	 * Stop the currently playing script
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Script")
	void StopCurrentScript();

	/**
	 * Pre-bake all scripts in the collection on the ACE server
	 * This converts text prompts to audio and processes audio-to-face
	 * @param bAsync - If true, pre-baking happens asynchronously
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Script")
	void PreBakeAllScripts(bool bAsync = true);

	/**
	 * Pre-bake a specific script for a state
	 * @param StateName - State name to pre-bake script for
	 * @param bAsync - If true, pre-baking happens asynchronously
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Script")
	void PreBakeScriptForState(FName StateName, bool bAsync = true);

	/**
	 * Check if a script exists for a state
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|ACE Script")
	bool HasScriptForState(FName StateName) const;

	/**
	 * Get script for a specific state
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|ACE Script")
	FAIFacemaskACEScript GetScriptForState(FName StateName) const;

	/**
	 * Handle narrative state change (called by experience base)
	 * @param OldState - Previous state name
	 * @param NewState - New state name
	 * @param NewStateIndex - Index of new state
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|ACE Script")
	void HandleNarrativeStateChanged(FName OldState, FName NewState, int32 NewStateIndex);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Whether the script manager is initialized */
	bool bIsInitialized = false;

	/** Timer for script playback */
	float ScriptPlaybackTimer = 0.0f;

	/** Current script line start time */
	float CurrentScriptLineStartTime = 0.0f;

	/** Whether we're waiting for script start delay */
	bool bWaitingForStartDelay = false;

	/** Start delay timer */
	float StartDelayTimer = 0.0f;

	/**
	 * Start playing a script line
	 */
	void StartScriptLine(int32 LineIndex);

	/**
	 * Advance to next script line (or finish script)
	 */
	void AdvanceToNextScriptLine();

	/**
	 * Finish current script
	 */
	void FinishCurrentScript();

	/**
	 * Request script playback from ACE server
	 * NOOP: TODO - Implement HTTP request to ACE server
	 */
	void RequestScriptPlaybackFromACE(const FAIFacemaskACEScript& Script, int32 StartLineIndex = 0);

	/**
	 * Request script pre-baking from ACE server
	 * NOOP: TODO - Implement HTTP request to ACE server for pre-baking
	 */
	void RequestScriptPreBakeFromACE(const FAIFacemaskACEScript& Script);
};

