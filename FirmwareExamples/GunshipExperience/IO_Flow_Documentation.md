# Gunship Experience - Complete IO Flow Documentation

## Overview

This document maps all communication paths in the Gunship Experience system, from Gun ECUs through the Gunship ECU to the Game Engine (GunshipExperience), and finally to Console/VR Players.

## Communication Architecture

```
┌─────────────┐         ┌──────────────┐         ┌─────────────────┐         ┌──────────────┐
│  Gun ECU    │────────▶│ Gunship ECU  │────────▶│ Game Engine     │────────▶│ VR Players   │
│  (4×)       │         │  (Primary)   │         │ (GunshipExp)    │         │ Console      │
└─────────────┘         └──────────────┘         └─────────────────┘         └──────────────┘
     │                         │                           │                          │
     │                         │                           │                          │
     └─────────────────────────┴───────────────────────────┴──────────────────────────┘
                    (All on same LAN, wired/wireless)
```

## Complete IO Flow Map

### 1. Gun ECU → Gunship ECU (Telemetry)

**Status:** ✅ **IMPLEMENTED**

**Connection:**
- **Protocol:** LBEAST UDP binary protocol
- **Port:** Gunship ECU listens on port 8892
- **Transport:** Wired Ethernet (recommended) or WiFi
- **Topology:** Star (4× Gun ECUs → 1× Gunship ECU)

**Data Sent (20 Hz, configurable):**
- **Ch 10+n:** Button states (combined Button0 || Button1)
- **Ch 20+n:** Fire intensity (float, 0.0-1.0)
- **Ch 30+n:** Active solenoid temperature (float, °C)
- **Ch 40+n:** Active solenoid ID (int32, if redundant)
- **Ch 50+n:** Total solenoid count (int32, if redundant)
- **Ch 60-67:** Individual solenoid temperatures (float, 2 per station)
- **Ch 70+n:** Driver module temperature (float, °C)
- **Ch 80+n:** Fire command active (bool)
- **Ch 90+n:** Thermal shutdown (bool)
- **Ch 95+n:** PWM throttle (float, 0.5-1.0)

**Gunship ECU Processing:**
- Parses all incoming telemetry
- Aggregates into `FGunECUState[4]` structs
- Tracks station connection status (2-second timeout)
- Computes `CanFire` = PlaySessionActive && !ThermalShutdown

---

### 2. Gunship ECU → Gun ECU (Commands & Game State)

**Status:** ✅ **IMPLEMENTED**

**Connection:**
- **Protocol:** LBEAST UDP binary protocol
- **Port:** Gun ECUs listen on ports 8888-8891 (one per station)
- **Transport:** Wired Ethernet (recommended) or WiFi

**Data Sent:**
- **Ch 9:** Play session active (bool, 10 Hz)
  - Controls gun firing authorization
  - Guns cannot fire unless `playSessionActive == true`
- **Ch 7:** Emergency stop (bool, on-demand)
  - Global E-stop for all systems

**Future (Not Yet Implemented):**
- **Ch 10+n:** Fire commands (bool, from game engine)
  - Currently: Game engine → Gunship ECU → Gun ECU (needs implementation)
  - Or: Game engine → Gun ECU directly (alternative architecture)

---

### 3. Server → Gunship ECU (Motion Commands & Control)

**Status:** ✅ **IMPLEMENTED**

**Connection:**
- **Protocol:** LBEAST UDP binary protocol
- **Port:** Gunship ECU listens on port 8888
- **Transport:** WiFi/UDP (wireless)

**Data Sent:**
- **Ch 0:** Pitch (float, degrees or normalized -1.0 to +1.0)
- **Ch 1:** Roll (float, degrees or normalized -1.0 to +1.0)
- **Ch 2:** TranslationY / Forward-Reverse (float, cm or normalized)
- **Ch 3:** TranslationZ / Vertical (float, cm or normalized)
- **Ch 4:** Duration (float, seconds)
- **Ch 5:** Calibration mode (bool)
- **Ch 6:** Operation mode (bool: auto-calibrate/fixed)
- **Ch 7:** Emergency stop (bool)
- **Ch 8:** Return to neutral (bool)
- **Ch 9:** Play session active (bool) ✅ **IMPLEMENTED**
- **Ch 100:** Button event update interval (int32, ms) ✅ **IMPLEMENTED**
- **Ch 101:** Telemetry update interval (int32, ms) ✅ **IMPLEMENTED**

**Struct Packets (More Efficient):**
- **Ch 100:** `FTiltState` struct (pitch and roll only)
- **Ch 101:** `FScissorLiftState` struct (Y and Z translations only)
- **Ch 200:** `FPlatformMotionCommand` struct (full command: pitch, roll, Y, Z, duration)

**Implementation:**
- `AGunshipExperience::SendGunshipTilt()` - sends normalized motion
- `AGunshipExperience::SendGunshipMotion()` - sends absolute motion
- `U4DOFPlatformController::SendTiltStruct()` - sends tilt struct
- `U4DOFPlatformController::SendScissorLiftStruct()` - sends lift struct

---

### 4. Gunship ECU → Server (Telemetry & Feedback)

**Status:** ✅ **IMPLEMENTED** (Firmware sends, Engine-side parsing complete)

**Connection:**
- **Protocol:** LBEAST UDP binary protocol
- **Port:** Server listens on port 8888 (same as send port, bidirectional)
- **Transport:** WiFi/UDP (wireless)

**Data Sent:**

**Position Feedback (10 Hz):**
- **Ch 100:** `FTiltState` struct (pitch and roll feedback)
- **Ch 101:** `FScissorLiftState` struct (Y and Z translation feedback)

**Gun Telemetry (Configurable, default 1 Hz):**
- **Ch 310:** `FGunButtonEvents` struct (all 4 stations, fast updates)
  - Contains: `Button0State[4]`, `Button1State[4]`, `Timestamp`
  - Update rate: Configurable via Ch 100 (default 50ms = 20 Hz)
  - **Status:** ✅ Firmware sends, ✅ Engine-side parsing implemented

- **Ch 311:** `FGunTelemetry` struct (all 4 stations, slow updates)
  - Contains: Temperatures, solenoid state, fire state, system state
  - Update rate: Configurable via Ch 101 (default 1000ms = 1 Hz)
  - **Status:** ✅ Firmware sends, ✅ Engine-side parsing implemented

**Engine-Side Implementation Status:**
- ✅ `U4DOFPlatformController::GetTiltStateFeedback()` - receives Ch 100
- ✅ `U4DOFPlatformController::GetScissorLiftStateFeedback()` - receives Ch 101
- ✅ `U4DOFPlatformController::GetGunButtonEvents()` - receives Ch 310
- ✅ `U4DOFPlatformController::GetGunTelemetry()` - receives Ch 311
- ✅ Struct definitions for `FGunButtonEvents` and `FGunTelemetry` in `Models/` folder

---

### 5. Server → Console (Telemetry Display)

**Status:** ⚠️ **PARTIALLY IMPLEMENTED** (Struct parsing complete, UMG display NOOP)

**Connection:**
- **Mode:** Dedicated local (console & server same build)
- **Protocol:** Internal (same process, shared memory/events)
- **Note:** Dedicated remote mode (separate builds) not supported in v1.0

**Required:**
- Parse `FGunTelemetry` struct from Ch 311 ✅ **IMPLEMENTED**
- Display on console monitor (UMG): ❌ **NOOP** (to be implemented)
  - Temperature data (all solenoids + driver modules)
  - Fire status (active/inactive, intensity, duration)
  - Safety status (can fire, thermal shutdown, connection)
  - Active solenoid ID (for redundancy systems)
  - Button states
  - System health (PWM throttle, connection status)

**Implementation:**
- **Command Console Integration:** Server Manager GUI (UMG-based)
- **Data Source:** `AGunshipExperience` or `U4DOFPlatformController::GetGunTelemetry()`
- **Display:** UMG widget template for real-time gun status monitoring

---

### 6. Server → VR Players (Button Events & Gun State)

**Status:** ❌ **NOT IMPLEMENTED** (Deferred to future commit)

**Required:**
- Parse `FGunButtonEvents` struct from Ch 310 ✅ **IMPLEMENTED**
- Relay button presses to VR players for gameplay ❌ **NOT IMPLEMENTED**
- Send gun state (firing, intensity) for visual/audio feedback ❌ **NOT IMPLEMENTED**
- Send gun transforms (from trackers) for rendering ❌ **NOT IMPLEMENTED**

**Note:** VR player transport implementation deferred to future commit. This includes both server-to-VR and VR-to-server communication paths.

---

### 7. VR Players → Server (Fire Commands)

**Status:** ❌ **NOT IMPLEMENTED** (Deferred to future commit)

**Current Flow (Not Yet Implemented):**
- VR player presses trigger → Server → Gunship ECU → Gun ECU → Solenoid fires

**Alternative Flow (Direct):**
- VR player presses trigger → Server → Gun ECU directly → Solenoid fires
- (Bypasses Gunship ECU for lower latency)

**Note:** VR player transport implementation deferred to future commit. This includes both server-to-VR and VR-to-server communication paths.

**Channel Mapping (When Implemented, Option A - Centralized):**
- Server → Gunship ECU: Ch 10+n (fire command for station n)
- Gunship ECU → Gun ECU: Ch 10+n (relay fire command)

---

## Summary: Implementation Status

| Connection | Direction | Status | Notes |
|------------|-----------|--------|-------|
| Gun ECU → Gunship ECU | Telemetry | ✅ Complete | All channels implemented, 20 Hz |
| Gunship ECU → Gun ECU | Game State | ✅ Complete | Play session control, E-stop |
| Server → Gunship ECU | Motion/Control | ✅ Complete | All motion commands, play session, rate control |
| Gunship ECU → Server | Position Feedback | ✅ Complete | Tilt and lift state feedback |
| Gunship ECU → Server | Button Events | ✅ Complete | Firmware sends (Ch 310), engine parsing implemented |
| Gunship ECU → Server | Gun Telemetry | ✅ Complete | Firmware sends (Ch 311), engine parsing implemented |
| Server → Console | Telemetry Display | ⚠️ Partial | Struct parsing complete, UMG display NOOP |
| Server → VR Players | Button Events | ❌ Deferred | Deferred to future commit |
| Server → VR Players | Gun State | ❌ Deferred | Deferred to future commit |
| VR Players → Server | Fire Commands | ❌ Deferred | Deferred to future commit |

---

## Next Steps (Priority Order)

### High Priority (Core Functionality)
1. ✅ **Add struct definitions to Unreal:** **COMPLETE**
   - `FGunButtonEvents` (Ch 310) - implemented in `Models/GunButtonEvents.h`
   - `FGunTelemetry` (Ch 311) - implemented in `Models/GunTelemetry.h`
   - Match firmware structs exactly for binary compatibility

2. ✅ **Implement engine-side parsing:** **COMPLETE**
   - Added handlers in `U4DOFPlatformController` for Ch 310 and Ch 311
   - `GetGunButtonEvents()` - parses Ch 310
   - `GetGunTelemetry()` - parses Ch 311
   - Available for use in `AGunshipExperience` or other game code

3. ❌ **Relay button events to VR players:** **DEFERRED**
   - Parse Ch 310 button events ✅ (parsing complete)
   - Replicate to VR clients via Unreal Replication or UDP multicast ❌ (deferred)
   - Trigger gameplay events (fire, reload, etc.) ❌ (deferred)

### Medium Priority (Console & Monitoring)
4. ⚠️ **Console monitor integration:** **PARTIAL**
   - Parse Ch 311 telemetry ✅ (parsing complete)
   - Display in Command Console / Server Manager GUI ❌ (UMG display NOOP)
   - Real-time status for all 4 guns ❌ (to be implemented)

5. **Gun transform tracking:**
   - Receive tracker poses from Gunship ECU (when implemented)
   - Replicate gun transforms to VR clients
   - Enable gun rendering in VR

### Low Priority (Optimization)
6. **Fire command routing:**
   - Implement Game Engine → Gunship ECU → Gun ECU path
   - Or: Direct Game Engine → Gun ECU path (architecture decision)
   - Add fire authorization logic

7. **Performance optimization:**
   - Event-driven button events (send only on change)
   - Adaptive telemetry rates based on network conditions
   - Compression for telemetry structs if bandwidth constrained

---

## Channel Reference Summary

### Game Engine → Gunship ECU
- **Ch 0-8:** Motion commands and control
- **Ch 9:** Play session active
- **Ch 100:** Button event update interval (int32, ms)
- **Ch 101:** Telemetry update interval (int32, ms)
- **Ch 100 (struct):** `FTiltState` (pitch/roll)
- **Ch 101 (struct):** `FScissorLiftState` (Y/Z translation)
- **Ch 200 (struct):** `FPlatformMotionCommand` (full command)

### Gunship ECU → Game Engine
- **Ch 100 (struct):** `FTiltState` feedback (10 Hz)
- **Ch 101 (struct):** `FScissorLiftState` feedback (10 Hz)
- **Ch 310 (struct):** `FGunButtonEvents` (configurable, default 20 Hz)
- **Ch 311 (struct):** `FGunTelemetry` (configurable, default 1 Hz)

### Gun ECU → Gunship ECU
- **Ch 10+n:** Button states
- **Ch 20+n:** Fire intensity
- **Ch 30+n:** Active solenoid temperature
- **Ch 40+n:** Active solenoid ID
- **Ch 50+n:** Total solenoid count
- **Ch 60-67:** Individual solenoid temperatures
- **Ch 70+n:** Driver module temperature
- **Ch 80+n:** Fire command active
- **Ch 90+n:** Thermal shutdown
- **Ch 95+n:** PWM throttle

### Gunship ECU → Gun ECU
- **Ch 9:** Play session active (10 Hz)
- **Ch 7:** Emergency stop (on-demand)

---

## Notes

- All communication uses LBEAST binary UDP protocol
- Structs must match exactly between firmware and engine (binary compatibility)
- Update rates are configurable server-side for bandwidth optimization
- Button events prioritized for low latency (20 Hz default)
- Telemetry optimized for bandwidth (1 Hz default)
- All 4 stations packed into single messages for efficiency

