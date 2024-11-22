//Receiver code

#include <esp_now.h>
#include <WiFi.h>

// Motor pin definitions
class MotorConfig {
public:
    // Right motor
    static constexpr int enableRightMotor = 22;
    static constexpr int rightMotorPin1 = 16;
    static constexpr int rightMotorPin2 = 17;
    // Left motor
    static constexpr int enableLeftMotor = 23;
    static constexpr int leftMotorPin1 = 18;
    static constexpr int leftMotorPin2 = 19;

    // PWM configuration
    static constexpr int PWMFreq = 1000;
    static constexpr int PWMResolution = 8;
    static constexpr int rightMotorPWMSpeedChannel = 4;
    static constexpr int leftMotorPWMSpeedChannel = 5;
};

// Motor calibration values
class MotorCalibration {
public:
    // Adjust these values to match your motors (range 0.0 to 1.0)
    static constexpr float leftMotorMultiplier = 1.0;  // Base multiplier for left motor
    static constexpr float rightMotorMultiplier = 1.0; // Base multiplier for right motor
    
    // Unified turn settings for consistent turning behavior
    static constexpr float turnSpeedMultiplier = 0.7;  // Controls overall turn speed
    static constexpr float turnRatio = 0.5;            // Ratio for inner/outer wheel speeds (0.0 to 1.0)
};

#define MAX_MOTOR_SPEED 255
#define SIGNAL_TIMEOUT 1000
unsigned long lastRecvTime = 0;

struct PacketData {
    byte xAxisValue;
    byte yAxisValue;
    byte switchPressed;
};
PacketData receiverData;

bool throttleAndSteeringMode = false;

class MotorControl {
public:
    static void init() {
        pinMode(MotorConfig::enableRightMotor, OUTPUT);
        pinMode(MotorConfig::rightMotorPin1, OUTPUT);
        pinMode(MotorConfig::rightMotorPin2, OUTPUT);
        
        pinMode(MotorConfig::enableLeftMotor, OUTPUT);
        pinMode(MotorConfig::leftMotorPin1, OUTPUT);
        pinMode(MotorConfig::leftMotorPin2, OUTPUT);

        // Set up PWM
        ledcSetup(MotorConfig::rightMotorPWMSpeedChannel, MotorConfig::PWMFreq, MotorConfig::PWMResolution);
        ledcSetup(MotorConfig::leftMotorPWMSpeedChannel, MotorConfig::PWMFreq, MotorConfig::PWMResolution);
        ledcAttachPin(MotorConfig::enableRightMotor, MotorConfig::rightMotorPWMSpeedChannel);
        ledcAttachPin(MotorConfig::enableLeftMotor, MotorConfig::leftMotorPWMSpeedChannel);
        
        stop();
    }

    static void simpleMovements(const PacketData& data) {
        if (data.yAxisValue <= 75) {  // Forward
            moveForward(MAX_MOTOR_SPEED);
        }
        else if (data.yAxisValue >= 175) {  // Backward
            moveBackward(MAX_MOTOR_SPEED);
        }
        else if (data.xAxisValue >= 175) {  // Right
            turn(MAX_MOTOR_SPEED, true);  // true for right turn
        }
        else if (data.xAxisValue <= 75) {  // Left
            turn(MAX_MOTOR_SPEED, false); // false for left turn
        }
        else {
            stop();
        }
    }

    static void throttleAndSteeringMovements(const PacketData& data) {
        int throttle = map(data.yAxisValue, 254, 0, -255, 255);
        int steering = map(data.xAxisValue, 0, 254, -255, 255);
        
        // Apply exponential curve to steering for better control
        steering = applyExponentialCurve(steering);
        
        int motorDirection = (throttle < 0) ? -1 : 1;
        
        // Calculate base speeds
        float rightMotorSpeed = abs(throttle);
        float leftMotorSpeed = abs(throttle);

        // Apply steering adjustments with unified turn behavior
        if (steering != 0) {
            // Calculate turn intensity (0.0 to 1.0)
            float turnIntensity = abs(steering) / 255.0;
            
            // Apply unified turn speed reduction
            float turnSpeed = MAX_MOTOR_SPEED * MotorCalibration::turnSpeedMultiplier * turnIntensity;
            
            if (steering > 0) {  // Turning right
                rightMotorSpeed = turnSpeed;
                leftMotorSpeed = turnSpeed * MotorCalibration::turnRatio;
            } else {  // Turning left
                rightMotorSpeed = turnSpeed * MotorCalibration::turnRatio;
                leftMotorSpeed = turnSpeed;
            }
        }

        // Apply motor-specific calibration
        rightMotorSpeed *= MotorCalibration::rightMotorMultiplier;
        leftMotorSpeed *= MotorCalibration::leftMotorMultiplier;

        // Constrain final values
        rightMotorSpeed = constrain(rightMotorSpeed, 0, 255);
        leftMotorSpeed = constrain(leftMotorSpeed, 0, 255);

        rotateMotor(rightMotorSpeed * motorDirection, leftMotorSpeed * motorDirection);
    }

private:
    static void moveForward(int speed) {
        rotateMotor(speed * MotorCalibration::rightMotorMultiplier, 
                   speed * MotorCalibration::leftMotorMultiplier);
    }

    static void moveBackward(int speed) {
        rotateMotor(-speed * MotorCalibration::rightMotorMultiplier, 
                   -speed * MotorCalibration::leftMotorMultiplier);
    }

    // Unified turn function replacing separate turnRight and turnLeft
    static void turn(int speed, bool turnRight) {
        // Calculate speeds using unified turn settings
        float outerWheelSpeed = speed * MotorCalibration::turnSpeedMultiplier;
        float innerWheelSpeed = -outerWheelSpeed * MotorCalibration::turnRatio;
        
        if (turnRight) {
            rotateMotor(outerWheelSpeed * MotorCalibration::rightMotorMultiplier, 
                       innerWheelSpeed * MotorCalibration::leftMotorMultiplier);
        } else {
            rotateMotor(innerWheelSpeed * MotorCalibration::rightMotorMultiplier, 
                       outerWheelSpeed * MotorCalibration::leftMotorMultiplier);
        }
    }

    static void stop() {
        rotateMotor(0, 0);
    }

    static int applyExponentialCurve(int input) {
        // Apply a mild exponential curve for more precise control
        float normalized = input / 255.0;
        float curved = normalized * normalized * normalized;
        return (int)(curved * 255.0);
    }

    static void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
        // Right motor direction
        digitalWrite(MotorConfig::rightMotorPin1, rightMotorSpeed > 0);
        digitalWrite(MotorConfig::rightMotorPin2, rightMotorSpeed < 0);

        // Left motor direction
        digitalWrite(MotorConfig::leftMotorPin1, leftMotorSpeed > 0);
        digitalWrite(MotorConfig::leftMotorPin2, leftMotorSpeed < 0);

        // Set PWM speed
        ledcWrite(MotorConfig::rightMotorPWMSpeedChannel, abs(rightMotorSpeed));
        ledcWrite(MotorConfig::leftMotorPWMSpeedChannel, abs(leftMotorSpeed));
    }
};

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    if (len == 0) return;
    
    memcpy(&receiverData, incomingData, sizeof(receiverData));
    
    // Debug output
    String inputData = "values " + String(receiverData.xAxisValue) + "  " + 
                      String(receiverData.yAxisValue) + "  " + 
                      String(receiverData.switchPressed);
    Serial.println(inputData);

    // Toggle control mode if switch is pressed
    if (receiverData.switchPressed) {
        throttleAndSteeringMode = !throttleAndSteeringMode;
    }

    // Apply appropriate control mode
    if (throttleAndSteeringMode) {
        MotorControl::throttleAndSteeringMovements(receiverData);
    } else {
        MotorControl::simpleMovements(receiverData);
    }
    
    lastRecvTime = millis();
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Initialize motor control
    MotorControl::init();
    
    // Register callback
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // Check for signal timeout
    if (millis() - lastRecvTime > SIGNAL_TIMEOUT) {
        MotorControl::simpleMovements({127, 127, 0}); // Stop motors
    }
}