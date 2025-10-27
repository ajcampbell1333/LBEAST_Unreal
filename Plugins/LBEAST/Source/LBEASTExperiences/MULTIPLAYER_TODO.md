# AIFacemask Multiplayer Implementation TODO

## Current Status

### ✅ **Implemented:**
- Server discovery (UDP broadcast beacon)
- Client discovery (listening for servers)
- Server metadata broadcast (player count, state, version)
- Dedicated server enforcement
- Experience Loop state machine (local only)
- Embedded Systems button input (local only)

### ❌ **NOT Implemented:**
- Actual client-server connection
- Game state replication
- Player spawning/management
- Button press replication
- Experience Loop state replication
- Authority checks

---

## What Needs to Be Built

### **1. Client Connection**

**File:** `AIFacemaskExperience.cpp` line 266

Currently:
```cpp
// TODO: Implement actual connection logic using Unreal's networking API
```

Should be:
```cpp
void AAIFacemaskExperience::OnServerDiscovered(const FLBEASTServerInfo& ServerInfo)
{
    if (ServerInfo.ExperienceType == TEXT("AIFacemask") && ServerInfo.bAcceptingConnections)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            FString ConnectURL = FString::Printf(TEXT("%s:%d"), *ServerInfo.ServerIP, ServerInfo.ServerPort);
            PC->ClientTravel(ConnectURL, ETravelType::TRAVEL_Absolute);
        }
    }
}
```

---

### **2. State Replication**

**File:** `AIFacemaskExperience.h`

Add replicated properties:

```cpp
UCLASS(Blueprintable, ClassGroup=(LBEAST))
class LBEASTEXPERIENCES_API AAIFacemaskExperience : public ALBEASTExperienceBase
{
    GENERATED_BODY()

    // ... existing code ...

    /** Current experience state (replicated to all clients) */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "LBEAST|AI Facemask")
    FName ReplicatedExperienceState;

    /** Button states for each live actor (replicated) */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "LBEAST|AI Facemask")
    TArray<bool> ReplicatedButtonStates;

    /** Override replication */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

**File:** `AIFacemaskExperience.cpp`

```cpp
#include "Net/UnrealNetwork.h"

void AAIFacemaskExperience::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAIFacemaskExperience, ReplicatedExperienceState);
    DOREPLIFETIME(AAIFacemaskExperience, ReplicatedButtonStates);
}
```

---

### **3. Server RPCs for Button Presses**

**File:** `AIFacemaskExperience.h`

```cpp
/** Server RPC: Advance experience (called by live actor button press) */
UFUNCTION(Server, Reliable, WithValidation)
void ServerAdvanceExperience();

/** Server RPC: Retreat experience (called by live actor button press) */
UFUNCTION(Server, Reliable, WithValidation)
void ServerRetreatExperience();
```

**File:** `AIFacemaskExperience.cpp`

```cpp
void AAIFacemaskExperience::ServerAdvanceExperience_Implementation()
{
    if (ExperienceLoop && ExperienceLoop->AdvanceState())
    {
        ReplicatedExperienceState = ExperienceLoop->GetCurrentStateName();
    }
}

bool AAIFacemaskExperience::ServerAdvanceExperience_Validation()
{
    return true;  // Add validation logic if needed
}

void AAIFacemaskExperience::ServerRetreatExperience_Implementation()
{
    if (ExperienceLoop && ExperienceLoop->RetreatState())
    {
        ReplicatedExperienceState = ExperienceLoop->GetCurrentStateName();
    }
}

bool AAIFacemaskExperience::ServerRetreatExperience_Validation()
{
    return true;
}
```

---

### **4. Authority Checks**

**File:** `AIFacemaskExperience.cpp`

Update `ProcessButtonInput()`:

```cpp
void AAIFacemaskExperience::ProcessButtonInput()
{
    // Only read buttons on server (which is connected to the physical microcontroller)
    if (!HasAuthority())
    {
        return;
    }

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
        ServerAdvanceExperience();  // Use RPC instead of direct call
    }

    // Button 1 (Left Wrist Backward) or Button 3 (Right Wrist Backward)
    if ((CurrentButtonStates[1] && !PreviousButtonStates[1]) || 
        (CurrentButtonStates[3] && !PreviousButtonStates[3]))
    {
        ServerRetreatExperience();  // Use RPC instead of direct call
    }

    // Store current states for next frame
    for (int32 i = 0; i < 4; i++)
    {
        PreviousButtonStates[i] = CurrentButtonStates[i];
    }

    // Update replicated button states
    ReplicatedButtonStates.SetNum(4);
    for (int32 i = 0; i < 4; i++)
    {
        ReplicatedButtonStates[i] = CurrentButtonStates[i];
    }
}
```

---

### **5. Player Spawning**

**File:** Create new `LBEASTPlayerController.h/cpp`

```cpp
UCLASS()
class LBEASTEXPERIENCES_API ALBEASTPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    /** Player role (live actor vs player) */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "LBEAST")
    bool bIsLiveActor = false;

    /** Live actor index (0-3) */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "LBEAST")
    int32 LiveActorIndex = -1;
};
```

**File:** `AIFacemaskExperience.cpp`

```cpp
void AAIFacemaskExperience::InitializeExperienceImpl()
{
    // ... existing code ...

    // Enable replication
    bReplicates = true;
    SetReplicateMovement(false);  // We don't need movement replication for this actor

    // ... rest of initialization ...
}
```

---

### **6. Experience Loop State Sync**

**File:** `ExperienceStateMachine.h`

```cpp
UCLASS(Blueprintable, BlueprintType, ClassGroup=(LBEAST))
class LBEASTCORE_API UExperienceStateMachine : public UObject
{
    GENERATED_BODY()

    // Add replication support
    virtual bool IsSupportedForNetworking() const override { return true; }

    /** Set state from replicated value (called on clients) */
    UFUNCTION(BlueprintCallable, Category = "LBEAST|ExperienceLoop")
    void SetCurrentStateFromReplication(FName StateName);
};
```

---

## Architecture: How It Should Work

```
┌─────────────────────────────────────────────────┐
│  Dedicated Server PC                            │
│  ──────────────────                             │
│  • Runs AIFacemaskExperience (Authority)        │
│  • Connected to ESP32 via WiFi                  │
│  • Reads button presses from GetDigitalInput()  │
│  • Updates ExperienceLoop state                 │
│  • Replicates state to all clients              │
│  • Runs Omniverse Audio2Face                    │
│  • Streams facial animation                     │
└─────────────────────────────────────────────────┘
                    │
                    │ Unreal Replication
                    ├─────────────┬─────────────┐
                    │             │             │
        ┌───────────▼──┐   ┌──────▼──────┐   ┌──▼───────────┐
        │  HMD 1       │   │  HMD 2      │   │  HMD 3       │
        │  (Client)    │   │  (Client)   │   │  (Client)    │
        │  • Live Actor│   │  • Player   │   │  • Player    │
        │  • Receives  │   │  • Receives │   │  • Receives  │
        │    state     │   │    state    │   │    state     │
        └──────────────┘   └─────────────┘   └──────────────┘
```

---

## Testing Checklist

Once implemented, test:

- [ ] Client can discover and connect to server
- [ ] Multiple clients can connect simultaneously
- [ ] Button press on server advances experience for all clients
- [ ] Experience state syncs correctly to late-joining clients
- [ ] Server can track which clients are live actors vs players
- [ ] Disconnection/reconnection works correctly
- [ ] Experience Loop state persists through client reconnects

---

## Notes

- **EmbeddedSystems only runs on server** - The physical microcontroller is connected to the server PC
- **Clients receive replicated state** - They don't read from hardware directly
- **Audio2Face streams separately** - Uses NVIDIA's streaming, not Unreal replication
- **This is standard Unreal networking** - Not custom like the Server Beacon

---

**Priority:** Medium (Server discovery works, but actual multiplayer gameplay doesn't)

**Estimated Work:** 4-6 hours for basic implementation, 8-12 hours with testing/polish

