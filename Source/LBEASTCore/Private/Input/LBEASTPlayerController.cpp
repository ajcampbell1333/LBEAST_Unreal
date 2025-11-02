// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "Input/LBEASTPlayerController.h"
#include "Input/LBEASTInputAdapter.h"
#include "LBEASTExperienceBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EngineUtils.h"

ALBEASTPlayerController::ALBEASTPlayerController()
{
	// Enable input by default
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void ALBEASTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Auto-find experience if not already assigned
	if (bAutoFindExperience && !CurrentExperience)
	{
		for (TActorIterator<ALBEASTExperienceBase> It(GetWorld()); It; ++It)
		{
			CurrentExperience = *It;
			UE_LOG(LogTemp, Log, TEXT("[LBEASTPlayerController] Auto-assigned CurrentExperience: %s"), *CurrentExperience->GetName());
			break; // Use first experience found
		}

		if (!CurrentExperience)
		{
			UE_LOG(LogTemp, Warning, TEXT("[LBEASTPlayerController] No experience found in world. Enhanced Input will not work."));
		}
	}

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, MappingPriority);
			UE_LOG(LogTemp, Log, TEXT("[LBEASTPlayerController] Added Input Mapping Context: %s"), *InputMappingContext->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[LBEASTPlayerController] No InputMappingContext assigned. Create one in the editor and assign it."));
		}
	}
}

void ALBEASTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC)
	{
		UE_LOG(LogTemp, Error, TEXT("[LBEASTPlayerController] Enhanced Input Component not found. Enable Enhanced Input plugin."));
		return;
	}

	// Bind digital buttons (pressed and released)
	if (IA_Button0)
	{
		EIC->BindAction(IA_Button0, ETriggerEvent::Started, this, &ALBEASTPlayerController::OnButton0Pressed);
		EIC->BindAction(IA_Button0, ETriggerEvent::Completed, this, &ALBEASTPlayerController::OnButton0Released);
	}
	if (IA_Button1)
	{
		EIC->BindAction(IA_Button1, ETriggerEvent::Started, this, &ALBEASTPlayerController::OnButton1Pressed);
		EIC->BindAction(IA_Button1, ETriggerEvent::Completed, this, &ALBEASTPlayerController::OnButton1Released);
	}
	if (IA_Button2)
	{
		EIC->BindAction(IA_Button2, ETriggerEvent::Started, this, &ALBEASTPlayerController::OnButton2Pressed);
		EIC->BindAction(IA_Button2, ETriggerEvent::Completed, this, &ALBEASTPlayerController::OnButton2Released);
	}
	if (IA_Button3)
	{
		EIC->BindAction(IA_Button3, ETriggerEvent::Started, this, &ALBEASTPlayerController::OnButton3Pressed);
		EIC->BindAction(IA_Button3, ETriggerEvent::Completed, this, &ALBEASTPlayerController::OnButton3Released);
	}
	if (IA_Button4)
	{
		EIC->BindAction(IA_Button4, ETriggerEvent::Started, this, &ALBEASTPlayerController::OnButton4Pressed);
		EIC->BindAction(IA_Button4, ETriggerEvent::Completed, this, &ALBEASTPlayerController::OnButton4Released);
	}
	if (IA_Button5)
	{
		EIC->BindAction(IA_Button5, ETriggerEvent::Started, this, &ALBEASTPlayerController::OnButton5Pressed);
		EIC->BindAction(IA_Button5, ETriggerEvent::Completed, this, &ALBEASTPlayerController::OnButton5Released);
	}
	if (IA_Button6)
	{
		EIC->BindAction(IA_Button6, ETriggerEvent::Started, this, &ALBEASTPlayerController::OnButton6Pressed);
		EIC->BindAction(IA_Button6, ETriggerEvent::Completed, this, &ALBEASTPlayerController::OnButton6Released);
	}
	if (IA_Button7)
	{
		EIC->BindAction(IA_Button7, ETriggerEvent::Started, this, &ALBEASTPlayerController::OnButton7Pressed);
		EIC->BindAction(IA_Button7, ETriggerEvent::Completed, this, &ALBEASTPlayerController::OnButton7Released);
	}

	// Bind analog axes (continuous triggering)
	if (IA_Axis0)
		EIC->BindAction(IA_Axis0, ETriggerEvent::Triggered, this, &ALBEASTPlayerController::OnAxis0Changed);
	if (IA_Axis1)
		EIC->BindAction(IA_Axis1, ETriggerEvent::Triggered, this, &ALBEASTPlayerController::OnAxis1Changed);
	if (IA_Axis2)
		EIC->BindAction(IA_Axis2, ETriggerEvent::Triggered, this, &ALBEASTPlayerController::OnAxis2Changed);
	if (IA_Axis3)
		EIC->BindAction(IA_Axis3, ETriggerEvent::Triggered, this, &ALBEASTPlayerController::OnAxis3Changed);

	UE_LOG(LogTemp, Log, TEXT("[LBEASTPlayerController] Enhanced Input bindings created successfully."));
}

// ========================================
// DIGITAL INPUT CALLBACKS
// ========================================

void ALBEASTPlayerController::OnButton0Pressed(const FInputActionValue& Value)  { InjectButton(0, true); }
void ALBEASTPlayerController::OnButton0Released(const FInputActionValue& Value) { InjectButton(0, false); }
void ALBEASTPlayerController::OnButton1Pressed(const FInputActionValue& Value)  { InjectButton(1, true); }
void ALBEASTPlayerController::OnButton1Released(const FInputActionValue& Value) { InjectButton(1, false); }
void ALBEASTPlayerController::OnButton2Pressed(const FInputActionValue& Value)  { InjectButton(2, true); }
void ALBEASTPlayerController::OnButton2Released(const FInputActionValue& Value) { InjectButton(2, false); }
void ALBEASTPlayerController::OnButton3Pressed(const FInputActionValue& Value)  { InjectButton(3, true); }
void ALBEASTPlayerController::OnButton3Released(const FInputActionValue& Value) { InjectButton(3, false); }
void ALBEASTPlayerController::OnButton4Pressed(const FInputActionValue& Value)  { InjectButton(4, true); }
void ALBEASTPlayerController::OnButton4Released(const FInputActionValue& Value) { InjectButton(4, false); }
void ALBEASTPlayerController::OnButton5Pressed(const FInputActionValue& Value)  { InjectButton(5, true); }
void ALBEASTPlayerController::OnButton5Released(const FInputActionValue& Value) { InjectButton(5, false); }
void ALBEASTPlayerController::OnButton6Pressed(const FInputActionValue& Value)  { InjectButton(6, true); }
void ALBEASTPlayerController::OnButton6Released(const FInputActionValue& Value) { InjectButton(6, false); }
void ALBEASTPlayerController::OnButton7Pressed(const FInputActionValue& Value)  { InjectButton(7, true); }
void ALBEASTPlayerController::OnButton7Released(const FInputActionValue& Value) { InjectButton(7, false); }

// ========================================
// ANALOG INPUT CALLBACKS
// ========================================

void ALBEASTPlayerController::OnAxis0Changed(const FInputActionValue& Value) { InjectAxis(0, Value.Get<float>()); }
void ALBEASTPlayerController::OnAxis1Changed(const FInputActionValue& Value) { InjectAxis(1, Value.Get<float>()); }
void ALBEASTPlayerController::OnAxis2Changed(const FInputActionValue& Value) { InjectAxis(2, Value.Get<float>()); }
void ALBEASTPlayerController::OnAxis3Changed(const FInputActionValue& Value) { InjectAxis(3, Value.Get<float>()); }

// ========================================
// HELPERS
// ========================================

void ALBEASTPlayerController::InjectButton(int32 ButtonIndex, bool bPressed)
{
	if (!CurrentExperience)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LBEASTPlayerController] CurrentExperience is null. Cannot inject button input."));
		return;
	}

	if (!CurrentExperience->InputAdapter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LBEASTPlayerController] Experience has no InputAdapter. Cannot inject button input."));
		return;
	}

	if (bDebugLogInput)
	{
		UE_LOG(LogTemp, Log, TEXT("[LBEASTPlayerController] Button %d %s"), ButtonIndex, bPressed ? TEXT("Pressed") : TEXT("Released"));
	}

	if (bPressed)
		CurrentExperience->InputAdapter->InjectButtonPress(ButtonIndex);
	else
		CurrentExperience->InputAdapter->InjectButtonRelease(ButtonIndex);
}

void ALBEASTPlayerController::InjectAxis(int32 AxisIndex, float Value)
{
	if (!CurrentExperience)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LBEASTPlayerController] CurrentExperience is null. Cannot inject axis input."));
		return;
	}

	if (!CurrentExperience->InputAdapter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LBEASTPlayerController] Experience has no InputAdapter. Cannot inject axis input."));
		return;
	}

	if (bDebugLogInput)
	{
		UE_LOG(LogTemp, Log, TEXT("[LBEASTPlayerController] Axis %d = %.2f"), AxisIndex, Value);
	}

	CurrentExperience->InputAdapter->InjectAxisValue(AxisIndex, Value);
}

