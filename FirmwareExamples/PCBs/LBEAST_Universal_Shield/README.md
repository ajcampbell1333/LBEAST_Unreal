# LBEAST Universal Shield

## Overview

Multi-platform core shield providing unified hardware design for all LBEAST ECU firmware. Supports ESP32-S3, STM32, and Arduino as parent ECUs via personality adapters. Provides 8× CAT5 aux ports for Ethernet-based child ECUs (Raspberry Pi, Jetson Nano, or any Ethernet-capable device).

## Project Files

- **LBEAST_Universal_Shield.kicad_pro** - KiCAD project file
- **LBEAST_Universal_Shield.kicad_sch** - Schematic file
- **LBEAST_Universal_Shield.kicad_pcb** - PCB layout file

## Power Architecture

**Hybrid Approach:**
```
12V/24V Input (Barrel Jack)
    ↓
Buck Converter (LM2576: 12V/24V → 5V, 60V max input)
    ↓
    ├─→ 5V to MCU VIN (via adapter)
    │       ↓
    │   MCU Onboard Regulator (5V → 3.3V)
    │       ↓
    │   MCU 3.3V Output (via adapter)
    │       └─→ Shield Logic Components (LAN8720A, LEDs)
    │
    ├─→ 5V directly to Aux Port Pins 4 (≥5.0A total across 8 ports)
    │
    └─→ 5V to Shield 3.3V LDO (AP2112K-3.3TRG1 or AMS1117-3.3, ≥2A)
            ↓
        Shield 3.3V LDO Output
            └─→ Shield Logic Components (LAN8720A, LEDs)
```

**Key Points:**
- Buck converter (LM2576) handles 12V/24V input (up to 29.2V for fully charged 24V LiFePO4)
- Aux port power: 5V directly from buck converter (≥5.0A total capacity)
- Shield logic: 3.3V from shield LDO or MCU 3.3V output (via adapter)
- High-current MCUs (ESP32-S3, STM32F407): Adapter routes MCU 3.3V directly
- Low-current MCUs (Arduino): Adapter includes 3.3V regulator (AMS1117-3.3)

## Components

### Primary Components

**Ethernet PHY:**
- **LAN8720A** (QFN-32) - 100 Mbps Ethernet PHY
- Management: MDC/MDIO to MCU via adapter
- Clock: External 25MHz crystal (3225 SMD recommended)
- Reset: ESP32-S3 EN pin (GPIO3) or separate GPIO

**Power Management:**
- **LM2576** (TO-220/TO-263) - Buck converter (12V/24V → 5V, ≥5A, 60V max input)
- **AP2112K-3.3TRG1 or AMS1117-3.3** (SOT-223) - 3.3V LDO (5V → 3.3V, ≥2A) for shield logic only

**CAN Transceiver:**
- **ADM3057E** - Isolated CAN-FD transceiver (3kV isolation)
- VIO: +3.3V (logic supply)
- VCC: +5V (isoPower supply)
- VISOOUT → VISOIN (jump required)

**LEDs (3 total):**
- Power LED (Red): +3.3V → R1 (270Ω) → LED → GND
- Ethernet Link LED (Green): LAN8720A LED1 → R2 (270Ω) → LED → GND
- Ethernet Speed LED (Green): LAN8720A LED2 → R3 (270Ω) → LED → GND

### Connectors

**Power:**
- **Kycon KLDVHCX-0202-A-LT** - 2.1mm DC barrel jack (12V/24V input, vertical mount, ~15-20mm shell height)
- 12V/24V header: Passthrough for motors/solenoids
- 5V header: To MCU VIN (via adapter)

### Optional MCU Cooling Fan

- **Mechanical**:
  - Add a large through-hole window directly under the MCU socket so airflow can pass through the board when a fan is mounted below the module.
  - Flank the 44-pin MCU header with four plated mounting holes (M2/M2.5) laid out on a 40×40 mm square to accept standard standoffs for 40 mm fans. The same pattern works for top- or bottom-mounted fans; leave clearance for the ESP32 heat shield.
- **Electrical**:
  - Add a 2-pin JST-PH (or locking equivalent) header near the MCU slot labelled `FAN_PWR` (Pin 1 = +VIN 12/24 V, Pin 2 = FAN_SW).
  - Route Pin 1 to the raw 12 V/24 V rail immediately after the barrel jack (ahead of the LM2576 buck) with the same fuse/polyswitch protection as the rest of the VIN net.
  - Route Pin 2 to the drain of a logic-level N‑channel MOSFET (e.g., AO3400A/AOZ1282) so the MCU can switch the low side of the fan; tie the source to GND and add a 10 kΩ gate pull-down plus ~100 Ω gate resistor.
  - Place a flyback diode (1N5819 or SS14) across the fan connector (cathode on +VIN, anode on FAN_SW) to clamp inductive kick when the MOSFET turns off.
  - Provide a 0.1 µF/50 V snubber capacitor near the connector if the harness run is long (>0.5 m).
  - Default control pin suggestion: reserve ESP32-S3 `GPIO42` (Pad 26) for `FAN_CTRL`. This GPIO is currently unused on the shield and supports high-current drive; route it through the adapter header to the MOSFET gate. Other MCUs can remap as needed.
  - Label the net `FAN_CTRL` so firmware can PWM the fan if desired; recommend a 25 kHz PWM to stay out of the audible band.

**Ethernet:**
- **Adam Tech MTJ-883X1** (main) - RJ45 jack, vertical mount, 16.38mm shell height
- **Adam Tech MTJ-883X1** (8× aux ports) - Same as main connector

**E-Stop:**
- **2-pin pin header** (2.54mm pitch) - Emergency stop input (2-wire switch)
- Pin 1: E-Stop signal (to MCU interrupt pin via adapter, with RC debouncing)
- Pin 2: GND
- Hardware debouncing: 10kΩ pull-up + 100nF capacitor (1ms time constant)

**CAN Bus:**
- **3-pin connector** (vibration-resistant recommended: Phoenix Contact MSTB, Deutsch DT, Molex MX150, or JST EH)
- Pin 1: CANH
- Pin 2: CANL
- Pin 3: CAN_GND (always routed on PCB, even if unused in 2-wire networks)

**MCU Interface:**
- **44-pin 2×22 stacking female header** - ESP32-S3 socket/through-hole footprint

## Pin Mappings

### ESP32-S3 Aux Port ADC/PWM

| Aux Port | ADC GPIO | ADC Channel | PWM GPIO | PWM LEDC Channel |
|----------|----------|-------------|----------|------------------|
| 1 | GPIO4 | ADC1_CH3 | GPIO12 | LEDC_CH0 |
| 2 | GPIO5 | ADC1_CH4 | GPIO13 | LEDC_CH1 |
| 3 | GPIO6 | ADC1_CH5 | GPIO14 | LEDC_CH2 |
| 4 | GPIO7 | ADC1_CH6 | GPIO15 | LEDC_CH3 |
| 5 | GPIO8 | ADC1_CH7 | GPIO16 | LEDC_CH4 |
| 6 | GPIO9 | ADC1_CH8 | GPIO1 | LEDC_CH5 |
| 7 | GPIO10 | ADC1_CH9 | GPIO2 | LEDC_CH6 |
| 8 | GPIO11 | ADC2_CH0 | GPIO39 | LEDC_CH7 |

**KiCAD Net Labels:**
- ADC: `AUX1_ADC` through `AUX8_ADC`
- PWM: `AUX1_PWM` through `AUX8_PWM`

### LAN8720A Ethernet PHY

| LAN8720A Pin | ESP32-S3 GPIO | ESP32-S3 Pin | Function |
|--------------|---------------|--------------|----------|
| MDC | GPIO17 | Pin 10 | Management Data Clock |
| MDIO | GPIO18 | Pin 11 | Management Data I/O |
| RESET_N | EN (GPIO3) | Pin 3 | Reset (active LOW) |
| XTAL1/XTAL2 | External 25MHz crystal | - | Clock (3225 SMD recommended) |

**Ethernet Signals:**
- TXP/TXN → Main connector (J5) and all 8 aux ports (pins 1-2)
- RXP/RXN → Main connector (J5) and all 8 aux ports (pins 3, 6)

### ADM3057E CAN Transceiver

| ADM3057E Pin | ESP32-S3 Connection | Notes |
|--------------|---------------------|-------|
| TXD | GPIO40 (Pad 30) | CAN transmit |
| RXD | GPIO41 (Pad 29) | CAN receive |
| STBY | +3.3V (tie high) | Normal operation |
| SILENT | GND (tie low) | Normal operation |
| RS | GND (tie low) | Normal mode |
| VIO | +3.3V | Logic supply |
| VCC | +5V | isoPower supply |
| VISOOUT | Jump to VISOIN | Required |
| VISOIN | Jump to VISOOUT | Required |
| CANH | CAN connector Pin 1 | Bus high |
| CANL | CAN connector Pin 2 | Bus low |
| GNDISO | CAN connector Pin 3 | Isolated ground |

**Note:** GPIO4/5 are used for aux port ADCs, so CAN uses GPIO40/41.

### E-Stop

- **Signal:** ESP32-S3 interrupt-capable GPIO (GPIO45, GPIO46, or GPIO47 recommended)
- **Hardware Debouncing:** 10kΩ pull-up (R6) + 100nF capacitor (C11) = 1ms time constant
- **Net Labels:** `E_STOP_RAW` (before filter), `E_STOP` (after filter, to MCU)

## Aux Port Pinout (CAT5 T568B)

| RJ45 Pin | T568B Color | Signal | Connection |
|----------|-------------|--------|------------|
| 1 | Orange/White | Ethernet TX+ | LAN8720A TXP |
| 2 | Orange | Ethernet TX- | LAN8720A TXN |
| 3 | Green/White | Ethernet RX+ | LAN8720A RXP |
| 4 | Blue | **+5V Power** | Buck converter 5V (direct, ≥5.0A total) |
| 5 | Blue/White | **GND** | Common ground |
| 6 | Green | Ethernet RX- | LAN8720A RXN |
| 7 | Brown/White | **ADC** | MCU ADC pin via adapter (parent ECU only) |
| 8 | Brown | **PWM** | MCU PWM pin via adapter (parent ECU only) |

**Cable Requirements:**
- Standard straight-through Cat5e/Cat6 (T568B both ends)
- 24-26 AWG, UTP or STP
- Tested up to 50m (100m for light loads)

**Usage:**
- **Child ECUs (Pi/Jetson):** Connect via Ethernet (pins 1-3, 6). Use device's native ADC/PWM. Communication via UDP binary protocol.
- **Parent ECU I/O:** Access ADC/PWM on pins 7-8 for direct hardware control (ESP32/STM32/Arduino native).

## Bill of Materials

### Universal Shield (≈ $21-23 @ 1k pcs)

| Qty | Component | Part Number | Purpose | Unit Cost |
|-----|-----------|-------------|---------|-----------|
| 1 | Ethernet PHY | LAN8720A | 100 Mbps Ethernet | $1.50 |
| 1 | Ethernet connector (main) | Adam Tech MTJ-883X1 | Main RJ45 | $0.53 |
| 8 | Ethernet connector (aux) | Adam Tech MTJ-883X1 | Aux ports 1-8 | $0.53 × 8 |
| 1 | DC barrel jack | Kycon KLDVHCX-0202-A-LT | 12-24V input | $0.60 |
| 1 | Buck converter | LM2576 | 12V/24V → 5V (60V max) | $3.00 |
| 1 | 3.3V LDO | AP2112K-3.3TRG1 or AMS1117-3.3 | Shield logic power | $0.25 |
| 1 | CAN transceiver | ADM3057E | Isolated CAN-FD | $5.50 |
| 1 | E-Stop connector | 2-pin pin header | Emergency stop | $0.10 |
| 1 | MCU header | 44-pin 2×22 stacking | ESP32-S3 interface | $1.20 |
| - | Misc (caps, resistors, LEDs, protection) | - | Standard components | ~$2.00 |
| 1 (opt) | 40 mm fan | Sunon HA40101V4-1000U-A99 | Optional MCU cooling fan (12 V PWM) | $3.50 |

### Personality Adapters (Parent ECU Only)

| Platform | Adapter | Extra Components | Cost Adder | Total Cost |
|----------|---------|------------------|------------|------------|
| ESP32-S3 | Direct | None (native ADC/PWM) | $0 | ~$1 |
| STM32 | Direct | None (native ADC/PWM) | $0 | ~$1-2 |
| Arduino | 5V→3.3V | 6× TXS0108E (level shifters) | ~$7 | ~$8-9 |

**Note:** Raspberry Pi and Jetson Nano are **child ECUs** (connect via Ethernet aux ports), not adapters.

## Platform Support

**Parent ECU (via adapters):**
- ESP32-S3 (socket/through-hole) - Native ADC/PWM
- ESP32-WROOM-32 - Native ADC/PWM
- STM32 (Black Pill, Nucleo, etc.) - Native ADC/PWM
- Arduino Uno/Mega - Requires level shifters (TXS0108E)

**Child ECUs (via Ethernet aux ports):**
- Raspberry Pi 4/5 - Native GPIO/ADC/PWM, UDP communication
- Jetson Nano/Xavier NX - Native GPIO/ADC/PWM, UDP communication
- Any Ethernet-capable device - Device-dependent I/O, UDP communication

## Design Specifications

- **Aux Ports:** 8× CAT5 RJ45 (Ethernet + 5V power + 1× ADC + 1× PWM per port)
- **Ethernet:** 100 Mbps full-duplex (LAN8720A)
- **Power Input:** 12V/24V (up to 29.2V for fully charged 24V LiFePO4)
- **Aux Port Power:** 5V directly from buck converter (≥5.0A total across 8 ports)
- **Shield Logic Power:** 3.3V from shield LDO or MCU 3.3V (≥2A capacity)
- **CAN Bus:** Isolated CAN-FD (ADM3057E, 3kV isolation)
- **Connectors:** All vertical mount with tall shells (16.38mm) for potting

## Research Folder

PCB R&D, analysis documents, and troubleshooting guides are in the `Research/` subfolder:
- Power architecture analysis and options
- Component finding/creation guides
- KiCAD setup and troubleshooting
- CAN bus wiring and connector research
- Connector selection guides
