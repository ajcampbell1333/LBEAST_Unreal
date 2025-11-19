# LBEAST Firmware Examples

**Firmware examples, templates, and documentation for LBEAST Embedded Systems integration.**

---

## 📁 Directory Structure

```
FirmwareExamples/
├── Base/                          # Generic examples and templates for all experiences
│   ├── Templates/                  # Reusable header templates
│   │   ├── LBEAST_Wireless_TX.h   # Wireless transmission template
│   │   ├── LBEAST_Wireless_RX.h   # Wireless reception template
│   │   ├── LBEAST_CAN.h           # CAN bus communication template
│   │   ├── ScissorLift_Controller.h  # Scissor lift control (CAN or GPIO)
│   │   └── ActuatorSystem_Controller.h  # Actuator system control
│   └── Examples/                   # Functionality-based examples
│       ├── ButtonMotor_Example.ino              # Main example (all platforms)
│       ├── ScissorLift_Controller.ino          # Scissor lift standalone
│       └── ActuatorSystem_Controller.ino       # Actuator system standalone
│
├── EscapeRoom/                     # Escape room specific examples
│   └── DoorLock/                   # Door lock control examples
│       └── DoorLock_Example.ino                 # Main example (all platforms)
│
└── GunshipExperience/              # Gunship experience specific examples
    ├── GunshipExperience_ECU.ino   # Parent ECU for 4DOF motion platform (uses Universal Shield)
    ├── Gun_ECU.ino                  # Child ECU for per-station gun control
    └── README.md                    # GunshipExperience firmware documentation
```

---

## 🎯 Quick Start

### **Using Templates**

1. **Copy template header** to your sketch directory:
   ```cpp
   // Copy from: FirmwareExamples/Base/Templates/LBEAST_Wireless_RX.h
   // To: YourSketch/LBEAST_Wireless_RX.h
   ```

2. **Include in your sketch**:
   ```cpp
   #include "LBEAST_Wireless_RX.h"
   
   void setup() {
     LBEAST_Wireless_Init("VR_Arcade_LAN", "password", 8888);
   }
   
   void loop() {
     LBEAST_ProcessIncoming();
   }
   ```

### **Using Examples**

1. **Choose your platform** (ESP32, ESP8266, Arduino+WiFi, etc.)
2. **Open the appropriate example**:
   - Use main example (`ButtonMotor_Example.ino` or `DoorLock_Example.ino`) for most platforms
   - Use ESP8266 variant if using ESP8266 (shows platform-specific pin config)
3. **Adjust GPIO pins** in Configuration section to match your hardware
4. **Configure WiFi credentials** and Unreal PC IP address
5. **Upload to your microcontroller**

---

## 📚 Documentation

- **[Base/Templates/README.md](Base/Templates/README.md)** - Template usage guide
- **[Base/Examples/README.md](Base/Examples/README.md)** - Base example documentation
- **[EscapeRoom/README.md](EscapeRoom/README.md)** - Escape room examples guide
- **[GunshipExperience/README.md](GunshipExperience/README.md)** - Gunship experience examples guide
- **[GunshipExperience/Gunship_Hardware_Specs.md](GunshipExperience/Gunship_Hardware_Specs.md)** - Complete hardware specifications for gun solenoid kickers

---

## 🔧 Platform Support

All examples support multiple platforms. The main examples work on all platforms with minor GPIO pin adjustments.

| Platform | Wireless | Example File | Notes |
|----------|----------|--------------|-------|
| **ESP32** | ✅ Built-in | All examples | Full support, recommended |
| **ESP8266** | ✅ Built-in | All examples | Limited GPIO pins, adjust pin assignments |
| **Arduino + WiFi Shield** | ✅ Via shield | All examples | Adjust GPIO pins, use shield library |
| **STM32 + WiFi Module** | ✅ Via module | All examples | Adjust GPIO pins, use module library |
| **Raspberry Pi** | ✅ Built-in | All examples | Adjust GPIO pins, use Linux sockets |
| **Jetson Nano** | ✅ Built-in | All examples | Adjust GPIO pins, use Linux sockets |

**Note:** All examples are platform-agnostic. Adjust GPIO pin assignments in the Configuration section to match your hardware. See comments in each example for platform-specific pin recommendations.

---

## 📝 Notes

### **FirmwareExamples Folder**

**Important:** The `FirmwareExamples/` folder is **not a special folder** in either engine. It's purely for **organization and documentation** of firmware examples and templates.

- ✅ **Safe to reorganize** - No engine dependencies
- ✅ **Safe to rename** - No code references
- ✅ **Documentation only** - Examples and templates for developers
- ✅ **Not included in builds** - Firmware files are not game engine assets

**Note:** We use `FirmwareExamples/` instead of `Resources/` to avoid Unity's special `Resources/` folder behavior (which includes all assets in builds). Firmware examples are not Unity assets and should not be included in game builds.

**Organization:** All embedded hardware firmware examples are located in `FirmwareExamples/`, organized by experience type. Game engine code references these examples in documentation but does not depend on their location.

---

## 🚀 Experience-Specific Examples

Examples are organized by experience type:

- **Base/** - Generic examples usable by any experience
  - `ScissorLift_Controller.ino` - Standalone scissor lift control (CAN bus or GPIO)
  - `ActuatorSystem_Controller.ino` - Standalone 4-actuator hydraulic control
  - `ButtonMotor_Example.ino` - Generic button & motor example
- **EscapeRoom/** - Escape room specific (door locks, props, sensors)
- **GunshipExperience/** - Gunship experience (parent ECU for 4DOF motion platform, child ECUs for per-station gun control)
- **AIFacemask/** - (Future) Live actor costume examples
- **MovingPlatform/** - (Future) Motion platform sensor examples

## 🔌 CAN Bus Support

The scissor lift controller supports **CAN bus communication** for manufacturer ECUs (e.g., Genie/Skyjack lifts):

- **CAN Bus Mode (default):** Sends joystick commands to manufacturer ECU via CAN bus
- **Direct GPIO Mode:** Direct motor control for custom builds or testing
- **Platform Support:** ESP32 (native TWAI), Arduino (MCP2515), STM32 (native), Linux (SocketCAN)

See `GunshipExperience/README.md` for CAN bus configuration instructions.

---

## 📄 License

MIT License - Copyright (c) 2025 AJ Campbell

---

**Built for LBEAST - Location-Based Entertainment Activation Standard**

