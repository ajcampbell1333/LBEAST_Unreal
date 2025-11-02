// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LBEASTHMDInterface.h"
#include "LBEASTTrackingInterface.h"
#include "Networking/LBEASTServerCommandProtocol.h"
#include "LBEASTExperienceInterface.h"
#include "LBEASTExperienceBase.generated.h"

// Forward declarations
class ULBEASTInputAdapter;

/**
 * Server mode for multiplayer experiences
 */
UENUM(BlueprintType)
enum class ELBEASTServerMode : uint8
{
	/** Dedicated server (no local player, headless capable) */
	DedicatedServer UMETA(DisplayName = "Dedicated Server"),
	
	/** Listen server (host player + server) */
	ListenServer UMETA(DisplayName = "Listen Server"),
	
	/** Client only (connect to existing server) */
	Client UMETA(DisplayName = "Client"),
	
	/** Standalone (no networking) */
	Standalone UMETA(DisplayName = "Standalone")
};

/**
 * Base class for all LBEAST Experience Templates
 * 
 * Experience Templates are pre-configured, drag-and-drop solutions that combine
 * multiple LBEAST APIs to create complete LBE experiences. They provide:
 * - Pre-configured hardware setups
 * - Default component arrangements
 * - Blueprint-friendly interfaces
 * - Quick deployment capabilities
 * 
 * Developers can use these as-is or extend them for custom experiences.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(LBEAST))
class LBEASTEXPERIENCES_API ALBEASTExperienceBase : public AActor, public ILBEASTExperienceInterface
{
	GENERATED_BODY()
	
public:	
	ALBEASTExperienceBase();

	/** HMD configuration for this experience */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Experience")
	FLBEASTHMDConfig HMDConfig;

	/** Tracking configuration for this experience */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Experience")
	FLBEASTTrackingConfig TrackingConfig;

	/** Whether to auto-initialize on BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Experience")
	bool bAutoInitialize = true;

	/** Whether this experience supports multiplayer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Experience")
	bool bMultiplayerEnabled = false;

	/** Server mode (dedicated, listen, client, standalone) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Experience|Networking")
	ELBEASTServerMode ServerMode = ELBEASTServerMode::Standalone;

	/** Whether to enforce the required server mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Experience|Networking")
	bool bEnforceServerMode = false;

	/** Required server mode (used when bEnforceServerMode is true) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|Experience|Networking")
	ELBEASTServerMode RequiredServerMode = ELBEASTServerMode::Standalone;

	// ========================================
	// COMPONENTS
	// ========================================

	/** 
	 * Input adapter component for handling all input sources (embedded systems, VR, keyboard, etc.)
	 * Auto-created in constructor. Configure in InitializeExperienceImpl().
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|Components")
	TObjectPtr<ULBEASTInputAdapter> InputAdapter;

	/**
	 * Command protocol for receiving remote commands from Command Console
	 * Auto-created on dedicated server. Processes commands in Tick().
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LBEAST|Components")
	TObjectPtr<ULBEASTServerCommandProtocol> CommandProtocol;

	/**
	 * Initialize the experience
	 * Called automatically if bAutoInitialize is true, or manually by developer
	 * @return true if initialization was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Experience")
	virtual bool InitializeExperience();

	/**
	 * Shutdown the experience and cleanup resources
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Experience")
	virtual void ShutdownExperience();

	/**
	 * Check if the experience is currently active and initialized
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Experience")
	bool IsExperienceActive() const { return bIsInitialized; }

	/**
	 * Get the number of players this experience supports
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Experience")
	virtual int32 GetMaxPlayers() const { return 1; }

	// ========================================
	// ILBEASTExperienceInterface Implementation
	// ========================================

	/** Get the InputAdapter component (implements ILBEASTExperienceInterface) */
	virtual ULBEASTInputAdapter* GetInputAdapter() const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Whether the experience has been initialized */
	bool bIsInitialized = false;

	/**
	 * Override this to perform custom initialization logic
	 */
	virtual bool InitializeExperienceImpl();

	/**
	 * Override this to perform custom shutdown logic
	 */
	virtual void ShutdownExperienceImpl();

	virtual void Tick(float DeltaTime) override;

	/**
	 * Handle incoming command from Command Console
	 * Override this to handle custom commands in derived classes
	 */
	UFUNCTION()
	virtual void OnCommandReceived(const FLBEASTServerCommandMessage& Command, ULBEASTServerCommandProtocol* Protocol);

	/**
	 * Initialize command protocol for dedicated server mode
	 */
	void InitializeCommandProtocol();
};

