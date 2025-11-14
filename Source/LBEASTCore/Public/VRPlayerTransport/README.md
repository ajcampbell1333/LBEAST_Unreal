# VR Player Transport System

## Overview

The VR Player Transport system provides experience-agnostic replication of OpenXR HMD and hand tracking data for LAN multiplayer VR experiences. This enables all LBEAST experiences to support real-time 6DOF multiplayer where each player's head and hands are visible to other players.

## Architecture

### Components

1. **`ULBEASTVRPlayerReplicationComponent`** - Captures OpenXR data from local player and replicates it to all clients
2. **`FLBEASTXRReplicatedData`** - Data structure containing HMD and hand tracking transforms
3. **`ALBEASTVRPlayerPawn`** (optional) - Base pawn class with replication component pre-configured

### Data Flow

```
Local Player (Client)
  ↓
Capture OpenXR Data (HMD + Hand Keypoints)
  ↓
ULBEASTVRPlayerReplicationComponent
  ↓
Replicate to Server
  ↓
Server Replicates to All Clients
  ↓
Remote Players Receive Data
  ↓
LBEASTHandGestureRecognizer Uses Replicated Data
```

## Usage

### Option 1: Use Base Pawn Class (Recommended for New Projects)

```cpp
// Create a Blueprint child of ALBEASTVRPlayerPawn
// The replication component is automatically included
```

### Option 2: Add Component to Existing Pawn

```cpp
// In your pawn class constructor
VRReplicationComponent = CreateDefaultSubobject<ULBEASTVRPlayerReplicationComponent>(TEXT("VRReplicationComponent"));
```

### Option 3: Add in Blueprint

1. Open your VR player pawn Blueprint
2. Add Component → `LBEAST VR Player Replication Component`
3. Configure replication settings if needed

## Integration with Hand Gesture Recognition

The `LBEASTHandGestureRecognizer` automatically uses replicated data for remote players when:

1. `bOnlyProcessLocalPlayer` is set to `false`
2. `ULBEASTVRPlayerReplicationComponent` is present on the pawn

```cpp
// In your experience setup
ULBEASTHandGestureRecognizer* GestureRecognizer = GetHandGestureRecognizer();
if (GestureRecognizer)
{
    // Enable gesture recognition for all players (local + remote)
    GestureRecognizer->bOnlyProcessLocalPlayer = false;
}
```

## Replicated Data

### HMD Data
- Position (FVector)
- Rotation (FRotator)
- Tracking state (bool)

### Hand Data (per hand, left and right)
- Wrist transform
- Hand center (MiddleMetacarpal)
- All 5 fingertip transforms (Thumb, Index, Middle, Ring, Little)
- Hand tracking active state

## Configuration

### Replication Update Rate
- Default: 60 Hz
- Configurable: 10-120 Hz
- Higher = smoother but more bandwidth

### Enable/Disable Replication
- Set `bEnableReplication = false` to disable (e.g., single-player)

## Performance Considerations

- **Bandwidth**: ~2-5 KB/s per player at 60 Hz (depends on compression)
- **CPU**: Minimal overhead (capture only on local player)
- **Network**: Uses Unreal's native replication system (reliable, ordered)

## Future Enhancements

- Replicate all hand keypoints (full hand skeleton) for advanced gesture recognition
- Compression for bandwidth optimization
- Interpolation for smoother remote player movement
- Prediction for reduced latency

## Example: Gunship Experience Integration

```cpp
// In AGunshipExperience or your player pawn
void SetupVRPlayer(APawn* PlayerPawn)
{
    // Add replication component if not present
    if (!PlayerPawn->FindComponentByClass<ULBEASTVRPlayerReplicationComponent>())
    {
        ULBEASTVRPlayerReplicationComponent* ReplicationComp = 
            NewObject<ULBEASTVRPlayerReplicationComponent>(PlayerPawn);
        ReplicationComp->AttachToComponent(PlayerPawn->GetRootComponent(), 
            FAttachmentTransformRules::KeepWorldTransform);
        ReplicationComp->RegisterComponent();
    }
    
    // Configure gesture recognition for all players
    if (ULBEASTHandGestureRecognizer* GestureRecognizer = 
        PlayerPawn->FindComponentByClass<ULBEASTHandGestureRecognizer>())
    {
        GestureRecognizer->bOnlyProcessLocalPlayer = false;
    }
}
```

## Troubleshooting

### Replication Not Working
- Ensure pawn has `bReplicates = true`
- Check that component is replicated: `SetIsReplicatedByDefault(true)`
- Verify network role: Should be `ROLE_AutonomousProxy` on client, `ROLE_SimulatedProxy` on remote clients

### Hand Tracking Not Visible on Remote Players
- Ensure `ULBEASTVRPlayerReplicationComponent` is present on pawn
- Check `bEnableReplication = true`
- Verify OpenXR hand tracking is active on local player

### Gesture Recognition Not Working for Remote Players
- Set `bOnlyProcessLocalPlayer = false` on `LBEASTHandGestureRecognizer`
- Ensure `ULBEASTVRPlayerReplicationComponent` is present on pawn
- Check that hand tracking data is being replicated (use debug visualization)

