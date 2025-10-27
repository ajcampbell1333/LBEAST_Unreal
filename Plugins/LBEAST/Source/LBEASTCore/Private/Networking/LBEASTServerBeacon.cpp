// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "Networking/LBEASTServerBeacon.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Common/UdpSocketBuilder.h"

// Magic number to identify LBEAST beacon packets
static const uint32 LBEAST_BEACON_MAGIC = 0x4C424541;  // "LBEA" in hex
static const uint32 LBEAST_BEACON_VERSION = 1;

ULBEASTServerBeacon::ULBEASTServerBeacon()
{
	BroadcastPort = 7778;
	BroadcastInterval = 2.0f;
	ServerTimeout = 10.0f;
}

ULBEASTServerBeacon::~ULBEASTServerBeacon()
{
	Stop();
}

bool ULBEASTServerBeacon::StartServerBroadcast(const FLBEASTServerInfo& ServerInfo)
{
	if (bIsActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTServerBeacon: Already active"));
		return false;
	}

	CurrentServerInfo = ServerInfo;
	CurrentServerInfo.LastBeaconTime = FPlatformTime::Seconds();

	if (!CreateBroadcastSocket())
	{
		UE_LOG(LogTemp, Error, TEXT("LBEASTServerBeacon: Failed to create broadcast socket"));
		return false;
	}

	bIsActive = true;
	bIsServerMode = true;
	TimeSinceLastBroadcast = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("LBEASTServerBeacon: Started broadcasting as server '%s' (%s) on port %d"), 
		*CurrentServerInfo.ServerName, *CurrentServerInfo.ExperienceType, BroadcastPort);

	// Send initial broadcast immediately
	SendBroadcast();

	return true;
}

bool ULBEASTServerBeacon::StartClientDiscovery()
{
	if (bIsActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTServerBeacon: Already active"));
		return false;
	}

	if (!CreateListenSocket())
	{
		UE_LOG(LogTemp, Error, TEXT("LBEASTServerBeacon: Failed to create listen socket"));
		return false;
	}

	bIsActive = true;
	bIsServerMode = false;

	UE_LOG(LogTemp, Log, TEXT("LBEASTServerBeacon: Started listening for servers on port %d"), BroadcastPort);

	return true;
}

void ULBEASTServerBeacon::Stop()
{
	if (!bIsActive)
	{
		return;
	}

	CleanupSockets();

	bIsActive = false;
	DiscoveredServers.Empty();

	UE_LOG(LogTemp, Log, TEXT("LBEASTServerBeacon: Stopped"));
}

TArray<FLBEASTServerInfo> ULBEASTServerBeacon::GetDiscoveredServers() const
{
	TArray<FLBEASTServerInfo> Result;
	DiscoveredServers.GenerateValueArray(Result);
	return Result;
}

bool ULBEASTServerBeacon::GetServerByExperienceType(const FString& ExperienceType, FLBEASTServerInfo& OutServerInfo) const
{
	for (const auto& Pair : DiscoveredServers)
	{
		if (Pair.Value.ExperienceType == ExperienceType && Pair.Value.bAcceptingConnections)
		{
			OutServerInfo = Pair.Value;
			return true;
		}
	}
	return false;
}

void ULBEASTServerBeacon::UpdateServerInfo(const FLBEASTServerInfo& NewServerInfo)
{
	if (!bIsServerMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTServerBeacon: UpdateServerInfo only works in server mode"));
		return;
	}

	CurrentServerInfo = NewServerInfo;
	CurrentServerInfo.LastBeaconTime = FPlatformTime::Seconds();
}

void ULBEASTServerBeacon::Tick(float DeltaTime)
{
	if (!bIsActive)
	{
		return;
	}

	if (bIsServerMode)
	{
		// Periodic broadcast
		TimeSinceLastBroadcast += DeltaTime;
		if (TimeSinceLastBroadcast >= BroadcastInterval)
		{
			SendBroadcast();
			TimeSinceLastBroadcast = 0.0f;
		}
	}
	else
	{
		// Receive packets from servers
		ReceivePackets();

		// Check for server timeouts
		CheckServerTimeouts();
	}
}

TArray<uint8> ULBEASTServerBeacon::SerializeServerInfo(const FLBEASTServerInfo& ServerInfo) const
{
	FBufferArchive Writer;

	// Write magic number and version
	uint32 Magic = LBEAST_BEACON_MAGIC;
	uint32 Version = LBEAST_BEACON_VERSION;
	Writer << Magic;
	Writer << Version;

	// Write server info
	FString ServerIP = ServerInfo.ServerIP;
	int32 ServerPort = ServerInfo.ServerPort;
	FString ExperienceType = ServerInfo.ExperienceType;
	FString ServerName = ServerInfo.ServerName;
	int32 CurrentPlayers = ServerInfo.CurrentPlayers;
	int32 MaxPlayers = ServerInfo.MaxPlayers;
	FString ExperienceState = ServerInfo.ExperienceState;
	FString ServerVersion = ServerInfo.ServerVersion;
	bool bAcceptingConnections = ServerInfo.bAcceptingConnections;

	Writer << ServerIP;
	Writer << ServerPort;
	Writer << ExperienceType;
	Writer << ServerName;
	Writer << CurrentPlayers;
	Writer << MaxPlayers;
	Writer << ExperienceState;
	Writer << ServerVersion;
	Writer << bAcceptingConnections;

	return TArray<uint8>(Writer.GetData(), Writer.Num());
}

bool ULBEASTServerBeacon::DeserializeServerInfo(const TArray<uint8>& Data, FLBEASTServerInfo& OutServerInfo) const
{
	if (Data.Num() < 8)  // At least magic + version
	{
		return false;
	}

	FMemoryReader Reader(Data);

	// Read and validate magic number
	uint32 Magic = 0;
	uint32 Version = 0;
	Reader << Magic;
	Reader << Version;

	if (Magic != LBEAST_BEACON_MAGIC)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTServerBeacon: Invalid magic number in packet"));
		return false;
	}

	if (Version != LBEAST_BEACON_VERSION)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTServerBeacon: Unsupported beacon version %d"), Version);
		return false;
	}

	// Read server info
	Reader << OutServerInfo.ServerIP;
	Reader << OutServerInfo.ServerPort;
	Reader << OutServerInfo.ExperienceType;
	Reader << OutServerInfo.ServerName;
	Reader << OutServerInfo.CurrentPlayers;
	Reader << OutServerInfo.MaxPlayers;
	Reader << OutServerInfo.ExperienceState;
	Reader << OutServerInfo.ServerVersion;
	Reader << OutServerInfo.bAcceptingConnections;

	OutServerInfo.LastBeaconTime = FPlatformTime::Seconds();

	return !Reader.IsError();
}

void ULBEASTServerBeacon::SendBroadcast()
{
	if (!BroadcastSocket)
	{
		return;
	}

	// Update timestamp
	CurrentServerInfo.LastBeaconTime = FPlatformTime::Seconds();

	// Serialize server info
	TArray<uint8> Data = SerializeServerInfo(CurrentServerInfo);

	// Create broadcast address
	TSharedRef<FInternetAddr> BroadcastAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	BroadcastAddr->SetBroadcastAddress();
	BroadcastAddr->SetPort(BroadcastPort);

	// Send broadcast
	int32 BytesSent = 0;
	BroadcastSocket->SendTo(Data.GetData(), Data.Num(), BytesSent, *BroadcastAddr);

	if (BytesSent != Data.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTServerBeacon: Failed to send complete broadcast packet (%d/%d bytes)"), 
			BytesSent, Data.Num());
	}
}

void ULBEASTServerBeacon::ReceivePackets()
{
	if (!ListenSocket)
	{
		return;
	}

	TSharedRef<FInternetAddr> Sender = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	uint8 Buffer[1024];
	int32 BytesRead = 0;

	while (ListenSocket->RecvFrom(Buffer, sizeof(Buffer), BytesRead, *Sender))
	{
		if (BytesRead > 0)
		{
			TArray<uint8> Data(Buffer, BytesRead);
			FLBEASTServerInfo ServerInfo;

			if (DeserializeServerInfo(Data, ServerInfo))
			{
				// Override ServerIP with actual sender IP (more reliable than self-reported)
				ServerInfo.ServerIP = Sender->ToString(false);

				bool bIsNewServer = !DiscoveredServers.Contains(ServerInfo.ServerIP);

				// Update or add server
				DiscoveredServers.Add(ServerInfo.ServerIP, ServerInfo);

				if (bIsNewServer)
				{
					UE_LOG(LogTemp, Log, TEXT("LBEASTServerBeacon: Discovered server '%s' (%s) at %s:%d"), 
						*ServerInfo.ServerName, *ServerInfo.ExperienceType, *ServerInfo.ServerIP, ServerInfo.ServerPort);

					OnServerDiscovered.Broadcast(ServerInfo);
				}
			}
		}
	}
}

void ULBEASTServerBeacon::CheckServerTimeouts()
{
	float CurrentTime = FPlatformTime::Seconds();
	TArray<FString> ServersToRemove;

	for (const auto& Pair : DiscoveredServers)
	{
		float TimeSinceLastBeacon = CurrentTime - Pair.Value.LastBeaconTime;
		if (TimeSinceLastBeacon > ServerTimeout)
		{
			ServersToRemove.Add(Pair.Key);
		}
	}

	// Remove timed-out servers
	for (const FString& ServerIP : ServersToRemove)
	{
		UE_LOG(LogTemp, Warning, TEXT("LBEASTServerBeacon: Server %s timed out"), *ServerIP);
		DiscoveredServers.Remove(ServerIP);
		OnServerLost.Broadcast(ServerIP);
	}
}

bool ULBEASTServerBeacon::CreateBroadcastSocket()
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (!SocketSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("LBEASTServerBeacon: Failed to get socket subsystem"));
		return false;
	}

	BroadcastSocket = FUdpSocketBuilder(TEXT("LBEAST_Broadcast"))
		.AsReusable()
		.WithBroadcast()
		.Build();

	if (!BroadcastSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("LBEASTServerBeacon: Failed to create broadcast socket"));
		return false;
	}

	return true;
}

bool ULBEASTServerBeacon::CreateListenSocket()
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (!SocketSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("LBEASTServerBeacon: Failed to get socket subsystem"));
		return false;
	}

	// Bind to any address on the specified port
	FIPv4Address BindAddress = FIPv4Address::Any;

	ListenSocket = FUdpSocketBuilder(TEXT("LBEAST_Listen"))
		.AsReusable()
		.AsNonBlocking()
		.BoundToAddress(BindAddress)
		.BoundToPort(BroadcastPort)
		.WithReceiveBufferSize(2048)
		.Build();

	if (!ListenSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("LBEASTServerBeacon: Failed to create listen socket on port %d"), BroadcastPort);
		return false;
	}

	return true;
}

void ULBEASTServerBeacon::CleanupSockets()
{
	if (BroadcastSocket)
	{
		BroadcastSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(BroadcastSocket);
		BroadcastSocket = nullptr;
	}

	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
		ListenSocket = nullptr;
	}
}

