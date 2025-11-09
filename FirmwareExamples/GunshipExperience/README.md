# Gunship Experience Firmware Examples

**Embedded firmware examples for GunshipExperience motion platform control.**

These examples demonstrate how to control a complete 4DOF motion platform system consisting of:
- **Scissor Lift:** Electric lift for vertical translation (TranslationZ) and forward/reverse (TranslationY)
- **Actuator System:** 4-gang hydraulic actuators for pitch and roll only (yaw restricted)

---

## 📁 Examples

### **Standalone Modules**

Located in `../Base/Examples/`:

| File | Description | Use Case |
|------|-------------|----------|
| **`ScissorLift_Controller.ino`** | Standalone scissor lift control | Use when you only need vertical translation |
| **`ActuatorSystem_Controller.ino`** | Standalone 4-actuator system control | Use when you only need pitch/roll/lateral (no vertical) |

### **Combined ECU**

Located in `GunshipExperience/`:

| File | Description | Use Case |
|------|-------------|----------|
| **`GunshipExperience_ECU.ino`** | Combined ECU for complete 4DOF control | Use for GunshipExperience (combines both modules) |

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

## 🔌 Hardware Setup

### **Scissor Lift**

The scissor lift controller supports **two modes**:

#### **CAN Bus Mode (Recommended for Genie/Skyjack Lifts)**

For manufacturer ECUs with CAN bus interfaces (e.g., Genie, Skyjack):

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

**Important:** Configure CAN IDs to match your manufacturer's ECU protocol. Refer to your lift's CAN bus documentation for correct CAN IDs and message formats.

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

**Note:** See `COST_ANALYSIS.md` for complete hardware bill of materials.

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

---

## 📊 Cost Reference

See `COST_ANALYSIS.md` in `Source/LBEASTExperiences/` for complete hardware cost breakdown:
- Scissor lift: ~$8,000 (used, with certification)
- Hydraulic system: ~$5,500 (4 actuators + pump + valves + sensors + ESP32 PCB)
- **Total motion platform:** ~$13,500

---

## 🔗 Related Documentation

- **[Base/Templates/README.md](../../Base/Templates/README.md)** - Using wireless templates
- **[Base/Examples/README.md](../../Base/Examples/README.md)** - Base example documentation
- **[COST_ANALYSIS.md](../../../Source/LBEASTExperiences/COST_ANALYSIS.md)** - Complete cost breakdown
- **[GunshipExperience.h](../../../Source/LBEASTExperiences/Public/GunshipExperience.h)** - Unreal API

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

