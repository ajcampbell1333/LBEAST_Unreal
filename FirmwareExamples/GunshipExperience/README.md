# Gunship Experience Firmware Examples

**Embedded firmware examples for GunshipExperience motion platform control.**

These examples demonstrate how to control a complete 4DOF motion platform system consisting of:
- **Scissor Lift:** Electric lift for vertical translation (TranslationZ) and forward/reverse (TranslationY)
- **Actuator System:** 4-gang hydraulic actuators for pitch and roll only (yaw restricted)

**Gunship Rig Chassis:** The "Gunship Rig chassis" referenced throughout this document is a **modified scissor lift** with the railed ground removed and replaced by a **custom actuating tilt frame** with 4 seats attached at its corners. The scissor lift provides vertical translation and forward/reverse movement, while the custom tilt frame (driven by 4 hydraulic actuators) provides pitch and roll motion for the seated players.

---

## 🎯 Recommended Default Hardware Configuration

**The recommended default hardware deployment for GunshipExperience is:**

- **1× LBUS (LBEAST Universal Shield)** - Parent ECU mounted on the motion platform chassis
  - Hosts ESP32-S3 motherboard ECU
  - Controls scissor lift and actuator system directly
  - Aggregates telemetry from 4× child ECUs
  - Connects to game server via wireless network adapter
  
- **4× Child Shields (LBCS)** - One per player station, embedded in each player's gun
  - Each hosts its own ESP32-S3 child ECU
  - Controls dual thumb buttons and N× solenoid kickers
  - Monitors solenoid temperatures and thermal management
  - Reports telemetry to parent ECU at 10-30 Hz
  
- **Network Architecture:**
  - **Wired Ethernet (PoE):** 4× Child Shields connect to LBUS via Ethernet cables routed through the Gunship Rig chassis
  - LBUS provides 5V power to all 4× Child Shields via PoE (pins 4&5) with DIP switch control per port
  - **Wireless:** LBUS connects to game server's router via wireless network adapter (USB WiFi dongle or built-in ESP32 WiFi)
  
- **Power Architecture:**
  - **DC Power:** All 5 units (1× LBUS + 4× Child Shields) powered by the Rig's onboard 24V LiFePO4 battery pack
  - LBUS receives 24V via barrel jack, converts to 5V for aux ports and 3.3V for logic
  - Child Shields receive 5V via PoE from LBUS (no local power required)
  - Solenoid drivers on each gun receive 24V from Rig's battery pack (separate from shield power)
  - **⚠️ Safety Requirement:** Power cable from battery to LBUS must be a **coiled stretch cable** to support the 3-5ft. max height limit of the scissor lift
  - **⚠️ Safety Requirement:** A **rip cord** must be calibrated shorter than the full extension of the power cable and wired into the E-STOP circuit. If software attempts to send the lift too high, the rip cord will activate E-STOP before the cable reaches full extension, preventing damage to the power connection

**Benefits of this configuration:**
- ✅ **Single power source:** All ECUs powered from Rig's battery pack
- ✅ **Wired reliability:** Low-latency Ethernet communication (< 1 ms) between parent and child ECUs
- ✅ **Modular design:** Each gun is a self-contained unit with embedded ECU
- ✅ **Power management:** DIP switches allow per-port power control for troubleshooting
- ✅ **Cost-effective:** Standard Ethernet cables and connectors, no custom harnesses
- ✅ **Scalable:** Easy to add/remove gun stations by connecting/disconnecting Ethernet cables

**Firmware Compatibility:**
- `GunshipExperience_ECU.ino` → Flashes to LBUS (parent ECU)
- `Gun_ECU.ino` → Flashes to each Child Shield (child ECU, 4× instances with different STATION_ID)

**Note:** Firmware pin mappings must be configured for Child Shield's Ethernet PHY pins (see firmware configuration section below).

---

## 📁 Examples

### **Standalone Modules**

Located in `../Base/Examples/`:

| File | Description | Use Case |
|------|-------------|----------|
| **`ScissorLift_Controller.ino`** | Standalone scissor lift control | Use when you only need vertical translation |
| **`ActuatorSystem_Controller.ino`** | Standalone 4-actuator system control | Use when you only need pitch/roll/lateral (no vertical) |

### **Parent ECU**

Located in `GunshipExperience/`:

| File | Description | Use Case |
|------|-------------|----------|
| **`GunshipExperience_ECU.ino`** | Parent ECU for complete 4DOF control (uses Universal Shield) | Use for GunshipExperience (combines both modules, interfaces with child ECUs) |

### **Child ECUs (Per-Station Controllers)**

**Note:** Child ECU firmware example (`Gun_ECU.ino`) is available. See **Hardware Specification** below for solenoid/driver details.

**Architecture:**
- **4× Child ECUs** (one per player station, `Gun_ECU.ino`) connect to the parent ECU (`GunshipExperience_ECU.ino`) in a star topology
- Each child ECU handles:
  - Dual thumb button input (debounced, rate-limited)
  - N× solenoid kicker control (with optional redundancy and thermal management)
  - SteamVR Ultimate tracker pose reading (or relay from parent ECU)
  - Telemetry reporting to parent ECU (10–30 Hz)
- **Connection:** Wired Ethernet (recommended, < 1 ms latency) or WiFi to parent ECU
- **Protocol:** LBEAST UDP binary protocol (same as engine communication)

**Parent ECU (GunshipExperience_ECU):**
- Uses the Universal Shield (primary ECU for the experience)
- Aggregates data from all 4× child ECUs
- Controls scissor lift platform directly (pitch/roll/Y/Z translation)
- Relays fused gun/platform state to game engine via wireless UDP

---

## 🔩 Hardware Specification (Child ECU Haptics)

**Default haptic:** Dual 24 V pull-type solenoids inside the gun chassis, driven by the child ECU for each station.

### Recommended Baseline

- **Solenoids:** 2× Johnson/Guardian pull-type (24 V, 20–40 N @ 5‑8 mm stroke).  
- **Drivers:** 2× Pololu G2 High-Power Motor Driver 24v13 (one per solenoid).  
- **Cooling:** Small aluminum heatsink + 40–50 mm fan per driver module.  
- **Supply:** 24 VDC rail, budget 3–5 A per gun (72–120 W).  
- **Cost:** ≈ $150–$200 per station (solenoids + drivers + cooling + ECU).

### Redundancy & Thermal Logic

- Install **N ≥ 1** solenoids; ECU alternates sessions using the coolest coil (< 80 °C).  
- Each solenoid has an NTC sensor; driver modules also monitored.  
- PWM duty throttles (100 → 50 %) if *all* coils/driver modules exceed 70 °C; hard shutdown at 85 °C.  
- Telemetry exposes: solenoid temps array, active solenoid ID, total solenoid count, PWM throttle %, thermal shutdown flags.

### Mechanical & Electrical Notes

- Recoil mass is coupled to handle chassis; use elastomer pads to reduce audible ringing.  
- Rubber-mount SteamVR trackers and add software filtering (10–20 Hz low-pass) to suppress solenoid-induced jitter.  
- Driver stage: logic-level MOSFET or H-bridge with flyback diode; IBT‑2 modules OK for prototypes, Pololu G2 recommended for production.  
- Harnessing: locking connectors with strain relief; star-ground at PSU return; separate logic and coil wiring as needed.

### Network / ECU Architecture

- Each gun is a **child ECU** (ESP32/STM32/etc.) sending telemetry (buttons, temps, fire state) to the **parent ECU**; receives play-session and E-stop commands.  
- Parent ECU aggregates four child ECUs, drives the motion base, and relays fused state to the engine via UDP.  
- Recommended topology: Ethernet star (parent ↔ 4 child ECUs) + Wi‑Fi link to the game server.

---

## 🎯 Quick Start

### **For GunshipExperience (Complete 4DOF)**

1. **Open** `GunshipExperience/GunshipExperience_ECU.ino`
2. **Configure WiFi credentials:**
   ```cpp
   const char* ssid = "VR_Arcade_LAN";
   const char* password = "your_password_here";
   ```
3. **Adjust hardware pins** in Configuration section to match your setup
4. **Upload to ESP32** (or your chosen microcontroller)
5. **Connect from Unreal/Unity** - the ECU will receive motion commands automatically

### **For Standalone Use**

If you only need one subsystem:

- **Scissor Lift Only:** Use `../Base/Examples/ScissorLift_Controller.ino`
- **Actuators Only:** Use `../Base/Examples/ActuatorSystem_Controller.ino`

---

## ✅ Installation Checklist

**Use this checklist to ensure all critical configuration steps are completed before deployment:**

### **Parent ECU (LBUS - GunshipExperience_ECU)**

- [ ] **WiFi Credentials:** Configure `ssid` and `password` in `GunshipExperience_ECU.ino` to match your network
- [ ] **Game Server IP:** Set `unrealIP` (or Unity server IP) to the correct address
- [ ] **Hardware Pins:** Verify all GPIO pin assignments match your LBUS configuration (scissor lift, actuators)
- [ ] **Power Supply:** Connect 24V LiFePO4 battery pack via barrel jack
- [ ] **Coiled Power Cable:** Ensure power cable from battery to LBUS is a coiled stretch cable for lift extension
- [ ] **Rip Cord:** Install and calibrate rip cord shorter than full cable extension, wired into E-STOP circuit
- [ ] **Ethernet Ports:** Verify all 4 aux ports are ready for Child Shield connections
- [ ] **DIP Switches:** Configure per-port power control (enable 5V on ports connected to Child Shields)
- [ ] **Upload Firmware:** Flash `GunshipExperience_ECU.ino` to LBUS ESP32-S3

### **Child ECUs (4× Child Shields - Gun_ECU)**

- [ ] **Station ID Configuration (CRITICAL):** Each Child Shield must have a unique `STATION_ID`:
  - [ ] Station 0: Set `const uint8_t STATION_ID = 0;` in `Gun_ECU.ino` → Flash to first Child Shield
  - [ ] Station 1: Set `const uint8_t STATION_ID = 1;` in `Gun_ECU.ino` → Flash to second Child Shield
  - [ ] Station 2: Set `const uint8_t STATION_ID = 2;` in `Gun_ECU.ino` → Flash to third Child Shield
  - [ ] Station 3: Set `const uint8_t STATION_ID = 3;` in `Gun_ECU.ino` → Flash to fourth Child Shield
  - ⚠️ **Warning:** Duplicate station IDs will cause telemetry conflicts and incorrect button/fire command routing
- [ ] **Parent ECU IP:** Set `parentECU_IP` in `Gun_ECU.ino` to match LBUS's IP address (or use DHCP discovery)
- [ ] **Communication Mode:** Verify `COMMUNICATION_MODE = COMM_MODE_ETHERNET` (not WiFi)
- [ ] **Ethernet Pin Configuration:** Confirm Ethernet pins match Child Shield (MDC=GPIO17, MDIO=GPIO18, POWER=-1)
- [ ] **Solenoid Pin Mapping:** Verify solenoid PWM pins are correct for Child Shield breakout (GPIO5, 27, 19, 21, 22, 23, 25, 26)
- [ ] **Temperature Sensor Pins:** Verify ADC pins are correct for Child Shield breakout (GPIO32, 33, 34, 1, 6, 7, 8, 9, 14)
- [ ] **Button Pins:** Verify button pins (GPIO2, GPIO4) are connected to physical buttons
- [ ] **Ethernet Cables:** Route Ethernet cables from each Child Shield to LBUS aux ports (J1-J4) through Rig chassis
- [ ] **Power via PoE:** Verify 5V power is enabled on corresponding LBUS aux port DIP switches
- [ ] **24V Solenoid Power:** Connect 24V power to solenoid drivers (separate from Child Shield 5V power)
- [ ] **Upload Firmware:** Flash `Gun_ECU.ino` to each Child Shield ESP32-S3 with correct `STATION_ID`

### **Network Configuration**

- [ ] **LBUS WiFi Connection:** Verify LBUS connects to game server's router via wireless adapter
- [ ] **Ethernet Star Topology:** Verify all 4 Child Shields connect to LBUS via Ethernet (not daisy-chained)
- [ ] **IP Address Assignment:** Configure static IPs or verify DHCP assignment for all ECUs
- [ ] **Port Configuration:** Verify UDP ports match firmware defaults:
  - Parent ECU RX: Port 8888 (from game engine)
  - Parent ECU TX: Port 8892 (receives from child ECUs)
  - Child ECU TX: Ports 8888-8891 (one per station, sends to parent)
  - Child ECU RX: Port 8892 (receives from parent)

### **Hardware Integration**

- [ ] **Scissor Lift:** Connect motor control, position sensors, and limit switches to LBUS
- [ ] **Actuator System:** Connect 4× hydraulic actuator valve drivers and position sensors to LBUS
- [ ] **Gun Buttons:** Connect dual thumb buttons to each Child Shield (GPIO2, GPIO4)
- [ ] **Solenoid Drivers:** Connect Pololu G2 drivers to Child Shield PWM pins and 24V power
- [ ] **Temperature Sensors:** Connect NTC thermistors to Child Shield ADC pins (one per solenoid + driver module)
- [ ] **SteamVR Trackers:** Mount and configure trackers on each gun (or relay pose from parent ECU)

### **Safety & Testing**

- [ ] **E-STOP Circuit:** Verify E-STOP is wired correctly and tested
- [ ] **Rip Cord:** Test rip cord activation at maximum safe lift height
- [ ] **Power Limits:** Verify 4A fuse on LBUS aux power bus is installed
- [ ] **Thermal Management:** Verify temperature sensors are reading correctly (check Serial output)
- [ ] **Network Connectivity:** Test Ethernet connection between each Child Shield and LBUS
- [ ] **Telemetry Flow:** Verify child ECUs are sending telemetry to parent ECU (check Serial output)
- [ ] **Button Response:** Test button presses on each gun and verify they appear in parent ECU telemetry
- [ ] **Fire Commands:** Test fire commands from game engine and verify solenoids activate
- [ ] **Motion Platform:** Test scissor lift and actuator system independently before full integration

### **Post-Installation Verification**

- [ ] **Serial Monitor:** Monitor Serial output from all 5 ECUs to verify communication
- [ ] **Game Engine Connection:** Verify LBUS receives motion commands from game engine
- [ ] **Button Events:** Verify button presses from all 4 guns appear in game engine
- [ ] **Fire Commands:** Verify fire commands from game engine activate correct solenoids
- [ ] **Telemetry Display:** Verify gun telemetry (temperatures, fire state) appears in game engine console
- [ ] **Motion Response:** Verify motion platform responds correctly to game engine commands

**Common Oversights:**
- ❌ Forgetting to set unique `STATION_ID` for each Child Shield (causes telemetry conflicts)
- ❌ Not enabling 5V power on LBUS aux port DIP switches (Child Shields won't power on)
- ❌ Incorrect parent ECU IP address in Child Shield firmware (no telemetry flow)
- ❌ Ethernet cables not using all 8 wires (PoE won't work with 4-wire cables)
- ❌ Not calibrating rip cord length (safety risk if lift extends too high)

---

## 🔌 Hardware Setup

### **Vertical Lift Platform Options**

**⚠️ Warranty & Liability Warning:**
- **Using industrial equipment (scissor lifts, pallet stackers, etc.) for VR motion platforms voids all manufacturer warranties.**
- **Manufacturers do not support or endorse using their equipment in this manner.**
- **Modifications (removing forks, adding tilt chassis, bypassing safety interlocks) are done at your own risk.**
- **You are solely responsible for safety, regulatory compliance, and liability.**
- **Always consult qualified engineers and follow local regulations for amusement ride/theme park equipment.**

**Platform Options:**
- **Scissor Lifts** (Genie, Skyjack, etc.) - $8K+ used, 10-15ft lift height, designed for personnel platforms
- **Electric Pallet Stackers** - $2-5K new, 6-10ft lift height, 2500lb+ capacity, built-in forward/reverse drive
- Both require CAN bus reverse-engineering or direct GPIO control
- Both require structural modifications (removing forks/rails, adding tilt chassis)
- Both have safety interlocks that may need to be satisfied or bypassed

### **Scissor Lift / Pallet Stacker Control**

The lift controller supports **two modes**:

#### **CAN Bus Mode (Recommended for Manufacturer ECUs)**

For manufacturer ECUs with CAN bus interfaces (e.g., Genie/Skyjack scissor lifts, Crown/Raymond pallet stackers):

```
ESP32 CAN TX (GPIO 4) ──[CAN Transceiver]── Lift ECU CAN High
ESP32 CAN RX (GPIO 5) ──[CAN Transceiver]── Lift ECU CAN Low
ESP32 GPIO 14 ──[Position Sensor]── Analog Input (optional, if not using CAN feedback)
ESP32 GPIO 15 ──[Top Limit Switch]── Digital Input (optional)
ESP32 GPIO 16 ──[Bottom Limit Switch]── Digital Input (optional)
```

**Configuration:**
```cpp
config.useCANBus = true;
config.canBaudRate = 500000;              // Check manufacturer spec (125k, 250k, or 500k)
config.canIdJoystick = 0x180;             // Manufacturer-specific CAN ID
config.canIdControl = 0x200;              // E-stop and control commands
config.canIdFeedback = 0x280;             // Position feedback (if available)
```

**Important:** Configure CAN IDs to match your manufacturer's ECU protocol. Refer to your lift/stacker's CAN bus documentation for correct CAN IDs and message formats.

**⚠️ Pallet Stacker Considerations:**
- **Safety Interlocks:** Most pallet stackers require operator presence detection (weight sensor, handle grip, seat switch). These may need to be satisfied or bypassed for automated control.
- **Speed:** Pallet stackers are typically slower than scissor lifts (0.1-0.2 m/s vs. 0.3-0.5 m/s lift speed). Verify this meets your motion platform requirements.
- **Form Factor:** Pallet stackers are narrower (24-30" vs. 36-48" for scissor lifts). Ensure tilt chassis and 4-seat mounting is structurally sound after fork removal.
- **Height Range:** Verify lift height (typically 6-10ft) meets your vertical travel requirements.
- **Forward/Reverse:** Built-in drive motors eliminate need for separate forward/reverse motor, but speeds are typically slower (2-4 mph vs. 5-8 mph for scissor lifts).

#### **Direct GPIO Mode (Custom Builds)**

For custom builds or testing without manufacturer ECU:

```
ESP32 GPIO 12 ──[Relay/Motor Driver]── Lift Motor Up
ESP32 GPIO 13 ──[Relay/Motor Driver]── Lift Motor Down
ESP32 GPIO 14 ──[Position Sensor]── Analog Input
ESP32 GPIO 15 ──[Top Limit Switch]── Digital Input (optional)
ESP32 GPIO 16 ──[Bottom Limit Switch]── Digital Input (optional)
```

**Configuration:**
```cpp
config.useCANBus = false;
config.motorUpPin = 12;
config.motorDownPin = 13;
config.positionSensorPin = 14;
```

### **Actuator System**

```
ESP32 GPIO 18 ──[Valve Driver]── Actuator 0 (Front Left) Proportional Valve
ESP32 GPIO 19 ──[Valve Driver]── Actuator 1 (Front Right) Proportional Valve
ESP32 GPIO 21 ──[Valve Driver]── Actuator 2 (Rear Left) Proportional Valve
ESP32 GPIO 22 ──[Valve Driver]── Actuator 3 (Rear Right) Proportional Valve

ESP32 GPIO 32 ──[4-20mA Sensor]── Actuator 0 Position (via 250Ω resistor)
ESP32 GPIO 33 ──[4-20mA Sensor]── Actuator 1 Position (via 250Ω resistor)
ESP32 GPIO 34 ──[4-20mA Sensor]── Actuator 2 Position (via 250Ω resistor)
ESP32 GPIO 35 ──[4-20mA Sensor]── Actuator 3 Position (via 250Ω resistor)
```

**Note:** See `COST_ANALYSIS.md` for complete hardware bill of materials. Child ECU haptic details are summarized in the **Hardware Specification** section above.

---

## 📡 Protocol & Channel Mapping

### **GunshipExperience_ECU Channel Mapping**

Matches `FPlatformMotionCommand` from Unreal/Unity:

| Channel | Type | Description | Range |
|---------|------|-------------|-------|
| **0** | Float | Pitch (degrees) | -10° to +10° |
| **1** | Float | Roll (degrees) | -10° to +10° |
| **2** | Float | TranslationY / Lateral (cm or normalized) | -20 cm to +20 cm, or -1.0 to +1.0 |
| **3** | Float | TranslationZ / Vertical (cm or normalized) | 0 cm to 300 cm, or -1.0 to +1.0 |
| **4** | Float | Duration (seconds) | 0.1+ seconds |
| **5** | Bool | Emergency Stop | true = stop all systems |
| **6** | Bool | Return to Neutral | true = return all to center |

### **Standalone Modules**

**ScissorLift_Controller:**
- Channel 0: Vertical translation (float)
- Channel 1: Emergency stop (bool)
- Channel 2: Return to neutral (bool)

**ActuatorSystem_Controller:**
- Channel 0: Pitch (float, degrees)
- Channel 1: Roll (float, degrees)
- Channel 2: Lateral translation (float, cm or normalized)
- Channel 3: Emergency stop (bool)
- Channel 4: Return to neutral (bool)

---

## 💻 Usage in Unreal/Unity

### **Unreal Engine**

```cpp
// In GunshipExperience
AGunshipExperience* Gunship = GetWorld()->SpawnActor<AGunshipExperience>();

// Send normalized motion (RECOMMENDED)
Gunship->SendGunshipTilt(0.5f, 0.8f, 0.2f, 0.1f, 1.5f);  // TiltX (roll), TiltY (pitch), ForwardOffset, VerticalOffset, Duration

// Advanced: Use absolute angles
Gunship->SendGunshipMotion(8.0f, 5.0f, 10.0f, 15.0f, 1.5f);  // pitch, roll, forwardOffset (cm), verticalOffset (cm), duration

// Emergency stop
Gunship->EmergencyStop();

// Return to neutral
Gunship->ReturnToNeutral(2.0f);
```

### **Unity**

```csharp
// In GunshipExperience
GunshipExperience gunship = FindObjectOfType<GunshipExperience>();

// Send normalized motion (RECOMMENDED)
gunship.SendGunshipTilt(0.5f, 0.8f, 0.2f, 0.1f, 1.5f);  // TiltX (roll), TiltY (pitch), ForwardOffset, VerticalOffset, Duration

// Advanced: Use absolute angles
gunship.SendGunshipMotion(8.0f, 5.0f, 10.0f, 15.0f, 1.5f);  // pitch, roll, forwardOffset (cm), verticalOffset (cm), duration

// Emergency stop
gunship.EmergencyStop();

// Return to neutral
gunship.ReturnToNeutral(2.0f);
```

---

## 🔧 Configuration

### **Motion Parameters**

Adjust these in the Configuration section of each example:

```cpp
// Scissor Lift
config.maxHeightCm = 300.0f;                // Adjust to your lift
config.minHeightCm = 0.0f;
config.softwareUpperLimitCm = 90.0f;        // Software-defined upper limit
config.maxForwardReverseCm = 90.0f;          // Max forward/reverse travel (safety limit)
config.enableForwardReverse = true;         // Set false if bolted to floor

// Actuator System
config.maxPitchDeg = 10.0f;                 // Maximum pitch angle
config.maxRollDeg = 10.0f;                  // Maximum roll angle
config.actuatorStrokeCm = 7.62f;            // Actuator stroke (3 inches)
config.platformWidthCm = 150.0f;            // Platform dimensions
config.platformLengthCm = 200.0f;
```

### **CAN Bus Configuration (Scissor Lift)**

For CAN bus mode, configure manufacturer-specific settings:

```cpp
// CAN Bus Settings
config.useCANBus = true;
config.canBaudRate = 500000;                 // 125000, 250000, or 500000 (check manufacturer spec)
config.canIdJoystick = 0x180;               // Joystick command CAN ID (manufacturer-specific)
config.canIdControl = 0x200;                // Control commands (E-stop, etc.)
config.canIdFeedback = 0x280;              // Position feedback CAN ID (if available)
config.canCSPin = 10;                       // MCP2515 CS pin (only for MCP2515)
config.useCANFeedback = false;              // Set true if ECU provides position via CAN

// Position Sensor (used if useCANFeedback = false)
config.positionSensorPin = 14;               // GPIO analog input (use -1 if using CAN feedback)
```

**Finding Your CAN IDs:**

⚠️ **Important:** CAN bus protocol documentation (including CAN IDs) is typically **proprietary** and not publicly available. **No open-source projects have been found that document Genie/Skyjack CAN protocols.**

**Note:** Some Skyjack models use SKYCODED™ control systems that may **not use CAN bus** - verify your specific model has CAN bus capability before proceeding.

Here are practical approaches:

1. **Contact Manufacturer Support:**
   - **Genie:** Contact Lift Connect team at AWP.LiftConnect@terex.com
   - **Skyjack:** Contact technical support through authorized dealers
   - Request CAN bus protocol documentation for integration purposes
   - May require confidentiality agreements or authorized service center status

2. **Use CAN Bus Analyzer (Reverse Engineering):**

   **How It Works:**
   - Splice into the CAN bus wires (CAN High and CAN Low) using a T-connector or tap
   - Connect the CAN analyzer in parallel (doesn't interrupt normal operation)
   - Operate the lift using the OEM joystick/controls while the analyzer captures all CAN traffic
   - The analyzer logs all messages with their CAN IDs and data payloads
   - Correlate joystick movements with CAN messages to identify which IDs control which functions
   
   **Recommended CAN Bus Analyzers:**
   
   | Device | Price Range | Notes |
   |--------|------------|-------|
   | **[CANtact](https://linklayer.github.io/cantact/)** | $50-100 | Open-source USB-CAN adapter, good for beginners |
   | **[Peak PCAN-USB](https://www.peak-system.com/PCAN-USB.199.0.html)** | $200-300 | Professional tool, excellent software, widely used |
   | **[Kvaser Leaf Light](https://www.kvaser.com/product/kvaser-leaf-light-v2/)** | $300-400 | Professional grade, excellent for complex analysis |
   | **[USB-CAN Analyzer (Chinese clones)** | $20-50 | Budget option, may have driver/software issues |
   | **[Arduino + MCP2515](https://github.com/collin80/ESP32RET)** | $10-20 | DIY option, requires programming knowledge |
   
   **Software Tools:**
   - **CANoe/CANalyzer** (Vector) - Professional, expensive, industry standard
   - **PCAN-View** (Peak) - Free software for Peak devices, user-friendly
   - **candump/cansend** (Linux SocketCAN) - Command-line tools, free
   - **SavvyCAN** - Open-source cross-platform CAN analyzer, free
   - **CANtact Tools** - Open-source tools for CANtact devices
   
   **Setup Process:**
   1. Identify CAN bus connector on lift ECU (typically 2-pin: CAN High, CAN Low)
   2. Use a CAN bus T-connector or tap to connect analyzer in parallel
   3. Connect analyzer to your computer via USB
   4. Start CAN monitoring software
   5. Operate lift manually with OEM joystick (up, down, forward, reverse, stop)
   6. Observe CAN messages appearing in real-time
   7. Correlate joystick actions with CAN IDs and data patterns
   8. Document which CAN IDs correspond to which functions
   
   **What to Look For:**
   - Messages that appear when joystick is moved (likely control commands)
   - Messages that change frequency or data when lift moves (likely position feedback)
   - Messages that appear when E-stop is pressed (likely safety commands)
   - Message data bytes that correlate with joystick position (e.g., 0x00 = neutral, 0x7F = full up)
   
   **General CAN reverse engineering resources:**
   - [CanBusHack](https://canbushack.com) - CAN bus analysis tools and methodologies
   - [CAN Reverse Engineering GitHub](https://github.com/brent-stone/CAN_Reverse_Engineering) - General CAN analysis scripts
   - Automotive CAN reverse engineering case studies (methodology can be adapted)
   
   This reverse-engineering approach is often necessary when documentation isn't available

3. **Service Manuals:**
   - Check service manuals for wiring diagrams and ECU pinouts
   - May contain CAN bus connector information but rarely include protocol details
   - Genie service manuals available at: genielift.com/en/support/manuals

4. **Authorized Dealers/Service Centers:**
   - Authorized dealers often have access to proprietary technical documentation
   - Service centers may be able to provide CAN bus protocol information

**Note:** Default values (0x180, 0x200, 0x280) in the example code are **placeholders only** - you must replace them with your actual manufacturer's CAN IDs. The message format may also need adjustment based on your ECU's protocol. You will likely need to reverse-engineer the protocol yourself using CAN bus analysis tools.

### **PID Tuning**

For the actuator system, adjust PID parameters:

```cpp
const float KP = 2.0f;  // Proportional gain (increase for faster response)
const float KI = 0.1f;   // Integral gain (increase to reduce steady-state error)
const float KD = 0.5f;  // Derivative gain (increase to reduce overshoot)
```

---

## 🛡️ Safety Features

All examples include:

- ✅ **Emergency Stop:** Immediate halt of all motion (sent via CAN bus in CAN mode)
- ✅ **Limit Switches:** Hardware limits for scissor lift (top/bottom) - GPIO or CAN feedback
- ✅ **Software Limits:** Position limits for actuators and lift travel
- ✅ **Position Feedback:** Closed-loop control with position sensors (GPIO or CAN)
- ✅ **Safety Checks:** Continuous monitoring of limits during motion
- ✅ **E-Stop Smoothing:** Gradual deceleration when emergency stop is activated
- ✅ **Auto-Calibrate Mode:** Automatic return to neutral after timeout (configurable)
- ⚠️ **Rip Cord Safety:** Power cable from battery to LBUS must use a coiled stretch cable to support lift extension. A rip cord must be calibrated shorter than the full cable extension and wired into the E-STOP circuit. If software attempts to send the lift too high, the rip cord activates E-STOP before the cable reaches full extension, preventing damage to the power connection.

---

## 📊 Cost Reference

See `COST_ANALYSIS.md` in `Source/LBEASTExperiences/` for complete hardware cost breakdown:
- Scissor lift: ~$8,000 (used, with certification)
- Hydraulic system: ~$5,500 (4 actuators + pump + valves + sensors + ESP32 PCB)
- **Total motion platform:** ~$13,500

---

## 🔁 IO Flow Snapshot

```
Child ECU (Gun_ECU) ──▶ Parent ECU (GunshipExperience_ECU) ──▶ Game Engine ──▶ Console / VR Players
```

| Link | Direction | Status | Notes |
|------|-----------|--------|-------|
| Child → Parent | Telemetry (UDP, 20 Hz default) | ✅ | Buttons, fire intensity, redundancy temps, PWM throttle, faults |
| Parent → Child | Game state (UDP, 10 Hz + on-demand) | ✅ | Play session active, emergency stop (fire command relay TBD) |
| Server → Parent | Motion/control | ✅ | Ch0‑9 floats/bools + struct packets (`FPlatformMotionCommand`) |
| Parent → Server | Motion feedback + gun telemetry | ✅ | Ch100/101 feedback structs, Ch310 button events, Ch311 gun telemetry |
| Server → Console | Telemetry display | ⚠️ | Parsing done; UMG console widget pending |
| Server ↔ VR Players | Button/fire replication | ❌ | Deferred (future VR transport work) |

**Channel reference**

- **Game engine → parent:** Ch0‑4 (pitch/roll/Y/Z/duration), Ch7 (E‑stop), Ch9 (play session), Ch100/101 (rate control), Ch200 struct for full command.  
- **Parent → engine:** Ch100/101 feedback structs, `FGunButtonEvents` (Ch310, configurable 20 Hz), `FGunTelemetry` (Ch311, configurable 1 Hz).  
- **Child → parent:** Per-station channels (10+n button, 20+n fire intensity, 30+n temps, etc.) plus redundancy/driver telemetry.  
- **Parent → child:** Ch9 play session (authorizes firing) and Ch7 E-stop; fire command propagation planned.

---

## 📝 Notes

### **Platform-Agnostic Design**

Both standalone modules are designed to be:
- ✅ **Reusable:** Can be used independently or combined
- ✅ **Platform-agnostic:** Works on ESP32, ESP8266, Arduino, STM32, Raspberry Pi, Jetson Nano
- ✅ **Modular:** Easy to integrate into different experience types

### **Combined ECU**

The `GunshipExperience_ECU.ino` combines both modules into a single controller:
- Coordinates both subsystems simultaneously
- Unified emergency stop and safety functions
- Matches `FPlatformMotionCommand` interface from Unreal/Unity

---

## 📄 License

MIT License - Copyright (c) 2025 AJ Campbell

---

**Built for LBEAST - Location-Based Entertainment Activation Standard**

