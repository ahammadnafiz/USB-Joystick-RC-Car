# USB Joystick RC Car Assembly Guide

## Table of Contents
1. [Required Components](#required-components)
2. [Tools Needed](#tools-needed)
3. [Safety Precautions](#safety-precautions)
4. [Assembly Steps](#assembly-steps)
5. [Software Installation](#software-installation)
6. [Testing and Calibration](#testing-and-calibration)
7. [Troubleshooting](#troubleshooting)

## Required Components

### Electronics
- 2× ESP32 Development Boards (WROOM recommended)
- 1× USB Joystick (compatible with USB host)
- 1× L298N Motor Driver Module
- 4× DC Motors (12V recommended)
- 1× Battery pack for motors (7.4V-12V LiPo or similar)
- 1× USB power bank for ESP32s (optional)
- 1× RC Car Chassis Kit (or custom chassis)
- Male-to-Female jumper wires (at least 20)
- Female-to-Female jumper wires (at least 10)

### Additional Materials
- Double-sided tape or mounting brackets
- Cable ties
- Heat shrink tubing (various sizes)
- Electrical tape
- Chassis mounting screws and nuts
- Battery holder or mounting solution

## Tools Needed
- Soldering iron and solder
- Wire stripper
- Phillips and flathead screwdrivers
- Small pliers
- Hot glue gun
- Multimeter
- USB cable for programming ESP32s
- Computer with Arduino IDE installed

## Safety Precautions
1. Always disconnect power before making any connections
2. Double-check all wiring before powering on
3. Use appropriate fuses for motor circuit protection
4. Keep battery voltage within recommended ranges
5. Ensure proper ventilation when soldering
6. Wear safety glasses during assembly
7. Keep workspace clean and organized

## Assembly Steps

### 1. Chassis Assembly
1. Unpack and identify all chassis components
2. Follow chassis kit instructions to assemble the basic frame
3. Mount the motors in their designated positions
4. Ensure motor shafts rotate freely
5. Attach wheels to motor shafts

### 2. Motor Driver Setup
1. Mount the L298N motor driver in a central location on the chassis
2. Connect motors to L298N:
   - Right motors to OUT1 and OUT2
   - Left motors to OUT3 and OUT4
3. Add heat shrink tubing to motor connections
4. Secure all wires with cable ties

### 3. Receiver ESP32 Connections
1. Mount the receiver ESP32 near the L298N
2. Connect ESP32 to L298N:
   ```
   ESP32 -> L298N
   GPIO 16 -> IN1
   GPIO 17 -> IN2
   GPIO 18 -> IN3
   GPIO 19 -> IN4
   GPIO 22 -> ENA
   GPIO 23 -> ENB
   GND -> GND
   VIN -> 5V
   ```
3. Double-check all connections with multimeter
4. Secure ESP32 with mounting tape or brackets

### 4. Power System Setup
1. Install battery holder in chassis
2. Connect battery power to L298N:
   - Battery positive (+) to L298N 12V
   - Battery negative (-) to L298N GND
3. Add power switch in battery positive line
4. Install fuse holder in positive line
5. Verify voltage at all connection points

### 5. Transmitter Assembly
1. Prepare USB joystick connection:
   ```
   ESP32 -> USB Joystick
   GPIO 16 -> D+ (Green)
   GPIO 17 -> D- (White)
   GND -> GND (Black)
   5V -> VCC (Red)
   ```
2. Use heat shrink tubing on all connections
3. Mount transmitter ESP32 in suitable enclosure
4. Add power switch if using battery power

## Software Installation

### 1. Arduino IDE Setup
1. Install Arduino IDE from arduino.cc
2. Add ESP32 board support URL in preferences
3. Install ESP32 board package
4. Install required libraries:
   - ESP-NOW
   - WiFi
   - ESP32-USB-Soft-Host

### 2. Programming Steps
1. Upload transmitter code:
   - Open transmitter code in Arduino IDE
   - Select correct ESP32 board model
   - Set correct port
   - Upload code
   - Note down MAC address displayed in serial monitor

2. Update receiver code:
   - Replace MAC address in receiver code with transmitter's MAC
   - Upload to receiver ESP32
   - Verify successful compilation

## Testing and Calibration

### 1. Initial Testing
1. Power up receiver first
2. Connect transmitter to power
3. Verify ESP-NOW connection (check serial monitor)
4. Test basic movements:
   - Forward
   - Backward
   - Left turn
   - Right turn
   - Stop

### 2. Motor Calibration
1. Adjust `MotorCalibration` values in receiver code:
   ```cpp
   leftMotorMultiplier = 1.0;  // Adjust if needed
   rightMotorMultiplier = 1.0; // Adjust if needed
   turnSpeedMultiplier = 0.7;  // Modify for turn speed
   turnRatio = 0.5;            // Adjust turning behavior
   ```
2. Upload modified code
3. Test and repeat until movement is smooth

## Troubleshooting

### Common Issues and Solutions

1. Motors Don't Move
   - Check power connections
   - Verify motor driver pins
   - Test motor driver outputs with multimeter
   - Check ESP-NOW connection status

2. Erratic Movement
   - Calibrate motor multipliers
   - Check for loose connections
   - Verify joystick readings in serial monitor
   - Check battery voltage

3. Communication Problems
   - Verify MAC addresses match
   - Check WiFi mode settings
   - Reset both ESP32s
   - Monitor serial output for errors

4. Joystick Issues
   - Check USB connections
   - Verify joystick data in serial monitor
   - Test with different USB cable
   - Check power supply stability

### Maintenance Tips
1. Regularly check all connections
2. Keep motors clean and lubricated
3. Monitor battery voltage during operation
4. Periodically tighten chassis screws
5. Keep firmware updated

---

For additional support or questions, refer to the project documentation or community forums. Remember to always disconnect power before making any modifications to the system.