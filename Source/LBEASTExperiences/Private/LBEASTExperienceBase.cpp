// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "LBEASTExperienceBase.h"
#include "Input/LBEASTInputAdapter.h"
#include "Networking/LBEASTServerCommandProtocol.h"
#include "ExperienceLoop/ExperienceStateMachine.h"
#include "LBEASTWorldPositionCalibrator.h"
#include "GameFramework/GameStateBase.h"

ALBEASTExperienceBase::ALBEASTExperienceBase()
{
	// Enable ticking for command protocol processing
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Create input adapter component
	InputAdapter = CreateDefaultSubobject<ULBEASTInputAdapter>(TEXT("InputAdapter"));

	// Create command protocol component (will be initialized on dedicated server)
	CommandProtocol = CreateDefaultSubobject<ULBEASTServerCommandProtocol>(TEXT("CommandProtocol"));

	// Narrative state machine will be created in InitializeExperienceImpl if bUseNarrativeStateMachine is true
	NarrativeStateMachine = nullptr;

	// Create world position calibrator (available to all experiences)
	WorldPositionCalibrator = CreateDefaultSubobject<ULBEASTWorldPositionCalibrator>(TEXT("WorldPositionCalibrator"));

	// Default HMD configuration
	HMDConfig.HMDType = ELBEASTHMDType::OpenXR;
	HMDConfig.bEnablePassthrough = false;
	HMDConfig.PassthroughAlpha = 0.0f;

	// Default tracking configuration
	TrackingConfig.TrackingSystem = ELBEASTTrackingSystem::SteamVRTrackers;
	TrackingConfig.ExpectedDeviceCount = 0;
}

void ALBEASTExperienceBase::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoInitialize)
	{
		InitializeExperience();
	}
}

void ALBEASTExperienceBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ShutdownExperience();
	Super::EndPlay(EndPlayReason);
}

bool ALBEASTExperienceBase::InitializeExperience()
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTExperience: Already initialized"));
		return true;
	}

	UE_LOG(LogTemp, Log, TEXT("LBEASTExperience: Initializing experience..."));

	// Call derived class implementation
	if (!InitializeExperienceImpl())
	{
		UE_LOG(LogTemp, Error, TEXT("LBEASTExperience: Failed to initialize experience"));
		return false;
	}

	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("LBEASTExperience: Initialization complete"));
	return true;
}

void ALBEASTExperienceBase::ShutdownExperience()
{
	if (!bIsInitialized)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("LBEASTExperience: Shutting down experience..."));

	ShutdownExperienceImpl();

	bIsInitialized = false;
	UE_LOG(LogTemp, Log, TEXT("LBEASTExperience: Shutdown complete"));
}

bool ALBEASTExperienceBase::InitializeExperienceImpl()
{
	// Initialize command protocol if running as dedicated server
	InitializeCommandProtocol();

	// Initialize narrative state machine if enabled
	if (bUseNarrativeStateMachine && !NarrativeStateMachine)
	{
		NarrativeStateMachine = NewObject<UExperienceStateMachine>(this, UExperienceStateMachine::StaticClass());
		if (NarrativeStateMachine)
		{
			// Bind to state change events (dynamic delegate uses AddDynamic)
			NarrativeStateMachine->OnStateChanged.AddDynamic(this, &ALBEASTExperienceBase::HandleNarrativeStateChanged);
			UE_LOG(LogTemp, Log, TEXT("LBEASTExperienceBase: Narrative state machine created"));
		}
	}

	// Base implementation - override in derived classes
	return true;
}

void ALBEASTExperienceBase::ShutdownExperienceImpl()
{
	// Stop command protocol if running
	if (CommandProtocol && CommandProtocol->IsListening())
	{
		CommandProtocol->StopListening();
	}

	// Base implementation - override in derived classes
}

void ALBEASTExperienceBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tick command protocol if listening (dedicated server mode)
	if (CommandProtocol && CommandProtocol->IsListening())
	{
		CommandProtocol->Tick(DeltaTime);
	}
}

ULBEASTInputAdapter* ALBEASTExperienceBase::GetInputAdapter() const
{
	return InputAdapter;
}

void ALBEASTExperienceBase::InitializeCommandProtocol()
{
	UWorld* World = GetWorld();
	if (!World || World->GetNetMode() != NM_DedicatedServer)
	{
		// Not running as dedicated server, skip command protocol
		return;
	}

	if (!CommandProtocol)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTExperienceBase: CommandProtocol not created"));
		return;
	}

	// Start listening for commands
	if (CommandProtocol->StartListening())
	{
		// Bind to command received event
		CommandProtocol->OnCommandReceived.AddDynamic(this, &ALBEASTExperienceBase::OnCommandReceived);
		UE_LOG(LogTemp, Log, TEXT("LBEASTExperienceBase: Command protocol listening on port 7779"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTExperienceBase: Failed to start command protocol"));
	}
}

void ALBEASTExperienceBase::OnCommandReceived(const FLBEASTServerCommandMessage& Command, ULBEASTServerCommandProtocol* Protocol)
{
	UE_LOG(LogTemp, Log, TEXT("LBEASTExperienceBase: Received command %d (seq: %d)"), 
		(uint8)Command.Command, Command.SequenceNumber);

	// Handle base commands
	switch (Command.Command)
	{
	case ELBEASTServerCommand::RequestStatus:
	{
		// Get current player count (NOOP: TODO - Implement proper player tracking)
		int32 CurrentPlayerCount = 0;
		UWorld* World = GetWorld();
		if (World && World->GetGameState())
		{
			// Try to get player count from game state
			CurrentPlayerCount = World->GetGameState()->PlayerArray.Num();
		}

		// Build status JSON response
		FString StatusData = FString::Printf(
			TEXT("{\"IsRunning\":%s,\"IsInitialized\":%s,\"CurrentPlayers\":%d,\"MaxPlayers\":%d,\"ExperienceState\":\"%s\"}"),
			bIsInitialized ? TEXT("true") : TEXT("false"),
			bIsInitialized ? TEXT("true") : TEXT("false"),
			CurrentPlayerCount,
			GetMaxPlayers(),
			bIsInitialized ? TEXT("Active") : TEXT("Idle")
		);

		// Send response back to client
		if (Protocol)
		{
			TSharedPtr<FInternetAddr> SenderAddr = Protocol->GetLastSenderAddress();
			if (SenderAddr.IsValid())
			{
				FLBEASTServerResponseMessage Response(true, TEXT("Status"), StatusData);
				Protocol->SendResponse(Response, SenderAddr.ToSharedRef());
				UE_LOG(LogTemp, Log, TEXT("LBEASTExperienceBase: Sent status response (Players: %d/%d)"), 
					CurrentPlayerCount, GetMaxPlayers());
			}
		}
		break;
	}
	case ELBEASTServerCommand::Shutdown:
	{
		UE_LOG(LogTemp, Log, TEXT("LBEASTExperienceBase: Shutdown command received"));
		ShutdownExperience();
		
		// Send confirmation response
		if (Protocol)
		{
			TSharedPtr<FInternetAddr> SenderAddr = Protocol->GetLastSenderAddress();
			if (SenderAddr.IsValid())
			{
				FLBEASTServerResponseMessage Response(true, TEXT("Shutdown initiated"));
				Protocol->SendResponse(Response, SenderAddr.ToSharedRef());
			}
		}
		break;
	}
	default:
		// Other commands handled by derived classes
		break;
	}
}

// ========================================
// NARRATIVE STATE MACHINE API
// ========================================

UExperienceStateMachine* ALBEASTExperienceBase::GetNarrativeStateMachine() const
{
	return NarrativeStateMachine;
}

FName ALBEASTExperienceBase::GetCurrentNarrativeState() const
{
	if (NarrativeStateMachine && NarrativeStateMachine->bIsRunning)
	{
		return NarrativeStateMachine->GetCurrentStateName();
	}
	return NAME_None;
}

bool ALBEASTExperienceBase::AdvanceNarrativeState()
{
	if (!NarrativeStateMachine || !NarrativeStateMachine->bIsRunning)
	{
		return false;
	}
	return NarrativeStateMachine->AdvanceState();
}

bool ALBEASTExperienceBase::RetreatNarrativeState()
{
	if (!NarrativeStateMachine || !NarrativeStateMachine->bIsRunning)
	{
		return false;
	}
	return NarrativeStateMachine->RetreatState();
}

bool ALBEASTExperienceBase::JumpToNarrativeState(FName StateName)
{
	if (!NarrativeStateMachine || !NarrativeStateMachine->bIsRunning)
	{
		return false;
	}
	return NarrativeStateMachine->JumpToState(StateName);
}

void ALBEASTExperienceBase::HandleNarrativeStateChanged(FName OldState, FName NewState, int32 NewStateIndex)
{
	// Call Blueprint implementable event
	OnNarrativeStateChanged(OldState, NewState, NewStateIndex);
}

