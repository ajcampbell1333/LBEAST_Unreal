// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "ProLighting.h"
#include "IDMXTransport.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

/** Minimal Art-Net transport (UDP) */
class PROLIGHTING_API FArtNetTransport : public IDMXTransport
{
public:
	FArtNetTransport(const FString& InIP, int32 InPort, int32 InNet, int32 InSubNet)
		: TargetIP(InIP), Port(InPort), Net(InNet), SubNet(InSubNet) {}

	virtual bool Initialize() override
	{
		ISocketSubsystem* SS = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		if (!SS) return false;
		Socket = SS->CreateSocket(NAME_DGram, TEXT("LBEAST_ArtNetTransport"), false);
		if (!Socket) return false;
		Address = SS->CreateInternetAddr();
		bool bValid = false;
		Address->SetIp(*TargetIP, bValid);
		Address->SetPort(Port);
		if (!bValid)
		{
			Shutdown();
			return false;
		}
		Socket->SetBroadcast(true);
		return true;
	}

	virtual void Shutdown() override
	{
		if (Socket)
		{
			Socket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
			Socket = nullptr;
		}
		Address.Reset();
	}

	virtual bool IsConnected() const override { return Socket != nullptr && Address.IsValid(); }

	virtual void SendDMX(int32 Universe, const TArray<uint8>& DMXData) override
	{
		if (!IsConnected()) return;
		TArray<uint8> Packet = BuildArtDmxPacket(Universe, DMXData);
		int32 BytesSent = 0;
		Socket->SendTo(Packet.GetData(), Packet.Num(), BytesSent, *Address);
	}

private:
	TArray<uint8> BuildArtDmxPacket(int32 Universe, const TArray<uint8>& DMXData) const
	{
		TArray<uint8> Packet;
		Packet.SetNumUninitialized(18 + 512);
		int32 Offset = 0;
		FMemory::Memcpy(Packet.GetData() + Offset, "Art-Net\0", 8); Offset += 8;
		Packet[Offset++] = 0x00; Packet[Offset++] = 0x50; // OpDmx (0x5000 LE)
		Packet[Offset++] = 0x0E; Packet[Offset++] = 0x00; // ProtVer 14
		Packet[Offset++] = 0x00; // Sequence
		Packet[Offset++] = 0x00; // Physical
		int32 ArtUniverse = (SubNet * 16) + Universe;
		Packet[Offset++] = ArtUniverse & 0xFF;
		Packet[Offset++] = (ArtUniverse >> 8) & 0xFF;
		Packet[Offset++] = 0x02; // Length hi (512)
		Packet[Offset++] = 0x00; // Length lo
		FMemory::Memset(Packet.GetData() + Offset, 0, 512);
		int32 CopyLen = FMath::Min(512, DMXData.Num());
		FMemory::Memcpy(Packet.GetData() + Offset, DMXData.GetData(), CopyLen);
		return Packet;
	}

private:
	FString TargetIP;
	int32 Port = 6454;
	int32 Net = 0;
	int32 SubNet = 0;
	FSocket* Socket = nullptr;
	TSharedPtr<FInternetAddr> Address;
};


