/*
 * LBEAST ESP32 Secured Firmware (AES-128 + HMAC-SHA1)
 * 
 * This firmware implements the LBEAST encrypted protocol with:
 * - AES-128-CTR encryption
 * - HMAC-SHA1 authentication (truncated to 8 bytes)
 * - Key derivation from shared secret
 * 
 * Hardware Requirements:
 * - ESP32 DevKit (any variant with WiFi)
 * - 4 tactile buttons connected to GPIO 2, 4, 5, 18 (with pull-up resistors)
 * - 6 vibration motors connected to GPIO 12, 13, 14, 25, 26, 27 (with driver transistors)
 * 
 * Libraries Required:
 * - mbedtls (included in ESP32 Arduino core)
 * 
 * Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.
 */

#include <WiFi.h>
#include <WiFiUdp.h>
#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include "mbedtls/sha1.h"

// =====================================
// Configuration
// =====================================

// WiFi credentials (change to match your LAN)
const char* ssid = "VR_Arcade_LAN";
const char* password = "your_password_here";

// Unreal PC IP and port
IPAddress unrealIP(192, 168, 1, 100);  // Change to your Unreal PC's IP
uint16_t unrealPort = 8888;
uint16_t localPort = 8888;

// Shared secret (MUST match Unreal Config.SharedSecret)
const char* sharedSecret = "CHANGE_ME_IN_PRODUCTION_2025";

// Security level (0 = None, 1 = HMAC, 2 = Encrypted)
const int securityLevel = 2;  // 2 = AES-128 + HMAC (recommended for production)

// Button pins (INPUT_PULLUP)
const int buttonPins[4] = {2, 4, 5, 18};
bool buttonStates[4] = {false, false, false, false};
bool lastButtonStates[4] = {false, false, false, false};

// Vibration motor pins (PWM)
const int motorPins[6] = {12, 13, 14, 25, 26, 27};

// UDP
WiFiUDP udp;

// Protocol constants
const uint8_t PACKET_START_MARKER = 0xAA;

enum DataType {
  TYPE_BOOL = 0,
  TYPE_INT32 = 1,
  TYPE_FLOAT = 2,
  TYPE_STRING = 3,
  TYPE_BYTES = 4
};

// Cryptography
uint8_t derivedAESKey[16];   // 128-bit AES key
uint8_t derivedHMACKey[32];  // 256-bit HMAC key
uint32_t randomState;        // RNG state for IV generation

// =====================================
// Key Derivation
// =====================================

void deriveKeys() {
  // Derive AES key: SHA1(Secret + "AES128_LBEAST_2025")
  {
    mbedtls_sha1_context sha1;
    mbedtls_sha1_init(&sha1);
    mbedtls_sha1_starts(&sha1);
    mbedtls_sha1_update(&sha1, (const unsigned char*)sharedSecret, strlen(sharedSecret));
    mbedtls_sha1_update(&sha1, (const unsigned char*)"AES128_LBEAST_2025", 18);
    
    uint8_t hash[20];
    mbedtls_sha1_finish(&sha1, hash);
    mbedtls_sha1_free(&sha1);
    
    // Use first 16 bytes for AES-128
    memcpy(derivedAESKey, hash, 16);
  }
  
  // Derive HMAC key: SHA1(Secret + "HMAC_LBEAST_2025")
  {
    mbedtls_sha1_context sha1;
    mbedtls_sha1_init(&sha1);
    mbedtls_sha1_starts(&sha1);
    mbedtls_sha1_update(&sha1, (const unsigned char*)sharedSecret, strlen(sharedSecret));
    mbedtls_sha1_update(&sha1, (const unsigned char*)"HMAC_LBEAST_2025", 16);
    
    uint8_t hash[20];
    mbedtls_sha1_finish(&sha1, hash);
    mbedtls_sha1_free(&sha1);
    
    // Use all 20 bytes + pad for HMAC key
    memcpy(derivedHMACKey, hash, 20);
    memset(derivedHMACKey + 20, 0, 12);
  }
  
  Serial.println("Keys derived from shared secret");
}

// =====================================
// AES-128-CTR Encryption/Decryption
// =====================================

void encryptAES128(const uint8_t* plaintext, int length, uint8_t* ciphertext, uint32_t iv) {
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, derivedAESKey, 128);
  
  // CTR mode encryption
  int blockCount = (length + 15) / 16;
  for (int blockIdx = 0; blockIdx < blockCount; blockIdx++) {
    // Create counter block
    uint8_t counterBlock[16] = {0};
    uint32_t currentCounter = iv + blockIdx;
    counterBlock[0] = (currentCounter) & 0xFF;
    counterBlock[1] = (currentCounter >> 8) & 0xFF;
    counterBlock[2] = (currentCounter >> 16) & 0xFF;
    counterBlock[3] = (currentCounter >> 24) & 0xFF;
    counterBlock[4] = (blockIdx) & 0xFF;
    counterBlock[5] = (blockIdx >> 8) & 0xFF;
    counterBlock[6] = (blockIdx >> 16) & 0xFF;
    counterBlock[7] = (blockIdx >> 24) & 0xFF;
    
    // Encrypt counter block
    uint8_t encryptedCounter[16];
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, counterBlock, encryptedCounter);
    
    // XOR with plaintext
    int bytesInBlock = min(16, length - blockIdx * 16);
    for (int i = 0; i < bytesInBlock; i++) {
      ciphertext[blockIdx * 16 + i] = plaintext[blockIdx * 16 + i] ^ encryptedCounter[i];
    }
  }
  
  mbedtls_aes_free(&aes);
}

void decryptAES128(const uint8_t* ciphertext, int length, uint8_t* plaintext, uint32_t iv) {
  // CTR mode decryption is identical to encryption
  encryptAES128(ciphertext, length, plaintext, iv);
}

// =====================================
// HMAC-SHA1
// =====================================

void calculateHMAC(const uint8_t* data, int length, uint8_t* hmac) {
  const int blockSize = 64;
  const uint8_t ipad = 0x36;
  const uint8_t opad = 0x5C;
  
  // Prepare key
  uint8_t key[blockSize];
  memset(key, 0, blockSize);
  memcpy(key, derivedHMACKey, 32);
  
  // Inner hash: H((K' ⊕ ipad) || m)
  mbedtls_sha1_context sha1;
  mbedtls_sha1_init(&sha1);
  mbedtls_sha1_starts(&sha1);
  
  uint8_t temp[blockSize];
  for (int i = 0; i < blockSize; i++) {
    temp[i] = key[i] ^ ipad;
  }
  mbedtls_sha1_update(&sha1, temp, blockSize);
  mbedtls_sha1_update(&sha1, data, length);
  
  uint8_t innerHash[20];
  mbedtls_sha1_finish(&sha1, innerHash);
  
  // Outer hash: H((K' ⊕ opad) || inner_hash)
  mbedtls_sha1_starts(&sha1);
  for (int i = 0; i < blockSize; i++) {
    temp[i] = key[i] ^ opad;
  }
  mbedtls_sha1_update(&sha1, temp, blockSize);
  mbedtls_sha1_update(&sha1, innerHash, 20);
  
  uint8_t outerHash[20];
  mbedtls_sha1_finish(&sha1, outerHash);
  mbedtls_sha1_free(&sha1);
  
  // Return first 8 bytes (truncated)
  memcpy(hmac, outerHash, 8);
}

bool validateHMAC(const uint8_t* data, int length, const uint8_t* expectedHMAC) {
  uint8_t calculatedHMAC[8];
  calculateHMAC(data, length, calculatedHMAC);
  
  // Constant-time comparison
  uint8_t diff = 0;
  for (int i = 0; i < 8; i++) {
    diff |= calculatedHMAC[i] ^ expectedHMAC[i];
  }
  
  return (diff == 0);
}

// =====================================
// Random Number Generation
// =====================================

uint32_t generateRandomIV() {
  // XORshift PRNG
  randomState ^= randomState << 13;
  randomState ^= randomState >> 17;
  randomState ^= randomState << 5;
  return randomState;
}

// =====================================
// Setup
// =====================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nLBEAST ESP32 Secured Firmware Starting...");

  // Configure button pins
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Configure motor pins
  for (int i = 0; i < 6; i++) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }

  // Initialize RNG
  randomState = esp_random();

  // Derive encryption keys
  if (securityLevel > 0) {
    deriveKeys();
    if (securityLevel == 2) {
      Serial.println("Security: AES-128 + HMAC");
    } else {
      Serial.println("Security: HMAC only");
    }
  } else {
    Serial.println("Security: DISABLED (Development Only)");
  }

  // Connect to WiFi
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.printf("ESP32 IP: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("Unreal IP: %s:%d\n", unrealIP.toString().c_str(), unrealPort);

  // Start UDP
  udp.begin(localPort);
  Serial.printf("UDP listening on port %d\n", localPort);

  Serial.println("ESP32 ready!");
}

// =====================================
// Main Loop
// =====================================

void loop() {
  // Read button states and send to Unreal
  readButtons();

  // Receive commands from Unreal
  receiveCommands();

  delay(10); // ~100Hz polling
}

// =====================================
// Button Reading
// =====================================

void readButtons() {
  for (int i = 0; i < 4; i++) {
    // Read button (active LOW due to pull-up)
    buttonStates[i] = (digitalRead(buttonPins[i]) == LOW);

    // Send on state change
    if (buttonStates[i] != lastButtonStates[i]) {
      sendBool(i, buttonStates[i]);
      Serial.printf("Button %d: %s\n", i, buttonStates[i] ? "PRESSED" : "RELEASED");
      lastButtonStates[i] = buttonStates[i];
    }
  }
}

// =====================================
// Command Receiving
// =====================================

void receiveCommands() {
  int packetSize = udp.parsePacket();
  if (packetSize == 0) return;

  // Read packet
  uint8_t buffer[256];
  int len = udp.read(buffer, 256);

  if (len < 1 || buffer[0] != PACKET_START_MARKER) {
    Serial.println("Invalid start marker");
    return;
  }

  uint8_t type, channel;
  uint8_t payload[256];
  int payloadLen = 0;

  // Parse based on security level
  if (securityLevel == 2) {
    // Encrypted: [0xAA][IV:4][Encrypted(Type|Ch|Payload):N][HMAC:8]
    if (len < 15) {
      Serial.println("Encrypted packet too small");
      return;
    }

    // Validate HMAC
    if (!validateHMAC(buffer, len - 8, &buffer[len - 8])) {
      Serial.println("HMAC validation failed");
      return;
    }

    // Extract IV
    uint32_t iv = buffer[1] | (buffer[2] << 8) | (buffer[3] << 16) | (buffer[4] << 24);

    // Decrypt
    int ciphertextLen = len - 13;
    uint8_t plaintext[256];
    decryptAES128(&buffer[5], ciphertextLen, plaintext, iv);

    // Parse plaintext
    type = plaintext[0];
    channel = plaintext[1];
    payloadLen = ciphertextLen - 2;
    memcpy(payload, &plaintext[2], payloadLen);
  }
  else if (securityLevel == 1) {
    // HMAC only: [0xAA][Type][Ch][Payload][HMAC:8]
    if (len < 12) {
      Serial.println("HMAC packet too small");
      return;
    }

    // Validate HMAC
    if (!validateHMAC(buffer, len - 8, &buffer[len - 8])) {
      Serial.println("HMAC validation failed");
      return;
    }

    type = buffer[1];
    channel = buffer[2];
    payloadLen = len - 11;
    memcpy(payload, &buffer[3], payloadLen);
  }
  else {
    // No security: [0xAA][Type][Ch][Payload][CRC:1]
    if (len < 5) {
      Serial.println("Packet too small");
      return;
    }

    // Validate CRC
    uint8_t crc = 0;
    for (int i = 0; i < len - 1; i++) {
      crc ^= buffer[i];
    }
    if (crc != buffer[len - 1]) {
      Serial.println("CRC mismatch");
      return;
    }

    type = buffer[1];
    channel = buffer[2];
    payloadLen = len - 4;
    memcpy(payload, &buffer[3], payloadLen);
  }

  // Handle command based on type
  switch (type) {
    case TYPE_BOOL:
      handleBool(channel, payload[0] != 0);
      break;

    case TYPE_INT32:
      handleInt32(channel, *(int32_t*)payload);
      break;

    case TYPE_FLOAT:
      handleFloat(channel, *(float*)payload);
      break;

    case TYPE_STRING:
      handleString(channel, &payload[1], payload[0]);
      break;

    default:
      Serial.printf("Unknown type: %d\n", type);
      break;
  }
}

// =====================================
// Command Handlers
// =====================================

void handleBool(uint8_t channel, bool value) {
  Serial.printf("Received Bool - Ch:%d Val:%s\n", channel, value ? "true" : "false");
  if (channel < 6) {
    digitalWrite(motorPins[channel], value ? HIGH : LOW);
  }
}

void handleInt32(uint8_t channel, int32_t value) {
  Serial.printf("Received Int32 - Ch:%d Val:%d\n", channel, value);
  if (channel < 6) {
    analogWrite(motorPins[channel], constrain(value, 0, 255));
  }
}

void handleFloat(uint8_t channel, float value) {
  Serial.printf("Received Float - Ch:%d Val:%.3f\n", channel, value);
  if (channel < 6) {
    int pwm = (int)(value * 255.0f);
    analogWrite(motorPins[channel], constrain(pwm, 0, 255));
  }
}

void handleString(uint8_t channel, uint8_t* data, uint8_t length) {
  char str[256];
  memcpy(str, data, length);
  str[length] = '\0';
  Serial.printf("Received String - Ch:%d Val:%s\n", channel, str);
}

// =====================================
// Sending to Unreal
// =====================================

void sendBool(uint8_t channel, bool value) {
  // Build payload
  uint8_t payload[1] = {value ? (uint8_t)1 : (uint8_t)0};
  sendPacket(TYPE_BOOL, channel, payload, 1);
}

void sendInt32(uint8_t channel, int32_t value) {
  uint8_t payload[4];
  payload[0] = (value) & 0xFF;
  payload[1] = (value >> 8) & 0xFF;
  payload[2] = (value >> 16) & 0xFF;
  payload[3] = (value >> 24) & 0xFF;
  sendPacket(TYPE_INT32, channel, payload, 4);
}

void sendFloat(uint8_t channel, float value) {
  uint8_t payload[4];
  uint32_t intValue = *(uint32_t*)&value;
  payload[0] = (intValue) & 0xFF;
  payload[1] = (intValue >> 8) & 0xFF;
  payload[2] = (intValue >> 16) & 0xFF;
  payload[3] = (intValue >> 24) & 0xFF;
  sendPacket(TYPE_FLOAT, channel, payload, 4);
}

void sendPacket(uint8_t type, uint8_t channel, const uint8_t* payload, int payloadLen) {
  uint8_t packet[256];
  int packetLen = 0;

  if (securityLevel == 2) {
    // Encrypted: [0xAA][IV:4][Encrypted(Type|Ch|Payload):N][HMAC:8]
    
    // Build plaintext
    uint8_t plaintext[256];
    plaintext[0] = type;
    plaintext[1] = channel;
    memcpy(&plaintext[2], payload, payloadLen);
    int plaintextLen = 2 + payloadLen;
    
    // Generate IV
    uint32_t iv = generateRandomIV();
    
    // Encrypt
    uint8_t ciphertext[256];
    encryptAES128(plaintext, plaintextLen, ciphertext, iv);
    
    // Build packet
    packet[0] = PACKET_START_MARKER;
    packet[1] = (iv) & 0xFF;
    packet[2] = (iv >> 8) & 0xFF;
    packet[3] = (iv >> 16) & 0xFF;
    packet[4] = (iv >> 24) & 0xFF;
    memcpy(&packet[5], ciphertext, plaintextLen);
    packetLen = 5 + plaintextLen;
    
    // Calculate HMAC
    uint8_t hmac[8];
    calculateHMAC(packet, packetLen, hmac);
    memcpy(&packet[packetLen], hmac, 8);
    packetLen += 8;
  }
  else if (securityLevel == 1) {
    // HMAC only: [0xAA][Type][Ch][Payload][HMAC:8]
    packet[0] = PACKET_START_MARKER;
    packet[1] = type;
    packet[2] = channel;
    memcpy(&packet[3], payload, payloadLen);
    packetLen = 3 + payloadLen;
    
    // Calculate HMAC
    uint8_t hmac[8];
    calculateHMAC(packet, packetLen, hmac);
    memcpy(&packet[packetLen], hmac, 8);
    packetLen += 8;
  }
  else {
    // No security: [0xAA][Type][Ch][Payload][CRC:1]
    packet[0] = PACKET_START_MARKER;
    packet[1] = type;
    packet[2] = channel;
    memcpy(&packet[3], payload, payloadLen);
    packetLen = 3 + payloadLen;
    
    // Calculate CRC
    uint8_t crc = 0;
    for (int i = 0; i < packetLen; i++) {
      crc ^= packet[i];
    }
    packet[packetLen++] = crc;
  }

  // Send via UDP
  udp.beginPacket(unrealIP, unrealPort);
  udp.write(packet, packetLen);
  udp.endPacket();
}

