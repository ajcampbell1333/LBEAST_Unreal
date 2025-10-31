// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Networking/LBEASTServerCommandProtocol.h"
#include "Networking/LBEASTServerBeacon.h"
#include "LBEASTServerManagerWidget.generated.h"

// Forward declarations
class ULBEASTServerBeacon;

/**
 * Connection mode for Server Manager
 */
UENUM(BlueprintType)
enum class ELBEASTConnectionMode : uint8
{
	Local		UMETA(DisplayName = "Local (Launch Server)"),
	Remote		UMETA(DisplayName = "Remote (Connect to Server)")
};

/**
 * Server configuration data
 */
USTRUCT(BlueprintType)
struct FServerConfiguration
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	FString ExperienceType = TEXT("AIFacemask");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	FString ServerName = TEXT("LBEAST Server");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	int32 MaxPlayers = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	int32 Port = 7777;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	FString MapName = TEXT("/Game/Maps/LBEASTMap");
};

/**
 * Server runtime status
 */
USTRUCT(BlueprintType)
struct FServerStatus
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Server")
	bool bIsRunning = false;

	UPROPERTY(BlueprintReadOnly, Category = "Server")
	int32 CurrentPlayers = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Server")
	FString ExperienceState = TEXT("Idle");

	UPROPERTY(BlueprintReadOnly, Category = "Server")
	float Uptime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Server")
	int32 ProcessID = 0;
};

/**
 * Omniverse Audio2Face status
 */
USTRUCT(BlueprintType)
struct FOmniverseStatus
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Omniverse")
	bool bIsConnected = false;

	UPROPERTY(BlueprintReadOnly, Category = "Omniverse")
	FString StreamStatus = TEXT("Inactive");

	UPROPERTY(BlueprintReadOnly, Category = "Omniverse")
	int32 ActiveFaceStreams = 0;
};

/**
 * LBEAST Server Manager Widget
 * 
 * Main UI for the server management application.
 * Design this widget in UMG Blueprint Editor with:
 * 
 * LAYOUT:
 * ┌────────────────────────────────────────┐
 * │  LBEAST Server Manager                 │
 * ├────────────────────────────────────────┤
 * │  Configuration:                        │
 * │  Experience: [Dropdown]                │
 * │  Server Name: [Text Input]             │
 * │  Max Players: [Number Input]           │
 * │  Port: [Number Input]                  │
 * │                                         │
 * │  [Start Server]  [Stop Server]         │
 * ├────────────────────────────────────────┤
 * │  Status:                               │
 * │  ● Running                             │
 * │  Players: 2/4                          │
 * │  State: Act1                           │
 * │  Uptime: 00:15:32                      │
 * ├────────────────────────────────────────┤
 * │  Omniverse Audio2Face:                 │
 * │  Status: ● Connected                   │
 * │  Face Streams: 1 active                │
 * │  [Configure Omniverse]                 │
 * ├────────────────────────────────────────┤
 * │  Logs:                                 │
 * │  [Scrollable Text Box]                 │
 * └────────────────────────────────────────┘
 */
UCLASS()
class LBEAST_UNREAL_API ULBEASTServerManagerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Connection mode (Local or Remote) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Server Manager")
	ELBEASTConnectionMode ConnectionMode = ELBEASTConnectionMode::Local;

	/** Current server configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Server Manager")
	FServerConfiguration ServerConfig;

	/** Remote server configuration (only used in Remote mode) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Server Manager", meta = (EditCondition = "ConnectionMode == ELBEASTConnectionMode::Remote"))
	FString RemoteServerIP = TEXT("127.0.0.1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Server Manager", meta = (EditCondition = "ConnectionMode == ELBEASTConnectionMode::Remote"))
	int32 RemoteServerPort = 7777;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Server Manager", meta = (EditCondition = "ConnectionMode == ELBEASTConnectionMode::Remote"))
	int32 RemoteCommandPort = 7779;

	/** Enable authentication for remote connections (not needed for local same-desk setups) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Server Manager|Security", meta = (EditCondition = "ConnectionMode == ELBEASTConnectionMode::Remote"))
	bool bEnableAuthentication = false;

	/** Shared secret for authentication (must match server configuration) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Server Manager|Security", meta = (EditCondition = "ConnectionMode == ELBEASTConnectionMode::Remote && bEnableAuthentication", PasswordField = true))
	FString SharedSecret = TEXT("CHANGE_ME_IN_PRODUCTION");

	/** Current server status */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Server Manager")
	FServerStatus ServerStatus;

	/** Omniverse connection status */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Server Manager")
	FOmniverseStatus OmniverseStatus;

	/**
	 * Start the dedicated server with current configuration
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	bool StartServer();

	/**
	 * Stop the running dedicated server
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	bool StopServer();

	/**
	 * Check if server process is running and update status
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	void UpdateServerStatus();

	/**
	 * Check Omniverse connection and update status
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	void UpdateOmniverseStatus();

	/**
	 * Get available experience types
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	TArray<FString> GetAvailableExperienceTypes() const;

	/**
	 * Add a log message to the UI
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	void AddLogMessage(const FString& Message);

	/**
	 * Open Omniverse configuration panel
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	void OpenOmniverseConfig();

	/**
	 * Connect to remote server (Remote mode only)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	bool ConnectToRemoteServer();

	/**
	 * Disconnect from remote server (Remote mode only)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	void DisconnectFromRemoteServer();

	/**
	 * Check if connected to remote server
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	bool IsRemoteConnected() const;

	/**
	 * Get list of discovered servers (from beacon)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Server Manager")
	TArray<FLBEASTServerInfo> GetDiscoveredServers() const;

protected:
	/** Handle to the server process */
	FProcHandle ServerProcessHandle;

	/** Network beacon for real-time server status */
	UPROPERTY()
	TObjectPtr<class ULBEASTServerBeacon> ServerBeacon;

	/** Command protocol for remote server control */
	UPROPERTY()
	TObjectPtr<class ULBEASTServerCommandProtocol> CommandProtocol;

	/** Path to the dedicated server executable */
	FString GetServerExecutablePath() const;

	/** Build command-line arguments for server launch */
	FString BuildServerCommandLine() const;

	/** Poll server for status updates */
	void PollServerStatus();

	/** Handle server status broadcast received */
	UFUNCTION()
	void OnServerStatusReceived(const struct FLBEASTServerInfo& ServerInfo);

	/** Timer for status polling */
	float StatusPollTimer = 0.0f;
	float StatusPollInterval = 1.0f;

	/** Expected server IP (set when starting server or connecting) */
	FString ExpectedServerIP;
	
	/** Expected server port (set when starting server or connecting) */
	int32 ExpectedServerPort = 0;

	/** Handle command response from remote server */
	UFUNCTION()
	void OnCommandResponse(const FLBEASTServerResponseMessage& Response);

	/** Handle server discovered via beacon (for auto-connect) */
	UFUNCTION()
	void OnServerDiscoveredForConnection(const struct FLBEASTServerInfo& ServerInfo);
};

