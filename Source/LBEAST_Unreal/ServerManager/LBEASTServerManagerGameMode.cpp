// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "LBEASTServerManagerGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ALBEASTServerManagerGameMode::ALBEASTServerManagerGameMode()
{
	// Set default pawn to none (we don't need a player character for server manager)
	DefaultPawnClass = nullptr;
}

void ALBEASTServerManagerGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Create and display the server manager UI
	if (ServerManagerWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			ServerManagerWidget = CreateWidget<UUserWidget>(PC, ServerManagerWidgetClass);
			if (ServerManagerWidget)
			{
				ServerManagerWidget->AddToViewport();
				
				// Set input mode to UI only
				PC->SetInputMode(FInputModeUIOnly());
				PC->bShowMouseCursor = true;

				UE_LOG(LogTemp, Log, TEXT("LBEASTServerManager: UI initialized"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LBEASTServerManager: ServerManagerWidgetClass not set!"));
	}
}

