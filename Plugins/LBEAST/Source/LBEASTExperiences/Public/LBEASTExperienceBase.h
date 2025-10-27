// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LBEASTHMDInterface.h"
#include "LBEASTTrackingInterface.h"
#include "LBEASTExperienceBase.generated.h"

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
class LBEASTEXPERIENCES_API ALBEASTExperienceBase : public AActor
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
};

