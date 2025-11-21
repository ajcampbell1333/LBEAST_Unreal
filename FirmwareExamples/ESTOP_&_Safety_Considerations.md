# E-Stop & Safety Considerations

<details>
<summary><strong>⚠️Author Disclaimer:</strong></summary>

<div style="margin-left: 20px;">
This plugin provides code and advice that may or may not run on systems your local and state officials may classify as  "amusement rides" or "theme park rides" which may fall under ASTM standards or other local regulations. LBEAST's author disclaims any and all liability for any use of this code, including for safety of guests or patrons, regulatory readiness, etc. Please review the local regulations in your area prior to executing this code in any public venue. You are responsible for compliance in your state.
</div>

</details><br>

## Overview

E-Stop (Emergency Stop) systems are **external safety systems** implemented upstream from everything in an LBEAST experience that requires significant current, including hydraulics, servo motors, low-current PCBs that drive these systems, etc. The rule-of-thumb is... if it's physically possible for the thing to cause a bruise or worse under ANY scenario ever... E-Stop.

E-Stop systems cut power at the battery/power supply level, before power reaches the shield board or any other system components. This document provides guidance for sizing, installing, testing, and maintaining E-Stop systems for LBEAST installations.

**⚠️ Critical:** E-Stop systems are safety-critical. Never bypass, modify, or disable E-Stop circuits. Always test E-Stop functionality before each use. Consult qualified electrical engineer for installation and testing if unsure of compliance requirements.

## E-Stop Circuit Requirements

**Location:** High-side switching (cutting +V) immediately after main circuit breaker, as close to battery/power supply as possible, before barrel jack.

**Standard Practice:** Single contactor at battery/power supply cuts ALL power (high-current loads + shield board).

(lib_symbols
	(symbol "power:GND"
		(power)
		(pin_numbers
			(hide yes)
		)
		(pin_names
			(offset 0)
			(hide yes)
		)
		(exclude_from_sim no)
		(in_bom yes)
		(on_board yes)
		(property "Reference" "#PWR"
			(at 0 -6.35 0)
			(effects
				(font (size 1.27 1.27))
				(hide yes)
			)
		)
		(property "Value" "GND"
			(at 0 -3.81 0)
			(effects
				(font (size 1.27 1.27))
			)
		)
		(property "Footprint" ""
			(at 0 0 0)
			(effects
				(font (size 1.27 1.27))
				(hide yes)
			)
		)
		(property "Datasheet" ""
			(at 0 0 0)
			(effects
				(font (size 1.27 1.27))
				(hide yes)
			)
		)
		(property "Description" "Power symbol creates a global label with name \"GND\" , ground"
			(at 0 0 0)
			(effects
				(font (size 1.27 1.27))
				(hide yes)
			)
		)
		(property "ki_keywords" "global power"
			(at 0 0 0)
			(effects
				(font (size 1.27 1.27))
				(hide yes)
			)
		)
		(symbol "GND_0_1"
			(polyline
				(pts
					(xy 0 0) (xy 0 -1.27) (xy 1.27 -1.27) (xy 0 -2.54) (xy -1.27 -1.27) (xy 0 -1.27)
				)
				(stroke (width 0) (type default))
				(fill (type none))
			)
		)
		(symbol "GND_1_1"
			(pin power_in line
				(at 0 0 270)
				(length 0)
				(name "~"
					(effects
						(font (size 1.27 1.27))
					)
				)
				(number "1"
					(effects
						(font (size 1.27 1.27))
					)
				)
			)
		)
		(embedded_fonts no)
	)
)
(symbol
	(lib_id "power:GND")
	(at 162.56 153.67 0)
	(unit 1)
	(exclude_from_sim no)
	(in_bom yes)
	(on_board yes)
	(dnp no)
	(fields_autoplaced yes)
	(uuid "dd79c449-3789-44f3-a0e5-6859e6f44193")
	(property "Reference" "#PWR026"
		(at 162.56 160.02 0)
		(effects
			(font (size 1.27 1.27))
			(hide yes)
		)
	)
	(property "Value" "GND"
		(at 162.56 158.75 0)
		(effects
			(font (size 1.27 1.27))
		)
	)
	(property "Footprint" ""
		(at 162.56 153.67 0)
		(effects
			(font (size 1.27 1.27))
			(hide yes)
		)
	)
	(property "Datasheet" ""
		(at 162.56 153.67 0)
		(effects
			(font (size 1.27 1.27))
			(hide yes)
		)
	)
	(property "Description" "Power symbol creates a global label with name \"GND\" , ground"
		(at 162.56 153.67 0)
		(effects
			(font (size 1.27 1.27))
			(hide yes)
		)
	)
	(pin "1"
		(uuid "ddaf8e9a-de08-4a99-a788-d5de9882a918")
	)
	(instances
		(project "LBEAST_Universal_Shield"
			(path ""
				(reference "#PWR026")
				(unit 1)
			)
		)
	)
)
**Standards Compliance:** IEC 60204-1, NFPA 79 (high-side switching required).

**Why High-Side Switching?**
- **Fault tolerance:** Shorts to GND can't bypass high-side switch
- **Better isolation:** Removes voltage from load side when opened
- **Standards compliance:** IEC 60204-1, NFPA 79 require high-side switching
- **Easier fault detection:** Can detect failed-open contactor (voltage present when it shouldn't be)

**Circuit Topology:**
```
Battery/Power Supply (+24_12V)
    ↓
Main Circuit Breaker/Fuse (overcurrent protection)
    ↓
High-Side Relay/Contactor (controlled by E-Stop switch)
    ↓
E-Stop Switch (NC, latching) - controls relay coil
    ├─→ Relay Coil (cuts ALL power: high-current loads + board)
    └─→ Distribution (fuses, breakers, individual loads)
```

## E-Stop Contactor Sizing

The E-Stop contactor must handle the combined current draw of all systems:

### Scissor Lift Systems

- **Skyjack scissor lifts:** Reference Skyjack operator manuals for specific model current ratings
  - Typical range: 15-30A @ 24V for small lifts (SJ3219, SJ3226)
  - Typical range: 30-60A @ 24V for medium lifts (SJ4632, SJ4642)
  - Typical range: 60-100A @ 24V for large lifts (SJ6832, SJ6942)
- **Genie scissor lifts:** Reference Genie operator manuals for specific model current ratings
  - Typical range: 15-30A @ 24V for small lifts (GS-1930, GS-2632)
  - Typical range: 30-60A @ 24V for medium lifts (GS-3246, GS-4047)
  - Typical range: 60-100A @ 24V for large lifts (GS-5390, GS-6868)

### Hydraulic Actuator Systems

- **Hydraulic pump motors:** Reference pump manufacturer datasheets
  - Typical range: 20-50A @ 24V for small pumps (1-2 HP)
  - Typical range: 50-100A @ 24V for medium pumps (3-5 HP)
  - Typical range: 100-200A @ 24V for large pumps (7.5-10 HP)
- **Actuator drivers/valves:** Reference driver/valve manufacturer datasheets
  - Typical range: 5-15A @ 24V per actuator driver
  - Typical range: 1-5A @ 24V per solenoid valve

### Shield Board Power

- Shield board + MCU: ~2-5A @ 24V (typically <1A, but allow headroom)

### Total Current Calculation

```
Total E-Stop Contactor Rating = 
    Scissor Lift Current (from manufacturer manual)
    + Hydraulic Pump Current (from pump datasheet)
    + (Actuator Driver Current × Number of Drivers)
    + (Solenoid Valve Current × Number of Valves)
    + Shield Board Current (~5A safety margin)
    + 25% Safety Margin (for inrush, peak loads, future expansion)
```

### Example Sizing

- Small system (1 small scissor lift + 1 small pump + 2 actuators): ~50-75A @ 24V → **100A contactor recommended**
- Medium system (1 medium scissor lift + 1 medium pump + 4 actuators): ~100-150A @ 24V → **150-200A contactor recommended**
- Large system (1 large scissor lift + 1 large pump + 6+ actuators): ~200-300A @ 24V → **300-400A contactor recommended**

### Contactor Selection

**What is a Contactor?**
A contactor is an electrically-controlled switch designed to handle high currents (typically 50A-400A or more). Think of it like a heavy-duty light switch, but instead of flipping it by hand, it's controlled by a small electrical signal (like the E-Stop switch). When the E-Stop switch is pressed, it sends a signal to the contactor's coil (a small electromagnet), which mechanically opens the contactor's main contacts, cutting all power to your system. Contactors are specifically designed for high-current applications and have built-in arc suppression to safely interrupt large currents without damage. For E-Stop applications, contactors are preferred over regular relays because they're rated for higher currents, have more robust mechanical construction, and are designed for safety-critical applications.

**Selection Criteria:**
- Use industrial-grade contactors rated for **DC operation** (DC contactors have different arc suppression than AC)
- Ensure contactor is rated for the full voltage range (12V-24V, up to 29.2V for fully charged 24V LiFePO4)
- Consider contactor with auxiliary contacts for status monitoring
- Verify contactor coil voltage matches E-Stop switch control voltage (typically 12V or 24V)
- Contactor must be **positive-opening** (mechanical opening, not dependent on spring return)

### References

- Skyjack operator manuals: [Skyjack Support](https://www.skyjack.com/en/support/)
- Genie operator manuals: [Genie Support](https://www.genielift.com/en/support/)
- Hydraulic pump datasheets: Consult pump manufacturer (Parker, Bosch Rexroth, etc.)
- Actuator driver datasheets: Consult driver manufacturer (Parker, Moog, etc.)

## Standards Compliance

- **IEC 60204-1** (Safety of machinery - Electrical equipment): Requires E-Stop systems to be Category 0 (immediate removal of power) or Category 1 (controlled stop then power removal). High-side switching is mandatory.
- **NFPA 79** (Electrical Standard for Industrial Machinery): Requires E-Stop circuits to be "fail-safe" (any single fault does not prevent E-Stop function). High-side switching with positive-opening contacts required.
- **OSHA 29 CFR 1910** (General Industry): Requires emergency stop devices on machinery where employee exposure to hazards exists.
- **ASTM F2291** (Amusement Rides and Devices): May apply if system is classified as an amusement ride in your jurisdiction.

## Installation Requirements

- E-Stop contactor must be rated for **125% of maximum continuous current** (per NFPA 79)
- Contactor must be **DC-rated** (not AC contactor used for DC - different arc suppression)
- Contactor must be **positive-opening** (mechanical opening, not dependent on spring return)
- E-Stop switch must be **readily accessible** and **clearly marked**
- E-Stop circuit must be **fail-safe** (single fault does not prevent operation)
- All E-Stop wiring must be **separate from control wiring** (dedicated safety circuit)

## Testing Requirements

### Initial Installation Testing

1. **Functional Test:**
   - Verify E-Stop switch immediately cuts all power when pressed
   - Verify E-Stop switch latches in "off" position until manually reset
   - Verify all systems (high-current loads + shield board) de-energize simultaneously
   - Verify E-Stop cannot be bypassed by any single fault

2. **Load Testing (Required for Compliance):**
   - **Test total load at the battery with accurate simulated max weight capacity to confirm continuous operation at max current within expected tolerance.**
   - Apply maximum rated load to all systems simultaneously:
     - Scissor lift at maximum platform capacity (reference manufacturer manual)
     - Hydraulic pump at maximum pressure/flow
     - All actuators at maximum extension/retraction force
     - All solenoids energized
   - Measure actual current draw at battery terminals using calibrated clamp meter or shunt
   - Verify continuous operation for minimum 30 minutes at max load
   - Verify E-Stop contactor current rating is ≥125% of measured max current
   - Document test results (current readings, duration, ambient temperature)

3. **Inrush Current Testing:**
   - Measure peak inrush current during system startup (all motors starting simultaneously)
   - Verify contactor can handle inrush without contact welding or degradation
   - Typical inrush: 3-5× continuous current for 100-500ms

4. **Voltage Drop Testing:**
   - Measure voltage drop across E-Stop contactor contacts at max load
   - Verify voltage drop <2% of supply voltage (per NFPA 79)
   - Excessive voltage drop indicates contactor undersizing or contact degradation

### Periodic Testing (Maintenance)

- **Daily:** Visual inspection of E-Stop switch (accessibility, clear marking, no damage)
- **Weekly:** Functional test (press E-Stop, verify all systems de-energize)
- **Monthly:** Load test at 50% capacity (verify contactor operation under load)
- **Annually:** Full load test (repeat initial installation testing)
- **After any maintenance:** Functional test before returning to service

## Documentation Requirements

- Maintain test records (date, tester name, results, any failures)
- Document contactor part number, current rating, and installation date
- Document all E-Stop circuit modifications
- Keep manufacturer datasheets for contactor and E-Stop switch on file

## Failure Modes and Troubleshooting

- **Contactor fails to open:** Check for contact welding (excessive current, undersized contactor)
- **Contactor opens but power remains:** Check for bypass wiring or secondary power sources
- **E-Stop switch doesn't latch:** Replace switch (mechanical failure)
- **Excessive voltage drop:** Check contactor contacts for pitting/corrosion, verify proper sizing

## Additional Safety Considerations

- **QTY2 Up-to-code Fire Emergency Fire Extinguishers:** One at the Ops Tech Console and another near any hydraulic equipment.
- **Movable stairs:** Any system that causes players to be lifted into the air must have a physical means of egress in an e-stop emergency.
- **Hydraulically-actuated equipment should have multiple manual and auto e-stops** located at console and on device.
- **Theme park safety regulations vary by state** - take steps to abide by the same rules that apply to carnival equipment in your state.
- **The author of LBEAST disclaims any liability resulting in the use of this free software.**

