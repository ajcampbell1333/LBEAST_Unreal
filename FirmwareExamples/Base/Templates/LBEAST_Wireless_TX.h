/*
 * LBEAST Wireless TX Template Header
 * 
 * Standalone wireless transmission template for LBEAST EmbeddedSystems protocol.
 * Include this header in your microcontroller sketch to easily send data to Unreal Engine.
 * 
 * Supports multiple platforms:
 * - ESP32 (WiFi UDP)
 * - ESP8266 (WiFi UDP)
 * - Arduino with WiFi Shield (WiFi UDP)
 * - STM32 with WiFi module (WiFi UDP)
 * - Raspberry Pi (WiFi UDP)
 * - Jetson Nano (WiFi UDP)
 * 
 * For platforms without built-in wireless, see LBEAST_Serial_TX.h
 * 
 * Protocol: Binary LBEAST protocol
 * Packet Format: [0xAA][Type][Channel][Payload...][CRC]
 * 
 * Usage:
 *   #include "LBEAST_Wireless_TX.h"
 *   
 *   void setup() {
 *     LBEAST_Wireless_Init("VR_Arcade_LAN", "password", IPAddress(192,168,1,100), 8888);
 *   }
 *   
 *   void loop() {
 *     LBEAST_SendBool(0, true);  // Send button press
 *     LBEAST_SendFloat(1, 0.75f); // Send sensor value
 *   }
 * 
 * Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.
 */

#ifndef LBEAST_WIRELESS_TX_H
#define LBEAST_WIRELESS_TX_H

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
  // STM32 with WiFi module - adjust includes based on your WiFi module
  #include <WiFi.h>
  #include <WiFiUdp.h>
#elif defined(__RASPBERRY_PI__) || defined(RASPBERRY_PI)
  #define LBEAST_PLATFORM_RASPBERRY_PI
  // Raspberry Pi - use standard socket libraries
  #include <WiFi.h>
  #include <WiFiUdp.h>
#elif defined(__JETSON_NANO__) || defined(JETSON_NANO)
  #define LBEAST_PLATFORM_JETSON
  // Jetson Nano - use standard socket libraries
  #include <WiFi.h>
  #include <WiFiUdp.h>
#else
  #error "LBEAST_Wireless_TX.h: Platform not supported. Use LBEAST_Serial_TX.h for serial communication."
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
IPAddress LBEAST_TargetIP(192, 168, 1, 100);
uint16_t LBEAST_TargetPort = 8888;
bool LBEAST_Initialized = false;

/**
 * Initialize wireless communication
 * @param ssid WiFi network name
 * @param password WiFi password
 * @param targetIP Unreal Engine PC IP address
 * @param targetPort UDP port (default 8888)
 */
void LBEAST_Wireless_Init(const char* ssid, const char* password, IPAddress targetIP, uint16_t targetPort = 8888) {
  Serial.begin(115200);
  Serial.println("\nLBEAST Wireless TX Initializing...");
  
  LBEAST_TargetIP = targetIP;
  LBEAST_TargetPort = targetPort;
  
  // Connect to WiFi
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  Serial.printf("Local IP: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("Target IP: %s:%d\n", LBEAST_TargetIP.toString().c_str(), LBEAST_TargetPort);
  
  LBEAST_Initialized = true;
  Serial.println("LBEAST Wireless TX Ready!");
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
 * Send bool value
 */
void LBEAST_SendBool(uint8_t channel, bool value) {
  if (!LBEAST_Initialized) return;
  
  uint8_t packet[5];
  packet[0] = LBEAST_PACKET_START_MARKER;
  packet[1] = LBEAST_TYPE_BOOL;
  packet[2] = channel;
  packet[3] = value ? 1 : 0;
  packet[4] = LBEAST_CalculateCRC(packet, 4);
  
  LBEAST_UDP.beginPacket(LBEAST_TargetIP, LBEAST_TargetPort);
  LBEAST_UDP.write(packet, 5);
  LBEAST_UDP.endPacket();
}

/**
 * Send int32 value
 */
void LBEAST_SendInt32(uint8_t channel, int32_t value) {
  if (!LBEAST_Initialized) return;
  
  uint8_t packet[8];
  packet[0] = LBEAST_PACKET_START_MARKER;
  packet[1] = LBEAST_TYPE_INT32;
  packet[2] = channel;
  packet[3] = (value) & 0xFF;
  packet[4] = (value >> 8) & 0xFF;
  packet[5] = (value >> 16) & 0xFF;
  packet[6] = (value >> 24) & 0xFF;
  packet[7] = LBEAST_CalculateCRC(packet, 7);
  
  LBEAST_UDP.beginPacket(LBEAST_TargetIP, LBEAST_TargetPort);
  LBEAST_UDP.write(packet, 8);
  LBEAST_UDP.endPacket();
}

/**
 * Send float value
 */
void LBEAST_SendFloat(uint8_t channel, float value) {
  if (!LBEAST_Initialized) return;
  
  uint8_t packet[8];
  packet[0] = LBEAST_PACKET_START_MARKER;
  packet[1] = LBEAST_TYPE_FLOAT;
  packet[2] = channel;
  
  // Reinterpret float as uint32 for byte-by-byte transmission
  uint32_t intValue = *(uint32_t*)&value;
  packet[3] = (intValue) & 0xFF;
  packet[4] = (intValue >> 8) & 0xFF;
  packet[5] = (intValue >> 16) & 0xFF;
  packet[6] = (intValue >> 24) & 0xFF;
  packet[7] = LBEAST_CalculateCRC(packet, 7);
  
  LBEAST_UDP.beginPacket(LBEAST_TargetIP, LBEAST_TargetPort);
  LBEAST_UDP.write(packet, 8);
  LBEAST_UDP.endPacket();
}

/**
 * Send string value
 */
void LBEAST_SendString(uint8_t channel, const char* str) {
  if (!LBEAST_Initialized) return;
  
  uint8_t len = strlen(str);
  if (len > 255) len = 255;
  
  uint8_t packet[256];
  packet[0] = LBEAST_PACKET_START_MARKER;
  packet[1] = LBEAST_TYPE_STRING;
  packet[2] = channel;
  packet[3] = len;
  memcpy(&packet[4], str, len);
  packet[4 + len] = LBEAST_CalculateCRC(packet, 4 + len);
  
  LBEAST_UDP.beginPacket(LBEAST_TargetIP, LBEAST_TargetPort);
  LBEAST_UDP.write(packet, 5 + len);
  LBEAST_UDP.endPacket();
}

/**
 * Send bytes/struct packet (for struct-based MVC pattern)
 */
void LBEAST_SendBytes(uint8_t channel, uint8_t* data, uint8_t length) {
  if (!LBEAST_Initialized) return;
  if (length > 255) length = 255;
  
  uint8_t packet[256];
  packet[0] = LBEAST_PACKET_START_MARKER;
  packet[1] = LBEAST_TYPE_BYTES;
  packet[2] = channel;
  packet[3] = length;
  memcpy(&packet[4], data, length);
  packet[4 + length] = LBEAST_CalculateCRC(packet, 4 + length);
  
  LBEAST_UDP.beginPacket(LBEAST_TargetIP, LBEAST_TargetPort);
  LBEAST_UDP.write(packet, 5 + length);
  LBEAST_UDP.endPacket();
}

#endif // LBEAST_WIRELESS_TX_H

