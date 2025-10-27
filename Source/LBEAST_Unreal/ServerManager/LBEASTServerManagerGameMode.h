// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LBEASTServerManagerGameMode.generated.h"

/**
 * Server Manager Game Mode
 * 
 * Special game mode for the LBEAST Server Manager application.
 * Provides a UMG interface for:
 * - Starting/stopping dedicated game servers
 * - Monitoring server status (player count, experience state)
 * - Configuring Omniverse Audio2Face integration
 * - Viewing real-time logs
 * 
 * This runs on the dedicated server PC with a monitor/GUI.
 */
UCLASS()
class LBEAST_UNREAL_API ALBEASTServerManagerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALBEASTServerManagerGameMode();

protected:
	virtual void BeginPlay() override;

public:
	/** Widget class for the server manager UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LBEAST|Server Manager")
	TSubclassOf<class UUserWidget> ServerManagerWidgetClass;

	/** Instance of the server manager UI */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Server Manager")
	TObjectPtr<class UUserWidget> ServerManagerWidget;
};

