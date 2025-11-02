# AIFacemask Multiplayer Implementation TODO

## Current Status

### ✅ **Implemented:**
- Server discovery (UDP broadcast beacon)
- Client discovery (listening for servers)
- Server metadata broadcast (player count, state, version)
- Dedicated server enforcement
- Experience Loop state machine (with authority checks)
- Embedded Systems button input (authority-only)
- **Server RPCs for button presses** ✨ NEW
- **Authority checks for input processing** ✨ NEW
- **Input-agnostic request system** ✨ NEW
- **VR controller support (Blueprint override)** ✨ NEW

### ❌ **NOT Implemented:**
- Actual client-server connection (Step 1)
- Game state replication (Step 2)
- Player spawning/management (Step 5)
- Experience Loop state replication (Step 2, 6)

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

### **3. Server RPCs for Button Presses** ✅ **IMPLEMENTED**

**Status:** Complete with input-agnostic design

**Implementation:**
- `RequestAdvanceExperience()` / `RequestRetreatExperience()` - Public API
- `ServerAdvanceExperience()` / `ServerRetreatExperience()` - RPCs
- `AdvanceExperienceInternal()` / `RetreatExperienceInternal()` - Authority-only state changes

**Key Features:**
- Input-agnostic: Works with EmbeddedSystems, VR controllers, keyboard, AI, etc.
- Network-agnostic: Works with dedicated and listen servers
- Automatic routing: Calls internal functions on authority, sends RPCs on clients

**See:** `INPUT_ARCHITECTURE.md` for complete documentation

---

### **4. Authority Checks** ✅ **IMPLEMENTED**

**Status:** Complete with flexible input system

**Implementation:**
- `Tick()` checks `HasAuthority()` before processing input
- `ProcessEmbeddedSystemInput()` - ESP32 wrist buttons (authority-only)
- `ProcessVRControllerInput()` - VR controllers (Blueprint Native Event, authority-only)

**Key Features:**
- Only authority (server/listen host) reads input
- Clients will receive replicated state (Step 2, not yet implemented)
- Multiple input sources can coexist (EmbeddedSystems + VR + custom)

**See:** `INPUT_ARCHITECTURE.md` for complete documentation

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

## Progress Summary

### ✅ **Completed (Steps 3 & 4):**
- Server RPCs for button presses
- Authority checks
- Input-agnostic request system
- Multiple input source support
- **Time Taken:** ~2 hours

### 🚧 **Remaining (Steps 1, 2, 5, 6):**
- Client connection (Step 1)
- State replication (Step 2)
- Player spawning (Step 5)
- Experience Loop state sync (Step 6)
- **Estimated:** 3-5 hours for basic implementation, 6-10 hours with testing/polish

**Priority:** Medium-High (Input system complete, but clients can't see state changes yet)

**Next Up:** Steps 1 & 2 (Client connection + State replication)

