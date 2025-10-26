// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "ExperienceTemplates/AIFacemaskExperience.h"
#include "AIFaceController.h"
#include "EmbeddedDeviceController.h"
#include "ExperienceLoop/ExperienceStateMachine.h"

AAIFacemaskExperience::AAIFacemaskExperience()
{
	// Enable ticking for button input processing
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Create components
	FaceController = CreateDefaultSubobject<UAIFaceController>(TEXT("FaceController"));
	CostumeController = CreateDefaultSubobject<UEmbeddedDeviceController>(TEXT("CostumeController"));

	// Create Experience Loop state machine
	ExperienceLoop = CreateDefaultSubobject<UExperienceStateMachine>(TEXT("ExperienceLoop"));

	// Configure for multiplayer by default
	bMultiplayerEnabled = true;

	// Enable passthrough for live actors
	HMDConfig.bEnablePassthrough = true;
}

bool AAIFacemaskExperience::InitializeExperienceImpl()
{
	if (!Super::InitializeExperienceImpl())
	{
		return false;
	}

	// Initialize AI Face Controller (autonomous)
	if (FaceController && LiveActorMesh)
	{
		FAIFaceConfig FaceConfig;
		FaceConfig.TargetMesh = LiveActorMesh;
		FaceConfig.bUseAIGeneration = true;  // Autonomous AI-driven
		FaceConfig.UpdateRate = 30.0f;

		if (!FaceController->InitializeAIFace(FaceConfig))
		{
			UE_LOG(LogTemp, Error, TEXT("AIFacemaskExperience: Failed to initialize face controller"));
			return false;
		}
		
		UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: AI Face initialized (autonomous mode)"));
	}

	// Initialize Costume Controller (wrist-mounted buttons + haptics)
	if (CostumeController)
	{
		FEmbeddedDeviceConfig DeviceConfig;
		DeviceConfig.DeviceType = ELBEASTMicrocontrollerType::ESP32;
		DeviceConfig.Protocol = ELBEASTCommProtocol::WiFi;
		DeviceConfig.DeviceAddress = TEXT("192.168.1.50");
		DeviceConfig.Port = 8888;
		DeviceConfig.InputChannelCount = 4;  // 4 wrist buttons (2 left, 2 right)
		DeviceConfig.OutputChannelCount = 8; // 8 haptic vibrators

		if (!CostumeController->InitializeDevice(DeviceConfig))
		{
			UE_LOG(LogTemp, Warning, TEXT("AIFacemaskExperience: Costume controller initialization failed, continuing without embedded systems"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: Wrist controls connected (4 buttons)"));
		}
	}

	// Initialize Experience Loop with default states
	if (ExperienceLoop)
	{
		TArray<FExperienceState> DefaultStates;
		DefaultStates.Add(FExperienceState(FName("Intro"), TEXT("Introduction sequence")));
		DefaultStates.Add(FExperienceState(FName("Tutorial"), TEXT("Player tutorial")));
		DefaultStates.Add(FExperienceState(FName("Act1"), TEXT("First act")));
		DefaultStates.Add(FExperienceState(FName("Act2"), TEXT("Second act")));
		DefaultStates.Add(FExperienceState(FName("Finale"), TEXT("Finale sequence")));
		DefaultStates.Add(FExperienceState(FName("Credits"), TEXT("End credits")));

		ExperienceLoop->Initialize(DefaultStates);
		ExperienceLoop->OnStateChanged.AddDynamic(this, &AAIFacemaskExperience::OnExperienceStateChanged);
		ExperienceLoop->StartExperience();
		
		UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: Experience Loop initialized with %d states"), DefaultStates.Num());
	}

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: Initialized with %d live actors and %d players"), NumberOfLiveActors, NumberOfPlayers);
	return true;
}

void AAIFacemaskExperience::ShutdownExperienceImpl()
{
	// Stop experience loop
	if (ExperienceLoop)
	{
		ExperienceLoop->StopExperience();
	}

	// Disconnect embedded systems
	if (CostumeController && CostumeController->IsDeviceConnected())
	{
		CostumeController->DisconnectDevice();
	}

	Super::ShutdownExperienceImpl();
}

void AAIFacemaskExperience::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Process button input from wrist-mounted controls
	ProcessButtonInput();
}

void AAIFacemaskExperience::ProcessButtonInput()
{
	if (!CostumeController || !CostumeController->IsDeviceConnected() || !ExperienceLoop)
	{
		return;
	}

	// Read current button states
	bool CurrentButtonStates[4];
	for (int32 i = 0; i < 4; i++)
	{
		CurrentButtonStates[i] = CostumeController->GetDigitalInput(i);
	}

	// Button 0 (Left Wrist Forward) or Button 2 (Right Wrist Forward)
	if ((CurrentButtonStates[0] && !PreviousButtonStates[0]) || 
	    (CurrentButtonStates[2] && !PreviousButtonStates[2]))
	{
		AdvanceExperience();
	}

	// Button 1 (Left Wrist Backward) or Button 3 (Right Wrist Backward)
	if ((CurrentButtonStates[1] && !PreviousButtonStates[1]) || 
	    (CurrentButtonStates[3] && !PreviousButtonStates[3]))
	{
		RetreatExperience();
	}

	// Store current states for next frame
	for (int32 i = 0; i < 4; i++)
	{
		PreviousButtonStates[i] = CurrentButtonStates[i];
	}
}

void AAIFacemaskExperience::OnExperienceStateChanged(FName OldState, FName NewState, int32 NewStateIndex)
{
	UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: State changed from '%s' to '%s' (Index: %d)"), 
		*OldState.ToString(), *NewState.ToString(), NewStateIndex);
	
	// Override this function in Blueprint to trigger game events based on state changes
}

FName AAIFacemaskExperience::GetCurrentExperienceState() const
{
	if (ExperienceLoop)
	{
		return ExperienceLoop->GetCurrentStateName();
	}
	return NAME_None;
}

bool AAIFacemaskExperience::AdvanceExperience()
{
	if (ExperienceLoop)
	{
		return ExperienceLoop->AdvanceState();
	}
	return false;
}

bool AAIFacemaskExperience::RetreatExperience()
{
	if (ExperienceLoop)
	{
		return ExperienceLoop->RetreatState();
	}
	return false;
}
