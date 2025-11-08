# LBEAST Firmware Examples

**Firmware examples, templates, and documentation for LBEAST Embedded Systems integration.**

---

## 📁 Directory Structure

```
FirmwareExamples/
├── Base/                          # Generic examples and templates for all experiences
│   ├── Templates/                  # Reusable header templates
│   │   ├── LBEAST_Wireless_TX.h   # Wireless transmission template
│   │   └── LBEAST_Wireless_RX.h   # Wireless reception template
│   └── Examples/                   # Functionality-based examples
│       ├── ButtonMotor_Example.ino              # Main example (all platforms)
│       └── ButtonMotor_Example_ESP8266.ino      # ESP8266 variant
│
└── EscapeRoom/                     # Escape room specific examples
    └── DoorLock/                   # Door lock control examples
        ├── DoorLock_Example.ino                 # Main example (all platforms)
        └── DoorLock_Example_ESP8266.ino        # ESP8266 variant
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

---

## 🔧 Platform Support

All examples support multiple platforms. The main examples work on all platforms with minor GPIO pin adjustments.

| Platform | Wireless | Example File | Notes |
|----------|----------|--------------|-------|
| **ESP32** | ✅ Built-in | `ButtonMotor_Example.ino` | Full support, recommended |
| **ESP8266** | ✅ Built-in | `ButtonMotor_Example_ESP8266.ino` | Limited GPIO pins, variant provided |
| **Arduino + WiFi Shield** | ✅ Via shield | `ButtonMotor_Example.ino` | Adjust GPIO pins, use shield library |
| **STM32 + WiFi Module** | ✅ Via module | `ButtonMotor_Example.ino` | Adjust GPIO pins, use module library |
| **Raspberry Pi** | ✅ Built-in | `ButtonMotor_Example.ino` | Adjust GPIO pins, use Linux sockets |
| **Jetson Nano** | ✅ Built-in | `ButtonMotor_Example.ino` | Adjust GPIO pins, use Linux sockets |

**Note:** Platform-specific variants (like `*_ESP8266.ino`) are provided as reference implementations showing platform-specific pin configurations. The main examples work on all platforms with configuration adjustments.

---

## 📝 Notes

### **FirmwareExamples Folder**

**Important:** The `FirmwareExamples/` folder is **not a special folder** in either engine. It's purely for **organization and documentation** of firmware examples and templates.

- ✅ **Safe to reorganize** - No engine dependencies
- ✅ **Safe to rename** - No code references
- ✅ **Documentation only** - Examples and templates for developers
- ✅ **Not included in builds** - Firmware files are not game engine assets

**Note:** We use `FirmwareExamples/` instead of `Resources/` to avoid Unity's special `Resources/` folder behavior (which includes all assets in builds). Firmware examples are not Unity assets and should not be included in game builds.

---

## 🚀 Experience-Specific Examples

Examples are organized by experience type:

- **Base/** - Generic examples usable by any experience
- **EscapeRoom/** - Escape room specific (door locks, props, sensors)
- **AIFacemask/** - (Future) Live actor costume examples
- **MovingPlatform/** - (Future) Motion platform sensor examples

---

## 📄 License

MIT License - Copyright (c) 2025 AJ Campbell

---

**Built for LBEAST - Location-Based Entertainment Activation Standard**

