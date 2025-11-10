/*
 * LBEAST Wireless RX Template Header
 * 
 * Standalone wireless reception template for LBEAST EmbeddedSystems protocol.
 * Include this header in your microcontroller sketch to easily receive commands from Unreal Engine.
 * 
 * Supports multiple platforms:
 * - ESP32 (WiFi UDP)
 * - ESP8266 (WiFi UDP)
 * - Arduino with WiFi Shield (WiFi UDP)
 * - STM32 with WiFi module (WiFi UDP)
 * - Raspberry Pi (WiFi UDP)
 * - Jetson Nano (WiFi UDP)
 * 
 * For platforms without built-in wireless, see LBEAST_Serial_RX.h
 * 
 * Protocol: Binary LBEAST protocol
 * Packet Format: [0xAA][Type][Channel][Payload...][CRC]
 * 
 * Usage:
 *   #include "LBEAST_Wireless_RX.h"
 *   
 *   void setup() {
 *     LBEAST_Wireless_Init("VR_Arcade_LAN", "password", 8888);
 *   }
 *   
 *   void loop() {
 *     LBEAST_ProcessIncoming();  // Call this regularly
 *   }
 *   
 *   // Implement handlers
 *   void LBEAST_HandleBool(uint8_t channel, bool value) {
 *     // Handle bool command
 *   }
 *   
 *   void LBEAST_HandleFloat(uint8_t channel, float value) {
 *     // Handle float command
 *   }
 * 
 * Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.
 */

#ifndef LBEAST_WIRELESS_RX_H
#define LBEAST_WIRELESS_RX_H

// Platform detection
#if defined(ESP32)
  #define LBEAST_PLATFORM_ESP
  #include <WiFi.h>
  #include <WiFiUdp.h>
#elif defined(ESP8266)
  #define LBEAST_PLATFORM_ESP
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
#elif defined(ARDUINO_ARCH_STM32)
  #define LBEAST_PLATFORM_STM32
  #include <WiFi.h>
  #include <WiFiUdp.h>
#elif defined(__RASPBERRY_PI__) || defined(RASPBERRY_PI)
  #define LBEAST_PLATFORM_RASPBERRY_PI
  #include <WiFi.h>
  #include <WiFiUdp.h>
#elif defined(__JETSON_NANO__) || defined(JETSON_NANO)
  #define LBEAST_PLATFORM_JETSON
  #include <WiFi.h>
  #include <WiFiUdp.h>
#else
  #error "LBEAST_Wireless_RX.h: Platform not supported. Use LBEAST_Serial_RX.h for serial communication."
#endif

// Protocol constants
#define LBEAST_PACKET_START_MARKER 0xAA

enum LBEASTDataType {
  LBEAST_TYPE_BOOL = 0,
  LBEAST_TYPE_INT32 = 1,
  LBEAST_TYPE_FLOAT = 2,
  LBEAST_TYPE_STRING = 3,
  LBEAST_TYPE_BYTES = 4
};

// Global UDP object
#if defined(LBEAST_PLATFORM_ESP) || defined(LBEAST_PLATFORM_STM32)
WiFiUDP LBEAST_UDP;
#elif defined(LBEAST_PLATFORM_RASPBERRY_PI) || defined(LBEAST_PLATFORM_JETSON)
WiFiUDP LBEAST_UDP;
#else
#error "Platform UDP not defined"
#endif

// Configuration
uint16_t LBEAST_LocalPort = 8888;
bool LBEAST_Initialized = false;

// Handler function prototypes (implement these in your sketch)
void LBEAST_HandleBool(uint8_t channel, bool value);
void LBEAST_HandleInt32(uint8_t channel, int32_t value);
void LBEAST_HandleFloat(uint8_t channel, float value);
void LBEAST_HandleString(uint8_t channel, const char* str, uint8_t length);
void LBEAST_HandleBytes(uint8_t channel, uint8_t* data, uint8_t length);

/**
 * Initialize wireless communication
 * @param ssid WiFi network name
 * @param password WiFi password
 * @param localPort UDP port to listen on (default 8888)
 */
void LBEAST_Wireless_Init(const char* ssid, const char* password, uint16_t localPort = 8888) {
  Serial.begin(115200);
  Serial.println("\nLBEAST Wireless RX Initializing...");
  
  LBEAST_LocalPort = localPort;
  
  // Connect to WiFi
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  Serial.printf("Local IP: %s\n", WiFi.localIP().toString().c_str());
  
  // Start UDP listener
  LBEAST_UDP.begin(LBEAST_LocalPort);
  Serial.printf("UDP listening on port %d\n", LBEAST_LocalPort);
  
  LBEAST_Initialized = true;
  Serial.println("LBEAST Wireless RX Ready!");
}

/**
 * Calculate CRC checksum
 */
uint8_t LBEAST_CalculateCRC(uint8_t* data, int length) {
  uint8_t crc = 0;
  for (int i = 0; i < length; i++) {
    crc ^= data[i];
  }
  return crc;
}

/**
 * Process incoming packets
 * Call this regularly in your loop()
 */
void LBEAST_ProcessIncoming() {
  if (!LBEAST_Initialized) return;
  
  int packetSize = LBEAST_UDP.parsePacket();
  if (packetSize == 0) return;
  
  // Read packet
  uint8_t buffer[256];
  int len = LBEAST_UDP.read(buffer, 256);
  
  if (len < 5) {
    Serial.println("LBEAST: Packet too small");
    return;
  }
  
  // Validate start marker
  if (buffer[0] != LBEAST_PACKET_START_MARKER) {
    Serial.printf("LBEAST: Invalid start marker: 0x%02X\n", buffer[0]);
    return;
  }
  
  // Validate CRC
  uint8_t receivedCRC = buffer[len - 1];
  uint8_t calculatedCRC = LBEAST_CalculateCRC(buffer, len - 1);
  if (receivedCRC != calculatedCRC) {
    Serial.println("LBEAST: CRC mismatch");
    return;
  }
  
  // Parse packet
  uint8_t type = buffer[1];
  uint8_t channel = buffer[2];
  
  switch (type) {
    case LBEAST_TYPE_BOOL:
      LBEAST_HandleBool(channel, buffer[3] != 0);
      break;
      
    case LBEAST_TYPE_INT32:
      if (len >= 8) {
        int32_t value = (int32_t)buffer[3] | 
                       ((int32_t)buffer[4] << 8) | 
                       ((int32_t)buffer[5] << 16) | 
                       ((int32_t)buffer[6] << 24);
        LBEAST_HandleInt32(channel, value);
      }
      break;
      
    case LBEAST_TYPE_FLOAT:
      if (len >= 8) {
        uint32_t intValue = (uint32_t)buffer[3] | 
                           ((uint32_t)buffer[4] << 8) | 
                           ((uint32_t)buffer[5] << 16) | 
                           ((uint32_t)buffer[6] << 24);
        float value = *(float*)&intValue;
        LBEAST_HandleFloat(channel, value);
      }
      break;
      
    case LBEAST_TYPE_STRING:
      if (len >= 5) {
        uint8_t strLen = buffer[3];
        if (strLen > 0 && len >= 5 + strLen) {
          char str[256];
          memcpy(str, &buffer[4], strLen);
          str[strLen] = '\0';
          LBEAST_HandleString(channel, str, strLen);
        }
      }
      break;
      
    case LBEAST_TYPE_BYTES:
      if (len >= 5) {
        uint8_t byteLen = buffer[3];
        if (byteLen > 0 && len >= 5 + byteLen) {
          // Extract bytes (skip length byte at buffer[3])
          LBEAST_HandleBytes(channel, &buffer[4], byteLen);
        }
      }
      break;
      
    default:
      Serial.printf("LBEAST: Unknown type: %d\n", type);
      break;
  }
}

// Default handler implementations (override in your sketch)
__attribute__((weak)) void LBEAST_HandleBool(uint8_t channel, bool value) {
  Serial.printf("LBEAST: Bool - Ch:%d Val:%s\n", channel, value ? "true" : "false");
}

__attribute__((weak)) void LBEAST_HandleInt32(uint8_t channel, int32_t value) {
  Serial.printf("LBEAST: Int32 - Ch:%d Val:%d\n", channel, value);
}

__attribute__((weak)) void LBEAST_HandleFloat(uint8_t channel, float value) {
  Serial.printf("LBEAST: Float - Ch:%d Val:%.3f\n", channel, value);
}

__attribute__((weak)) void LBEAST_HandleString(uint8_t channel, const char* str, uint8_t length) {
  Serial.printf("LBEAST: String - Ch:%d Val:%s\n", channel, str);
}

__attribute__((weak)) void LBEAST_HandleBytes(uint8_t channel, uint8_t* data, uint8_t length) {
  Serial.printf("LBEAST: Bytes - Ch:%d Len:%d\n", channel, length);
  // Default implementation just logs - override in your sketch to parse struct packets
}

#endif // LBEAST_WIRELESS_RX_H

