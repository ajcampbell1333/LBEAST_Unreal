# LBEAST Wireless Communication Templates

**Standalone RX/TX header templates for easy integration of wireless communication in microcontroller firmware sketches.**

---

## 📦 Overview

These header templates provide a simple, drop-in solution for bidirectional wireless communication between microcontrollers and Unreal Engine using the LBEAST EmbeddedSystems protocol.

### Supported Platforms

| Platform | Built-in Wireless | Template File |
|----------|------------------|---------------|
| **ESP32** | ✅ WiFi | `LBEAST_Wireless_TX.h` / `LBEAST_Wireless_RX.h` |
| **ESP8266** | ✅ WiFi | `LBEAST_Wireless_TX.h` / `LBEAST_Wireless_RX.h` |
| **Arduino + WiFi Shield** | ✅ WiFi (via shield) | `LBEAST_Wireless_TX.h` / `LBEAST_Wireless_RX.h` |
| **STM32 + WiFi Module** | ✅ WiFi (via module) | `LBEAST_Wireless_TX.h` / `LBEAST_Wireless_RX.h` |
| **Raspberry Pi** | ✅ WiFi | `LBEAST_Wireless_TX.h` / `LBEAST_Wireless_RX.h` |
| **Jetson Nano** | ✅ WiFi | `LBEAST_Wireless_TX.h` / `LBEAST_Wireless_RX.h` |
| **Arduino (no WiFi)** | ❌ | Use Serial templates (coming soon) |
| **STM32 (no WiFi)** | ❌ | Use Serial templates (coming soon) |

---

## 🚀 Quick Start

### **Transmitting to Unreal (TX)**

```cpp
#include "LBEAST_Wireless_TX.h"

void setup() {
  // Initialize wireless communication
  LBEAST_Wireless_Init(
    "VR_Arcade_LAN",                    // WiFi SSID
    "your_password",                     // WiFi password
    IPAddress(192, 168, 1, 100),        // Unreal PC IP
    8888                                 // UDP port
  );
}

void loop() {
  // Send button press
  if (digitalRead(BUTTON_PIN) == LOW) {
    LBEAST_SendBool(0, true);
  }
  
  // Send sensor value
  float sensorValue = analogRead(SENSOR_PIN) / 1024.0f;
  LBEAST_SendFloat(1, sensorValue);
  
  delay(10);
}
```

### **Receiving from Unreal (RX)**

```cpp
#include "LBEAST_Wireless_RX.h"

// Implement handlers
void LBEAST_HandleBool(uint8_t channel, bool value) {
  if (channel == 0) {
    // Unlock door
    digitalWrite(LOCK_PIN, value ? HIGH : LOW);
  }
}

void LBEAST_HandleFloat(uint8_t channel, float value) {
  if (channel == 1) {
    // Set motor speed (0.0-1.0)
    analogWrite(MOTOR_PIN, (int)(value * 255));
  }
}

void setup() {
  // Initialize wireless communication
  LBEAST_Wireless_Init(
    "VR_Arcade_LAN",                    // WiFi SSID
    "your_password",                     // WiFi password
    8888                                 // UDP port
  );
}

void loop() {
  // Process incoming commands
  LBEAST_ProcessIncoming();
  
  delay(10);
}
```

---

## 📋 API Reference

### **TX Template (`LBEAST_Wireless_TX.h`)**

#### Initialization
```cpp
void LBEAST_Wireless_Init(const char* ssid, const char* password, IPAddress targetIP, uint16_t targetPort = 8888);
```

#### Transmission Functions
```cpp
void LBEAST_SendBool(uint8_t channel, bool value);
void LBEAST_SendInt32(uint8_t channel, int32_t value);
void LBEAST_SendFloat(uint8_t channel, float value);
void LBEAST_SendString(uint8_t channel, const char* str);
```

### **RX Template (`LBEAST_Wireless_RX.h`)**

#### Initialization
```cpp
void LBEAST_Wireless_Init(const char* ssid, const char* password, uint16_t localPort = 8888);
```

#### Reception Functions
```cpp
void LBEAST_ProcessIncoming();  // Call regularly in loop()
```

#### Handler Functions (Implement in your sketch)
```cpp
void LBEAST_HandleBool(uint8_t channel, bool value);
void LBEAST_HandleInt32(uint8_t channel, int32_t value);
void LBEAST_HandleFloat(uint8_t channel, float value);
void LBEAST_HandleString(uint8_t channel, const char* str, uint8_t length);
```

---

## 📊 Protocol Details

### **Packet Format**

```
[Marker:1] [Type:1] [Channel:1] [Payload:N] [CRC:1]
```

| Field | Size | Description |
|-------|------|-------------|
| Marker | 1 byte | Always `0xAA` (start of packet) |
| Type | 1 byte | Data type (0=Bool, 1=Int32, 2=Float, 3=String) |
| Channel | 1 byte | Channel/pin number (0-255) |
| Payload | N bytes | Data (variable length) |
| CRC | 1 byte | XOR checksum of all preceding bytes |

### **Data Types**

| Type | Value | Payload Size | Example |
|------|-------|--------------|---------|
| Bool | 0 | 1 byte | `true` / `false` |
| Int32 | 1 | 4 bytes | `42` |
| Float | 2 | 4 bytes | `3.14f` |
| String | 3 | 1-255 bytes | `"Hello"` |

---

## 🔒 Security

For production deployments, use the secured firmware templates with AES-128 encryption and HMAC authentication. See the EmbeddedSystems module documentation for secured firmware examples.

**Security Levels:**
- **None** - Development only (CRC checksum)
- **HMAC** - Authentication (prevents spoofing)
- **Encrypted** - Full confidentiality (AES-128 + HMAC)

---

## 🛠️ Platform-Specific Notes

### **ESP32 / ESP8266**
- Built-in WiFi support
- No additional hardware required
- Works out of the box

### **Arduino + WiFi Shield**
- Requires compatible WiFi shield (e.g., ESP8266-based)
- May need to adjust library includes based on shield model

### **STM32 + WiFi Module**
- Requires external WiFi module (e.g., ESP8266, ESP32)
- Use appropriate STM32 WiFi library for your module

### **Raspberry Pi / Jetson Nano**
- Uses standard Linux socket libraries
- May require additional WiFi configuration

---

## 📝 Integration Checklist

- [ ] Copy `LBEAST_Wireless_TX.h` and/or `LBEAST_Wireless_RX.h` to your sketch directory
- [ ] Configure WiFi credentials (SSID, password)
- [ ] Set Unreal PC IP address
- [ ] Implement handler functions (for RX)
- [ ] Call `LBEAST_ProcessIncoming()` in loop() (for RX)
- [ ] Test connection with Unreal Engine
- [ ] Configure security settings for production

---

## 🐛 Troubleshooting

### **"Platform not supported" error**
- Your platform doesn't have built-in wireless
- Use Serial templates instead (coming soon)

### **WiFi connection fails**
- Check SSID and password
- Verify WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Check signal strength

### **No packets received**
- Verify Unreal PC IP address is correct
- Check firewall allows UDP port 8888
- Ensure both devices are on same network
- Use Wireshark to monitor network traffic

### **CRC mismatch errors**
- Check for interference on WiFi network
- Verify both TX and RX use same protocol version
- Enable debug mode to inspect packets

---

## 📚 Additional Resources

- **[Base/Examples/README.md](../Examples/README.md)** - Complete example firmware
- **[EscapeRoom/README.md](../../EscapeRoom/README.md)** - Experience-specific examples
- **[EmbeddedSystems Module README](../../../Source/EmbeddedSystems/README.md)** - Full API documentation

---

## 📄 License

MIT License - Copyright (c) 2025 AJ Campbell

---

**Built for LBEAST - Location-Based Entertainment Activation Standard**

