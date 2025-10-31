// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "LBEASTServerBeacon.generated.h"

/**
 * Server information broadcast over LAN
 */
USTRUCT(BlueprintType)
struct FLBEASTServerInfo
{
	GENERATED_BODY()

	/** Server IP address */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Networking")
	FString ServerIP;

	/** Server port */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Networking")
	int32 ServerPort = 7777;

	/** Experience type (e.g., "AIFacemask", "Gunship") */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Networking")
	FString ExperienceType;

	/** Server name/identifier */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Networking")
	FString ServerName;

	/** Current player count */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Networking")
	int32 CurrentPlayers = 0;

	/** Maximum player count */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Networking")
	int32 MaxPlayers = 8;

	/** Current experience state (e.g., "Lobby", "InProgress", "Complete") */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Networking")
	FString ExperienceState;

	/** Server version (for compatibility checks) */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Networking")
	FString ServerVersion = TEXT("1.0.0");

	/** Timestamp of last beacon (for timeout detection) */
	float LastBeaconTime = 0.0f;

	/** Is this server accepting new connections? */
	UPROPERTY(BlueprintReadOnly, Category = "LBEAST|Networking")
	bool bAcceptingConnections = true;
};

/**
 * Delegate for server discovery events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerDiscovered, const FLBEASTServerInfo&, ServerInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerLost, const FString&, ServerIP);

/**
 * LBEAST Server Beacon
 * 
 * Handles automatic server discovery on LAN using UDP broadcasting.
 * 
 * SERVER MODE:
 * - Broadcasts server presence every X seconds
 * - Includes server metadata (experience type, player count, etc.)
 * - Runs on dedicated server to advertise availability
 * 
 * CLIENT MODE:
 * - Listens for server broadcasts
 * - Maintains list of available servers
 * - Auto-connects to appropriate server
 * - Detects when servers go offline
 * 
 * Perfect for LBE installations with multiple concurrent experiences.
 */
UCLASS(BlueprintType)
class LBEASTCORE_API ULBEASTServerBeacon : public UObject
{
	GENERATED_BODY()

public:
	ULBEASTServerBeacon();
	virtual ~ULBEASTServerBeacon();

	/** Broadcast port for server discovery (same for all LBEAST installations) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Networking")
	int32 BroadcastPort = 7778;

	/** How often server broadcasts presence (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Networking")
	float BroadcastInterval = 2.0f;

	/** How long before considering a server lost (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|Networking")
	float ServerTimeout = 10.0f;

	/** Fired when a new server is discovered */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|Networking")
	FOnServerDiscovered OnServerDiscovered;

	/** Fired when a server is no longer responding */
	UPROPERTY(BlueprintAssignable, Category = "LBEAST|Networking")
	FOnServerLost OnServerLost;

	/**
	 * Start broadcasting as a server (Dedicated Server only)
	 * @param ServerInfo - Information about this server to broadcast
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Networking")
	bool StartServerBroadcast(const FLBEASTServerInfo& ServerInfo);

	/**
	 * Start listening for server broadcasts (Clients only)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Networking")
	bool StartClientDiscovery();

	/**
	 * Stop broadcasting/listening
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Networking")
	void Stop();

	/**
	 * Get list of currently discovered servers
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Networking")
	TArray<FLBEASTServerInfo> GetDiscoveredServers() const;

	/**
	 * Get a specific server by experience type
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Networking")
	bool GetServerByExperienceType(const FString& ExperienceType, FLBEASTServerInfo& OutServerInfo) const;

	/**
	 * Update server info (for servers to update player count, state, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|Networking")
	void UpdateServerInfo(const FLBEASTServerInfo& NewServerInfo);

	/** Is this beacon active? */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Networking")
	bool IsActive() const { return bIsActive; }

	/** Is this beacon in server mode? */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LBEAST|Networking")
	bool IsServerMode() const { return bIsServerMode; }

	/** Tick function for periodic broadcasts and server timeout checks */
	void Tick(float DeltaTime);

private:
	FSocket* BroadcastSocket = nullptr;
	FSocket* ListenSocket = nullptr;
	
	bool bIsActive = false;
	bool bIsServerMode = false;
	
	FLBEASTServerInfo CurrentServerInfo;
	TMap<FString, FLBEASTServerInfo> DiscoveredServers;  // Key = ServerIP
	
	float TimeSinceLastBroadcast = 0.0f;

	/** Serialize server info to binary for network transmission */
	TArray<uint8> SerializeServerInfo(const FLBEASTServerInfo& ServerInfo) const;

	/** Deserialize server info from binary */
	bool DeserializeServerInfo(const TArray<uint8>& Data, FLBEASTServerInfo& OutServerInfo) const;

	/** Send broadcast packet */
	void SendBroadcast();

	/** Receive and process incoming packets */
	void ReceivePackets();

	/** Check for server timeouts */
	void CheckServerTimeouts();

	/** Create broadcast socket */
	bool CreateBroadcastSocket();

	/** Create listen socket */
	bool CreateListenSocket();

	/** Cleanup sockets */
	void CleanupSockets();
};



