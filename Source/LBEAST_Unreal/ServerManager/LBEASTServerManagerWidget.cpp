// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "LBEASTServerManagerWidget.h"
#include "Networking/LBEASTServerBeacon.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Misc/FileHelper.h"

void ULBEASTServerManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize default configuration
	ServerConfig.ExperienceType = TEXT("AIFacemask");
	ServerConfig.ServerName = TEXT("LBEAST Server");
	ServerConfig.MaxPlayers = 4;
	ServerConfig.Port = 7777;
	ServerConfig.MapName = TEXT("/Game/Maps/LBEASTMap");

	// Initialize network beacon for real-time status updates
	ServerBeacon = NewObject<ULBEASTServerBeacon>(this, TEXT("ServerStatusBeacon"));
	if (ServerBeacon)
	{
		ServerBeacon->OnServerDiscovered.AddDynamic(this, &ULBEASTServerManagerWidget::OnServerStatusReceived);
		ServerBeacon->StartClientDiscovery();
		AddLogMessage(TEXT("Server status beacon initialized (listening on port 7778)"));
	}

	AddLogMessage(TEXT("Server Manager initialized"));
}

void ULBEASTServerManagerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Tick the server beacon for network updates
	if (ServerBeacon && ServerBeacon->IsActive())
	{
		ServerBeacon->Tick(InDeltaTime);
	}

	// Poll server status
	StatusPollTimer += InDeltaTime;
	if (StatusPollTimer >= StatusPollInterval)
	{
		StatusPollTimer = 0.0f;
		PollServerStatus();
	}

	// Update uptime if server is running
	if (ServerStatus.bIsRunning)
	{
		ServerStatus.Uptime += InDeltaTime;
	}
}

bool ULBEASTServerManagerWidget::StartServer()
{
	if (ServerStatus.bIsRunning)
	{
		AddLogMessage(TEXT("ERROR: Server is already running"));
		return false;
	}

	// Get server executable path
	FString ServerPath = GetServerExecutablePath();
	if (!FPaths::FileExists(ServerPath))
	{
		AddLogMessage(FString::Printf(TEXT("ERROR: Server executable not found at %s"), *ServerPath));
		AddLogMessage(TEXT("Please build the dedicated server target first."));
		return false;
	}

	// Build command line
	FString CommandLine = BuildServerCommandLine();

	AddLogMessage(FString::Printf(TEXT("Starting server: %s %s"), *ServerPath, *CommandLine));

	// Launch server process
	uint32 ProcessID = 0;
	ServerProcessHandle = FPlatformProcess::CreateProc(
		*ServerPath,
		*CommandLine,
		true,  // bLaunchDetached
		false, // bLaunchHidden
		false, // bLaunchReallyHidden
		&ProcessID,
		0,     // PriorityModifier
		nullptr, // OptionalWorkingDirectory
		nullptr, // PipeWriteChild
		nullptr  // PipeReadChild
	);
	ServerStatus.ProcessID = static_cast<int32>(ProcessID);

	if (ServerProcessHandle.IsValid())
	{
		ServerStatus.bIsRunning = true;
		ServerStatus.Uptime = 0.0f;
		ServerStatus.ExperienceState = TEXT("Starting...");
		
		// Save expected server info for beacon matching
		ExpectedServerIP = TEXT("127.0.0.1"); // Localhost for local server
		ExpectedServerPort = ServerConfig.Port;
		
		AddLogMessage(FString::Printf(TEXT("Server started successfully (PID: %d)"), ServerStatus.ProcessID));
		AddLogMessage(TEXT("Listening for server status broadcasts..."));
		return true;
	}
	else
	{
		AddLogMessage(TEXT("ERROR: Failed to start server process"));
		return false;
	}
}

bool ULBEASTServerManagerWidget::StopServer()
{
	if (!ServerStatus.bIsRunning)
	{
		AddLogMessage(TEXT("ERROR: No server is currently running"));
		return false;
	}

	if (!ServerProcessHandle.IsValid())
	{
		AddLogMessage(TEXT("ERROR: Invalid server process handle"));
		ServerStatus.bIsRunning = false;
		return false;
	}

	AddLogMessage(TEXT("Stopping server..."));

	// Terminate the server process
	FPlatformProcess::TerminateProc(ServerProcessHandle);
	FPlatformProcess::CloseProc(ServerProcessHandle);

	ServerStatus.bIsRunning = false;
	ServerStatus.CurrentPlayers = 0;
	ServerStatus.ExperienceState = TEXT("Stopped");
	ServerStatus.ProcessID = 0;

	AddLogMessage(TEXT("Server stopped"));
	return true;
}

void ULBEASTServerManagerWidget::UpdateServerStatus()
{
	// Check if process is still running
	if (ServerProcessHandle.IsValid())
	{
		if (!FPlatformProcess::IsProcRunning(ServerProcessHandle))
		{
			AddLogMessage(TEXT("WARNING: Server process terminated unexpectedly"));
			ServerStatus.bIsRunning = false;
			ServerStatus.CurrentPlayers = 0;
			ServerStatus.ExperienceState = TEXT("Crashed");
			FPlatformProcess::CloseProc(ServerProcessHandle);
		}
	}

	// TODO: Query actual server status via network beacon or log file parsing
	// For now, this is just a stub
}

void ULBEASTServerManagerWidget::UpdateOmniverseStatus()
{
	// TODO: Implement Omniverse Audio2Face connection check
	// This would connect to Omniverse Nucleus or Audio2Face API
	// For now, this is just a stub
	
	OmniverseStatus.bIsConnected = false;
	OmniverseStatus.StreamStatus = TEXT("Not Configured");
	OmniverseStatus.ActiveFaceStreams = 0;
}

TArray<FString> ULBEASTServerManagerWidget::GetAvailableExperienceTypes() const
{
	TArray<FString> ExperienceTypes;
	ExperienceTypes.Add(TEXT("AIFacemask"));
	ExperienceTypes.Add(TEXT("MovingPlatform"));
	ExperienceTypes.Add(TEXT("Gunship"));
	ExperienceTypes.Add(TEXT("CarSim"));
	ExperienceTypes.Add(TEXT("FlightSim"));
	return ExperienceTypes;
}

void ULBEASTServerManagerWidget::AddLogMessage(const FString& Message)
{
	// Log to Unreal console
	UE_LOG(LogTemp, Log, TEXT("[ServerManager] %s"), *Message);

	// TODO: Add to UI log widget
	// This would be implemented in Blueprint by binding to this function
	// and appending to a TextBlock or ScrollBox
}

void ULBEASTServerManagerWidget::OpenOmniverseConfig()
{
	// TODO: Open a sub-panel or dialog for Omniverse configuration
	// This would include:
	// - Omniverse Nucleus connection settings
	// - Audio2Face server address
	// - Face stream configuration
	
	AddLogMessage(TEXT("Omniverse configuration not yet implemented"));
}

FString ULBEASTServerManagerWidget::GetServerExecutablePath() const
{
	// Build path to dedicated server executable
	FString ProjectDir = FPaths::ProjectDir();
	FString BinariesDir = FPaths::Combine(ProjectDir, TEXT("Binaries"), FPlatformProcess::GetBinariesSubdirectory());
	
#if PLATFORM_WINDOWS
	FString ExecutableName = TEXT("LBEAST_UnrealServer.exe");
#elif PLATFORM_LINUX
	FString ExecutableName = TEXT("LBEAST_UnrealServer");
#else
	FString ExecutableName = TEXT("LBEAST_UnrealServer");
#endif

	return FPaths::Combine(BinariesDir, ExecutableName);
}

FString ULBEASTServerManagerWidget::BuildServerCommandLine() const
{
	TArray<FString> Args;

	// Map to load
	Args.Add(ServerConfig.MapName);

	// Server flags
	Args.Add(TEXT("-server"));
	Args.Add(TEXT("-log"));

	// Port
	Args.Add(FString::Printf(TEXT("-port=%d"), ServerConfig.Port));

	// Experience type (custom parameter)
	Args.Add(FString::Printf(TEXT("-ExperienceType=%s"), *ServerConfig.ExperienceType));

	// Max players
	Args.Add(FString::Printf(TEXT("-MaxPlayers=%d"), ServerConfig.MaxPlayers));

	// Join all arguments
	return FString::Join(Args, TEXT(" "));
}

void ULBEASTServerManagerWidget::PollServerStatus()
{
	if (!ServerStatus.bIsRunning)
	{
		return;
	}

	UpdateServerStatus();
	
	// Real-time status updates now come via OnServerStatusReceived callback
	// from the network beacon. This function just verifies process is alive.
}

void ULBEASTServerManagerWidget::OnServerStatusReceived(const FLBEASTServerInfo& ServerInfo)
{
	// Only process status for our managed server
	// (Match by port since IP might be reported differently for localhost)
	if (ServerInfo.ServerPort != ExpectedServerPort)
	{
		return; // This is a different server on the network
	}

	// Only process if our server is marked as running
	if (!ServerStatus.bIsRunning)
	{
		return;
	}

	// Update real-time status from server broadcast
	ServerStatus.CurrentPlayers = ServerInfo.CurrentPlayers;
	ServerStatus.ExperienceState = ServerInfo.ExperienceState;

	// Log significant state changes
	static FString LastState = TEXT("");
	if (ServerStatus.ExperienceState != LastState)
	{
		AddLogMessage(FString::Printf(TEXT("Server state changed to: %s"), *ServerStatus.ExperienceState));
		LastState = ServerStatus.ExperienceState;
	}

	// Log player count changes
	static int32 LastPlayerCount = -1;
	if (ServerStatus.CurrentPlayers != LastPlayerCount)
	{
		AddLogMessage(FString::Printf(TEXT("Player count changed to: %d/%d"), 
			ServerStatus.CurrentPlayers, ServerConfig.MaxPlayers));
		LastPlayerCount = ServerStatus.CurrentPlayers;
	}
}

