// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "AIFacemaskExperience.h"
#include "AIFaceController.h"
#include "EmbeddedDeviceController.h"
#include "ExperienceLoop/ExperienceStateMachine.h"
#include "Networking/LBEASTServerBeacon.h"

AAIFacemaskExperience::AAIFacemaskExperience()
{
	// Enable ticking for button input processing and server beacon
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Create components
	FaceController = CreateDefaultSubobject<UAIFaceController>(TEXT("FaceController"));
	CostumeController = CreateDefaultSubobject<UEmbeddedDeviceController>(TEXT("CostumeController"));

	// Create Experience Loop state machine
	ExperienceLoop = CreateDefaultSubobject<UExperienceStateMachine>(TEXT("ExperienceLoop"));

	// Create Server Beacon for automatic discovery
	ServerBeacon = CreateDefaultSubobject<ULBEASTServerBeacon>(TEXT("ServerBeacon"));

	// Configure for multiplayer with dedicated server (REQUIRED for AI processing offload)
	bMultiplayerEnabled = true;
	ServerMode = ELBEASTServerMode::DedicatedServer;
	bEnforceServerMode = true;
	RequiredServerMode = ELBEASTServerMode::DedicatedServer;

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

	// Initialize Server Beacon for automatic discovery/connection
	if (ServerBeacon)
	{
		// Check if we're running as dedicated server or client
		UWorld* World = GetWorld();
		if (World && World->GetNetMode() == NM_DedicatedServer)
		{
			// Server mode: broadcast presence
			FLBEASTServerInfo ServerInfo;
			ServerInfo.ServerIP = TEXT("0.0.0.0");  // Will be overridden by client with actual sender IP
			ServerInfo.ServerPort = 7777;  // TODO: Get from project settings
			ServerInfo.ExperienceType = TEXT("AIFacemask");
			ServerInfo.ServerName = FString::Printf(TEXT("AIFacemask Server %s"), *FDateTime::Now().ToString());
			ServerInfo.CurrentPlayers = 0;  // TODO: Track actual player count
			ServerInfo.MaxPlayers = NumberOfLiveActors + NumberOfPlayers;
			ServerInfo.ExperienceState = TEXT("Lobby");
			ServerInfo.ServerVersion = TEXT("1.0.0");
			ServerInfo.bAcceptingConnections = true;

			if (ServerBeacon->StartServerBroadcast(ServerInfo))
			{
				UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: Server beacon broadcasting"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AIFacemaskExperience: Failed to start server beacon"));
			}
		}
		else if (World && World->GetNetMode() == NM_Client)
		{
			// Client mode: listen for servers
			if (ServerBeacon->StartClientDiscovery())
			{
				UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: Client beacon listening for servers"));

				// Bind to server discovery event for auto-connection
				ServerBeacon->OnServerDiscovered.AddDynamic(this, &AAIFacemaskExperience::OnServerDiscovered);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AIFacemaskExperience: Failed to start client beacon"));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: Initialized with %d live actors and %d players"), NumberOfLiveActors, NumberOfPlayers);
	return true;
}

void AAIFacemaskExperience::ShutdownExperienceImpl()
{
	// Stop server beacon
	if (ServerBeacon && ServerBeacon->IsActive())
	{
		ServerBeacon->Stop();
	}

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

	// Tick server beacon for broadcasts/discovery
	if (ServerBeacon && ServerBeacon->IsActive())
	{
		ServerBeacon->Tick(DeltaTime);
	}

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

void AAIFacemaskExperience::OnServerDiscovered(const FLBEASTServerInfo& ServerInfo)
{
	UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: Discovered server '%s' (%s) at %s:%d - Current players: %d/%d"), 
		*ServerInfo.ServerName, *ServerInfo.ExperienceType, *ServerInfo.ServerIP, ServerInfo.ServerPort,
		ServerInfo.CurrentPlayers, ServerInfo.MaxPlayers);

	// Auto-connect to first available AIFacemask server
	if (ServerInfo.ExperienceType == TEXT("AIFacemask") && ServerInfo.bAcceptingConnections)
	{
		UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: Auto-connecting to server at %s:%d"), 
			*ServerInfo.ServerIP, ServerInfo.ServerPort);

		// TODO: Implement actual connection logic using Unreal's networking API
		// For now, just log the connection intent
		// In production, you would do something like:
		// APlayerController* PC = GetWorld()->GetFirstPlayerController();
		// if (PC)
		// {
		//     FString ConnectCommand = FString::Printf(TEXT("%s:%d"), *ServerInfo.ServerIP, ServerInfo.ServerPort);
		//     PC->ConsoleCommand(*FString::Printf(TEXT("open %s"), *ConnectCommand));
		// }
	}
}

