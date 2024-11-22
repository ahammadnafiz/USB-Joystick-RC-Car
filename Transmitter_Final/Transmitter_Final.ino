// Transmitter code

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32-USB-Soft-Host.h>

#define DEBUG_ALL
#define FORCE_TEMPLATED_NOPS

// Define pins for WROOM
#define PROFILE_NAME "Default Wroom"
#define DP_P0 16
#define DM_P0 17
#define DP_P1 22
#define DM_P1 23
#define DP_P2 18
#define DM_P2 19
#define DP_P3 13
#define DM_P3 15

usb_pins_config_t USB_Pins_Config = {
    DP_P0, DM_P0,
    DP_P1, DM_P1,
    DP_P2, DM_P2,
    DP_P3, DM_P3
};

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t receiverMacAddress[] = {0x2C, 0xBC, 0xBB, 0x0D, 0x88, 0x38};

struct PacketData {
  byte xAxisValue;
  byte yAxisValue;
  byte switchPressed;
};
PacketData data;

uint8_t lastValidData[8] = {0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int mapAndAdjustJoystickDeadBandValues(int value, bool reverse) {
  if (value >= 2200) {
    value = map(value, 2200, 4095, 127, 254);
  }
  else if (value <= 1800) {
    value = map(value, 1800, 0, 127, 0);  
  }
  else {
    value = 127;
  }
  
  if (reverse) {
    value = 254 - value;
  }
  return value;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}

static void my_USB_PrintCB(uint8_t usbNum, uint8_t byte_depth, uint8_t* usb_data, uint8_t data_len) {
  bool isZeroSequence = true;

  for (int i = 0; i < data_len; i++) {
    if (usb_data[i] != 0x00) {
      isZeroSequence = false;
      break;
    }
  }

  if (isZeroSequence) {
    memcpy(usb_data, lastValidData, data_len);
  } else {
    memcpy(lastValidData, usb_data, data_len);
  }

  // Convert raw USB data to mapped joystick values
  int x = (usb_data[0] - 0x7F) * 16 + 2047; // Scale to 0-4095 range
  int y = -(usb_data[1] - 0x7F) * 16 + 2047; // Invert Y-axis, scale to 0-4095 range

  // Apply deadband mapping logic
  data.xAxisValue = mapAndAdjustJoystickDeadBandValues(x, false);
  data.yAxisValue = mapAndAdjustJoystickDeadBandValues(y, false);

  // Check for button press 
  data.switchPressed = (usb_data[2] != 0);

  // Debugging: Print joystick values
  Serial.print("X-Axis: ");
  Serial.print(data.xAxisValue);
  Serial.print(" | Y-Axis: ");
  Serial.print(data.yAxisValue);
  Serial.print(" | Button: ");
  Serial.println(data.switchPressed);

  // Send via ESP-NOW
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &data, sizeof(data));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }    
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // Set ESP32 to station mode

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("Success: Initialized ESP-NOW");
  }

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6); // Set receiver's MAC address
  peerInfo.channel = 0;  // Use default channel (0: same as current WiFi channel)
  peerInfo.encrypt = false; // Encryption not used

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  } else {
    Serial.println("Success: Added peer");
  }

  // Initialize USB Soft Host
  delay(5000);
  printf("USB Soft Host Test for %s\n", PROFILE_NAME);

  USH.setOnConfigDescCB(Default_USB_ConfigDescCB);
  USH.setOnIfaceDescCb(Default_USB_IfaceDescCb);
  USH.setOnHIDDevDescCb(Default_USB_HIDDevDescCb);
  USH.setOnEPDescCb(Default_USB_EPDescCb);

  USH.init(USB_Pins_Config, nullptr, my_USB_PrintCB);
}


void loop() {
  delay(50); // Prevent watchdog warnings
}