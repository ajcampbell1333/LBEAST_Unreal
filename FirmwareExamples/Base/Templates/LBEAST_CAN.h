/*
 * LBEAST CAN Bus Communication Template
 * 
 * Platform-agnostic CAN bus abstraction for communicating with manufacturer ECUs
 * (e.g., Genie/Skyjack scissor lift ECUs, motion platform controllers).
 * 
 * Supports multiple platforms:
 * - ESP32: Native CAN controller (TWAI)
 * - Arduino: MCP2515 CAN controller via SPI
 * - STM32: Native CAN controller
 * - Raspberry Pi / Jetson Nano: SocketCAN (Linux)
 * 
 * Usage:
 *   #include "LBEAST_CAN.h"
 *   
 *   void setup() {
 *     LBEAST_CAN_Init(500000); // 500 kbps baud rate
 *   }
 *   
 *   void loop() {
 *     LBEAST_CAN_SendCommand(0x123, data, 8); // Send CAN frame
 *   }
 * 
 * Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.
 */

#ifndef LBEAST_CAN_H
#define LBEAST_CAN_H

#include <Arduino.h>

// Platform detection
#if defined(ESP32)
  #define LBEAST_PLATFORM_ESP32
  #include "driver/twai.h"
#elif defined(ESP8266)
  #define LBEAST_PLATFORM_ESP8266
  // ESP8266 doesn't have native CAN - requires MCP2515
  #define LBEAST_CAN_USE_MCP2515
#elif defined(ARDUINO_ARCH_STM32)
  #define LBEAST_PLATFORM_STM32
  #include "CAN.h" // STM32 CAN library
#elif defined(__linux__) || defined(__unix__)
  #define LBEAST_PLATFORM_LINUX
  // SocketCAN for Linux (Raspberry Pi, Jetson Nano)
  #include <linux/can.h>
  #include <linux/can/raw.h>
  #include <sys/socket.h>
  #include <net/if.h>
#else
  // Default to MCP2515 for Arduino and other platforms
  #define LBEAST_CAN_USE_MCP2515
#endif

// MCP2515 includes (if needed)
#ifdef LBEAST_CAN_USE_MCP2515
  #include <SPI.h>
  #include <mcp2515.h>
#endif

// CAN bus configuration
static bool LBEAST_CAN_Initialized = false;
static uint32_t LBEAST_CAN_BaudRate = 500000; // Default 500 kbps

// MCP2515 instance (if using)
#ifdef LBEAST_CAN_USE_MCP2515
  static MCP2515* LBEAST_CAN_MCP2515 = nullptr;
  static int LBEAST_CAN_CS_PIN = 10; // Default CS pin (configurable)
#endif

// ESP32 TWAI configuration
#ifdef LBEAST_PLATFORM_ESP32
  static twai_general_config_t LBEAST_CAN_TWAI_Config;
  static twai_timing_config_t LBEAST_CAN_TWAI_Timing;
  static twai_filter_config_t LBEAST_CAN_TWAI_Filter;
#endif

// Linux SocketCAN
#ifdef LBEAST_PLATFORM_LINUX
  static int LBEAST_CAN_Socket = -1;
  static const char* LBEAST_CAN_Interface = "can0"; // Default interface
#endif

// =====================================
// Platform-Specific Initialization
// =====================================

#ifdef LBEAST_PLATFORM_ESP32
bool LBEAST_CAN_Init_ESP32(uint32_t baudRate) {
  LBEAST_CAN_BaudRate = baudRate;
  
  // Configure GPIO pins for CAN (default: GPIO 4 = TX, GPIO 5 = RX)
  // These can be changed via twai_general_config_t if needed
  LBEAST_CAN_TWAI_Config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_4, GPIO_NUM_5, TWAI_MODE_NORMAL);
  LBEAST_CAN_TWAI_Config.alerts_enabled = TWAI_ALERT_NONE;
  
  // Timing configuration based on baud rate
  if (baudRate == 500000) {
    LBEAST_CAN_TWAI_Timing = TWAI_TIMING_CONFIG_500KBITS();
  } else if (baudRate == 250000) {
    LBEAST_CAN_TWAI_Timing = TWAI_TIMING_CONFIG_250KBITS();
  } else if (baudRate == 125000) {
    LBEAST_CAN_TWAI_Timing = TWAI_TIMING_CONFIG_125KBITS();
  } else {
    // Custom timing (default to 500 kbps)
    LBEAST_CAN_TWAI_Timing = TWAI_TIMING_CONFIG_500KBITS();
  }
  
  LBEAST_CAN_TWAI_Filter = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  
  // Install and start CAN driver
  esp_err_t result = twai_driver_install(&LBEAST_CAN_TWAI_Config, &LBEAST_CAN_TWAI_Timing, &LBEAST_CAN_TWAI_Filter);
  if (result != ESP_OK) {
    Serial.printf("LBEAST_CAN: ESP32 TWAI install failed: %d\n", result);
    return false;
  }
  
  result = twai_start();
  if (result != ESP_OK) {
    Serial.printf("LBEAST_CAN: ESP32 TWAI start failed: %d\n", result);
    return false;
  }
  
  Serial.println("LBEAST_CAN: ESP32 TWAI initialized successfully");
  return true;
}
#endif

#ifdef LBEAST_CAN_USE_MCP2515
bool LBEAST_CAN_Init_MCP2515(uint32_t baudRate, int csPin = 10) {
  LBEAST_CAN_BaudRate = baudRate;
  LBEAST_CAN_CS_PIN = csPin;
  
  if (LBEAST_CAN_MCP2515 == nullptr) {
    LBEAST_CAN_MCP2515 = new MCP2515(csPin);
  }
  
  SPI.begin();
  
  int result = LBEAST_CAN_MCP2515->reset();
  if (result != MCP2515_OK) {
    Serial.printf("LBEAST_CAN: MCP2515 reset failed: %d\n", result);
    return false;
  }
  
  // Set baud rate
  CAN_SPEED canSpeed = CAN_500KBPS;
  if (baudRate == 250000) {
    canSpeed = CAN_250KBPS;
  } else if (baudRate == 125000) {
    canSpeed = CAN_125KBPS;
  }
  
  result = LBEAST_CAN_MCP2515->setBitrate(canSpeed);
  if (result != MCP2515_OK) {
    Serial.printf("LBEAST_CAN: MCP2515 setBitrate failed: %d\n", result);
    return false;
  }
  
  result = LBEAST_CAN_MCP2515->setNormalMode();
  if (result != MCP2515_OK) {
    Serial.printf("LBEAST_CAN: MCP2515 setNormalMode failed: %d\n", result);
    return false;
  }
  
  Serial.println("LBEAST_CAN: MCP2515 initialized successfully");
  return true;
}
#endif

#ifdef LBEAST_PLATFORM_STM32
bool LBEAST_CAN_Init_STM32(uint32_t baudRate) {
  LBEAST_CAN_BaudRate = baudRate;
  
  if (!CAN.begin(baudRate)) {
    Serial.println("LBEAST_CAN: STM32 CAN begin failed");
    return false;
  }
  
  Serial.println("LBEAST_CAN: STM32 CAN initialized successfully");
  return true;
}
#endif

#ifdef LBEAST_PLATFORM_LINUX
bool LBEAST_CAN_Init_Linux(uint32_t baudRate, const char* interface = "can0") {
  LBEAST_CAN_BaudRate = baudRate;
  LBEAST_CAN_Interface = interface;
  
  LBEAST_CAN_Socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (LBEAST_CAN_Socket < 0) {
    Serial.printf("LBEAST_CAN: Linux socket creation failed\n");
    return false;
  }
  
  struct ifreq ifr;
  strcpy(ifr.ifr_name, interface);
  ioctl(LBEAST_CAN_Socket, SIOCGIFINDEX, &ifr);
  
  struct sockaddr_can addr;
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  
  if (bind(LBEAST_CAN_Socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    Serial.printf("LBEAST_CAN: Linux socket bind failed\n");
    close(LBEAST_CAN_Socket);
    LBEAST_CAN_Socket = -1;
    return false;
  }
  
  Serial.printf("LBEAST_CAN: Linux SocketCAN initialized on %s\n", interface);
  return true;
}
#endif

// =====================================
// Unified Initialization Function
// =====================================

/**
 * Initialize CAN bus communication
 * @param baudRate CAN bus baud rate (typically 125000, 250000, or 500000)
 * @param csPin MCP2515 CS pin (only used for MCP2515, default: 10)
 * @param interface Linux CAN interface name (only used for Linux, default: "can0")
 * @return true if initialization successful, false otherwise
 */
bool LBEAST_CAN_Init(uint32_t baudRate = 500000, int csPin = 10, const char* interface = "can0") {
  if (LBEAST_CAN_Initialized) {
    Serial.println("LBEAST_CAN: Already initialized");
    return true;
  }
  
  bool result = false;
  
#ifdef LBEAST_PLATFORM_ESP32
  result = LBEAST_CAN_Init_ESP32(baudRate);
#elif defined(LBEAST_CAN_USE_MCP2515)
  result = LBEAST_CAN_Init_MCP2515(baudRate, csPin);
#elif defined(LBEAST_PLATFORM_STM32)
  result = LBEAST_CAN_Init_STM32(baudRate);
#elif defined(LBEAST_PLATFORM_LINUX)
  result = LBEAST_CAN_Init_Linux(baudRate, interface);
#else
  Serial.println("LBEAST_CAN: Platform not supported");
  return false;
#endif
  
  if (result) {
    LBEAST_CAN_Initialized = true;
  }
  
  return result;
}

// =====================================
// Platform-Specific Send Functions
// =====================================

#ifdef LBEAST_PLATFORM_ESP32
bool LBEAST_CAN_Send_ESP32(uint32_t canId, uint8_t* data, uint8_t dataLength) {
  if (dataLength > 8) dataLength = 8;
  
  twai_message_t message;
  message.identifier = canId;
  message.data_length_code = dataLength;
  message.flags = TWAI_MSG_FLAG_NONE;
  
  for (int i = 0; i < dataLength; i++) {
    message.data[i] = data[i];
  }
  
  esp_err_t result = twai_transmit(&message, pdMS_TO_TICKS(100));
  return (result == ESP_OK);
}
#endif

#ifdef LBEAST_CAN_USE_MCP2515
bool LBEAST_CAN_Send_MCP2515(uint32_t canId, uint8_t* data, uint8_t dataLength) {
  if (LBEAST_CAN_MCP2515 == nullptr) return false;
  if (dataLength > 8) dataLength = 8;
  
  struct can_frame frame;
  frame.can_id = canId;
  frame.can_dlc = dataLength;
  
  for (int i = 0; i < dataLength; i++) {
    frame.data[i] = data[i];
  }
  
  int result = LBEAST_CAN_MCP2515->sendMessage(&frame);
  return (result == MCP2515_OK);
}
#endif

#ifdef LBEAST_PLATFORM_STM32
bool LBEAST_CAN_Send_STM32(uint32_t canId, uint8_t* data, uint8_t dataLength) {
  if (dataLength > 8) dataLength = 8;
  
  CAN.beginPacket(canId);
  for (int i = 0; i < dataLength; i++) {
    CAN.write(data[i]);
  }
  return (CAN.endPacket() == 1);
}
#endif

#ifdef LBEAST_PLATFORM_LINUX
bool LBEAST_CAN_Send_Linux(uint32_t canId, uint8_t* data, uint8_t dataLength) {
  if (LBEAST_CAN_Socket < 0) return false;
  if (dataLength > 8) dataLength = 8;
  
  struct can_frame frame;
  frame.can_id = canId;
  frame.can_dlc = dataLength;
  
  for (int i = 0; i < dataLength; i++) {
    frame.data[i] = data[i];
  }
  
  int result = write(LBEAST_CAN_Socket, &frame, sizeof(struct can_frame));
  return (result == sizeof(struct can_frame));
}
#endif

// =====================================
// Unified Send Function
// =====================================

/**
 * Send a CAN bus frame
 * @param canId CAN message ID (11-bit or 29-bit)
 * @param data Data array (max 8 bytes)
 * @param dataLength Number of bytes to send (1-8)
 * @return true if sent successfully, false otherwise
 */
bool LBEAST_CAN_SendCommand(uint32_t canId, uint8_t* data, uint8_t dataLength) {
  if (!LBEAST_CAN_Initialized) {
    Serial.println("LBEAST_CAN: Not initialized. Call LBEAST_CAN_Init() first.");
    return false;
  }
  
  if (dataLength == 0 || dataLength > 8) {
    Serial.println("LBEAST_CAN: Invalid data length (must be 1-8 bytes)");
    return false;
  }
  
#ifdef LBEAST_PLATFORM_ESP32
  return LBEAST_CAN_Send_ESP32(canId, data, dataLength);
#elif defined(LBEAST_CAN_USE_MCP2515)
  return LBEAST_CAN_Send_MCP2515(canId, data, dataLength);
#elif defined(LBEAST_PLATFORM_STM32)
  return LBEAST_CAN_Send_STM32(canId, data, dataLength);
#elif defined(LBEAST_PLATFORM_LINUX)
  return LBEAST_CAN_Send_Linux(canId, data, dataLength);
#else
  Serial.println("LBEAST_CAN: Platform not supported");
  return false;
#endif
}

// =====================================
// Helper Functions for Common Commands
// =====================================

/**
 * Send a joystick-style command to scissor lift ECU
 * This is a generic template - adapt CAN ID and data format to your manufacturer's protocol
 * 
 * @param verticalCommand Vertical lift command (-1.0 to +1.0, where +1.0 = up, -1.0 = down)
 * @param forwardCommand Forward/reverse command (-1.0 to +1.0, where +1.0 = forward, -1.0 = reverse)
 * @param canIdBase Base CAN ID for lift commands (manufacturer-specific)
 */
void LBEAST_CAN_SendLiftJoystickCommand(float verticalCommand, float forwardCommand, uint32_t canIdBase = 0x180) {
  // Generic joystick command format (8 bytes)
  // Adapt this to your manufacturer's specific protocol
  // Example format: [vertical_byte, forward_byte, reserved...]
  
  uint8_t canData[8] = {0};
  
  // Convert float commands (-1.0 to +1.0) to signed bytes (-127 to +127)
  int8_t verticalByte = (int8_t)(constrain(verticalCommand * 127.0f, -127, 127));
  int8_t forwardByte = (int8_t)(constrain(forwardCommand * 127.0f, -127, 127));
  
  canData[0] = (uint8_t)verticalByte;
  canData[1] = (uint8_t)forwardByte;
  // Bytes 2-7 reserved or used for additional commands (manufacturer-specific)
  
  // Send vertical command (typically separate CAN IDs for different functions)
  LBEAST_CAN_SendCommand(canIdBase, canData, 8);
  
  // Some manufacturers use separate CAN IDs for vertical vs forward/reverse
  // Uncomment and adjust if needed:
  // LBEAST_CAN_SendCommand(canIdBase + 1, &canData[1], 1); // Forward/reverse only
}

/**
 * Send emergency stop command to scissor lift ECU
 * @param enable true = activate E-stop, false = release E-stop
 * @param canIdBase Base CAN ID for control commands
 */
void LBEAST_CAN_SendLiftEmergencyStop(bool enable, uint32_t canIdBase = 0x200) {
  uint8_t canData[8] = {0};
  canData[0] = enable ? 0x01 : 0x00; // E-stop command byte
  // Additional bytes may be required by manufacturer protocol
  
  LBEAST_CAN_SendCommand(canIdBase, canData, 8);
}

#endif // LBEAST_CAN_H



