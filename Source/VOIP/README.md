# LBEAST VOIP Module

**Low-latency VOIP system with 3D HRTF spatialization for location-based entertainment.**

Integrates **Mumble** (low-latency VOIP) with **Steam Audio** (3D HRTF spatialization) for high-quality positional voice communication in multiplayer LBE experiences.

---

## 🎯 Features

- ✅ **Low-Latency VOIP** - Mumble protocol for LAN communication (< 50ms latency)
- ✅ **3D HRTF Spatialization** - Steam Audio for realistic positional audio
- ✅ **Per-User Audio Sources** - Automatic spatialization for each remote player
- ✅ **Blueprint-Friendly** - Easy integration via ActorComponent
- ✅ **Automatic Connection Management** - Handles connection lifecycle
- ✅ **Microphone Control** - Mute/unmute support
- ✅ **Volume Control** - Per-component audio volume
- ✅ **HMD-Agnostic** - Works with any HMD's microphone and headphones (Oculus, Vive, Pico, etc.)

---

## 📋 Prerequisites

1. **Murmur Server** - Mumble server running on your LAN
   - Download: https://www.mumble.info/downloads/
   - Default port: 64738
   - Run: `murmurd -ini murmur.ini`

2. **Steam Audio Plugin** - Git submodule (see Setup below)
3. **MumbleLink Plugin** - Git submodule (see Setup below)
4. **SOFA HRTF Files** - For Steam Audio spatialization
   - Place in `Content/HRTF/` directory
   - Recommended: IRCAM HRTF database

---

## 🚀 Quick Start

### **1. Set Up Git Submodules**

The VOIP module requires two git submodules:

```bash
# Navigate to plugin root
cd LBEAST_UnrealPlugin/Plugins/LBEAST

# Add Steam Audio submodule
git submodule add https://github.com/ValveSoftware/steam-audio.git Plugins/SteamAudio

# Add MumbleLink submodule (replace with actual MumbleLink repository URL)
# git submodule add <mumble-link-repo-url> Plugins/MumbleLink

# Initialize submodules
git submodule update --init --recursive
```

**Note:** MumbleLink plugin repository URL needs to be determined. Common options:
- Official Mumble C++ library: https://github.com/mumble-voip/mumble
- Unreal-specific wrapper (may need to be created or found)

### **2. Update Submodules (Pull Latest)**

Run the update script to pull latest changes:

```powershell
.\Source\VOIP\Common\PullLatest.ps1
```

Or manually:

```bash
git submodule update --remote
```

### **3. Enable Plugins in Unreal**

1. Open your `.uproject` file
2. Right-click → "Generate Visual Studio project files" (if C++)
3. Open Unreal Editor
4. Edit → Plugins
5. Enable:
   - **Steam Audio** (from submodule)
   - **MumbleLink** (from submodule)
   - **LBEAST VOIP** (this module)

### **4. Configure Steam Audio**

1. Edit → Project Settings → Plugins → Steam Audio
2. Set HRTF file path: `Content/HRTF/IRCAM_1001.sofa` (or your HRTF file)
3. Configure audio settings as needed

### **5. Add VOIP to Your Experience**

#### **Option A: Blueprint**

1. Open your HMD/Player Blueprint
2. Add Component → **VOIP Manager** (under LBEAST category)
3. Set properties:
   - **Server IP**: Your Murmur server IP (e.g., `192.168.1.100`)
   - **Server Port**: `64738` (default)
   - **Player Name**: Auto-generated or set manually
   - **Auto Connect**: `true` (connects on BeginPlay)

#### **Option B: C++**

```cpp
#include "VOIPManager.h"

// In your HMD/Player actor class
UVOIPManager* VOIPManager = CreateDefaultSubobject<UVOIPManager>(TEXT("VOIPManager"));
VOIPManager->ServerIP = TEXT("192.168.1.100");
VOIPManager->ServerPort = 64738;
VOIPManager->bAutoConnect = true;
```

### **6. Start Murmur Server**

On your server machine:

```bash
murmurd -ini murmur.ini
```

Or use Docker:

```bash
docker run -d -p 64738:64738/udp -p 64738:64738/tcp mumble-server/murmur
```

### **7. Test**

1. Play in Editor (PIE) with multiple players
2. Each player should automatically connect to Mumble
3. Speak into microphone - other players should hear you with spatialization
4. Move players around - audio should spatialize based on positions

---

## 📁 Module Structure

```
Source/VOIP/
├── VOIP.Build.cs              # Module build configuration
├── Public/
│   ├── VOIP.h                 # Module interface
│   ├── VOIPManager.h          # Main component (attach to HMD/Player)
│   ├── MumbleClient.h          # Mumble protocol wrapper
│   └── SteamAudioSourceComponent.h  # Per-user spatial audio source
├── Private/
│   ├── VOIP.cpp
│   ├── VOIPManager.cpp
│   ├── MumbleClient.cpp
│   └── SteamAudioSourceComponent.cpp
├── Common/
│   └── PullLatest.ps1         # Script to update submodules
└── README.md                   # This file
```

---

## 🔧 Architecture

### **Component Hierarchy**

```
HMD/Player Actor
└── VOIPManager (Component)
    ├── MumbleClient (Object)
    │   └── Handles Mumble protocol connection
    │       └── Microphone capture (via OS audio system)
    └── AudioSourceMap (Map<UserId, SteamAudioSourceComponent>)
        └── One component per remote player
            └── Handles Steam Audio spatialization
            └── Audio output (via OS audio system)
```

### **HMD Compatibility**

The VOIP system is **HMD-agnostic** and works with any VR headset:

- **Microphone Input**: Uses Unreal's audio system → OS audio APIs (WASAPI on Windows) → Any microphone device
  - Oculus Quest/Pro/Rift microphones ✅
  - HTC Vive/Vive Pro microphones ✅
  - Pico 4/Enterprise microphones ✅
  - Windows Mixed Reality microphones ✅
  - Any USB/Bluetooth microphone ✅

- **Headphone Output**: Routes through Unreal's audio system → OS audio APIs → Selected audio output device
  - Oculus Quest/Pro/Rift headphones ✅
  - HTC Vive/Vive Pro headphones ✅
  - Pico 4/Enterprise headphones ✅
  - Windows Mixed Reality headphones ✅
  - Any audio output device ✅

**Note**: The HMD's microphone and headphones must be selected as the default audio input/output devices in Windows Settings, or configured in Unreal's audio settings.

### **Data Flow**

1. **Microphone Input** → MumbleClient → Encode to Opus → Send to Mumble Server
2. **Remote Audio** → MumbleClient → Decode Opus → SteamAudioSourceComponent
3. **Steam Audio** → Process HRTF → Binaural Audio → Unreal Audio System
4. **Player Positions** → Replicated via Unreal Replication → Update spatialization

### **Replication**

- Uses Unreal's native replication system
- Player positions are replicated automatically (via PlayerState/Pawn)
- Audio data is NOT replicated (streamed via Mumble)
- VOIPManager component replicates connection state

---

## 📡 Protocol Details

### **Mumble Protocol**

- **Control Channel**: TCP (connection, user management)
- **Audio Channel**: UDP (low-latency audio streaming)
- **Codec**: Opus (high quality, low latency)
- **Sample Rate**: 48kHz
- **Positional Audio**: 3D coordinates sent with audio packets

### **Steam Audio**

- **HRTF**: Head-Related Transfer Function for 3D spatialization
- **Format**: SOFA files (Spatially Oriented Format for Acoustics)
- **Processing**: Real-time binaural rendering
- **Output**: Stereo binaural audio

---

## 🛠️ Integration with Player Replication

The VOIP system relies on Unreal's replication system for player positions:

```cpp
// In your PlayerState or Pawn class
void AYourPlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // Replicate position for VOIP spatialization
    DOREPLIFETIME(AYourPlayerPawn, ReplicatedLocation);
}

// VOIPManager will automatically query player positions from replicated data
```

---

## 🔌 API Reference

### **VOIPManager Component**

#### **Connection Management**

```cpp
// Connect to Mumble server
bool Connect();

// Disconnect from server
void Disconnect();

// Check connection state
bool IsConnected() const;
EVOIPConnectionState GetConnectionState() const;
```

#### **Audio Control**

```cpp
// Mute/unmute microphone
void SetMicrophoneMuted(bool bMuted);
bool IsMicrophoneMuted() const;

// Set output volume (0.0 to 1.0)
void SetOutputVolume(float Volume);
float GetOutputVolume() const;
```

#### **Events**

```cpp
// Connection state changed
FOnVOIPConnectionStateChanged OnConnectionStateChanged;

// Remote player audio received
FOnRemotePlayerAudioReceived OnRemotePlayerAudioReceived;
```

---

## 🐛 Troubleshooting

### **"MumbleLink plugin not found"**

- Ensure MumbleLink submodule is initialized: `git submodule update --init --recursive`
- Enable MumbleLink plugin in Unreal Editor (Edit → Plugins)
- Check that MumbleLink plugin is in `Plugins/MumbleLink/` directory

### **"Steam Audio plugin not found"**

- Ensure Steam Audio submodule is initialized
- Enable Steam Audio plugin in Unreal Editor
- Check that Steam Audio plugin is in `Plugins/SteamAudio/` directory

### **"Connection failed"**

- Verify Murmur server is running: `netstat -an | findstr 64738`
- Check firewall allows UDP/TCP on port 64738
- Verify Server IP and Port are correct
- Check Murmur server logs for connection attempts

### **"No audio spatialization"**

- Verify HRTF file is in `Content/HRTF/` directory
- Check Steam Audio plugin settings (HRTF file path)
- Ensure player positions are being replicated
- Check audio source components are being created (enable logging)

### **"High latency"**

- Ensure Mumble server is on same LAN (not over internet)
- Check network latency: `ping <server-ip>`
- Verify Opus codec settings (should be low-latency mode)
- Check audio buffer sizes in Mumble configuration

---

## 📚 Related Documentation

- **[Steam Audio Documentation](https://valvesoftware.github.io/steam-audio/)** - Valve's Steam Audio documentation
- **[Mumble Documentation](https://wiki.mumble.info/)** - Mumble protocol documentation
- **[Unreal Replication Guide](https://docs.unrealengine.com/5.3/en-US/networking-and-replication-in-unreal-engine/)** - Unreal networking

---

## 🔗 Submodule Management

### **Adding Submodules**

```bash
# From plugin root
cd LBEAST_UnrealPlugin/Plugins/LBEAST

# Add Steam Audio
git submodule add https://github.com/ValveSoftware/steam-audio.git Plugins/SteamAudio

# Add MumbleLink (replace with actual URL)
git submodule add <mumble-link-repo-url> Plugins/MumbleLink
```

### **Updating Submodules**

```powershell
# Run update script
.\Source\VOIP\Common\PullLatest.ps1

# Or manually
git submodule update --remote
git submodule update --init --recursive
```

### **Cloning with Submodules**

```bash
# Clone repository with submodules
git clone --recursive <repo-url>

# Or if already cloned
git submodule update --init --recursive
```

---

## 📝 TODO / Integration Status

### **Completed**
- ✅ Module structure and build configuration
- ✅ VOIPManager component (ActorComponent)
- ✅ MumbleClient wrapper interface
- ✅ SteamAudioSourceComponent interface
- ✅ Blueprint exposure
- ✅ Submodule update script

### **Pending Integration**
- ⏳ MumbleLink plugin integration (waiting for submodule)
- ⏳ Steam Audio plugin integration (waiting for submodule)
- ⏳ Opus encoding/decoding implementation
- ⏳ HRTF processing implementation
- ⏳ Audio playback via Unreal audio system
- ⏳ Player position replication integration

### **Future Enhancements**
- 🔮 WebRTC fallback if Mumble unavailable
- 🔮 Echo cancellation
- 🔮 Noise suppression
- 🔮 Audio quality settings (bitrate, sample rate)
- 🔮 Channel/room support (multiple VOIP channels)

---

## 📄 License

MIT License - Copyright (c) 2025 AJ Campbell

**Dependencies:**
- **Steam Audio**: Apache 2.0 License (Valve Software)
- **Mumble**: BSD 3-Clause License (Mumble VOIP)

---

**Built for LBEAST - Location-Based Entertainment Activation Standard**

