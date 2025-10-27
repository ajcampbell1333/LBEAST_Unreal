# LBEAST SDK for Unreal Engine

**Location-Based Entertainment Activation Standard Toolkit**

**Author Disclaimer**
This is a brand new plugin as of November 2025. Parts of it are not fully fleshed out. I built a lot of LBE for Fortune 10 brands in the 20-teens. This is the dream game-engine toolchain I wish we had back then, but I'm 100% certain it's full of unforeseen bugs in its current form. If you're seeing this message, it's because I have yet to deploy this plugin on a single professional project. I'm sure I will have lots of fixes to push after I've deployed it a few times in-situ. Use at your own risk in the meantime.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.5.4-blue.svg)](https://www.unrealengine.com/)

## Overview

LBEAST is a comprehensive SDK for developing VR and AR Location-Based Entertainment (LBE) experiences. This repository contains the **Unreal Engine 5.5.4** implementation of the LBEAST SDK.

The LBEAST SDK democratizes LBE development by providing:
- **Experience Templates** - Drag-and-drop complete LBE solutions
- **Low-Level APIs** - Technical modules for custom implementations
- **AI-Driven Facial Animation** for immersive theater live actors
- **Large-Scale Hydraulic Haptics** for motion platforms
- **Embedded Systems Integration** for costume and prop-mounted hardware
- **Multi-Player LAN Experiences** with state machine control
- **HMD Abstraction** supporting OpenXR, SteamVR, and Meta Quest
- **6DOF Tracking** with SteamVR trackers and future extensibility

> **Unity Version:** Also available at [github.com/ajcampbell1333/LBEAST_Unity](https://github.com/ajcampbell1333/LBEAST_Unity)

## Three-Tier Architecture

LBEAST uses a modular three-tier architecture:

### Tier 1: Low-Level APIs (Technical Modules)
Foundation modules providing core functionality:
- `LBEASTCore` - HMD/tracking abstraction, networking
- `AIFacemask` - Facial animation control
- `LargeHaptics` - Platform/gyroscope control
- `EmbeddedSystems` - Microcontroller integration

**Use these when:** Building custom experiences from scratch with full control.

### Tier 2: Experience Templates (Pre-Configured Actors)
Ready-to-use complete experiences combining multiple APIs:
- `AAIFacemaskExperience` - Live actor-driven multiplayer VR
- `AMovingPlatformExperience` - Standing hydraulic platform
- `AGunshipExperience` - 4-player seated platform
- `ACarSimExperience` - Racing/driving simulator
- `AFlightSimExperience` - Flight sim with HOTAS

**Use these when:** Rapid deployment of standard LBE configurations.

### Tier 3: Your Custom Game Logic
Build your specific experience (Tier 3) on top of templates (Tier 2) or APIs (Tier 1).

### When to Use What?

| Scenario | Use This | Why |
|----------|----------|-----|
| Building a gunship VR arcade game | `AGunshipExperience` | Pre-configured for 4 players, all hardware setup included |
| Building a racing game | `ACarSimExperience` | Simplified driving API, optimized motion profiles |
| Building a space combat game | `AFlightSimExperience` | HOTAS integration ready, continuous rotation supported |
| Custom 3-player standing platform | Low-Level APIs | Need custom configuration not covered by templates |
| Live actor-driven escape room | `AAIFacemaskExperience` | Live actor support, multiplayer, and embedded systems ready |
| Unique hardware configuration | Low-Level APIs | Full control over all actuators and systems |

**Rule of thumb:** Start with templates, drop to APIs only when you need customization beyond what templates offer.

## ⚠️ Terminology: Unreal Actors vs. Live Actors

**Important distinction for clarity:**

### Unreal Actor (AActor)
An **Unreal Actor** refers to Unreal Engine's base class `AActor` - the fundamental object that can be placed in a level. All Experience Templates (like `AAIFacemaskExperience`, `AMovingPlatformExperience`) inherit from `AActor`.

```cpp
// This is an Unreal Actor (engine class)
AAIFacemaskExperience* Experience = GetWorld()->SpawnActor<AAIFacemaskExperience>();
```

### Live Actor (Physical Performer)
A **Live Actor** refers to a **physical human performer** wearing VR equipment and/or costumes in the LBE installation. They drive in-game avatars with real-time facial animation and interact with players.

```cpp
// This configures support for 2 physical performers
Experience->NumberOfLiveActors = 2;  // Human performers wearing facemasks
Experience->NumberOfPlayers = 4;     // VR players
```

### Quick Reference
- **"Unreal Actor"** = C++ class that exists in the game world (`AActor`)
- **"Live Actor"** = Physical person performing in the experience
- **"Avatar"** = The in-game character controlled by a live actor
- **"Player"** = VR participant (not performing, just experiencing)

**In this documentation:**
- When we say "Actor" in code context (`AActor`, `SpawnActor`), we mean the Unreal Engine class
- When we say "Live Actor" or "live actors", we mean physical human performers
- Context should make it clear, but this distinction is important for the AI Facemask system

## Philosophy

LBEAST is **not** a no-code solution. It's a professional-grade toolchain designed for teams of programmers and technical artists building commercial LBE installations. 

The SDK provides:
- ✅ **C++ programmers** with robust APIs and extensible architecture
- ✅ **Blueprint artists** with drag-and-drop components and visual scripting
- ✅ **Content teams** with rapid deployment capabilities
- ✅ **Commercial projects** with free-to-use, MIT-licensed code

## Hardware-Agnostic Input System

### 🎮 Normalized Tilt Control (-1 to +1)

LBEAST uses a **joystick-style normalized input system** for all 5DOF hydraulic platforms. This means you write your game code once, and it works on any hardware configuration:

**Why Normalized Inputs?**
- ✅ **Hardware Independence:** Same game code works on platforms with 5° tilt or 15° tilt
- ✅ **Venue Flexibility:** Operators can upgrade/downgrade hardware without code changes
- ✅ **Intuitive API:** Think like a joystick: -1.0 (full left/back), 0.0 (center), +1.0 (full right/forward)
- ✅ **Automatic Scaling:** SDK maps your inputs to actual hardware capabilities

**Example:**
```cpp
// Your game sends: "tilt 50% right, 80% forward"
Platform->SendPlatformTilt(0.5f, 0.8f, 0.0f, 1.0f);

// On 5° max platform: Translates to Roll=2.5°, Pitch=4.0°
// On 15° max platform: Translates to Roll=7.5°, Pitch=12.0°
// Same code, automatically scaled!
```

**Axis Mapping:**
- **TiltX:** Left/Right roll (-1.0 = full left, +1.0 = full right)
- **TiltY:** Forward/Backward pitch (-1.0 = full backward, +1.0 = full forward)
- **VerticalOffset:** Up/Down translation (-1.0 = full down, +1.0 = full up)

**Advanced Users:** If you need precise control and know your hardware specs, angle-based APIs are available in the `Advanced` category.

## Features

### Experience Templates (Drag-and-Drop Solutions)

Experience Templates are complete, pre-configured Actors that you can drag into your level and use immediately. Each combines multiple low-level APIs into a cohesive, tested solution.

#### 🎭 AI Facemask Experience
**Class:** `AAIFacemaskExperience`

Deploy LAN multiplayer VR experiences where immersive theater live actors drive avatars with **autonomous AI-generated facial expressions**. The AI face operates independently using NVIDIA Audio2Face (Neural Face), while live actors control the experience flow through wrist-mounted buttons.

**⚠️ DEDICATED SERVER REQUIRED ⚠️**

This template **enforces** dedicated server mode. You **must** run a separate local PC as a headless dedicated server. This is **not optional** - the experience will fail to initialize if ServerMode is changed to Listen Server.

**Network Architecture:**
```
┌─────────────────────────────────────┐
│   Dedicated Server PC (Headless)   │
│                                     │
│  ┌───────────────────────────────┐ │
│  │  Unreal Dedicated Server      │ │ ← Multiplayer networking
│  │  (No HMD, no rendering)       │ │
│  └───────────────────────────────┘ │
│                                     │
│  ┌───────────────────────────────┐ │
│  │  NVIDIA Omniverse             │ │ ← AI Workflow:
│  │  - Speech Recognition         │ │   Speech → NLU → Emotion
│  │  - NLU (Natural Language)     │ │              ↓
│  │  - Emotion Detection          │ │        Audio2Face
│  │  - Audio2Face (Neural Face)   │ │              ↓
│  └───────────────────────────────┘ │   Facial animation stream
└─────────────────────────────────────┘
               │
        LAN Network (UDP/TCP)
               │
        ┌──────┴────────┐
        │               │
   VR HMD #1      VR HMD #2      (Live Actors)
   VR HMD #3      VR HMD #4      (Players)
```

**Why Dedicated Server?**
- **Performance**: Offloads heavy AI processing from VR HMDs
- **Parallelization**: Supports multiple live actors simultaneously
- **Reliability**: Isolated AI workflow prevents HMD performance degradation
- **Scalability**: Easy to add more live actors or players

**Automatic Server Discovery:**

LBEAST includes a **zero-configuration UDP broadcast system** for automatic server discovery:
- **Server**: Broadcasts presence every 2 seconds on port `7778`
- **Clients**: Automatically discover and connect to available servers
- **No Manual IP Entry**: Perfect for LBE installations where tech setup should be invisible
- **Multi-Experience Support**: Discover multiple concurrent experiences on the same LAN
- **Server Metadata**: Includes experience type, player count, version, current state

When a client HMD boots up, it automatically finds the dedicated server and connects - zero configuration required!

**Architecture:**
- **AI Face**: Fully autonomous, driven by NVIDIA Audio2Face for natural conversation
- **Live Actor Role**: Experience director, NOT puppeteer
- **Wrist Controls**: 4 buttons (2 left, 2 right) to advance/retreat through experience states
- **Experience Loop**: Built-in state machine (Intro → Tutorial → Act1 → Act2 → Finale → Credits)
- **Server Mode**: **ENFORCED** to Dedicated Server (attempting to change will fail initialization)

**Includes:**
- Pre-configured `UAIFaceController` (autonomous facial animation bridge)
- Pre-configured `UEmbeddedDeviceController` (wrist buttons)
- Pre-configured `UExperienceStateMachine` (story progression)
- LAN multiplayer support (configurable live actor/player counts)
- Passthrough mode for live actors to help players

**Button Layout:**
- **Left Wrist**: Button 0 (Forward), Button 1 (Backward)
- **Right Wrist**: Button 2 (Forward), Button 3 (Backward)

**Quick Start:**
```cpp
// In your level
AAIFacemaskExperience* Experience = GetWorld()->SpawnActor<AAIFacemaskExperience>();
Experience->NumberOfLiveActors = 1;
Experience->NumberOfPlayers = 4;
Experience->LiveActorMesh = MyCharacterMesh;

// ServerMode is already set to DedicatedServer by default
// DO NOT CHANGE IT - initialization will fail if you do

Experience->InitializeExperience();  // Will validate server mode

// React to experience state changes
FName CurrentState = Experience->GetCurrentExperienceState();

// Manually trigger state changes (usually handled by wrist buttons automatically)
Experience->AdvanceExperience();
Experience->RetreatExperience();
```

**❌ What Happens If You Try to Use Listen Server:**
```
========================================
⚠️  SERVER MODE CONFIGURATION ERROR ⚠️
========================================
This experience REQUIRES ServerMode to be set to 'DedicatedServer'
Current ServerMode is set to 'ListenServer'

Please change ServerMode in the Details panel to 'DedicatedServer'
========================================
```

**Blueprint Events:**
Override `OnExperienceStateChanged` to trigger game events:
```cpp
void OnExperienceStateChanged(FName OldState, FName NewState, int32 NewStateIndex)
{
    if (NewState == "Act1")
    {
        // Spawn enemies, trigger cutscene, etc.
    }
}
```

#### 🎢 Moving Platform Experience

**Class:** `AMovingPlatformExperience`

Single-player standing VR experience on an unstable hydraulic platform with safety harness. Provides pitch, roll, and Y/Z translation for immersive motion.

**Includes:**
- Pre-configured 5DOF hydraulic platform (4 actuators + scissor lift)
- 10° pitch and roll capability
- Vertical translation for rumble/earthquake effects
- Emergency stop and return-to-neutral functions
- Blueprint-friendly motion commands

**Quick Start:**
```cpp
AMovingPlatformExperience* Platform = GetWorld()->SpawnActor<AMovingPlatformExperience>();
Platform->MaxPitch = 10.0f;
Platform->MaxRoll = 10.0f;
Platform->InitializeExperience();

// Send normalized tilt (RECOMMENDED - hardware-agnostic)
// -1.0 to +1.0 automatically scales to hardware capabilities
Platform->SendPlatformTilt(0.3f, -0.5f, 0.0f, 2.0f);  // TiltX (right), TiltY (backward), Vertical, Duration

// Advanced: Use absolute angles if you need precise control
Platform->SendPlatformMotion(5.0f, -3.0f, 20.0f, 2.0f);  // pitch, roll, vertical, duration
```

#### 🚁 Gunship Experience

**Class:** `AGunshipExperience`

Four-player seated VR experience on a hydraulic platform. Perfect for multiplayer gunship, helicopter, spaceship, or multi-crew vehicle simulations.

**Includes:**
- Pre-configured 5DOF hydraulic platform (6 actuators + scissor lift)
- 4 pre-defined seat positions
- LAN multiplayer support (4 players)
- Synchronized motion for all passengers
- Emergency stop and safety functions

**Quick Start:**
```cpp
AGunshipExperience* Gunship = GetWorld()->SpawnActor<AGunshipExperience>();
Gunship->InitializeExperience();

// Send normalized motion (RECOMMENDED - hardware-agnostic)
// Values from -1.0 to +1.0 automatically scale to hardware capabilities
Gunship->SendGunshipTilt(0.5f, 0.8f, 0.2f, 1.5f);  // TiltX (right), TiltY (forward), Vertical, Duration

// Advanced: Use absolute angles if you need precise control
Gunship->SendGunshipMotion(8.0f, 5.0f, 10.0f, 15.0f, 1.5f);  // pitch, roll, lateral, vertical, duration
```

#### 🏎️ Car Sim Experience

**Class:** `ACarSimExperience`

Single-player seated racing/driving simulator on a hydraulic platform. Perfect for arcade racing games and driving experiences.

**Includes:**
- Pre-configured 5DOF hydraulic platform optimized for driving
- Motion profiles for cornering, acceleration, and bumps
- Compatible with racing wheels and pedals (via Unreal Input)
- Simplified API for driving simulation

**Quick Start:**
```cpp
ACarSimExperience* CarSim = GetWorld()->SpawnActor<ACarSimExperience>();
CarSim->InitializeExperience();

// Use normalized driving API (RECOMMENDED - hardware-agnostic)
CarSim->SimulateCornerNormalized(-0.8f, 0.5f);      // Left turn (normalized -1 to +1)
CarSim->SimulateAccelerationNormalized(0.5f, 0.5f); // Accelerate (normalized -1 to +1)
CarSim->SimulateBump(0.8f, 0.2f);                   // Road bump (intensity 0-1)

// Advanced: Use absolute angles if you need precise control
CarSim->SimulateCorner(-8.0f, 0.5f);         // Left turn (degrees)
CarSim->SimulateAcceleration(5.0f, 0.5f);    // Accelerate (degrees)
```

#### ✈️ Flight Sim Experience

**Class:** `AFlightSimExperience`

Single-player flight simulator using a two-axis gyroscope for continuous rotation beyond 360 degrees. Perfect for realistic flight arcade games and space combat.

**Includes:**
- Pre-configured 2DOF gyroscope system (continuous pitch/roll)
- **HOTAS controller integration:**
  - Logitech G X56 support
  - Thrustmaster T.Flight support
  - Joystick, throttle, and pedal controls
  - Configurable sensitivity and axis inversion
- Continuous rotation (720°, 1080°, unlimited)
- Blueprint-accessible input reading

**Quick Start:**
```cpp
AFlightSimExperience* FlightSim = GetWorld()->SpawnActor<AFlightSimExperience>();
FlightSim->HOTASType = ELBEASTHOTASType::LogitechX56;
FlightSim->bEnableJoystick = true;
FlightSim->bEnableThrottle = true;
FlightSim->InitializeExperience();

// Read HOTAS input in Tick
FVector2D Joystick = FlightSim->GetJoystickInput();  // X=roll, Y=pitch
float Throttle = FlightSim->GetThrottleInput();

// Send continuous rotation command (can exceed 360°)
FlightSim->SendContinuousRotation(720.0f, 360.0f, 4.0f);  // Two barrel rolls!
```

---

### Low-Level APIs (Advanced/Custom Usage)

For developers who need full control or want to build custom experiences from scratch, LBEAST provides low-level APIs. These are the same APIs used internally by the Experience Templates.

#### 🤖 AIFace API
**Module:** `AIFacemask`

Configure and play AI facial animations on any transform node (typically HMD).

```cpp
UAIFaceController* FaceController = CreateDefaultSubobject<UAIFaceController>(TEXT("FaceController"));
FAIFaceConfig Config;
Config.TargetMesh = CharacterMesh;
Config.bUseAIGeneration = true;
FaceController->InitializeAIFace(Config);
```

#### 🎛️ LargeHaptics API
**Module:** `LargeHaptics`

Manual control of individual hydraulic actuators, gyroscopes, and scissor lift translation.

**5DOF Platform Example:**
```cpp
UHapticPlatformController* PlatformController = CreateDefaultSubobject<UHapticPlatformController>(TEXT("Platform"));
FHapticPlatformConfig Config;
Config.PlatformType = ELBEASTPlatformType::CarSim_SinglePlayer;
PlatformController->InitializePlatform(Config);

FPlatformMotionCommand Command;
Command.Pitch = 5.0f;  // degrees
Command.Roll = -3.0f;
Command.Duration = 2.0f;  // seconds
PlatformController->SendMotionCommand(Command);
```

**2DOF Flight Sim with HOTAS Example:**
```cpp
UHapticPlatformController* FlightSimController = CreateDefaultSubobject<UHapticPlatformController>(TEXT("FlightSim"));
FHapticPlatformConfig Config;
Config.PlatformType = ELBEASTPlatformType::FlightSim_2DOF;
Config.GyroscopeConfig.MaxRotationSpeed = 90.0f;  // degrees per second

// Configure HOTAS controller
Config.GyroscopeConfig.HOTASType = ELBEASTHOTASType::LogitechX56;  // or ThrustmasterTFlight
Config.GyroscopeConfig.bEnableJoystick = true;
Config.GyroscopeConfig.bEnableThrottle = true;
Config.GyroscopeConfig.bEnablePedals = true;
Config.GyroscopeConfig.JoystickSensitivity = 1.5f;
Config.GyroscopeConfig.ThrottleSensitivity = 1.0f;

FlightSimController->InitializePlatform(Config);

// Read HOTAS input
FVector2D JoystickInput = FlightSimController->GetHOTASJoystickInput();  // X = roll, Y = pitch
float ThrottleInput = FlightSimController->GetHOTASThrottleInput();
float PedalInput = FlightSimController->GetHOTASPedalInput();

// Send gyroscope command
FPlatformMotionCommand Command;
Command.Pitch = 720.0f;  // Two full rotations
Command.Roll = 360.0f;   // One full roll
Command.bUseContinuousRotation = true;  // Enable continuous rotation
Command.Duration = 4.0f;
FlightSimController->SendMotionCommand(Command);
```

#### 🔌 EmbeddedSystems API
**Module:** `EmbeddedSystems`

Throughput to/from embedded PCBs supporting Arduino, ESP32, STM32, Raspberry Pi, and Jetson.

```cpp
UEmbeddedDeviceController* DeviceController = CreateDefaultSubobject<UEmbeddedDeviceController>(TEXT("Device"));
FEmbeddedDeviceConfig Config;
Config.DeviceType = ELBEASTMicrocontrollerType::ESP32;
Config.Protocol = ELBEASTCommProtocol::WiFi;
Config.DeviceAddress = "192.168.1.50";
DeviceController->InitializeDevice(Config);

// Trigger haptic in costume
DeviceController->TriggerHapticPulse(0, 0.8f, 0.5f);
```

## Installation

### Prerequisites

- **Unreal Engine 5.5.4** or later
- **Visual Studio 2022** (Windows)
- **SteamVR** (for VR features)
- **OpenXR Runtime** (Meta Quest, Windows Mixed Reality, etc.)

### Option 1: Install as Plugin (Recommended)

1. Clone this repository
2. Copy the `LBEAST_Unreal/Plugins/LBEAST` folder to your project's `Plugins` directory
3. Regenerate Visual Studio project files (right-click `.uproject` → Generate Visual Studio project files)
4. Build your project
5. Enable the plugin in Unreal Editor: **Edit → Plugins → LBEAST**

### Option 2: Use This Project Directly

1. Clone this repository
2. Right-click `LBEAST_Unreal.uproject` → Generate Visual Studio project files
3. Open `LBEAST_Unreal.sln` in Visual Studio
4. Build and run

## Quick Start

### Option A: Use Experience Templates (Recommended for Rapid Deployment)

1. **Enable the plugin** in Unreal Editor: Edit → Plugins → LBEAST
2. **Restart the editor**
3. **Drag an experience template** into your level from the Content Browser:
   - Search for "LBEAST" in the C++ Classes folder
   - Find `AAIFacemaskExperience`, `AMovingPlatformExperience`, etc.
4. **Configure in Details panel** and hit Play!

### Option B: Use Low-Level APIs (For Custom Implementations)

### 1. Enable Plugin Modules

In your project's `.uproject` file, add:

```json
{
  "Plugins": [
    {
      "Name": "LBEAST",
      "Enabled": true
    }
  ]
}
```

### 2. Add Module Dependencies

In your game module's `.Build.cs` file:

```csharp
PublicDependencyModuleNames.AddRange(new string[] { 
    "LBEASTCore", 
    "AIFacemask", 
    "LargeHaptics", 
    "EmbeddedSystems" 
});
```

### 3. Configure HMD System

```cpp
#include "LBEASTHMDInterface.h"

FLBEASTHMDConfig HMDConfig;
HMDConfig.HMDType = ELBEASTHMDType::OpenXR;
HMDConfig.bEnablePassthrough = true;  // For immersive live actors
// Apply to your VR pawn...
```

### 4. Add Components to Blueprints

LBEAST components are available in the **Add Component** menu:
- **AI Face Controller**
- **Haptic Platform Controller**
- **Embedded Device Controller**

All components are fully Blueprint-compatible with exposed properties and functions.

## Architecture

### Module Structure

```
LBEAST/
├── LBEASTCore          # Core systems, HMD/tracking abstraction
├── AIFacemask          # AI facial animation API
├── LargeHaptics        # Hydraulic platform & gyroscope control API
├── EmbeddedSystems     # Microcontroller integration API
└── LBEASTExperiences   # Pre-configured experience templates
    ├── AIFacemaskExperience
    ├── MovingPlatformExperience
    ├── GunshipExperience
    ├── CarSimExperience
    └── FlightSimExperience
```

### Networking

LBEAST v1.0 focuses on **local LAN multiplayer** using Unreal's built-in replication and Gameplay Ability System (GAS):

- **Listen Server** (one player acts as host) or **Dedicated Server** (headless PC for monitoring)
- No web-hosting or online matchmaking in v1.0
- Future versions may add cloud deployment

### Hardware Integration

All hardware communication is **abstracted** through interfaces:

- **HMD Interface** → OpenXR, SteamVR, Meta
- **Tracking Interface** → SteamVR Trackers (future: UWB, optical, ultrasonic)
- **Platform Controller** → UDP/TCP to hydraulic controller
- **Embedded Devices** → Serial, WiFi, Bluetooth, Ethernet

This allows you to:
1. Develop with simulated hardware
2. Integrate real hardware without changing game code
3. Swap hardware systems in configuration

## Documentation

### Example Projects

(Coming soon)

### API Reference

Full API documentation is available in the header files:
- `LBEASTCore/Public/` - Core interfaces
- `AIFacemask/Public/` - AI face API
- `LargeHaptics/Public/` - Haptics API
- `EmbeddedSystems/Public/` - Embedded API

### Video Tutorials

(Coming soon)

## Contributing

LBEAST is open-source under the MIT License. Contributions are welcome!

### Development Workflow

1. Fork this repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Code Standards

- Follow [Unreal Engine Coding Standard](https://docs.unrealengine.com/5.5/en-US/epic-cplusplus-coding-standard-for-unreal-engine/)
- Add XML documentation comments to public APIs
- Write Blueprint-friendly functions where appropriate
- Test with both C++ and Blueprint workflows

## Use Cases

LBEAST is designed for **commercial LBE installations** including:

- 🎬 **Movie/TV Promotional Activations** (Comic-Con, CES, Sundance, Tribeca)
- 🎮 **VR Arcades** with motion platforms
- 🎪 **Theme Park Attractions** with custom haptics
- 🎭 **Immersive Theater** with live actor-driven avatars
- 🏢 **Corporate Events** and brand experiences
- 🚀 **Research Labs** and academic projects

## Dedicated Server & Server Manager

The AIFacemask experience (and optionally other multiplayer experiences) uses a **dedicated server architecture** to offload AI processing and enable robust multi-player experiences.

### Architecture Overview

```
┌─────────────────────────────────────────────────┐
│  LBEAST Server Manager PC (with monitor)       │
│  ─────────────────────────────────────────────  │
│  • Runs Server Manager GUI (UMG interface)     │
│  • Launches dedicated server process           │
│  • Processes AI workflow (Speech → NLU →       │
│    Emotion → Audio2Face)                        │
│  • Streams facial animation to HMDs            │
└─────────────────────────────────────────────────┘
                    │
                    ├─ UDP Broadcast ──→ LAN (auto-discovery)
                    │
        ┌───────────┴───────────┐
        │                       │
        ▼                       ▼
   ┌─────────┐            ┌─────────┐
   │  HMD 1  │            │  HMD 2  │
   │ (Client)│            │ (Client)│
   └─────────┘            └─────────┘
```

### Building the Dedicated Server

1. **Set Build Configuration** to `Development Server` or `Shipping Server`
2. **Build** the project in Visual Studio
3. The server executable will be created: `Binaries/Win64/LBEAST_UnrealServer.exe`

### Option 1: Command-Line Launch (Quick)

Use the provided launch scripts:

**Windows:**
```batch
LaunchDedicatedServer.bat -experience AIFacemask -port 7777 -maxplayers 4
```

**Linux:**
```bash
./LaunchDedicatedServer.sh -experience AIFacemask -port 7777 -maxplayers 4
```

### Option 2: Server Manager GUI (Recommended)

The **LBEAST Server Manager** is a UMG-based application for managing dedicated servers with a graphical interface.

#### Starting the Server Manager

1. **Open** `LBEAST_Unreal.uproject`
2. **Load** the `ServerManager` map (or create one with `LBEASTServerManagerGameMode`)
3. **Play** in standalone mode

#### Server Manager Interface

```
┌────────────────────────────────────────┐
│  LBEAST Server Manager                 │
├────────────────────────────────────────┤
│  Configuration:                        │
│  Experience: [AIFacemask     ▼]        │
│  Server Name: [LBEAST Server]          │
│  Max Players: [4]                      │
│  Port: [7777]                          │
│                                         │
│  [Start Server]  [Stop Server]         │
├────────────────────────────────────────┤
│  Status:                               │
│  ● Running                             │
│  Players: 2/4                          │
│  State: Act1                           │
│  Uptime: 00:15:32                      │
├────────────────────────────────────────┤
│  Omniverse Audio2Face:                 │
│  Status: ● Connected                   │
│  Face Streams: 1 active                │
│  [Configure Omniverse]                 │
├────────────────────────────────────────┤
│  Logs:                                 │
│  [Server log output...]                │
└────────────────────────────────────────┘
```

#### Key Features

- **Start/Stop Servers** - Launch and manage dedicated server processes
- **Real-Time Monitoring** - Player count, experience state, uptime
- **Omniverse Integration** - Configure Audio2Face streaming (coming soon)
- **Live Logs** - View server output in real-time
- **Multi-Experience Support** - Switch between different experience types
- **Process Management** - Automatic detection of crashed servers

#### Creating a Custom Server Manager UI

The Server Manager is built on `ULBEASTServerManagerWidget`, which you can extend in Blueprint:

1. **Create** a new Widget Blueprint
2. **Parent Class:** `LBEASTServerManagerWidget`
3. **Design** your UI using UMG
4. **Bind** buttons to:
   - `StartServer()`
   - `StopServer()`
   - `OpenOmniverseConfig()`
5. **Set** the widget class in `LBEASTServerManagerGameMode`

### Automatic Server Discovery

Clients automatically discover and connect to dedicated servers on the LAN using UDP broadcast:

- **Server** broadcasts presence every 2 seconds on port `7778`
- **Clients** listen for broadcasts and auto-connect
- **No manual IP entry required** for venue deployments

See [Automatic Server Discovery](#automatic-server-discovery) for details.

### Omniverse Audio2Face Integration

The dedicated server PC should also run NVIDIA Omniverse with Audio2Face for real-time facial animation:

1. **Install** [NVIDIA Omniverse](https://www.nvidia.com/en-us/omniverse/)
2. **Install** Audio2Face from Omniverse Launcher
3. **Configure** Audio2Face to stream to your HMDs
4. **Connect** via the Server Manager Omniverse panel

> **Note:** Omniverse integration is in development. Current implementation provides the architecture and UI hooks.

---

## Roadmap

### v1.0 (Current)
- ✅ Core module architecture
- ✅ HMD abstraction (OpenXR, SteamVR, Meta)
- ✅ Tracking abstraction (SteamVR Trackers)
- ✅ AI Face API
- ✅ Large Haptics API (5DOF platforms + 2DOF gyroscope)
- ✅ HOTAS integration (Logitech X56, Thrustmaster T.Flight)
- ✅ Embedded Systems API
- ✅ **Experience Templates** (5 complete templates)
- ✅ **Dedicated Server** architecture
- ✅ **Server Manager GUI** (UMG-based)
- ✅ **Automatic Server Discovery** (UDP broadcast)
- 🔄 Omniverse Audio2Face integration (in progress)
- 🔄 Example projects (in progress)

### v1.1 (Planned)
- Real-time AI facial animation integration
- **Blueprint-only experience templates** (no C++ required)
- Performance profiling tools
- Hardware calibration utilities
- Visual configuration editor for platforms

### v2.0 (Future)
- Holographic eCommerce module (Looking Glass, Voxon)
- Cloud-based AI facial animation
- Custom tracking system plugins (UWB, optical, ultrasonic)
- Online multiplayer support
- AR headset support (if viable hardware emerges)

## Next Steps

Tasks in progress or ready for implementation:

- [ ] **Design the Default Server Manager UI** - Create polished UMG interface for server management
- [ ] **Omniverse Audio2Face Integration** - Connect to NVIDIA Omniverse for real-time facial animation streaming
- [ ] **Example Maps** - Create demonstration maps for each experience template
- [ ] **Blueprint Templates** - Create Blueprint-only versions of experience templates
- [ ] **Hardware Calibration Tool** - Build utility for calibrating hydraulic platforms

## Support

- **Issues:** [github.com/ajcampbell1333/LBEAST_Unreal/issues](https://github.com/ajcampbell1333/LBEAST_Unreal/issues)
- **Discussions:** [github.com/ajcampbell1333/LBEAST_Unreal/discussions](https://github.com/ajcampbell1333/LBEAST_Unreal/discussions)
- **Unity Version:** [github.com/ajcampbell1333/LBEAST_Unity](https://github.com/ajcampbell1333/LBEAST_Unity)

## License

Copyright (c) 2025 AJ Campbell

Licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Credits

Created by **AJ Campbell** with experience from dozens of LBE projects deployed at major entertainment events worldwide.

---

*LBEAST: Making location-based entertainment development accessible to everyone.*


