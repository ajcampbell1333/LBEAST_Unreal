# LBEAST SDK for Unreal Engine

<img src="Source/images/lbeast-logo.png" alt="LBEAST Logo" width="100%">

**Location-Based Entertainment Activation Standard Toolkit**

**Author Disclaimer**
This is a brand new plugin as of November 2025. Parts of it are not fully fleshed out. I built a lot of LBE for Fortune 10 brands in the 20-teens. This is the dream game-engine toolchain I wish we had back then, but I'm 100% certain it's full of unforeseen bugs in its current form. If you're seeing this message, it's because I have yet to deploy this plugin on a single professional project. I'm sure I will have lots of fixes to push after I've deployed it a few times in-situ. Use at your own risk in the meantime.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.5.4-blue.svg)](https://www.unrealengine.com/)

## Overview

LBEAST is a comprehensive SDK for developing VR and AR Location-Based Entertainment (LBE) experiences. This repository contains the **Unreal Engine 5.5.4** implementation of the LBEAST SDK.

The LBEAST SDK democratizes LBE development by providing:
- **Experience Genre Templates** - Drag-and-drop complete LBE solutions
- **Low-Level APIs** - Technical modules for custom implementations
- **AI-Driven Facial Animation** for immersive theater live actors (NVIDIA ACE automated pipeline)
- **Wireless Trigger Controls** - Embedded buttons in costume/clothes for narrative state machine control
- **Large-Scale Hydraulic Haptics** for motion platforms
- **Embedded Systems Integration** for costume and prop-mounted hardware
- **Multiplayer LAN Experiences** with state machine control
- **HMD Abstraction** supporting OpenXR, SteamVR, and Meta Quest
- **6DOF Tracking** with SteamVR trackers and future extensibility

> **🔗 Unity Version:** Also available at [github.com/ajcampbell1333/lbeast_unity](https://github.com/ajcampbell1333/lbeast_unity)

## Philosophy

<details>
<summary><strong>Who is LBEAST for?</strong></summary>

<div style="margin-left: 20px;">

LBEAST is for professional teams building commercial Location-Based Entertainment installations. It's a professional-grade toolchain designed for teams of programmers and technical artists.

**Target audiences:**
- **Theme park attraction designers/engineers/production staff** - Building immersive attractions with motion platforms, embedded systems, and live actor integration
- **VR Arcade venue designers/engineers/production staff** - Deploying multiplayer VR experiences with synchronized motion and professional audio/lighting
- **Brands at trade shows** interested in wowing audiences with VR
- **3rd-party VR developers** who want to deploy new content rapidly to theme parks and VR Arcades
- **VR educators** who want to expose students to professional toolchains used in commercial LBE production

The SDK provides:
- ✅ **C++ programmers** with robust APIs and extensible architecture
- ✅ **Blueprint artists** with drag-and-drop components and visual scripting
- ✅ **Content teams** with rapid deployment capabilities
- ✅ **Commercial projects** with free-to-use, MIT-licensed code

</div>

</details>

<details>
<summary><strong>Who is LBEAST not for?</strong></summary>

<div style="margin-left: 20px;">

Developers with little or no experience with C++ may struggle to put LBEAST to its fullest use. It is meant for a scale of production that would be challenging for lone developers. However, it can be a great learning tool for educators to prepare students to work on professional team projects.

**Important notes:**
- LBEAST is **not** a no-code solution. It requires programming knowledge (C++ or Blueprint scripting) to customize experiences beyond the provided templates.
- LBEAST is designed for **team-based production** with multiple developers, technical artists, and production staff.
- LBEAST provides blueprints, but it assumes tech art team members have access to C++ programmers on the team to back them up for customization.

</div>

</details>

## Three-Tier Architecture

LBEAST uses a modular three-tier architecture for code organization and server/client deployment.

### Code and Class Structure

<details>
<summary><strong>Tier 1: Low-Level APIs (Technical Modules)</strong></summary>

<div style="margin-left: 20px;">

Foundation modules providing core functionality:
- `LBEASTCore` - HMD/tracking abstraction, networking
- `AIFacemask` - Facial animation control
- `LargeHaptics` - Platform/gyroscope control
- `EmbeddedSystems` - Microcontroller integration
- `ProAudio` - Professional audio console control via OSC
- `ProLighting` - DMX lighting control (Art-Net, USB DMX)
- `VOIP` - Low-latency voice communication with 3D HRTF spatialization

**Use these when:** Building custom experiences from scratch with full control.

</div>

</details>

<details>
<summary><strong>Tier 2: Experience Genre Templates (Pre-Configured Actors)</strong></summary>

<div style="margin-left: 20px;">

Ready-to-use complete experiences combining multiple APIs:
- `AAIFacemaskExperience` - Live actor-driven multiplayer VR with wireless trigger buttons controlling automated AI facemask performances
- `AMovingPlatformExperience` - A 4-gang hydraulic platform on which a single VR player stands while hooked to a suspended cable harness to prevent falling
- `AGunshipExperience` - 4-player seated platform with 4DOF hydraulic motion driven by a 4-gang actuator platform with a player strapped in at each corner, all fixed to a hydraulic lift that can dangle players a few feet in the air
- `ACarSimExperience` - A racing/driving simulator where 1-4 player seats are bolted on top of a 4-gang hydraulic platform
- `AFlightSimExperience` - A single player flight sim with HOTAS controls in a 2-axis gyroscopic cockpit built with servo motors for pitch and roll 
- `AEscapeRoomExperience` - Puzzle-based escape room with embedded door lock/prop latch solenoids, sensors, and pro AV integration for light/sound and live improv actors

**Use these when:** Rapid deployment of standard LBE genres.

</div>

</details>

<details>
<summary><strong>Tier 3: Your Custom Game Logic</strong></summary>

<div style="margin-left: 20px;">

Build your specific experience (Tier 3) on top of templates (Tier 2) or APIs (Tier 1).

</div>

</details>

<details>
<summary><strong>When to Use What?</strong></summary>

<div style="margin-left: 20px;">

| Scenario | Use This | Why |
|----------|----------|-----|
| Building a gunship VR arcade game | `AGunshipExperience` | Pre-configured for 4 players, all hardware setup included |
| Building a racing game | `ACarSimExperience` | Simplified driving API, optimized motion profiles |
| Building a space combat game | `AFlightSimExperience` | HOTAS integration ready, continuous rotation supported |
| Custom 3-player standing platform | Low-Level APIs | Need custom configuration not covered by templates |
| Live actor-driven escape room | `AAIFacemaskExperience` | Wireless trigger buttons in costume control narrative state machine, automated AI facemask performances |
| Puzzle-based escape room | `AEscapeRoomExperience` | Narrative state machine, door locks, prop sensors, embedded systems |
| Unique hardware configuration | Low-Level APIs | Full control over all actuators and systems |

**Rule of thumb:** Start with templates, drop to APIs only when you need customization beyond what templates offer.

</div>

</details>

### LAN Server/Client Configuration

<details>
<summary><strong>Local Command Console</strong></summary>

<div style="margin-left: 20px;">

```
┌─────────────────────────────────────────────────┐
│  Single PC (Command Console + Server Manager)   │
│  ─────────────────────────────────────────────  │
│  • Command Console UI (UMG Panel)               │
│  • Server Manager (Dedicated Server Backend)    │
│  • All processing on one machine                │
└──────────────────────┬──────────────────────────┘
                       │
                       │ UDP WiFi (LAN)
                       │
        ┌──────────────┴──────────────┐
        │                             │
        ▼                             ▼
   ┌─────────┐                  ┌─────────┐
   │ VR HMD  │                  │ VR HMD  │
   │(Player) │                  │(Live    │
   │   1     │                  │ Actor 1)│
   └─────────┘                  └─────────┘
        │                             │
        │                             │
   ┌────┴────┐                  ┌────┴────┐
   │  ...    │                  │  ...    │
   │(Players)│                  │(Live    │
   │  N      │                  │ Actors) │
   └─────────┘                  └─────────┘
```

**Use Case:** Simple setup for single-player or lightweight multiplayer experiences. Command Console and Server Manager share the same PC.

</div>

</details>

<details>
<summary><strong>Dedicated Server + Separate Local Command Console</strong></summary>

<div style="margin-left: 20px;">

```
┌─────────────────────────────────────────────────┐
│  Server Manager PC (Dedicated Server)           │
│  ─────────────────────────────────────────────  │
│  • Handles all network traffic                  │
│  • Decision-making & game state logic           │
│  • Graphics processing offloaded from VR        │
│  • AI workflow (Speech → NLU → Emotion →        │
│    Audio2Face)                                  │
└──────────────────────┬──────────────────────────┘
                        │
                        │ UDP WiFi (LAN)
                        │
        ┌───────────────┴───────────────┐
        │                               │
        ▼                               ▼
   ┌─────────┐                    ┌─────────┐
   │ VR HMD  │                    │ VR HMD  │
   │(Player) │                    │(Live    │
   │   1     │                    │ Actor 1)│
   └─────────┘                    └─────────┘
        │                               │
        │                               │
   ┌────┴────┐                  ┌───────┴─┐
   │  ...    │                  │  ...    │
   │(Players)│                  │(Live    │
   │  N      │                  │ Actors) │
   └─────────┘                  └─────────┘
        │                               │
        │                               │
        └───────────────┬───────────────┘
                        │
                        │ UDP WiFi (LAN)
                        │
                        ▼
        ┌─────────────────────────────────────┐
        │  Command Console PC (Local Network) │
        │  ────────────────────────────────── │
        │  • Server Manager GUI (UMG Panel)   │
        │  • Admin Panel for Ops Tech         │
        │  • Experience control interface     │
        └─────────────────────────────────────┘
```

**Use Case:** Heavy processing workloads. Server Manager runs on dedicated PC, Command Console runs on separate PC on same LAN. Better performance isolation and HMD battery life.

</div>

</details>

<details>
<summary><strong>Dedicated Server + Remote Command Console</strong></summary>

<div style="margin-left: 20px;">

```
┌─────────────────────────────────────────────────┐
│  Server Manager PC (Dedicated Server)           │
│  ─────────────────────────────────────────────  │
│  • Handles all network traffic                  │
│  • Decision-making & game state logic           │
│  • Graphics processing offloaded from VR        │
│  • AI workflow (Speech → NLU → Emotion →        │
│    Audio2Face)                                  │
└──────────────────────┬──────────────────────────┘
                       │
                       │ UDP WiFi (LAN)
                       │
        ┌──────────────┴──────────────┐
        │                             │
        ▼                             ▼
   ┌─────────┐                  ┌─────────┐
   │ VR HMD  │                  │ VR HMD  │
   │(Player) │                  │(Live    │
   │   1     │                  │ Actor 1)│
   └─────────┘                  └─────────┘
        │                             │
        │                             │
   ┌────┴────┐                  ┌─────┴───┐
   │  ...    │                  │  ...    │
   │(Players)│                  │(Live    │
   │  N      │                  │ Actors) │
   └─────────┘                  └─────────┘
        │                              │
        │                              │
        └───────────────┬──────────────┘
                        │
                        │ UDP WiFi (LAN)
                        │
                        ▼
        ┌─────────────────────────────────────┐
        │  Internet Node (Router/Firewall)    │
        │  ────────────────────────────────── │
        │  • Network boundary                 │
        │  • Port forwarding required         │
        │  • VPN recommended for security     │
        └──────────────────────┬──────────────┘
                               │
                               │ UDP (Port 7779)
                               │ Internet/WAN
                               │
                               ▼
        ┌─────────────────────────────────────┐
        │  Command Console PC (Remote)        │
        │  ────────────────────────────────── │
        │  • Server Manager GUI (UMG Panel)   │
        │  • Admin Panel for Ops Tech         │
        │  • Experience control interface     │
        │  • Off-site monitoring/control      │
        └─────────────────────────────────────┘
```

**Use Case:** Off-site monitoring and control. Command Console connects to Server Manager over internet. **⚠️ Recommended for debugging/testing only. For general public operation, full internet isolation is recommended for security.** Requires authentication enabled in Command Protocol settings.

</div>

</details>

<details>
<summary><strong>When to Use What Configuration?</strong></summary>

<div style="margin-left: 20px;">

| Scenario | Recommended Configuration | Why |
|----------|---------------------------|-----|
| Basic single-player experience | **Local Command Console** (same PC as server) | Simple setup, no need for separate machines. Command Console launches and manages server locally. |
| Basic multiplayer with RPCs but no heavy data transferring wirelessly | **Local Command Console** (same PC as server) | Network traffic is lightweight (player positions, events). Local Command Console can manage server on same machine efficiently. |
| Lots of heavy graphics processing you want to offload from VR HMD(s) | **Dedicated Server + Separate Local Command Console** (separate PCs, same LAN) | Offload GPU-intensive rendering and AI processing to dedicated server PC. Command Console monitors and controls from separate machine on same LAN. Better performance isolation and HMD battery life. |
| Need to monitor the experience in real-time from off-site? | **Dedicated Server + Remote Command Console** (separate PCs, internet connection) ⚠️ | Remote Command Console can connect over internet to monitor server status, player count, experience state, and logs from a separate location. **⚠️ Recommended for debugging/testing only. For general public operation, full internet isolation is recommended for security.** Requires authentication enabled in Command Protocol settings. |

**Configuration Options:**
- **Local Command Console:** Command Console (UI Panel) and Server Manager (dedicated server) run on the same PC. Simple setup, one machine.
- **Dedicated Server + Separate Local Command Console:** Server Manager runs on dedicated PC, Command Console runs on separate PC on same LAN. Networked via UDP (port 7779). Better for heavy processing workloads.
- **Dedicated Server + Remote Command Console:** Server Manager runs on dedicated PC, Command Console runs on separate PC connected via internet. Networked via UDP (port 7779) over WAN. For off-site monitoring only (debugging/testing).

</div>

</details>

---

<details>
<summary><strong>⚠️ Unreal Terminology: Unreal Actors vs. Live Actors</strong></summary>

<div style="margin-left: 20px;">

**Important distinction for clarity:**

### Unreal Actor (AActor)
An **Unreal Actor** refers to Unreal Engine's base class `AActor` - the fundamental object that can be placed in a level. All Experience Genre Templates (like `AAIFacemaskExperience`, `AMovingPlatformExperience`) inherit from `AActor`.

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

</div>

</details>

## Features

### Experience Genre Templates (Drag-and-Drop Solutions)

Experience Genre Templates are complete, pre-configured Actors that you can drag into your level and use immediately. Each combines multiple low-level APIs into a cohesive, tested solution.

<details>
<summary><strong>🎭 AI Facemask Experience</strong></summary>

<div style="margin-left: 20px;">

**Class:** `AAIFacemaskExperience`

Deploy LAN multiplayer VR experiences where immersive theater live actors drive avatars with **fully automated AI-generated facial expressions**. The AI face is controlled entirely by NVIDIA ACE pipeline - no manual animation, rigging, or blend shape tools required.

**⚠️ DEDICATED SERVER REQUIRED ⚠️**

This template **enforces** dedicated server mode. You **must** run a separate local PC as a headless dedicated server. This is **not optional** - the experience will fail to initialize if ServerMode is changed to Listen Server.

**Network Architecture:**
```
┌─────────────────────────────────────┐
│   Dedicated Server PC (Headless)    │
│                                     │
│  ┌───────────────────────────────┐  │
│  │  Unreal Dedicated Server      │  │ ← Multiplayer networking
│  │  (No HMD, no rendering)       │  │
│  └───────────────────────────────┘  │
│                                     │
│  ┌───────────────────────────────┐  │
│  │  NVIDIA ACE Pipeline           │  │ ← AI Workflow:
│  │  - Speech Recognition         │  │   Audio → NLU → Emotion
│  │  - NLU (Natural Language)     │  │              ↓
│  │  - Emotion Detection          │  │   Facial Animation
│  │  - Facial Animation Gen       │  │   (Textures + Blend Shapes)
│  └───────────────────────────────┘  │              ↓
└─────────────────────────────────────┘   Stream to HMDs
               │
        LAN Network (UDP/TCP)
               │
        ┌──────┴────────┐
        │               │
   VR HMD #1      VR HMD #2      (Live Actors)
   VR HMD #3      VR HMD #4      (Players)
```

**AI Facial Animation (Fully Automated):**
- **NVIDIA ACE Pipeline**: Generates facial textures and blend shapes automatically
- **No Manual Control**: Live actors never manually animate facial expressions
- **No Rigging Required**: NVIDIA ACE handles all facial animation generation
- **Real-Time Application**: AIFaceController receives NVIDIA ACE output and applies to mesh
- **Mask-Like Tracking**: AIFace mesh is tracked on top of live actor's face in HMD
- **Context-Aware**: Facial expressions determined by audio, NLU, emotion, and narrative state machine
- **Automated Performances**: Each narrative state triggers fully automated AI facemask performances

**Live Actor Control (High-Level Flow Only):**
- **Wireless Trigger Buttons**: Embedded in live actor's costume/clothes (ESP32, WiFi-connected)
- **Narrative State Control**: Buttons advance/retreat the narrative state machine (Intro → Act1 → Act2 → Finale)
- **Automated Performance Triggers**: State changes trigger automated AI facemask performances - live actor controls when, not how
- **Experience Direction**: Live actor guides players through story beats by controlling narrative flow

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

**Complete System Flow:**

The AI Facemask system supports two workflows: **pre-baked scripts** (narrative-driven) and **real-time improv** (player interaction-driven).

**Pre-Baked Script Flow (Narrative-Driven):**
```
Live Actor presses wireless trigger button (embedded in costume)
    ↓
Narrative State Machine advances/retreats (Intro → Act1 → Act2 → Finale)
    ↓
ACE Script Manager triggers pre-baked script for new state
    ↓
NVIDIA ACE Server streams pre-baked facial animation (from cached TTS + Audio2Face)
    ↓
AIFaceController receives facial animation data (blend shapes + textures)
    ↓
Facial animation displayed on live actor's HMD-mounted mesh
```

**Real-Time Improv Flow (Player Interaction-Driven):**
```
Player speaks into HMD microphone
    ↓
VOIPManager captures audio → Sends to Mumble server
    ↓
Dedicated Server receives audio via Mumble
    ↓
ACE ASR Manager (visitor pattern) receives audio → Converts speech to text (NVIDIA Riva ASR)
    ↓
ACE Improv Manager receives text → Local LLM (with LoRA) generates improvised response
    ↓
Local TTS (NVIDIA Riva) converts text → audio
    ↓
Local Audio2Face (NVIDIA NIM) converts audio → facial animation
    ↓
Facial animation streamed to AIFaceController
    ↓
Facial animation displayed on live actor's HMD-mounted mesh
```

**Component Architecture:**
```
┌─────────────────────────────────────────────────────────────────┐
│  PLAYER HMD (Client)                                            │
│  ────────────────────────────────────────────────────────────   │
│  1. Player speaks into HMD microphone                           │
│  2. VOIPManager captures audio                                  │
│  3. Audio sent to Mumble server (Opus encoded)                  │
└───────────────────────┬─────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────────┐
│  MUMBLE SERVER (LAN)                                            │
│  ────────────────────────────────────────────────────────────   │
│  Routes audio to dedicated server                               │
└───────────────────────┬─────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────────┐
│  DEDICATED SERVER PC (Unreal Engine Server)                     │
│  ────────────────────────────────────────────────────────────   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  ACE ASR Manager (VOIP-to-AIFacemask Visitor Pattern)    │   │
│  │  - Receives audio from Mumble                            │   │
│  │  - Buffers audio (voice activity detection)              │   │
│  │  - Converts speech → text (NVIDIA Riva ASR)              │   │
│  │  - Triggers Improv Manager with text                     │   │
│  └───────────────────────┬──────────────────────────────────┘   │
│                          │                                      │
│                          ▼                                      │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  ACE Improv Manager                                      │   │
│  │  - Receives text from ASR Manager                        │   │
│  │  - Local LLM (Ollama/vLLM/NIM + LoRA) → Improvised text  │   │
│  │  - Local TTS (NVIDIA Riva) → Audio file                  │   │
│  │  - Local Audio2Face (NVIDIA NIM) → Facial animation      │   │
│  └───────────────────────┬──────────────────────────────────┘   │
│                          │                                      │
│                          ▼                                      │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  ACE Script Manager                                      │   │
│  │  - Manages pre-baked scripts                             │   │
│  │  - Triggers scripts on narrative state changes           │   │
│  │  - Pre-bakes scripts (TTS + Audio2Face) on ACE server    │   │
│  └──────────────────────────────────────────────────────────┘   │
└───────────────────────┬─────────────────────────────────────────┘
                        │
                        │ Facial Animation Data (Blend Shapes + Textures)
                        │
                        ▼
┌─────────────────────────────────────────────────────────────────┐
│  LIVE ACTOR HMD (Client)                                        │
│  ────────────────────────────────────────────────────────────   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  AIFaceController                                        │   │
│  │  - Receives facial animation data from server            │   │
│  │  - Applies blend shapes/textures to mesh                 │   │
│  │  - Real-time facial animation display                    │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

**Architecture:**
- **AI Face**: Fully autonomous, driven by NVIDIA ACE pipeline (Audio → NLU → Emotion → Facial Animation)
- **Live Actor Role**: High-level experience director via wireless trigger buttons, NOT facial puppeteer
- **Wireless Controls**: Embedded trigger buttons in live actor's costume/clothes (4 buttons total)
- **Narrative State Machine**: Live actor advances/retreats through story beats (Intro → Tutorial → Act1 → Act2 → Finale → Credits)
- **Automated Performances**: AI facemask performances are fully automated - live actor controls flow, not expressions
- **Server Mode**: **ENFORCED** to Dedicated Server (attempting to change will fail initialization)

**Live Actor Control System:**
- **Wireless Trigger Buttons**: Embedded in live actor's costume/clothes (ESP32-based, WiFi-connected)
- **High-Level Flow Control**: Buttons advance/retreat the narrative state machine, which triggers automated AI facemask performances
- **No Facial Control**: Live actor never manually controls facial expressions - NVIDIA ACE handles all facial animation
- **Experience Direction**: Live actor guides players through story beats by advancing/retreating narrative states

**Includes:**
- Pre-configured `UAIFaceController` (receives NVIDIA ACE output, applies to mesh)
- Pre-configured `UEmbeddedDeviceController` (wireless trigger buttons embedded in costume)
- Pre-configured `UExperienceStateMachine` (narrative story progression)
- Pre-configured `UAIFacemaskACEScriptManager` (pre-baked script collections)
- Pre-configured `UAIFacemaskACEImprovManager` (real-time improvised responses)
- Pre-configured `UAIFacemaskASRManager` (speech-to-text for player voice)
- LAN multiplayer support (configurable live actor/player counts)
- Passthrough mode for live actors to help players

**Button Layout (Embedded in Costume):**
- **Left Wrist/Clothing**: Button 0 (Advance narrative), Button 1 (Retreat narrative)
- **Right Wrist/Clothing**: Button 2 (Advance narrative), Button 3 (Retreat narrative)

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

// Live actor controls high-level flow via wireless trigger buttons embedded in costume
// Buttons advance/retreat narrative state machine, which triggers automated AI facemask performances
// Facial expressions are fully automated by NVIDIA ACE - no manual control needed

// React to experience state changes (triggered by live actor's buttons)
FName CurrentState = Experience->GetCurrentExperienceState();

// Programmatically trigger state changes (usually handled by wireless buttons automatically)
Experience->RequestAdvanceExperience();  // Advance narrative state
Experience->RequestRetreatExperience();  // Retreat narrative state
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
Override `OnExperienceStateChanged` to trigger game events when live actor advances/retreats narrative state via wireless trigger buttons:
```cpp
void OnExperienceStateChanged(FName OldState, FName NewState, int32 NewStateIndex)
{
    // State changes are triggered by live actor's wireless trigger buttons
    // Each state change triggers automated AI facemask performances
    if (NewState == "Act1")
    {
        // Spawn enemies, trigger cutscene, etc.
        // NVIDIA ACE will automatically generate facial expressions for this state
    }
}
```

</div>

</details>

<details>
<summary><strong>🎢 Moving Platform Experience</strong></summary>

<div style="margin-left: 20px;">

**Class:** `AMovingPlatformExperience`

Single-player standing VR experience on an unstable hydraulic platform with safety harness. Provides pitch, roll, and Y/Z translation for immersive motion.

**Includes:**
- Pre-configured 4DOF hydraulic platform (4 actuators + scissor lift)
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

</div>

</details>

<details>
<summary><strong>🚁 Gunship Experience</strong></summary>

<div style="margin-left: 20px;">

**Class:** `AGunshipExperience`

Four-player VR experience where each player is strapped to the corner of a hydraulic platform capable of 4DOF motion (pitch/roll/forward/reverse/lift-up/liftdown). Perfect for multiplayer gunship, helicopter, spaceship, or multi-crew vehicle simulations.

**Includes:**
- Pre-configured 4DOF hydraulic platform (6 actuators + scissor lift)
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
Gunship->SendGunshipTilt(0.5f, 0.8f, 0.2f, 0.1f, 1.5f);  // TiltX (roll), TiltY (pitch), ForwardOffset, VerticalOffset, Duration

// Advanced: Use absolute angles if you need precise control
Gunship->SendGunshipMotion(8.0f, 5.0f, 10.0f, 15.0f, 1.5f);  // pitch, roll, forwardOffset (cm), verticalOffset (cm), duration
```

</div>

</details>

<details>
<summary><strong>🏎️ Car Sim Experience</strong></summary>

<div style="margin-left: 20px;">

**Class:** `ACarSimExperience`

Single-player seated racing/driving simulator on a hydraulic platform. Perfect for arcade racing games and driving experiences.

**Includes:**
- Pre-configured 4DOF hydraulic platform optimized for driving
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

</div>

</details>

<details>
<summary><strong>✈️ Flight Sim Experience</strong></summary>

<div style="margin-left: 20px;">

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

</div>

</details>

<details>
<summary><strong>🚪 Escape Room Experience</strong></summary>

<div style="margin-left: 20px;">

**Class:** `AEscapeRoomExperience`

Puzzle-based escape room experience with narrative state machine, embedded door locks, and prop sensors. Perfect for interactive puzzle experiences with physical hardware integration.

**Includes:**
- Pre-configured narrative state machine (puzzle progression)
- Embedded door lock control (unlock/lock doors via microcontroller)
- Prop sensor integration (read sensor values from embedded devices)
- Automatic door unlocking based on puzzle state
- Door state callbacks (confirm when doors actually unlock)

**Quick Start:**
```cpp
AEscapeRoomExperience* EscapeRoom = GetWorld()->SpawnActor<AEscapeRoomExperience>();
EscapeRoom->InitializeExperience();

// Unlock a specific door (by index)
EscapeRoom->UnlockDoor(0);  // Unlock door 0

// Lock a door
EscapeRoom->LockDoor(0);

// Check if door is unlocked
bool bIsUnlocked = EscapeRoom->IsDoorUnlocked(0);

// Trigger a prop action (e.g., activate a sensor)
EscapeRoom->TriggerPropAction(0, 1.0f);  // Prop 0, value 1.0

// Read prop sensor value
float SensorValue = EscapeRoom->ReadPropSensor(0);

// Get current puzzle state
FName CurrentState = EscapeRoom->GetCurrentPuzzleState();
```

**Blueprint Events:**
Override `OnNarrativeStateChanged` to trigger game events:
```cpp
void OnNarrativeStateChanged(FName OldState, FName NewState, int32 NewStateIndex)
{
    if (NewState == "Puzzle1_Complete")
    {
        // Unlock next door, play sound, etc.
    }
}
```

</div>

</details>

---

### Low-Level APIs (Advanced/Custom Usage)

For developers who need full control or want to build custom experiences from scratch, LBEAST provides low-level APIs. These are the same APIs used internally by the Experience Genre Templates.

<details>
<summary><strong>🤖 AIFace API</strong></summary>

<div style="margin-left: 20px;">

**Module:** `AIFacemask`

Receive and apply NVIDIA ACE facial animation output to a live actor's HMD-mounted mesh.

**Important:** This is a receiver/display system - facial animation is fully automated by NVIDIA ACE. No manual control, keyframe animation, or rigging required.

```cpp
UAIFaceController* FaceController = CreateDefaultSubobject<UAIFaceController>(TEXT("FaceController"));
FAIFaceConfig Config;
Config.TargetMesh = LiveActorMesh;  // Mesh attached to live actor's HMD/head
Config.NVIDIAACEEndpointURL = TEXT("http://localhost:8080/ace");  // NVIDIA ACE endpoint
Config.UpdateRate = 30.0f;  // Receive updates at 30 Hz
FaceController->InitializeAIFace(Config);

// NVIDIA ACE will automatically stream facial animation data
// Component receives and applies it via ReceiveFacialAnimationData()
```

</div>

</details>

<details>
<summary><strong>🎛️ LargeHaptics API</strong></summary>

<div style="margin-left: 20px;">

**Module:** `LargeHaptics`

Manual control of individual hydraulic actuators, gyroscopes, and scissor lift translation.

<details>
<summary><strong>🎮 Hardware-Agnostic Input System - Normalized Tilt Control (-1 to +1)</strong></summary>

<div style="margin-left: 20px;">

LBEAST uses a **joystick-style normalized input system** for all 4DOF hydraulic platforms. This means you write your game code once, and it works on any hardware configuration:

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

</div>

</details>

**4DOF Platform Example:**
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

</div>

</details>

<details>
<summary><strong>🔌 EmbeddedSystems API</strong></summary>

<div style="margin-left: 20px;">
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

</div>

</details>

<details>
<summary><strong>🎛️ ProAudio API</strong></summary>

<div style="margin-left: 20px;">
**Module:** `ProAudio`

Hardware-agnostic professional audio console control via OSC. Uses Unreal Engine's built-in OSC plugin (no external dependencies).

**Example:**
```cpp
UProAudioController* AudioController = CreateDefaultSubobject<UProAudioController>(TEXT("AudioController"));
FLBEASTProAudioConfig Config;
Config.ConsoleType = ELBEASTProAudioConsole::BehringerX32;
Config.BoardIPAddress = TEXT("192.168.1.100");
Config.OSCPort = 10023;  // X32 default OSC port

AudioController->InitializeConsole(Config);

// Control channel fader (0.0 = -inf, 1.0 = 0dB)
AudioController->SetChannelFader(1, 0.75f);  // Channel 1 to 75%

// Mute/unmute channel
AudioController->SetChannelMute(2, true);   // Mute channel 2

// Set bus send (e.g., reverb send)
AudioController->SetChannelBusSend(1, 1, 0.5f);  // Channel 1 → Bus 1 at 50%

// Control master fader
AudioController->SetMasterFader(0.9f);  // Master to 90%
```

**Supported Consoles:**
- ✅ Behringer X32, M32, Wing
- ✅ Yamaha QL, CL, TF, DM7
- ✅ Allen & Heath SQ, dLive
- ✅ Soundcraft Si
- ✅ PreSonus StudioLive
- ✅ Custom (manual OSC paths)

**Benefits:**
- ✅ **No Max for Live** - Direct OSC to console (no intermediate software)
- ✅ **Native Unreal** - Uses built-in OSC plugin (no external dependencies)
- ✅ **Cross-Manufacturer** - Same API works with all supported boards

</div>

</details>

<details>
<summary><strong>💡 ProLighting API</strong></summary>

<div style="margin-left: 20px;">
**Module:** `ProLighting`

Hardware-agnostic DMX lighting control via Art-Net (UDP) or USB DMX interfaces. Provides fixture management, fade engine, and RDM discovery.

**Example:**
```cpp
UProLightingController* LightingController = CreateDefaultSubobject<UProLightingController>(TEXT("LightingController"));
FLBEASTProLightingConfig Config;
Config.TransportType = ELBEASTDMXTransport::ArtNet;
Config.ArtNetIPAddress = TEXT("192.168.1.200");
Config.ArtNetPort = 6454;  // Art-Net default port
Config.ArtNetUniverse = 0;

LightingController->InitializeLighting(Config);

// Register a fixture
FLBEASTDMXFixture Fixture;
Fixture.FixtureType = ELBEASTDMXFixtureType::RGBW;
Fixture.DMXAddress = 1;
Fixture.Universe = 0;
int32 FixtureId = LightingController->RegisterFixture(Fixture);

// Control fixture intensity (0.0 to 1.0)
LightingController->SetFixtureIntensity(FixtureId, 0.75f);

// Set RGBW color
LightingController->SetFixtureColorRGBW(FixtureId, 1.0f, 0.5f, 0.0f, 0.0f);  // Orange

// Start a fade
LightingController->StartFixtureFade(FixtureId, 0.0f, 1.0f, 2.0f);  // Fade from 0 to 1 over 2 seconds
```

**Supported Transports:**
- ✅ Art-Net (UDP) - Full support with auto-discovery
- ✅ USB DMX - Stubbed (coming soon)

**Features:**
- ✅ **Fixture Registry** - Virtual fixture management by ID
- ✅ **Fade Engine** - Time-based intensity fades
- ✅ **RDM Discovery** - Automatic fixture discovery (stubbed)
- ✅ **Art-Net Discovery** - Auto-detect Art-Net nodes on network
- ✅ **Multiple Fixture Types** - Dimmable, RGB, RGBW, Moving Head, Custom

</div>

</details>

<details>
<summary><strong>🎤 VOIP API</strong></summary>

<div style="margin-left: 20px;">
**Module:** `VOIP`

Low-latency voice communication with 3D HRTF spatialization using Mumble protocol and Steam Audio.

**Basic Example:**
```cpp
UVOIPManager* VOIPManager = CreateDefaultSubobject<UVOIPManager>(TEXT("VOIPManager"));
VOIPManager->ServerIP = TEXT("192.168.1.100");
VOIPManager->ServerPort = 64738;  // Mumble default port
VOIPManager->bAutoConnect = true;
VOIPManager->PlayerName = TEXT("Player_1");

// Connect to Mumble server
VOIPManager->Connect();

// Mute/unmute microphone
VOIPManager->SetMicrophoneMuted(false);

// Set output volume (0.0 to 1.0)
VOIPManager->SetOutputVolume(0.8f);

// Listen to connection events
VOIPManager->OnConnectionStateChanged.AddDynamic(this, &AMyActor::OnVOIPConnectionChanged);
```

**Custom Audio Processing (Visitor Pattern):**

If your experience genre template needs to process player voice (speech recognition, voice commands, audio analysis, etc.), use the **visitor interface pattern** to subscribe to audio events without coupling your module to VOIP:

```cpp
// 1. Create a component that implements IVOIPAudioVisitor
class MYEXPERIENCE_API UMyAudioProcessor : public UActorComponent, public IVOIPAudioVisitor
{
    GENERATED_BODY()

public:
    virtual void OnPlayerAudioReceived(int32 PlayerId, const TArray<float>& AudioData, 
                                       int32 SampleRate, const FVector& Position) override
    {
        // Process audio for your custom use case
        // AudioData is PCM (decoded from Opus), SampleRate is typically 48000
        ProcessVoiceCommand(AudioData, SampleRate);
    }
};

// 2. In your experience's InitializeExperienceImpl(), register as visitor:
void AMyExperience::InitializeExperienceImpl()
{
    // ... other initialization ...
    
    if (UVOIPManager* VOIPManager = FindComponentByClass<UVOIPManager>())
    {
        if (UMyAudioProcessor* AudioProcessor = FindComponentByClass<UMyAudioProcessor>())
        {
            VOIPManager->RegisterAudioVisitor(AudioProcessor);
        }
    }
}
```

**Why Use the Visitor Pattern?**

- ✅ **Decoupled Architecture** - VOIP module doesn't know about your experience
- ✅ **Multiple Visitors** - Multiple components can subscribe to the same audio stream
- ✅ **Clean Separation** - Your experience code stays in your experience module
- ✅ **Reusable Pattern** - Same approach works for any experience genre template

**Real-World Example:**

`AIFacemaskExperience` uses this pattern for speech recognition:
- `UAIFacemaskASRManager` implements `IVOIPAudioVisitor`
- Receives player audio → Converts to text → Triggers AI improv responses
- All AIFacemask code stays in the AIFacemask module, VOIP module remains decoupled

**Features:**
- ✅ **Mumble Protocol** - Low-latency VOIP (< 50ms on LAN)
- ✅ **Steam Audio** - 3D HRTF spatialization for positional audio
- ✅ **Per-User Audio Sources** - Automatic spatialization for each remote player
- ✅ **HMD-Agnostic** - Works with any HMD's microphone and headphones
- ✅ **Blueprint-Friendly** - Easy integration via ActorComponent
- ✅ **Visitor Pattern** - Subscribe to audio events without module coupling

**Prerequisites:**
- Murmur server running on LAN
- Steam Audio plugin (git submodule)
- MumbleLink plugin (git submodule)

</div>

</details>

## 📦 Installation

### Prerequisites

- **Unreal Engine 5.5.4** or later
- **Visual Studio 2022** (Windows)
- **SteamVR** (for VR features)
- **OpenXR Runtime** (Meta Quest, Windows Mixed Reality, etc.)

### Installation Methods

> **✅ Simple Installation:** The LBEAST repository root **is** the plugin folder. You can clone directly into your project's `Plugins/` directory - no need to extract subfolders or copy files manually.

<details>
<summary><strong>Option 1: Git Clone (Recommended - Simple One-Command Install)</strong></summary>

<div style="margin-left: 20px;">

The LBEAST repository root **is** the plugin folder. Simply clone directly into your project's `Plugins/` directory:

```bash
# From your Unreal project root
cd Plugins
git clone https://github.com/ajcampbell1333/lbeast_unreal.git LBEAST
```

**Windows PowerShell:**
```powershell
# From your Unreal project root
cd Plugins
git clone https://github.com/ajcampbell1333/lbeast_unreal.git LBEAST
```

That's it! The plugin is ready to use.

**Next steps:**
1. Regenerate Visual Studio project files (right-click `.uproject` → Generate Visual Studio project files)
2. Build your project
3. Enable the plugin in Unreal Editor: **Edit → Plugins → LBEAST**

**To update the plugin later:**
```bash
cd Plugins/LBEAST
git pull
```

</div>

</details>

<details>
<summary><strong>Option 2: Git Submodule (Recommended for Git-based Projects)</strong></summary>

<div style="margin-left: 20px;">

If your project uses Git, add LBEAST as a submodule:

```bash
# From your Unreal project root
cd Plugins
git submodule add https://github.com/ajcampbell1333/lbeast_unreal.git LBEAST
```

**Windows PowerShell:**
```powershell
# From your Unreal project root
cd Plugins
git submodule add https://github.com/ajcampbell1333/lbeast_unreal.git LBEAST
```

Then:
1. Regenerate Visual Studio project files
2. Build your project
3. Enable the plugin in Unreal Editor: **Edit → Plugins → LBEAST**

**To update the plugin later:**
```bash
cd Plugins/LBEAST
git submodule update --remote
```

**To clone a project that uses LBEAST as a submodule:**
```bash
git clone --recursive https://github.com/yourusername/yourproject.git
```

</div>

</details>

<details>
<summary><strong>Option 3: Download ZIP (Manual Installation)</strong></summary>

<div style="margin-left: 20px;">

1. **Download** the repository as a ZIP from GitHub
2. **Extract** the ZIP to a temporary location
3. **Copy the entire extracted folder** to your project's `Plugins/` directory and rename it to `LBEAST`:
   ```
   YourProject/
   └── Plugins/
       └── LBEAST/          ← Copy entire extracted folder here
           ├── LBEAST.uplugin
           ├── Source/
           ├── Content/
           └── ...
   ```

4. Regenerate Visual Studio project files (right-click `.uproject` → Generate Visual Studio project files)
5. Build your project
6. Enable the plugin in Unreal Editor: **Edit → Plugins → LBEAST**

</div>

</details>

> **📌 Note:** Unlike Unity's Package Manager, Unreal Engine doesn't have native "add plugin from Git URL" functionality in the editor. The methods above provide Git-based distribution alternatives. Git submodules are the most common approach for version-controlled plugin distribution.

## Quick Start

<details>
<summary><strong>Option A: Use Experience Genre Templates (Recommended for Rapid Deployment)</strong></summary>

<div style="margin-left: 20px;">

1. **Enable the plugin** in Unreal Editor: Edit → Plugins → LBEAST
2. **Restart the editor**
3. **Drag an experience genre template** into your level from the Content Browser:
   - Search for "LBEAST" in the C++ Classes folder
   - Find `AAIFacemaskExperience`, `AMovingPlatformExperience`, etc.
4. **Configure in Details panel** and hit Play!

</div>

</details>

<details>
<summary><strong>Option B: Use Low-Level APIs (For Custom Implementations)</strong></summary>

<div style="margin-left: 20px;">

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
- **Pro Audio Controller**

All components are fully Blueprint-compatible with exposed properties and functions.

</div>

</details>

## Architecture

### Module Structure

```
LBEAST/
├── LBEASTCore          # Core systems, HMD/tracking abstraction
├── AIFacemask          # AI facial animation API
├── LargeHaptics        # Hydraulic platform & gyroscope control API
├── EmbeddedSystems     # Microcontroller integration API
├── ProAudio            # Professional audio console control via OSC
├── ProLighting         # DMX lighting control (Art-Net, USB DMX)
├── VOIP                # Low-latency voice communication with 3D HRTF
└── LBEASTExperiences   # Pre-configured experience genre templates
    ├── AIFacemaskExperience
    ├── MovingPlatformExperience
    ├── GunshipExperience
    ├── CarSimExperience
    ├── FlightSimExperience
    └── EscapeRoomExperience
```

### Networking

LBEAST v0.1.0 focuses on **local LAN multiplayer** using Unreal's built-in replication and Gameplay Ability System (GAS):

- **Listen Server** (one player acts as host) or **Dedicated Server** (headless PC for monitoring)
- No web-hosting or online matchmaking in v0.1.0
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

> **Terminology Note:** 
> - **"Command Console"** (operations terminology) = The UI Panel (admin interface) used by Operations Technicians to monitor and control the experience
> - **"Server Manager"** (developer terminology) = The dedicated server backend that handles network traffic, decision-making, graphics processing offloaded from VR harnesses, and other computational tasks
>
> These are **separate components** that **may** run on the same CPU/PC, or may be networked on separate machines in close proximity.

The AIFacemask experience (and optionally other multiplayer experiences) uses a **dedicated server architecture** to offload AI processing and enable robust multi-player experiences.

### Architecture Overview

```
┌─────────────────────────────────────────────────┐
│  LBEAST Server Manager PC (Dedicated Server)    │
│  ─────────────────────────────────────────────  │
│  • Handles all network traffic                  │
│  • Decision-making & game state logic           │
│  • Graphics processing offloaded from VR        │
│  • AI workflow (Speech → NLU → Emotion →        │
│    Audio2Face)                                  │
│  • Streams facial animation to HMDs             │
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

┌─────────────────────────────────────────────────┐
│  Command Console PC (Optional - May be same)    │
│  ─────────────────────────────────────────────  │
│  • Server Manager GUI (UMG interface)           │
│  • Admin Panel for Ops Tech monitoring          │
│  • Experience control interface                 │
└─────────────────────────────────────────────────┘
         │ (May share same CPU/PC as Server Manager)
         │ OR networked separately
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

The **Command Console** (the admin UI Panel) is a UMG-based application for managing dedicated servers with a graphical interface. This provides the operations interface to monitor and control the **Server Manager** (dedicated server backend) which handles network traffic, decision-making, and graphics processing.

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
│                                        │
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

### **Remote Monitoring & Off-Site Access**

The Command Console supports remote connection to Server Managers over the network, enabling off-site monitoring and control.

#### **Local Network (LAN)**

- ✅ **Auto-Discovery:** Server Beacon automatically discovers servers on the local network (UDP broadcast on port 7778)
- ✅ **Command Protocol:** Direct UDP connection on port 7779 for remote control
- ✅ **Real-Time Status:** Status updates via Server Beacon broadcasts

#### **Internet/Off-Site Access**

The Command Protocol (UDP port 7779) **can work over the internet** with proper network configuration:

**What Works:**
- ✅ Command Protocol connects directly via UDP (not broadcast)
- ✅ Can send Start/Stop commands to remote servers
- ✅ Can request status via `RequestStatus` command
- ✅ Manual IP entry supported for known server addresses

**What Doesn't Work:**
- ❌ **Auto-Discovery:** Server Beacon (UDP broadcast) is LAN-only - routers don't forward broadcasts
- ❌ **Real-Time Status:** Server Beacon status updates won't work over internet
- ⚠️ **Workaround:** Use `RequestStatus` command for periodic status polling

**Security Considerations:**
- ⚠️ **Authentication:** Enable authentication for off-site connections (shared secret in Command Console settings)
- ⚠️ **Firewall:** Must open UDP port 7779 inbound on server firewall
- ⚠️ **Production:** For public operation, use VPN or full internet isolation
- ⚠️ **Debugging Only:** Off-site monitoring recommended for debugging/testing only

**Recommended Setup for Off-Site:**
1. **VPN Connection:** Connect via VPN between Command Console and Server Manager
2. **Manual IP Entry:** Enter server IP address manually (no auto-discovery over internet)
3. **Enable Authentication:** Configure shared secret in both Command Console and Server Manager
4. **Status Polling:** Use `RequestStatus` command for periodic status updates
5. **For Production:** Use full internet isolation - off-site monitoring is for debugging only

**Network Requirements:**
- Server must have public IP or port-forwarded private IP
- Firewall must allow UDP port 7779 inbound
- NAT traversal may require port forwarding or UPnP configuration

**Port Usage:**
- **7777:** Game server port (Unreal networking)
- **7778:** Server Beacon port (UDP discovery broadcasts - LAN only)
- **7779:** Command Protocol port (UDP command/control - works over internet)

---

## Roadmap

### v0.1.0 (Current)
- ✅ Core module architecture
- ✅ HMD abstraction (OpenXR, SteamVR, Meta)
- ✅ Tracking abstraction (SteamVR Trackers)
- ✅ AI Face API
- ✅ Large Haptics API (4DOF platforms + 2DOF gyroscope)
- ✅ HOTAS integration framework (Logitech X56, Thrustmaster T.Flight)
- ✅ Embedded Systems API
- ✅ **Experience Genre Templates** (AIFacemask, MovingPlatform, Gunship, CarSim, FlightSim, EscapeRoom)
- ✅ **Dedicated Server** architecture
- ✅ **Server Manager GUI** (UMG-based)
- ✅ **Automatic Server Discovery** (UDP broadcast)
- ✅ **NVIDIA ACE Integration Architecture** (data structures, visitor pattern, component architecture)

### v1.0 (Planned)
- [ ] **Finishing AIFacemask functionality** - Complete all NOOP implementations for NVIDIA ACE service integration (ASR, TTS, Audio2Face, LLM, script pre-baking)
- [ ] **Implementing HOTAS integration** - Complete HOTAS controller support with full input mapping and calibration
- [ ] **Adding Weight & Height Safety Check Embedded Firmware** - Safety firmware for motion platforms to prevent operation if weight/height limits are exceeded
- [ ] **Go-Kart Experience** - Electric go-karts, bumper cars, race boats, or bumper boats augmented by passthrough VR or AR headsets enabling overlaid virtual weapons and pickups that affect the performance of the vehicles

### v1.1 (Future)
- Real-time AI facial animation integration
- **Blueprint-only experience genre templates** (no C++ required)
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

### General Tasks

- [ ] **Pro Audio UMG Templates for Command Console** - Create drag-and-drop UMG widget templates (channel faders, mute buttons, bus sends) that auto-map to physical mixer channels on Behringer X32/M32/Wing, Yamaha QL/CL/TF, and other OSC-enabled consoles. Templates will use ProAudioController's bidirectional sync delegates to stay synchronized with physical console state.
- [ ] **Design the Default Server Manager UI** - Create polished UMG interface for server management
- [ ] **Example Maps** - Create demonstration maps for each experience genre template
- [ ] **Blueprint Templates** - Create Blueprint-only versions of experience genre templates
- [ ] **Hardware Calibration Tool** - Build utility for calibrating hydraulic platforms

### AI Facemask Experience - Service Integration (NOOP)

The AI Facemask system architecture is complete with all data structures, component integration, and visitor patterns in place. The following service integrations are marked as NOOP (Not Operationally Programmed) and need implementation:

#### **High Priority (Core Functionality)**

- [ ] **AIFaceController** - Receive facial animation data from NVIDIA ACE endpoint (HTTP/WebSocket client)
- [ ] **AIFaceController** - Apply blend shape weights to skeletal mesh morph targets
- [ ] **AIFaceController** - Apply facial texture to mesh material
- [ ] **ACE Script Manager** - Request script playback from NVIDIA ACE server (HTTP POST)
- [ ] **ACE Script Manager** - Request script pre-baking from NVIDIA ACE server (TTS → Audio, Audio → Facial data)
- [ ] **ACE Script Manager** - Async pre-baking support (background processing)
- [ ] **ACE ASR Manager** - Request ASR transcription from local ASR service (gRPC/HTTP to NVIDIA Riva ASR)
- [ ] **ACE ASR Manager** - Trigger improv after transcription (wire to ACEImprovManager)
- [ ] **ACE Improv Manager** - Request LLM response from local LLM (HTTP to Ollama/vLLM/NVIDIA NIM)
- [ ] **ACE Improv Manager** - Request TTS conversion from local TTS (gRPC to NVIDIA Riva TTS)
- [ ] **ACE Improv Manager** - Request Audio2Face conversion from local Audio2Face (HTTP/gRPC to NVIDIA NIM)
- [ ] **ACE Improv Manager** - Auto-trigger Audio2Face after TTS completes (callback chain)
- [ ] **ACE Improv Manager** - Monitor async response generation status (track LLM/TTS/Audio2Face operations)
- [ ] **AIFacemaskExperience** - Configure NVIDIA ACE endpoint URL (load from project settings/config)
- [ ] **AIFacemaskExperience** - Register ASR Manager as visitor with VOIPManager (wire visitor pattern)
- [ ] **AIFacemaskExperience** - Configure NVIDIA ACE server base URL (load from project settings/config)

#### **Medium Priority (Infrastructure)**

- [ ] **VOIPManager** - Decode Opus to PCM for visitors (decode Mumble Opus before passing to visitors)
- [ ] **VOIPManager** - Integrate with player replication system (get remote player positions)
- [ ] **Server Beacon** - Get server port from project settings (load port configuration)
- [ ] **Server Beacon** - Track actual player count (query Unreal networking)

#### **Low Priority (Optimization)**

- [ ] **AIFaceController** - Optimize blend shape application (batch updates, interpolation, caching)
- [ ] **AIFaceController** - Texture streaming optimization (efficient texture updates, compression)

**Implementation Notes:**
- All NOOP functions have detailed comments explaining expected API formats
- Backend-agnostic design supports multiple service providers (Ollama, vLLM, NVIDIA NIM, etc.)
- Visitor pattern keeps modules decoupled - VOIP doesn't know about AIFacemask
- All data structures are complete and ready for service integration

## Support

- **Issues:** [github.com/ajcampbell1333/lbeast_unreal/issues](https://github.com/ajcampbell1333/lbeast_unreal/issues)
- **Discussions:** [github.com/ajcampbell1333/lbeast_unreal/discussions](https://github.com/ajcampbell1333/lbeast_unreal/discussions)
- **Unity Version:** [github.com/ajcampbell1333/lbeast_unity](https://github.com/ajcampbell1333/lbeast_unity)

## License

Copyright (c) 2025 AJ Campbell

Licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Credits

Created by **AJ Campbell** with experience from dozens of LBE projects deployed at major entertainment events worldwide.

---

*LBEAST: Making location-based entertainment development accessible to everyone.*


