/*
 * LBEAST Gunship Experience ECU
 * 
 * Combined embedded control unit for GunshipExperience motion platform.
 * Integrates both scissor lift control and 4-gang actuator system control
 * into a single ECU for complete 4DOF motion platform control.
 * 
 * This ECU uses the ActuatorSystem_Controller.h and ScissorLift_Controller.h headers
 * to eliminate code duplication and provide modular, reusable controllers.
 * 
 * This ECU receives motion commands from Unreal/Unity GunshipExperience
 * and coordinates both subsystems:
 * - Scissor Lift: Handles vertical translation (TranslationZ) and forward/reverse (TranslationY)
 * - Actuator System: Handles pitch and roll only (2 DOF)
 * 
 * Functionality:
 * - Receives complete motion commands (pitch, roll, forward/reverse, vertical) from game engine
 * - Coordinates scissor lift and actuator system simultaneously
 * - Calibration system for both subsystems
 * - Auto-calibrate mode (auto-revert to zero after timeout) vs Fixed mode (hold position)
 * - Provides unified emergency stop and safety functions with smooth interpolation
 * - Position feedback from both subsystems
 * 
 * Supported Platforms:
 * - ESP32 (built-in WiFi) - Recommended for this application
 * - ESP8266 (built-in WiFi) - Limited GPIO, may need additional hardware
 * - Arduino + WiFi Shield (ESP8266-based) - Standard Arduino GPIO pins
 * - STM32 + WiFi Module (ESP8266/ESP32-based) - STM32 GPIO pins
 * - Raspberry Pi (built-in WiFi) - GPIO via WiringPi or pigpio
 * - Jetson Nano (built-in WiFi) - GPIO via Jetson GPIO library
 * 
 * Hardware Requirements:
 * - Microcontroller with WiFi capability (see supported platforms above)
 * - Electric scissor lift with motor control
 * - Optional: Forward/reverse drive motor (can be disabled if bolted to floor)
 * - 4-gang hydraulic actuator system with proportional valves
 * - Position sensors for all actuators and lift
 * - ESP32 custom PCB (see cost analysis) for valve drivers and sensor I/O
 * - Limit switches for safety (optional but recommended)
 * 
 * Protocol: Binary LBEAST protocol
 * Channel Mapping (matches FPlatformMotionCommand from Unreal):
 * - Channel 0: Pitch (float, degrees or normalized -1.0 to +1.0)
 * - Channel 1: Roll (float, degrees or normalized -1.0 to +1.0)
 * - Channel 2: TranslationY / Forward-Reverse (float, cm or normalized -1.0 to +1.0)
 * - Channel 3: TranslationZ / Vertical (float, cm or normalized -1.0 to +1.0)
 * - Channel 4: Duration (float, seconds) - time to reach target
 * - Channel 5: Calibration mode (bool, true = enter calibration mode)
 * - Channel 6: Operation mode (bool, true = auto-calibrate mode, false = fixed mode)
 * - Channel 7: Emergency stop (bool, true = stop all systems)
 * - Channel 8: Return to neutral (bool, true = return all to calibrated zero)
 * 
 * DOF Mapping:
 * - Actuators: Pitch and Roll only (2 DOF, yaw restricted)
 * - Scissor Lift: Vertical (TranslationZ) and Forward/Reverse (TranslationY) (2 DOF)
 * - Total: 4 DOF motion platform
 * 
 * This example uses the LBEAST_Wireless_RX.h template for receiving commands.
 * Copy LBEAST_Wireless_RX.h from FirmwareExamples/Base/Templates/ to your sketch directory.
 * 
 * Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.
 */

#include "LBEAST_Wireless_RX.h"
#include "../Base/Templates/ActuatorSystem_Controller.h"
#include "../Base/Templates/ScissorLift_Controller.h"

// =====================================
// Configuration
// =====================================

// WiFi credentials (change to match your LAN)
const char* ssid = "VR_Arcade_LAN";
const char* password = "your_password_here";

// Create controller instances
ScissorLiftController liftController;
ActuatorSystemController actuatorController;

// Unified state
float motionDuration = 1.0f; // Default duration in seconds
unsigned long motionStartTime = 0;
bool motionInProgress = false;

// =====================================
// Setup
// =====================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\nLBEAST Gunship Experience ECU");
  Serial.println("==============================\n");
  
  // Configure scissor lift
  ScissorLiftConfig liftConfig;
  liftConfig.motorUpPin = 12;
  liftConfig.motorDownPin = 13;
  liftConfig.positionSensorPin = 14;
  liftConfig.topLimitPin = 15;
  liftConfig.bottomLimitPin = 16;
  
  liftConfig.enableForwardReverse = true;
  liftConfig.motorForwardPin = 17;
  liftConfig.motorReversePin = 18;
  liftConfig.forwardLimitPin = 19;
  liftConfig.reverseLimitPin = 20;
  liftConfig.maxForwardReverseCm = 90.0f;
  
  liftConfig.maxHeightCm = 300.0f;
  liftConfig.minHeightCm = 0.0f;
  liftConfig.softwareUpperLimitCm = 90.0f;
  liftConfig.maxSpeedCmPerSec = 10.0f;
  
  liftConfig.autoCalibrateMode = true;
  liftConfig.autoCalibrateTimeoutMs = 2000;
  
  // Initialize scissor lift controller
  liftController.begin(liftConfig);
  
  // Configure actuator system
  ActuatorSystemConfig actuatorConfig;
  actuatorConfig.valvePins[0] = 21;
  actuatorConfig.valvePins[1] = 22;
  actuatorConfig.valvePins[2] = 25;
  actuatorConfig.valvePins[3] = 26;
  actuatorConfig.sensorPins[0] = 32;
  actuatorConfig.sensorPins[1] = 33;
  actuatorConfig.sensorPins[2] = 34;
  actuatorConfig.sensorPins[3] = 35;
  actuatorConfig.lowerLimitPins[0] = 27;
  actuatorConfig.lowerLimitPins[1] = 4;
  actuatorConfig.lowerLimitPins[2] = 5;
  actuatorConfig.lowerLimitPins[3] = 2;
  actuatorConfig.upperLimitPins[0] = 0;
  actuatorConfig.upperLimitPins[1] = 1;
  actuatorConfig.upperLimitPins[2] = 3;
  actuatorConfig.upperLimitPins[3] = 4;
  
  actuatorConfig.maxPitchDeg = 10.0f;
  actuatorConfig.maxRollDeg = 10.0f;
  actuatorConfig.actuatorStrokeCm = 7.62f;
  actuatorConfig.platformWidthCm = 150.0f;
  actuatorConfig.platformLengthCm = 200.0f;
  
  actuatorConfig.kp = 2.0f;
  actuatorConfig.ki = 0.1f;
  actuatorConfig.kd = 0.5f;
  
  actuatorConfig.autoCalibrateMode = true;
  actuatorConfig.autoCalibrateTimeoutMs = 2000;
  
  // Initialize actuator system controller
  actuatorController.begin(actuatorConfig);
  
  // Initialize wireless communication
  LBEAST_Wireless_Init(ssid, password, 8888);
  
  motionStartTime = millis();
  
  Serial.println("\nGunship Experience ECU Ready!");
  Serial.println("Waiting for motion commands from Unreal/Unity...\n");
  Serial.println("Channel Mapping:");
  Serial.println("  Ch 0: Pitch (degrees or normalized)");
  Serial.println("  Ch 1: Roll (degrees or normalized)");
  Serial.println("  Ch 2: TranslationY / Forward-Reverse (cm or normalized)");
  Serial.println("  Ch 3: TranslationZ / Vertical (cm or normalized)");
  Serial.println("  Ch 4: Duration (seconds)");
  Serial.println("  Ch 5: Calibration Mode (bool)");
  Serial.println("  Ch 6: Operation Mode (bool: auto-calibrate/fixed)");
  Serial.println("  Ch 7: Emergency Stop (bool)");
  Serial.println("  Ch 8: Return to Neutral (bool)\n");
  Serial.println("NOTICE: Both subsystems must be calibrated before use.\n");
}

// =====================================
// Main Loop
// =====================================

void loop() {
  // Process incoming LBEAST commands
  LBEAST_ProcessIncoming();
  
  // Update both controllers
  liftController.update();
  actuatorController.update();
  
  delay(10); // Control loop update rate (~100 Hz)
}

// =====================================
// LBEAST Command Handlers
// =====================================

void LBEAST_HandleFloat(uint8_t channel, float value) {
  switch (channel) {
    case 0:
      // Channel 0: Pitch (degrees or normalized -1.0 to +1.0)
      actuatorController.handleFloatCommand(0, value);
      break;
      
    case 1:
      // Channel 1: Roll (degrees or normalized -1.0 to +1.0)
      actuatorController.handleFloatCommand(1, value);
      break;
      
    case 2:
      // Channel 2: TranslationY / Forward-Reverse (cm or normalized -1.0 to +1.0)
      liftController.handleFloatCommand(1, value); // Channel 1 for forward/reverse in lift controller
      break;
      
    case 3:
      // Channel 3: TranslationZ / Vertical (cm or normalized -1.0 to +1.0)
      liftController.handleFloatCommand(0, value); // Channel 0 for vertical in lift controller
      break;
      
    case 4:
      // Channel 4: Duration (seconds)
      motionDuration = max(value, 0.1f);
      motionStartTime = millis();
      motionInProgress = true;
      Serial.printf("ECU: Motion duration = %.2f seconds\n", motionDuration);
      break;
  }
}

void LBEAST_HandleBool(uint8_t channel, bool value) {
  if (channel == 5) {
    // Channel 5: Calibration mode
    // Calibrate lift first, then actuators
    static bool calibratingLift = true;
    static bool calibrationActive = false;
    
    if (value && !calibrationActive) {
      // Start calibration
      calibrationActive = true;
      calibratingLift = true;
      liftController.setCalibrationMode(true);
      Serial.println("ECU: Entering calibration mode");
      Serial.println("First calibrate lift (bottom limit = zero), then actuators.");
      Serial.println("Upper limit is software-defined (90cm default).\n");
    } else if (!value && calibrationActive) {
      // End calibration
      if (calibratingLift) {
        // Finish lift calibration, start actuator calibration
        liftController.setCalibrationMode(false);
        if (liftController.isCalibrated()) {
          calibratingLift = false;
          actuatorController.setCalibrationMode(true);
          Serial.println("ECU: Lift calibrated. Now calibrating actuators...\n");
        }
      } else {
        // Finish actuator calibration
        actuatorController.setCalibrationMode(false);
        calibratingLift = true; // Reset for next calibration cycle
        calibrationActive = false;
        Serial.println("ECU: All subsystems calibrated!\n");
      }
    }
  } else if (channel == 6) {
    // Channel 6: Operation mode (true = auto-calibrate, false = fixed)
    liftController.setOperationMode(value);
    actuatorController.setOperationMode(value);
    Serial.printf("ECU: Operation mode = %s\n", value ? "Auto-calibrate" : "Fixed");
  } else if (channel == 7) {
    // Channel 7: Emergency stop
    liftController.setEmergencyStop(value);
    actuatorController.setEmergencyStop(value);
    if (value) {
      Serial.println("ECU: EMERGENCY STOP ACTIVATED");
    } else {
      Serial.println("ECU: Emergency stop released");
    }
  } else if (channel == 8) {
    // Channel 8: Return to neutral (calibrated zero)
    if (value) {
      liftController.returnToNeutral();
      actuatorController.returnToNeutral();
      Serial.println("ECU: Returning to calibrated zero position");
    }
  }
}
