# USB Joystick RC Car

A DIY RC car project that uses a USB joystick to control a four-wheeled vehicle via ESP32 microcontrollers and ESP-NOW wireless communication.

![Hero](Assets/Hero.png)

## 🚗 Features

- USB joystick support for intuitive control
- Wireless communication using ESP-NOW protocol
- Dual control modes:
  - Simple mode (4-direction control)
  - Throttle and steering mode (proportional control)
- Real-time control with minimal latency
- Automatic safety stop on signal loss
- Support for various USB joysticks
- Configurable motor calibration
- Built-in deadband compensation

## 📋 Prerequisites

### Hardware Requirements

#### Transmitter Unit
- ESP32 Development Board
- USB Joystick
- USB cables
- Power supply (USB or battery)

#### Receiver Unit
- ESP32 Development Board
- L298N Motor Driver
- 4× DC Motors (6-12V)
- Battery Pack (6-12V)
- Chassis (DIY or pre-made)
- Jumper Wires
- Power supply for ESP32

### Software Requirements

- Arduino IDE
- Required Libraries:
  - ESP32-USB-Soft-Host
  - ESP-NOW
  - WiFi.h

## 🛠️ Hardware Assembly

### Transmitter Setup

1. Connect the ESP32 to the USB joystick:
   - GPIO 16 → USB D+ (Green wire)
   - GPIO 17 → USB D- (White wire)
   - GND → USB GND (Black wire)
   - 5V → USB VCC (Red wire)

### Receiver Setup

1. Connect ESP32 to L298N Motor Driver:
   ```
   ESP32      →    L298N
   GPIO 16    →    IN1
   GPIO 17    →    IN2
   GPIO 18    →    IN3
   GPIO 19    →    IN4
   GPIO 22    →    ENA
   GPIO 23    →    ENB
   ```

2. Connect L298N to Motors:
   ```
   L298N      →    Motors
   OUT1, OUT2 →    Right Motors
   OUT3, OUT4 →    Left Motors
   ```

3. Power Connections:
   ```
   Battery (6-12V) →    L298N 12V
   L298N 5V       →    ESP32 VIN
   GND            →    Common Ground
   ```

## 💻 Software Installation

1. Install Required Libraries:
   ```bash
   # Clone ESP32-USB-Soft-Host library
   git clone https://github.com/tobozo/ESP32-USB-Soft-Host.git
   # Move to Arduino libraries folder
   ```

2. Configure Arduino IDE:
   - Select ESP32 board
   - Set upload speed to 115200
   - Select correct COM port

3. Upload Code:
   - Upload transmitter code to the controller ESP32
   - Upload receiver code to the car's ESP32

## ⚙️ Configuration

### MAC Address Setup

1. Get receiver's MAC address:
   - Upload and run the included MAC address finder sketch
   - Copy the printed MAC address

2. Update transmitter code:
   ```cpp
   uint8_t receiverMacAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // Replace with your MAC
   ```

### Motor Calibration

Adjust motor calibration values in `MotorCalibration` class:
```cpp
static constexpr float leftMotorMultiplier = 1.0;   // Adjust if needed
static constexpr float rightMotorMultiplier = 1.0;  // Adjust if needed
static constexpr float turnSpeedMultiplier = 0.7;   // Adjust turn speed
static constexpr float turnRatio = 0.5;             // Adjust turn sharpness
```

## 🎮 Usage

1. Power up both transmitter and receiver units
2. Wait for ESP-NOW connection (LED indicator)
3. Control modes:
   - Default: Simple 4-direction control
   - Press joystick button to toggle throttle/steering mode
4. Emergency stop: Release joystick or power off transmitter

## 🔧 Troubleshooting

Common issues and solutions:

1. Motors don't respond:
   - Check power connections
   - Verify MAC address configuration
   - Check motor driver connections

2. Erratic movement:
   - Calibrate motors using multiplier values
   - Check for loose connections
   - Verify power supply stability

3. Connection issues:
   - Ensure both units are powered
   - Check MAC address configuration
   - Verify ESP-NOW initialization

## 📝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request


## 🙏 Acknowledgments

- [ESP32-USB-Soft-Host](https://github.com/tobozo/ESP32-USB-Soft-Host) library
- ESP-NOW documentation and examples
- Arduino community for various inspirations

## 📞 Support

For support and questions:
- Open an issue
- Check existing documentation
- Review closed issues for similar problems

---
Made with ❤️ by Ahammad Nafiz